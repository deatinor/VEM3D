//
//  SolverVEM3D.h
//  Mesh3
//
//  Created by Stefano on 17/08/15.
//  Copyright (c) 2015 Stefano. All rights reserved.
//

#ifndef Mesh3_SolverVEM3D_h
#define Mesh3_SolverVEM3D_h

#include "Solver.h"
#include "SolverVEM.h"



/** Specilized class to solve VEM in 3D
 *
 *	It only implements computeB and computeKnownTerm from the parent class.
 *	Then it has to add the Polygon version of the methods to compute the boundary terms.
 */
template <typename real=double>
class SolverVEM3D: public SolverVEM<3,3,Polyhedron<3,real>,Monomial3D<real>,real> {
public:
	virtual Matrix<real,3,3> computeGPolygon(MonomialsPolygon<real>& monomial); // fatta da controllare
	virtual Matrix<real,4,Dynamic> computeB(const shared_ptr<Polyhedron<3,real>>& polyhedron,Monomial3D<real>& monomial); // sistemare l'integrale al bordo
	virtual Matrix<real,3,Dynamic> computeBPolygon(const shared_ptr<Polygon<3,real>>& polyhedron,MonomialsPolygon<real>& monomial); // da fare
	virtual Matrix<real,Dynamic,3> computeDPolygon(MonomialsPolygon<real>& monomial); // da fare
	
	
	virtual Matrix<real,3,Dynamic> computePIStarPolygon(Matrix<real,3,3>&G,Matrix<real,3,Dynamic>&B);
	
public:
	/** Standard constructor
	 */
	SolverVEM3D(std::function<real(const Point<3,real>&)> inputForceTerm):SolverVEM<3,3,Polyhedron<3,real>,Monomial3D<real>,real>::SolverVEM(inputForceTerm) {};
	/** 3D version of comuteKnownTerm
	 */
	virtual real computeKnownTerm(const shared_ptr<Polyhedron<3,real>>& element,const shared_ptr<MeshPoint<3,real>>& point);
	
};


// computeKnownTerm
template <typename real>
real SolverVEM3D<real>::computeKnownTerm(const shared_ptr<Polyhedron<3,real>>& element,const shared_ptr<MeshPoint<3,real>>& point) {
	
	real fValue=this->forceTerm(element->getCentroid()); // calcola la f nel primo punto di ogni poliedro, credo che basti essendo piecewise constant
	
//	for (int i=0;i<element->numberOfPoints;i++) {
//		fValue+=this->forceTerm(*element->point(i));
//	}
	
//	fValue/=element->numberOfPoints;
	
	real vhCoefficient=1.0/(element->numberOfPoints); // e' il P0 di ogni vh, sempre lo stesso per ogni vh
	real volume=element->getVolume(); // lo salva per non dovervi accedere per ogni punto
	
	return volume*vhCoefficient*fValue;
	
}



// G polygon
template <typename real>
Matrix<real,3,3> SolverVEM3D<real>::computeGPolygon(MonomialsPolygon<real>& monomial) {
//	auto& pointVector=monomial.element->pointVector;

	auto& element=monomial.element;
	real area=element->getArea();
	
	Matrix<real,3,3> GPolygon;
	
	
	// prima colonna
	GPolygon(0,0)=1;
	for (int i=1;i<3;i++) {
		GPolygon(i,0)=0;
	}
	
	// prima riga (tranne prima colonna)
	real value=0;
	for (int i=1; i<3; i++) {
		value=0;
		for (int k=0;k<element->numberOfPoints;k++) {
			//			cout<<monomial.evaluate(pointVector[k],i-1)<<endl;
			value+=monomial.evaluate(*(element->point(k)),i-1);
		}
		GPolygon(0,i)=value/element->numberOfPoints;
	}
	
	// il resto (non è più diagonale)
	GPolygon(1,1)=monomial.gradientX*monomial.gradientX*area;
	GPolygon(1,2)=monomial.gradientX*monomial.gradientY*area;
	GPolygon(2,1)=GPolygon(1,2);
	GPolygon(2,2)=monomial.gradientY*monomial.gradientY*area;
	
	//	for (int i=1;i<3;i++) {
	//		for (int j=1;j<3;j++) {
	//
	//			if (i==j) G(i,j)=monomial.gradient*monomial.gradient*area;
	//			else G(i,j)=0;
	//		}
	//	}
	
	return GPolygon;
	
}

