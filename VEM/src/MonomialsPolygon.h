#ifndef MonomialsPolygon_h
#define MonomialsPolygon_h

#include <memory>

using namespace std;

/** Class that inherits from Monomial, used to compute the similar quantities for a Polygon embedded in 3D
 *
 *  This is used to compute the VEM on the surfaces
 */
template <typename real>
class MonomialsPolygon: public Monomials<3, Polygon<3,real>,real> {
	public:
		// these are virtual indices used to project the Polygon on an appropriate plane
		long indexX;
		long indexY;
		long indexZ;
		Vector<3,real> gradientX;
		Vector<3,real> gradientY;
		
		MonomialsPolygon(const shared_ptr<Polygon<3,real>>& figure); //!< Constructor from Polygon

		// STANDARD METHOD
		/** Function to evaluate the monomial in a point
		 *
		 *	\param point Point in which one want to evaluate the monomial
		 *	\param i Coordinate to take into account (from 0 to 1 in a Polygon).
		 */
		real evaluate (const Point<3,real>& p, long i);
};

template <typename real>
MonomialsPolygon<real>::MonomialsPolygon(const shared_ptr<Polygon<3,real>>& figure): Monomials<3, Polygon<3,real>,real>(figure),indexX(0),indexY(1) {
	// Initialization of indexX and indexY
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
	// Compute the gradient
	gradientX[indexX]=(1.0-normal[indexX]*normal[indexX])/this->diameter;
	gradientX[indexY]=-normal[indexX]*normal[indexY]/this->diameter;
	gradientX[indexZ]=-normal[indexX]*normal[indexZ]/this->diameter;
	gradientY[indexX]=-normal[indexY]*normal[indexX]/this->diameter;
	gradientY[indexY]=(1-normal[indexY]*normal[indexY])/this->diameter;
	gradientY[indexZ]=-normal[indexY]*normal[indexZ]/this->diameter;
	
}

template <typename real>
real MonomialsPolygon<real>::evaluate(const Point<3,real>& p, long i){ //The value of the unit monomial, or a constant monomial is not taken into account here. For a sake of simplicity, these cases can be considered when building the matrix.
	// if i=0, return indexX; if i=1 return indexY
	if (i==0){
		return (p[indexX]-this->centroid[indexX])/this->diameter;
	} else {
		return (p[indexY]-this->centroid[indexY])/this->diameter;
	}
}

#endif /* MonomialsPolygon_h */
