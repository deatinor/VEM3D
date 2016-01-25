

#ifndef Monomials_Laplace_h
#define Monomials_Laplace_h

#include "Polyhedron.h"
#include <memory>

using namespace std;

/////////////////////////////////////////////////////////////////////
//////////                MONOMIALS                        //////////
/////////////////////////////////////////////////////////////////////

/** Class to evaluate the characteristics monomials involved in VEM
 */
template <long embedded,typename baseElement,typename real=double>
class Monomials {

public:

	const shared_ptr<baseElement>& element;	//!< Element can be Polygon or Polyhedron
	real diameter;
	Point<embedded,real> centroid;
	real gradient;

	// CONSTRUCTOR
	Monomials(const shared_ptr<baseElement>& figure): diameter(figure->getDiameter()),centroid(figure->getCentroid()),element(figure),gradient(1/diameter){}
	

	// STANDARD METHOD
	/** Function to evaluate the monomial in a point
	 *
	 *	\param point Point in which I want to evaluate the monomial
	 *	\param i Coordinate to keep into account (from 0 to 2 in a Polyhedron). 
	 */
	real evaluate (const Point<embedded,real>& p, long i); 
};

template <long embedded,typename baseElement,typename real>
real Monomials<embedded,baseElement,real>::evaluate(const Point<embedded,real>& point, long i){  // Mancherebbe il valore del monomio 1, ovvero la costante.. Per semplicità si può trattare a parte quando si riempie la matrice
	return (point[i]-centroid[i])/diameter;
}











#endif

