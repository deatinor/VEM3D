

#ifndef Monomials_Laplace_h
#define Monomials_Laplace_h

#include "Polyhedron.h"
#include <memory>

using namespace std;

/////////////////////////////////////////////////////////////////////
//////////                MONOMIALS                        //////////
/////////////////////////////////////////////////////////////////////

template <long embedded,typename baseElement,typename real=double>
class Monomials {

public:

	const shared_ptr<baseElement>& element;	// Il generico Element potrà essere un poligono o un poliedro
	real diameter;
	Point<embedded,real> centroid;
	real gradient;

	// CONSTRUCTOR
	Monomials(const shared_ptr<baseElement>& figure): diameter(figure->getDiameter()),centroid(figure->getCentroid()),element(figure),gradient(1/diameter){}
	

	// STANDARD METHOD
//	real evaluate (const shared_ptr<Point<embedded,real>> p, long i);
	real evaluate (const Point<embedded,real>& p, long i); // lascio solo questa per evitare conflitti

};

//template <long embedded,typename baseElement,typename real>		// La soluzione più bella sarebbe quella di usare function wrappers.. Quindi avere una sorta di vettore di funzioni
//real Monomials<embedded,baseElement,real>::evaluate(shared_ptr<Point<embedded,real>> p, long i){  // Mancherebbe il valore del monomio 1, ovvero la costante.. Per semplicità si può trattare a parte quando si riempie la matrice
//	return ((*p).coordinates[i]-centroid.coordinates[i])/diameter; 
//}

template <long embedded,typename baseElement,typename real>
real Monomials<embedded,baseElement,real>::evaluate(const Point<embedded,real>& p, long i){  // Mancherebbe il valore del monomio 1, ovvero la costante.. Per semplicità si può trattare a parte quando si riempie la matrice
	return (p[i]-centroid[i])/diameter;
}



///////////////////////////////////////////////////////////////////////
///////////           MONOMIALSPOLYGON					///////////////
///////////////////////////////////////////////////////////////////////

template <typename real>
class MonomialsPolygon: public Monomials<3, Polygon<3,real>,real> {
public:
	long indexX;
	long indexY;
	long indexZ;
	Vector<3,real> gradientX;
	Vector<3,real> gradientY;
	
	MonomialsPolygon(const shared_ptr<Polygon<3,real>>& figure);
//	real evaluate (const shared_ptr<Point<3,real>> p, long i);
	real evaluate (const Point<3,real>& p, long i);

};


template <typename real>
MonomialsPolygon<real>::MonomialsPolygon(const shared_ptr<Polygon<3,real>>& figure): Monomials<3, Polygon<3,real>,real>(figure),indexX(0),indexY(1) {
	// inizializzazione di indexX e indexY
	auto& normal=figure->getNormal();
	long indexZ=0;
	double maxNormalValue=0;
	for (int i=0;i<3;i++) {
		if (abs(normal[i])>maxNormalValue) {
			indexZ=i;
			maxNormalValue=abs(normal[i]);
		}
	}
	if (indexZ==0) {
		indexX=1;
		indexY=2;
	} else if (indexZ==1){
		indexY=2;
	}
	//	 calcola il gradiente
	
	
	
	gradientX[indexX]=(1.0-normal[indexX]*normal[indexX])/this->diameter;
	gradientX[indexY]=-normal[indexX]*normal[indexY]/this->diameter;
	gradientX[indexZ]=-normal[indexX]*normal[indexZ]/this->diameter;
	gradientY[indexX]=-normal[indexY]*normal[indexX]/this->diameter;
	gradientY[indexY]=(1-normal[indexY]*normal[indexY])/this->diameter;
	gradientY[indexZ]=-normal[indexY]*normal[indexZ]/this->diameter;
	
}

//template <typename real>
//real MonomialsPolygon<real>::evaluate(shared_ptr<Point<3,real>> p, long i){  // Mancherebbe il valore del monomio 1, ovvero la costante.. Per semplicità si può trattare a parte quando si riempie la matrice
//	// se i=0 ritorno indexX, se i=1 ritorno indexY
//	if (i==0){
//		return ((*p).coordinates[indexX]-this->centroid.coordinates[indexX])/this->diameter;
//	} else {
//		return ((*p).coordinates[indexY]-this->centroid.coordinates[indexY])/this->diameter;
//	}
//}

template <typename real>
real MonomialsPolygon<real>::evaluate(const Point<3,real>& p, long i){  // Mancherebbe il valore del monomio 1, ovvero la costante.. Per semplicità si può trattare a parte quando si riempie la matrice
	// se i=0 ritorno indexX, se i=1 ritorno indexY
	if (i==0){
		return (p[indexX]-this->centroid[indexX])/this->diameter;
	} else {
		return (p[indexY]-this->centroid[indexY])/this->diameter;
	}
}















#endif

