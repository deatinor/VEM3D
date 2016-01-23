//
//  Laplace.h
//  Mesh3
//
//  Created by Stefano on 06/08/15.
//  Copyright (c) 2015 Stefano. All rights reserved.
//

#ifndef Mesh3_Laplace_h
#define Mesh3_Laplace_h


#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/IterativeLinearSolvers>
#include <memory>
#include "Mesh.h"
#include "Monomials.h"
#include "Solver.h"
#include "BoundaryCondition.h"
#include "Error.h"
#include "Dirichlet3D.h"


//#include <Eigen/SparseCholesky>
//#include <Eigen/SparseLU>
//#include <Eigen/UmfPackSupport>


using namespace Eigen;

template <typename real>
using VectorX=Matrix<real, Dynamic, 1>;


template <long embedded,typename MeshType,typename real=double>
class Problem {
	
protected:
	const MeshType& mesh;
	
public:
	SparseMatrix<real> stiffnessMatrix;
	VectorX<real> knownTerm;
	VectorX<real> solution;
	
	real diffusionCoeff;
	
//	BoundaryCondition<embedded, MeshType, MeshElement,real>& boundaryCondition;
	
public:
	Problem(const MeshType& inputMesh,real inputDiffusionCoeff=1):mesh(inputMesh),stiffnessMatrix((int)inputMesh.numberOfPoints,(int)inputMesh.numberOfPoints),knownTerm(inputMesh.numberOfPoints),solution(inputMesh.numberOfPoints),diffusionCoeff(inputDiffusionCoeff){
		// inizializzo lo knownTerm a 0
		for (long i=0; i<inputMesh.numberOfPoints; i++) {
			knownTerm[i]=0;
		}
		
		
	};
	
	virtual void computeStiffnessMatrix()=0;
	virtual void computeKnownTerm()=0;
	virtual void computeSolution();
	virtual void operator()();
	
	virtual void displayError(std::function<real(Point<embedded,real>&)> realSolutionFunction);
	virtual void write(string outputPoints="points.point",string outputConnections="connections.conn",string outputSolution="solution.sol");
	virtual void writeSolution(string outputSolution="solution.sol");
	
};

// computeSolution
template <long embedded,typename MeshType,typename real>
void Problem<embedded,MeshType,real>::computeSolution() {
	
	BiCGSTAB<SparseMatrix<real>> solver;
	solution=solver.compute(stiffnessMatrix).solve(knownTerm);
	
}

// operator()
template <long embedded,typename MeshType,typename real>
void Problem<embedded,MeshType,real>::operator()() {
	
	clock_t startTime;
	double duration;
	cout<<endl<<endl<<"Assembling..."<<endl;
	startTime=clock();
	computeStiffnessMatrix();
	computeKnownTerm();
	duration = ( clock() - startTime ) / (double) CLOCKS_PER_SEC;
	cout<<"Completed: "<<duration<<endl<<endl;
	
	
	
	cout<<"Solving..."<<endl;
	startTime=clock();
	computeSolution();
	duration = ( clock() - startTime ) / (double) CLOCKS_PER_SEC;
	cout<<"Completed: "<<duration<<endl<<endl;
	
	cout<<"nonZeros: "<<stiffnessMatrix.nonZeros()<<endl;

}

// showError
template <long embedded,typename MeshType,typename real>
void Problem<embedded,MeshType,real>::displayError(std::function<real(Point<embedded,real>&)> realSolutionFunction) {
	Error<embedded,real> error(solution,mesh.getPointVector(),realSolutionFunction,stiffnessMatrix);
	error.displayError();
}

// write
template <long embedded,typename MeshType,typename real>
void Problem<embedded,MeshType,real>::write(string outputPoints,string outputConnections,string outputSolution) {
	mesh.write(outputPoints,outputConnections);
	writeSolution(outputSolution);
}

// writeSolution
template <long embedded,typename MeshType,typename real>
void Problem<embedded,MeshType,real>::writeSolution(string outputSolution) {
	ofstream file;
	file.open(outputSolution);
	for (int i=0; i<solution.size(); i++) {
		file<<solution[i];
		file<<"\n";
	}
	file.close();
}


//////////////////////////////////////////////////////////////////////////////////////////
/////////////					       Laplace							//////////////
//////////////////////////////////////////////////////////////////////////////////////////

template <typename real>
using Monomial3D=Monomials<3,Polyhedron<3,real>,real>;


template <long embedded,typename MeshType,typename SolverType,typename BoundaryConditionType,typename real=double>
class Laplace: public Problem<embedded,MeshType,real> {
private:
	// PRIVATE PROPERTIES

	
	
	
public:
	// PROPERTIES
	vector<Triplet<real>> tripletList;
	long numberOfElements;
	BoundaryConditionType boundaryCondition;
	SolverType solver;
	
	// CONSTRUCTORS
	Laplace(const MeshType& inputMesh,std::function<real(Point<embedded,real>&)> inputForceTerm,std::function<real(Point<embedded,real>&)> inputBoundaryFunction,real inputDiffusionCoeff=1):Problem<embedded,MeshType,real>(inputMesh,inputDiffusionCoeff),numberOfElements(inputMesh.numberOfElements),tripletList({}),boundaryCondition(inputMesh,inputBoundaryFunction),solver(inputForceTerm) {}
	
	// STANDARD METHODS
	void computeStiffnessMatrix(); // fatta, da controllare
	void computeKnownTerm(); // fatta, da controllare

	
};

// stiffnessMatrix
template <long embedded,typename MeshType,typename SolverType,typename BoundaryConditionType,typename real>
void Laplace<embedded,MeshType,SolverType,BoundaryConditionType,real>::computeStiffnessMatrix() {
	auto& mesh=this->mesh;
	
	// gira tutti i poliedri
	for (int i=0;i<numberOfElements;i++) {

		Matrix<real,Dynamic,Dynamic> Kloc=solver.computeLocalK(mesh.element(i));
		
		this->boundaryCondition.addToTripletList(Kloc,*mesh.element(i),tripletList);
		
	}
	this->boundaryCondition.assignBoundaryConditionOnStiffnessMatrix(tripletList);
	
	this->stiffnessMatrix.setFromTriplets(tripletList.begin(),tripletList.end());

}

// knownTerm
template <long embedded,typename MeshType,typename SolverType,typename BoundaryConditionType,typename real>
void Laplace<embedded,MeshType,SolverType,BoundaryConditionType,real>::computeKnownTerm() {
	
	auto& mesh=this->mesh;
	// gira tutti i poliedri
	for (int i=0;i<numberOfElements;i++) {
		auto& element=*mesh.element(i);
		
		
		// gira gli ID dei punti per aggiungere l'integrale al termine noto
//		auto& pointVector=polyhedron.pointVector;
		for (int j=0;j<element.numberOfPoints;j++) {
			long pointID=element.point(j)->getPointID();
			real addTerm=solver.computeKnownTerm(mesh.element(i),element.point(j));

			this->knownTerm[pointID]+=addTerm;//(volume*fValue*vhCoefficient);
		}
	
	}
	this->boundaryCondition.assignBoundaryConditionOnKnownTerm(this->knownTerm);
}
















#endif
