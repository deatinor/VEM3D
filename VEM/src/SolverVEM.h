
#ifndef SolverVEM_h
#define SolverVEM_h

#include <Eigen/Dense>
#include <Eigen/IterativeLinearSolvers>

using namespace Eigen;

/** Virtual basic class to solveVEM
 *
 *	The idea is to implement here all the methods common to 2D and 3D cases and then to specilize the remainings
 *
 *	The methods are mainly methods to compute the characteristics matrixes.
 *
 *	\param embedded Dimension of the space
 *	\param elementDimension dimension of each element (2 or 3)
 *	\param baseElement Polygon or Polyhedron
 *	\param MonomialType Monomials or MonomialPolygon. The last can be used in case of VEM on surfaces
 *	\param real double or long double
 */
template <long embedded,long elementDimension,typename baseElement,typename MonomialType,typename real=double>
class SolverVEM: public Solver<embedded,baseElement,Matrix<real,Dynamic,Dynamic>,real> {
protected:
	virtual Matrix<real,elementDimension+1,elementDimension+1> computeG(MonomialType& monomial);
	virtual Matrix<real,elementDimension+1,Dynamic> computeB(const shared_ptr<baseElement>& polyhedron,MonomialType& monomial)=0;
	virtual Matrix<real,Dynamic,elementDimension+1> computeD(MonomialType& monomial);
	
	virtual Matrix<real,elementDimension+1,Dynamic> computePIStar(Matrix<real,elementDimension+1,elementDimension+1>&G,Matrix<real,elementDimension+1,Dynamic>&B);
	virtual Matrix<real,Dynamic,Dynamic> computePI(Matrix<real,elementDimension+1,Dynamic>&PIStar,Matrix<real,Dynamic,elementDimension+1>&D);
	
public:
	/**	Basic constructor. A lot of paramethers are given as template.
	 *
	 *	\param inputForceTerm std::function as force term
	 */
	SolverVEM(std::function<real(const Point<embedded,real>&)> inputForceTerm):Solver<embedded,baseElement,Matrix<real, Dynamic, Dynamic>,real>::Solver(inputForceTerm) {};
	
	virtual Matrix<real,Dynamic,Dynamic> computeLocalK(const shared_ptr<baseElement>& element); //!< This is to compute the local stiffness matrix
	virtual real computeKnownTerm(const shared_ptr<baseElement>& element,const shared_ptr<MeshPoint<embedded,real>>& point)=0; //!< Virtual method to compute the known term.
};

// computeLocalK
template <long embedded,long elementDimension,typename baseElement,typename MonomialType,typename real>
Matrix<real,Dynamic,Dynamic> SolverVEM<embedded,elementDimension,baseElement,MonomialType,real>::computeLocalK(const shared_ptr<baseElement>& element) {
	
	
	MonomialType monomial(element);
	
	// computing the necessary matrixes
	
	//	cout<<"diameter: "<<monomial.Element->diameter()<<endl;
	//	cout<<"centroid: "<<monomial.Element->centroid<<endl;
	auto G=computeG(monomial);
	//		cout<<"G"<<endl<<G<<endl;
	auto B=computeB(element,monomial);
	//		cout<<"B"<<endl<<B<<endl;
	auto D=computeD(monomial);
	//		cout<<"D"<<endl<<D<<endl;
	//		cout<<"test: "<<endl<<B*D<<endl;
	
	//	cout<<(G-B*D).norm()<<endl;
	if ((G-B*D).norm()>1e-10) {
		cout<<"failed"<<endl<<G<<endl<<endl<<B*D<<endl;
		cout<<(G-B*D).norm()<<endl;
		
		cout<<*monomial.element<<endl;
	} else {
	}
	
	auto PIStar=computePIStar(G, B);
	//	cout<<"PIStar"<<endl<<PIStar<<endl;
	auto PI=computePI(PIStar,D);
	//	cout<<"PI"<<endl<<PI<<endl;
	
	
	// compute Kloc from the past matrixes
	for(int i=0; i<elementDimension+1; i++)
		G(0,i)=0;
	
	Matrix<real,Dynamic,Dynamic> I = Matrix<real,Dynamic,Dynamic>::Identity(PI.rows(),PI.cols());
	//	cout<<"I: "<<endl<<I<<endl;
	auto diameter=element->getDiameter();
	//	cout<<"diameter: "<<diameter<<endl;
	
	Matrix<real,Dynamic,Dynamic> Kloc= PIStar.transpose() * G * PIStar + diameter * ( I - PI ).transpose() * ( I - PI );
	//	cout<<"Kloc: "<<endl<<Kloc<<endl;
	
	return Kloc;
}

