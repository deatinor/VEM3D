
#ifndef Mesh3_SolverVEM2D_h
#define Mesh3_SolverVEM2D_h

template <typename real=double>
using Monomial2D=Monomials<2,Polygon<2,real>,real>;

/** Specilized class to solve VEM in 2D
 *
 *	It only implements computeB and computeKnownTerm
 */
template <typename real=double>
class SolverVEM2D: public SolverVEM<2,2,Polygon<2,real>,Monomial2D<real>,real> {
public:
	virtual Matrix<real,3,Dynamic> computeB(const shared_ptr<Polygon<2,real>>& polygon,Monomial2D<real>& monomial);

public:
	/** Standard constructor
	 */
	SolverVEM2D(std::function<real(const Point<2,real>&)> inputForceTerm):SolverVEM<2,2,Polygon<2,real>,Monomial2D<real>,real>::SolverVEM(inputForceTerm) {};
	
	/** Computes the known term of the element
	 *
	 *	Used a second order formula: we computes the f in the centroid
	 */
	virtual real computeKnownTerm(const shared_ptr<Polygon<2,real>>& element,const shared_ptr<MeshPoint<2,real>>& point);
	
};


template <typename real>
Matrix<real,3,Dynamic> SolverVEM2D<real>::computeB(const shared_ptr<Polygon<2,real>>& polygon,Monomial2D<real>& monomial){
	long numberOfPoints=polygon->numberOfPoints;
	Matrix<real,3,Dynamic> B(3,numberOfPoints);
	
	// first row
	for (int j=0; j<numberOfPoints; j++) {
		B(0,j)=1.0/numberOfPoints;
	}
	
	// inizialize the others to 0
	for (int i=1; i<3; i++) {
		for (int j=0; j<numberOfPoints; j++) {
			B(i,j)=0;
		}
	}
	
	// computing the boundary integral for the other terms
	real gradient=monomial.gradient;
	for (int j=0; j<numberOfPoints; j++) {
		// consider the 2 edges connected to the degree of freedom j
		auto edge1=(*polygon->point(j))-(*polygon->point(j-1+numberOfPoints));
		auto edge2=(*polygon->point(j+1))-(*polygon->point(j));
		
		Point<2,real> normalEdge1(edge1.y(),-edge1.x());
		Point<2,real> normalEdge2(edge2.y(),-edge2.x());
		
		// normalizing
		normalEdge1=normalEdge1/normalEdge1.norm();
		normalEdge2=normalEdge2/normalEdge2.norm();
		
		real edge1Norm=edge1.norm();
		real edge2Norm=edge2.norm();
		
		B(1,j)=gradient*(normalEdge1.x()*edge1Norm+normalEdge2.x()*edge2Norm)/2;
		B(2,j)=gradient*(normalEdge1.y()*edge1Norm+normalEdge2.y()*edge2Norm)/2;
	}
	return B;
}

template <typename real>
real SolverVEM2D<real>::computeKnownTerm(const shared_ptr<Polygon<2,real>>& element,const shared_ptr<MeshPoint<2,real>>& point) {
	real fValue=this->forceTerm(element->getCentroid()); // computing f in the centroid of each polyhedron
	
	real vhCoefficient=1.0/(element->numberOfPoints); // this is P0 of each vh, always the same for every vh
	real area=element->getArea(); // saved to not access to it for every point
	return area*vhCoefficient*fValue;
}




#endif
