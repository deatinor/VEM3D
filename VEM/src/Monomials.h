

#ifndef Monomials_Laplace_h
#define Monomials_Laplace_h

#include <memory>

using namespace std;

/////////////////////////////////////////////////////////////////////
//////////                MONOMIALS                        //////////
/////////////////////////////////////////////////////////////////////

/** Class to evaluate the characteristics monomials involved in VEM
 *
 *	It's appropriate for 2D and 3D case, but not for polygons embedded in 3D.
 */
template <long embedded,typename baseElement,typename real=double>
class Monomials {

public:

	const shared_ptr<baseElement>& element;	//!< Element can be Polygon or Polyhedron
	real diameter;  //!< Diameter of the element
	Point<embedded,real> centroid;  //!< Centroid of the element
	/** It's the gradient of the monomial. It's 1/diameter
	 *
	 *	It's a vector of elements that are all equals to 1/diameter, so it's sufficient to store it only one time.
	 */
	real gradient;

	// CONSTRUCTOR
	Monomials(const shared_ptr<baseElement>& figure): diameter(figure->getDiameter()),centroid(figure->getCentroid()),element(figure),gradient(1/diameter){}
	

	// STANDARD METHOD
	/** Function to evaluate the monomial in a point
	 *
	 *	\param point Point in which I want to evaluate the monomial
	 *	\param i Coordinate to keep into account (from 0 to 2 in a Polyhedron). 
	 *
	 *  \return the value of the component i of the monomial evaluated in p
	 *
	 *	No evaluation of the constant monomial, it can be made in an another part
	 */
	real evaluate (const Point<embedded,real>& p, long i); 
};

template <long embedded,typename baseElement,typename real>
real Monomials<embedded,baseElement,real>::evaluate(const Point<embedded,real>& point, long i){
	return (point[i]-centroid[i])/diameter;
}



template <typename real>
using Monomial3D=Monomials<3,Polyhedron<3,real>,real>;

template <typename real=double>
using Monomial2D=Monomials<2,Polygon<2,real>,real>;







#endif

