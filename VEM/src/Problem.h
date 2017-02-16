
#ifndef Problem_h
#define Problem_h


#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/IterativeLinearSolvers>
#include <memory>
#include "Error.h"



using namespace Eigen;

template <typename real>
using VectorX=Matrix<real, Dynamic, 1>;

/** Virtual class to represent a very generic problem.
 *
 *	The idea is to provide a general set of method common to a large class of numerical problem.
 *	The class is well suited for all the problems that can be solved by solving a linear system with a stiffness matrix and a known term.
 *	
 *	The child classes have to override the methods computeStiffnessMatrix() and computeKnownTerm()
 *
 *	It uses Eigen for storing all the matrixes and to solve the linear system
 *
 *	It also provide methods to display the error
 *
 *	\param embedded Dimension of the space
 *	\param MeshType Type of the file to read
 *	\param real double or long double
 */
template <long embedded,typename MeshType,typename real=double>
class Problem {
	
protected:
	const MeshType& mesh;	//!< Mesh on which the problem is based
	
public:
	SparseMatrix<real> stiffnessMatrix;
	VectorX<real> knownTerm;
	VectorX<real> solution;
	
	
	
public:
	/** General constructor for the Problem
	 *
	 *	It allocates the space for all the matrixes
	 */
	Problem(const MeshType& inputMesh):mesh(inputMesh),stiffnessMatrix((int)inputMesh.numberOfPoints,(int)inputMesh.numberOfPoints),knownTerm(inputMesh.numberOfPoints),solution(inputMesh.numberOfPoints) {
		// knownTerm initialized to 0
		for (long i=0; i<inputMesh.numberOfPoints; i++) {
			knownTerm[i]=0;
		}
		
		
	};
	
	virtual void computeStiffnessMatrix()=0; //!< Virtual method to compute the stiffness matrix
	virtual void computeKnownTerm()=0;	//!< Virtual method to compute the known term
	/** Method to compute solution. It solves the linear system.
	 *
	 *	It implements the biconjugate gradient stabilized method (BiCGSTAB). Override if you want to change it.
	 */
	virtual void computeSolution();
	virtual void operator()();			//!< Method to execute the method. FreeFem style.
	
	virtual void displayError(std::function<real(Point<embedded,real>&)> realSolutionFunction,string outputError="error.err",string errorAction="append"); //!<  It displays the error after the computation and prints it to a file
	virtual void write(string outputPoints="points.point",string outputConnections="connections.conn",string outputSolution="solution.sol");	//!< full output to file
	virtual void writeSolution(string outputSolution="solution.sol"); //!< Output to file of the solution
	
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
	cout<<endl<<endl<<"Assembling the matrix..."<<endl;
	startTime=clock();
	computeStiffnessMatrix();
	computeKnownTerm();
	duration = ( clock() - startTime ) / (double) CLOCKS_PER_SEC;
	cout<<"Completed: "<<duration<<endl<<endl;
	
	
	
	cout<<"Solving the problem..."<<endl;
	startTime=clock();
	computeSolution();
	duration = ( clock() - startTime ) / (double) CLOCKS_PER_SEC;
	cout<<"Completed: "<<duration<<endl<<endl;
	
	//cout<<"nonZeros: "<<stiffnessMatrix.nonZeros()<<endl;
	
}

// displayError
template <long embedded,typename MeshType,typename real>
void Problem<embedded,MeshType,real>::displayError(std::function<real(Point<embedded,real>&)> realSolutionFunction,string outputError,string errorAction) {
	Error<embedded,real> error(solution,mesh.getPointVector(),realSolutionFunction,stiffnessMatrix);
	
	error.displayError();
	error.writeError(outputError,errorAction);
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

#endif /* Problem_h */
