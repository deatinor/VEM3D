//
//  BoundaryCondition.h
//  Mesh3
//
//  Created by Stefano on 16/08/15.
//  Copyright (c) 2015 Stefano. All rights reserved.
//

#ifndef Mesh3_BoundaryCondition_h
#define Mesh3_BoundaryCondition_h

#include "Mesh.h"
#include "Mesh3D.h"
#include "Mesh2D.h"
#include "Laplace.h"
#include "Dirichlet3D.h"


using namespace Eigen;

template <typename real>
using VectorX=Matrix<real, Dynamic, 1>;


template <long embedded,typename MeshType,typename MeshElement,typename real=double>
class BoundaryCondition {
protected:
	const MeshType& mesh;
	std::function<real(Point<embedded,real>&)> boundaryFunction;
	
	BoundaryCondition(const MeshType& inputMesh,std::function<real(Point<embedded,real>&)> inputBoundaryFunction):mesh(inputMesh),boundaryFunction(inputBoundaryFunction) {};
public:
	
	virtual void addToTripletList(Matrix<real,Dynamic,Dynamic>& Kloc,MeshElement& element,vector<Triplet<real>>& tripletList)=0; // gli viene fornita la Kloc, decide se inserirla del tutto nella matrice o no
	// nel caso di Dirichlet condition la parte superiore della matrice è diagonale
	
	virtual void assignBoundaryConditionOnStiffnessMatrix(vector<Triplet<real>>& tripletList)=0;
	virtual void assignBoundaryConditionOnKnownTerm(VectorX<real>& knownTerm)=0; // sistema il termine noto a seconda del tipo di condizione
	
	
	
};

//,const vector<shared_ptr<MeshPoint<embedded,real>>>& pointVector


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




#endif
