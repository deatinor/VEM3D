//
//  SolverVEM2D.h
//  Mesh3
//
//  Created by Stefano on 17/08/15.
//  Copyright (c) 2015 Stefano. All rights reserved.
//

#ifndef Mesh3_SolverVEM2D_h
#define Mesh3_SolverVEM2D_h

template <typename real=double>
using Monomial2D=Monomials<2,Polygon<2,real>,real>;

template <typename real=double>
class SolverVEM2D: public SolverVEM<2,2,Polygon<2,real>,Monomial2D<real>,real> {
public:
//	virtual Matrix<real,3,3> computeG(Monomial2D<real>& monomial);  // fatta
	virtual Matrix<real,3,Dynamic> computeB(const shared_ptr<Polygon<2,real>>& polygon,Monomial2D<real>& monomial); // sistemare l'integrale al bordo
//	virtual Matrix<real,Dynamic,3> computeD(Monomial2D<real>& monomial); // fatta
	
	
//	virtual Matrix<real,3,Dynamic> computePIStar(Matrix<real,3,3>&G,Matrix<real,3,Dynamic>&B); // fatta
//	virtual Matrix<real,Dynamic,Dynamic> computePI(Matrix<real,3,Dynamic>&PIStar,Matrix<real,Dynamic,3>&D); // fatta
	
	
public:
	SolverVEM2D(std::function<real(Point<2,real>&)> inputForceTerm):SolverVEM<2,2,Polygon<2,real>,Monomial2D<real>,real>::SolverVEM(inputForceTerm) {};
	
	virtual real computeKnownTerm(const shared_ptr<Polygon<2,real>>& element,const shared_ptr<MeshPoint<2,real>>& point);
	
};


template <typename real>
Matrix<real,3,Dynamic> SolverVEM2D<real>::computeB(const shared_ptr<Polygon<2,real>>& polygon,Monomial2D<real>& monomial){
	long numberOfPoints=polygon->numberOfPoints;
	Matrix<real,3,Dynamic> B(3,numberOfPoints);
	
	// prima riga
	for (int j=0; j<numberOfPoints; j++) {
		B(0,j)=1.0/numberOfPoints;
	}
	
	// inizializzo il resto a 0
	for (int i=1; i<3; i++) {
		for (int j=0; j<numberOfPoints; j++) {
			B(i,j)=0;
		}
	}
	
	// calcolo l'integrale al bordo per gli altri termini
//	auto& pointVector=polygon->pointVector;
	real gradient=monomial.gradient;
	for (int j=0; j<numberOfPoints; j++) {
		// considero i due lati connessi al grado di libertà j
		auto edge1=(*polygon->point(j))-(*polygon->point(j-1+numberOfPoints));
		auto edge2=(*polygon->point(j+1))-(*polygon->point(j));
		
		Point<2,real> normalEdge1(edge1.y(),-edge1.x());
		Point<2,real> normalEdge2(edge2.y(),-edge2.x());
		// normalizzo
		normalEdge1=normalEdge1/normalEdge1.norm();
		normalEdge2=normalEdge2/normalEdge2.norm();
		
//		cout<<"point: "<<*pointVector[j]<<endl;
//		cout<<"normal1: "<<normalEdge1<<endl;
//		cout<<"normal2: "<<normalEdge2<<endl;
		
		real edge1Norm=edge1.norm();
		real edge2Norm=edge2.norm();
		
		B(1,j)=gradient*(normalEdge1.x()*edge1Norm+normalEdge2.x()*edge2Norm)/2;
		B(2,j)=gradient*(normalEdge1.y()*edge1Norm+normalEdge2.y()*edge2Norm)/2;
	}
	return B;
}

template <typename real>
real SolverVEM2D<real>::computeKnownTerm(const shared_ptr<Polygon<2,real>>& element,const shared_ptr<MeshPoint<2,real>>& point) {
	real fValue=this->forceTerm(*element->point(0)); // calcola la f nel primo punto di ogni poliedro, credo che basti essendo piecewise constant
	real vhCoefficient=1.0/(element->numberOfPoints); // e' il P0 di ogni vh, sempre lo stesso per ogni vh
	real area=element->getArea(); // lo salva per non dovervi accedere per ogni punto
	return area*vhCoefficient*fValue;
}




#endif