// B
template <typename real>
Matrix<real,4,Dynamic> SolverVEM3D<real>::computeB(const shared_ptr<Polyhedron<3,real>>& polyhedron,Monomial3D<real>& monomial) {
	long numberOfPoints=polyhedron->numberOfPoints;
	Matrix<real,4,Dynamic> B(4,numberOfPoints);
	
	// nella prima riga della matrice si usa P0, nel nostro caso 1/numberOfPoints
	real firstLine=1.0/numberOfPoints;
	for (int i=0;i<numberOfPoints;i++) {
		B(0,i)=firstLine;
	}
	
	// inizializzo il resto della matrice a 0
	for (int i=1;i<4;i++) {
		for (int j=0;j<numberOfPoints;j++) {
			B(i,j)=0;
		}
	}
	
	// nelle altre righe devo usare un integrale al bordo
//	auto& polygonVector=polyhedron->polygonVector;
//	auto& pointVector=polyhedron->pointVector;
	for (int k=0; k<polyhedron->numberOfPolygons; k++) {
		auto& face=polyhedron->polygon(k);
		long facePoints=face->numberOfPoints;
		real area=face->getArea();
		auto& normal=face->getNormal();
		//		auto centroid=face.centroid;
		
		
		///////////////////////////////
		// STRADA 2
		// è più complicato, bisogna calcolare il polinomio interpolante. Si scelgono 2 monomi tra x,y e z. È importante non scegliere quello corrispondente al maxNormalIndex (nel caso di poligono sul piano xy il maxNormalIndex corrisponde a z). Supponendo di scegliere x e y:
		// gli altri monomi formano una base per i polinomi di grado 1 nello spazio (genericamente un polinomio potrebbe avere anche la componente z, essendo definito su un piano in R3 la componente z la si ottiene sfruttando l'equazione del piano, la z è infatti dipendente dalle altre due)
		// per gestire questo si potrebbe creare una classe MonomialPolygon o qualcosa del genere specializzata per i poligoni in 3D
		// per la matrice G ricordardi di calcolare il gradiente tangenziale (quindi il gradiente di x sarebbe 1-Nx*Nx). Il prodotto del gradiente di x con il gradiente di y NON è 0. Devo proiettarlo sul piano.
		// per la matrice D è tutto normale, basta valutare il monomio nei vertici (meglio farla per controllare)
		// per la matrice B si riduce ad un integrale al bordo. Si deve calcolare la normale ad un lato usando il prodotto vettoriale tra il lato e la normale al poligono (attenzione al segno). L'integrale al bordo viene fatto con i nodi GLL (si riduce sostanzialmente a 1/2*(l1+l2) dove l1 e l2 sono i due lati adiacenti al vertice dove phi vale 1)
		// volendo se funziona si può scambiare questo ciclo con il precedente che probabilmente è più veloce
		MonomialsPolygon<real> monomialPolygon(face);
		
		
		// gli indexX e indexY sono all'interno del monomial
		auto GPolygon=computeGPolygon(monomialPolygon);
		//		cout<<"G"<<endl<<endl<<GPolygon<<endl<<endl;
		auto BPolygon=computeBPolygon(face,monomialPolygon);
		//		cout<<"B"<<endl<<endl<<BPolygon<<endl<<endl;
		auto DPolygon=computeDPolygon(monomialPolygon);
		//		cout<<"D"<<endl<<endl<<DPolygon<<endl<<endl;
		// inserire controllos G,B,D
		//			cout<<"test"<<endl<<BPolygon*DPolygon<<endl<<endl;
		if ((GPolygon-BPolygon*DPolygon).norm()>1e-13) {
			cout<<"GPolygon"<<endl<<endl<<GPolygon<<endl<<endl;
			cout<<"BPolygon"<<endl<<endl<<BPolygon<<endl<<endl;
			cout<<"DPolygon"<<endl<<endl<<DPolygon<<endl<<endl;
			
			cout<<"Failed"<<endl<<*face;
		}
		
		auto PIStarPolygon=computePIStarPolygon(GPolygon,BPolygon);
		//		cout<<"PIStar"<<endl<<endl<<PIStarPolygon<<endl<<endl;
		
		// ho ottenuto PIStar, ora devo integrare i monomi e moltiplicarli per il normalCoefficient
		real addTerm2=0;
		
		for (int i=1; i<4; i++) { // indice dei monomi, il reale monomio è quello di ordine i-1, questa i è la riga della matrice
			real normalCoefficient=monomial.gradient*normal[i-1]; // derivata normale del monomio, è la stessa per tutti
			real integralCoefficient=normalCoefficient*area; //aggiungo l'area
			
			
			
			// questa parte è corretta con i tetraedri ma non è esatta con figure più complesse
			
			
			///////////////////////////////
			// STRADA 1
			real addTerm1=integralCoefficient/facePoints;
			
			///////////////////////////////////////////////////
			
			
			// gli indici dei punti non sono gli stessi
			for (int m=0; m<facePoints; m++) {
				
				auto& point=face->point(m);
				addTerm2=0; // cambia a seconda del punto considerato
				
				addTerm2+=PIStarPolygon(0,m);
				addTerm2+=PIStarPolygon(1,m)*monomialPolygon.evaluate(face->getCentroid(),0);
				addTerm2+=PIStarPolygon(2,m)*monomialPolygon.evaluate(face->getCentroid(),1);
				
				addTerm2*=integralCoefficient;
				
				//				cout<<addTerm2<<"    "<<addTerm1<<endl;
				
				// ricerco l'indice giusto per inserirlo nella matrice
				for (int j=0;j<numberOfPoints;j++) {
					if (point==polyhedron->point(j)) {
						
						B(i,j)+=addTerm2;
						break;
					}
				}
			}
		}
	}
	return B;
}

