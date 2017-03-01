#ifndef Dirichlet_h
#define Dirichlet_h

using namespace Eigen;

/** Class for Dirichlet boundary condition. Inherits from BoundaryCondition
 *
 *	It implements the methods of the parent class BoundaryCondition
 *
 *	\param embedded Dimension of the space
 *	\param MeshType Kind of Mesh we have
 *	\param MeshElement Polygon or Polyhedron
 *	\param real double or long double
 *
 *	Typedefs
 *		- <b>Dirichlet3D</b>, for 3D mesh
 *		- <b>Dirichlet2D</b>, for 2D mesh
 */
template <long embedded,typename MeshType,typename MeshElement,typename real=double>
class Dirichlet: public BoundaryCondition<embedded,MeshType,MeshElement,real> {
	private:
		long numberOfPoints;
		long numberOfBoundaryPoints;
		
	public:
		/** Standard constructor
		 */
		Dirichlet(const MeshType& mesh,muParserInterface<embedded,real>& boundaryFunction):BoundaryCondition<embedded,MeshType,MeshElement,real>::BoundaryCondition(mesh,boundaryFunction),numberOfPoints(mesh.numberOfPoints),numberOfBoundaryPoints(mesh.numberOfBoundaryPoints) {};
		
		/** Decide whether the computed Kloc will be added to the global matrix.
		 *
		 *	If the point belongs to the boundary points, it will not be added.
		 */
		virtual void addToTripletList(Matrix<real,Dynamic,Dynamic>& Kloc,MeshElement& element,vector<Triplet<real>>& tripletList);
		
		/** Changes the stiffnessMatrix to take into account the boundary condition.
		 *
		 *	The upper left part of the matrix is made diagonal.
		 */
		virtual void assignBoundaryConditionOnStiffnessMatrix(vector<Triplet<real>>& tripletList);
		
		/** Changes the known term to take into account the boundary condition.
		 *
		 *	The upper part of the known term is made to keep into account the boundary condition.
		 */
		virtual void assignBoundaryConditionOnKnownTerm(VectorX<real>& knownTerm);
};


// addToTripletList
template <long embedded,typename MeshType,typename MeshElement,typename real>
void Dirichlet<embedded,MeshType,MeshElement,real>::addToTripletList(Matrix<real,Dynamic,Dynamic>& Kloc,MeshElement& element,vector<Triplet<real>>& tripletList) {
	long pointID1=0;
	long pointID2=0;
	
	for (long i=0; i<element.numberOfPoints; i++) {
		pointID1=element.point(i)->getPointID();
		if (pointID1<numberOfBoundaryPoints) {
			continue;
		}
		for (long j=0; j<element.numberOfPoints; j++) {
			pointID2=element.point(j)->getPointID();
			tripletList.push_back(Triplet<real>((int)pointID1,(int)pointID2,Kloc(i,j)));
		}
		
	}
}

// assignBoundaryConditionOnStiffnessMatrix
template <long embedded,typename MeshType,typename MeshElement,typename real>
void Dirichlet<embedded,MeshType,MeshElement,real>::assignBoundaryConditionOnStiffnessMatrix(vector<Triplet<real>>& tripletList) {
	for (long i=0; i<numberOfBoundaryPoints; i++) {
		tripletList.push_back(Triplet<real>((int)i,(int)i,1));
	}
}

// assignBoundaryConditionOnKnownTerm
template <long embedded,typename MeshType,typename MeshElement,typename real>
void Dirichlet<embedded,MeshType,MeshElement,real>::assignBoundaryConditionOnKnownTerm(VectorX<real>& knownTerm) {
	for (long i=0; i<numberOfBoundaryPoints; i++) {
		knownTerm[i]=this->boundaryFunction(*this->
											mesh.point(i));
	}
}


template <typename real=double>
using Dirichlet3D=Dirichlet<3,Mesh3D<real>,Polyhedron<3,real>,real>;

template <typename real=double>
using Dirichlet2D=Dirichlet<2,Mesh2D<real>,Polygon<2,real>,real>;


#endif /* Dirichlet_h */
