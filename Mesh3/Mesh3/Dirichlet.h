
#ifndef Dirichlet_h
#define Dirichlet_h


#include "Mesh.h"
#include "Mesh3D.h"
#include "Mesh2D.h"
#include "Laplace.h"
#include "BoundaryCondition.h"
#include "Dirichlet3D.h"


using namespace Eigen;

template <long embedded,typename MeshType,typename MeshElement,typename real=double>
class Dirichlet: public BoundaryCondition<embedded,MeshType,MeshElement,real> {
	long numberOfPoints;
	long numberOfBoundaryPoints;
	
	
public:
	Dirichlet(const MeshType& mesh,std::function<real(Point<embedded,real>&)> boundaryFunction):BoundaryCondition<embedded,MeshType,MeshElement,real>::BoundaryCondition(mesh,boundaryFunction),numberOfPoints(mesh.numberOfPoints),numberOfBoundaryPoints(mesh.numberOfBoundaryPoints) {};
	
	
	virtual void addToTripletList(Matrix<real,Dynamic,Dynamic>& Kloc,MeshElement& element,vector<Triplet<real>>& tripletList);
	virtual void assignBoundaryConditionOnStiffnessMatrix(vector<Triplet<real>>& tripletList);
	virtual void assignBoundaryConditionOnKnownTerm(VectorX<real>& knownTerm);
	
	
};


// addToTripletList
template <long embedded,typename MeshType,typename MeshElement,typename real>
void Dirichlet<embedded,MeshType,MeshElement,real>::addToTripletList(Matrix<real,Dynamic,Dynamic>& Kloc,MeshElement& element,vector<Triplet<real>>& tripletList) {
	long pointID1=0;
	long pointID2=0;
	
	//	auto& pointVector=element.pointVector;
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