// B polygon
template <typename real>
Matrix<real,3,Dynamic> SolverVEM3D<real>::computeBPolygon(const shared_ptr<Polygon<3,real>>& polygon,MonomialsPolygon<real>& monomial) {
	long numberOfPoints=polygon->numberOfPoints;
	Matrix<real,3,Dynamic> BPolygon(3,numberOfPoints);
	
	// nella prima riga della matrice si usa P0, nel nostro caso 1/numberOfPoints
	real firstLine=1.0/numberOfPoints;
	for (int i=0;i<numberOfPoints;i++) {
		BPolygon(0,i)=firstLine;
	}
	
	// inizializzo il resto della matrice a 0
	for (int i=1;i<3;i++) {
		for (int j=0;j<numberOfPoints;j++) {
			BPolygon(i,j)=0;
		}
	}
	
	// nelle altre uso un integrale al bordo
//	auto& pointVector=polygon->pointVector;
	for (int j=0;j<numberOfPoints;j++) { // j in questo caso è l'indice colonna della matrice
		auto edge=(*polygon->point(j+1))-(*polygon->point(j));
		auto edgeNormal=cross(edge,polygon->getNormal());
		edgeNormal=edgeNormal/edgeNormal.norm(); // lo normalizzo
		auto edgeLength=edge.norm();
		
		
		real addTermX=monomial.gradient*edgeNormal[monomial.indexX]*edgeLength/2;
		real addTermY=monomial.gradient*edgeNormal[monomial.indexY]*edgeLength/2;
		
		BPolygon(1,j)+=addTermX;
		BPolygon(2,j)+=addTermY;
		BPolygon(1,(j+1)%numberOfPoints)+=addTermX; // il % numberOfPoints per trattare il punto n+1
		BPolygon(2,(j+1)%numberOfPoints)+=addTermY;
	}
	
	return BPolygon;
}

// D polygon
template <typename real>
Matrix<real,Dynamic,3> SolverVEM3D<real>::computeDPolygon(MonomialsPolygon<real>& monomial) {
	long numberOfPoints=monomial.element->numberOfPoints;
//	auto& pointVector=monomial.element->pointVector;
	auto& element=monomial.element;
	Matrix<real,Dynamic,3> DPolygon(numberOfPoints,3);
	
	// prima colonna
	for (int j=0; j<numberOfPoints; j++) {
		DPolygon(j,0)=1;
	}
	
	// resto della matrice
	for (int i=0; i<numberOfPoints; i++) {
		for (int j=1;j<3;j++) {
			DPolygon(i,j)=monomial.evaluate(*element->point(i),j-1);
		}
	}
	
	return DPolygon;
}

// PIStar
template <typename real>
Matrix<real,3,Dynamic> SolverVEM3D<real>::computePIStarPolygon(Matrix<real,3,3>&G,Matrix<real,3,Dynamic>&B) {
	
	BiCGSTAB<Matrix<real,3,3>> solver;
	solver.compute(G);
	auto PIStarPolygon = solver.solve(B);
	
	return PIStarPolygon;
}












#endif