// G
template <long embedded,long elementDimension,typename baseElement,typename MonomialType,typename real>
Matrix<real,elementDimension+1,elementDimension+1> SolverVEM<embedded,elementDimension,baseElement,MonomialType,real>::computeG(MonomialType& monomial) {
	auto& element=monomial.element;
	
	real volume=monomial.element->space(); // volume if 3D, area if 2D
	
	Matrix<real,elementDimension+1,elementDimension+1> G;
	
	for (int i=0; i<elementDimension+1; i++) {
		for (int j=0;j<elementDimension+1;j++){
			G(i,j)=0;
		}
	}
	
	// first column
	G(0,0)=1;
	
	// first row (except first column)
	real value=0;
	for (int i=1; i<elementDimension+1; i++) {
		value=0;
		for (int k=0;k<element->numberOfPoints;k++) {
			value+=monomial.evaluate(*(element->point(k)),i-1);
		}
		value/=element->numberOfPoints;
		G(0,i)=value;
	}
	
	// diagonal
	for (int i=1;i<elementDimension+1;i++) {
		G(i,i)=monomial.gradient*monomial.gradient*volume;
	}
	
	return G;
}

// D
template <long embedded,long elementDimension,typename baseElement,typename MonomialType,typename real>
Matrix<real,Dynamic,elementDimension+1> SolverVEM<embedded,elementDimension,baseElement,MonomialType,real>::computeD(MonomialType& monomial) {
	
	long numberOfPoints=monomial.element->numberOfPoints;
	auto& element=monomial.element;
	Matrix<real,Dynamic,elementDimension+1> D(numberOfPoints,elementDimension+1);
	
	// first column
	for (int j=0; j<numberOfPoints; j++) {
		D(j,0)=1;
	}
	for (int i=0; i<numberOfPoints; i++) {
		for (int j=1;j<elementDimension+1;j++) {
			D(i,j)=0;
		}
	}
	
	// other terms of the matrix
	for (int i=0; i<numberOfPoints; i++) {
		for (int j=1;j<elementDimension+1;j++) {
			D(i,j)=monomial.evaluate(*element->point(i),j-1);
		}
	}
	
	return D;
}



// PIStar
template <long embedded,long elementDimension,typename baseElement,typename MonomialType,typename real>
Matrix<real,elementDimension+1,Dynamic> SolverVEM<embedded,elementDimension,baseElement,MonomialType,real>::computePIStar(Matrix<real, elementDimension + 1, elementDimension + 1> &G, Matrix<real, elementDimension + 1, Dynamic> &B) {
	
	BiCGSTAB<Matrix<real,elementDimension+1,elementDimension+1>> solver;
	solver.compute(G);
	auto PIStar = solver.solve(B);
	return PIStar;
}

// PI
template <long embedded,long elementDimension,typename baseElement,typename MonomialType,typename real>
Matrix<real,Dynamic,Dynamic> SolverVEM<embedded,elementDimension,baseElement,MonomialType,real>::computePI(Matrix<real, elementDimension + 1, Dynamic> &PIStar, Matrix<real, Dynamic, elementDimension + 1> &D) {
	auto PI = D * PIStar;
	return PI;
}













#endif /* SolverVEM_h */
