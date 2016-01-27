
#ifndef MonomialsPolygon_h
#define MonomialsPolygon_h

#include "Polyhedron.h"
#include <memory>

using namespace std;

///////////////////////////////////////////////////////////////////////
///////////           MONOMIALSPOLYGON					///////////////
///////////////////////////////////////////////////////////////////////

/** Class similar to Monomial, but used to compute the similar quantities for a Polygon embedded in 3D
 */
template <typename real>
class MonomialsPolygon: public Monomials<3, Polygon<3,real>,real> {
public:
	long indexX;
	long indexY;
	long indexZ;
	Vector<3,real> gradientX;
	Vector<3,real> gradientY;
	
	MonomialsPolygon(const shared_ptr<Polygon<3,real>>& figure); //!< Constructor from Polygon

	// STANDARD METHOD
	/** Function to evaluate the monomial in a point
	 *
	 *	\param point Point in which I want to evaluate the monomial
	 *	\param i Coordinate to keep into account (from 0 to 1 in a Polygon).
	 */
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
