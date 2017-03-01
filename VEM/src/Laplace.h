#ifndef Mesh3_Laplace_h
#define Mesh3_Laplace_h

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/IterativeLinearSolvers>
#include <memory>
#include "muParserInterface.h"

using namespace Eigen;

template <long embedded,typename MeshType,typename real>
class Problem;


/**	Class to solve a Laplacian Problem in 2D or 3D
 *
 *	Subclass of Problem, well suited for Laplacian problems.
 *
 *	It is a general class, with no specified way to solve the problem. 
 *	The Solver and the BoundaryCondition are specified as template parameters.
 *	One can implement any kind of solver, the only restriction is that he has to create a subclass of the classes Solver and BoundaryCondition and pass them as template arguments.
 *	A possible extension would be to create a FEM solver without varying this class.
 *
 *	\param embedded Dimension of the space
 *	\param MeshType Type of the mesh file to read
 *	\param SolverType Kind of Solver to use. Any subclass of Solver is allowed.
 *	\param BoundaryConditionType Kind of BoundaryCondition to use. Any subclass of BoundaryCondition is allowed.
 *	\param real double or long double
 */
template <long embedded,typename MeshType,typename SolverType,typename BoundaryConditionType,typename real=double>
class Laplace: public Problem<embedded,MeshType,real> {
	private:
		// PRIVATE PROPERTIES
		real diffusionCoeff;	//!< Coefficient of diffusion of the laplacian problem.

	public:
		// PROPERTIES
		vector<Triplet<real>> tripletList; //!< Vector used to fast build the stiffnessMatrix. Triplet is a class used by Eigen library.
		long numberOfElements;
		BoundaryConditionType boundaryCondition;
		SolverType solver;
		
		// CONSTRUCTORS
		/** Standard constructor
		 *
		 * \param inputMesh Mesh on which the problem is defined
		 * \param inputForceTerm muParserInterface containing the expression of the ForceTerm
		 * \param inputBoundaryFunction muParserInterface containing the expression of the boundary conditions
		 */
		Laplace(const MeshType& inputMesh,muParserInterface<embedded,real>& inputForceTerm,muParserInterface<embedded,real>& inputBoundaryFunction,real inputDiffusionCoeff=1):Problem<embedded,MeshType,real>(inputMesh),numberOfElements(inputMesh.numberOfElements),tripletList({}),boundaryCondition(inputMesh,inputBoundaryFunction),solver(inputForceTerm),diffusionCoeff(inputDiffusionCoeff) {}
		
		// STANDARD METHODS
		void computeStiffnessMatrix(); //!< General method. It invokes the methods of the Solver.
		void computeKnownTerm(); //!< General method. It invokes the methods of the Solver and BoundaryCondition.
};

// stiffnessMatrix
template <long embedded,typename MeshType,typename SolverType,typename BoundaryConditionType,typename real>
void Laplace<embedded,MeshType,SolverType,BoundaryConditionType,real>::computeStiffnessMatrix() {
	auto& mesh=this->mesh;
	
	// for each Polyhedron it calls the method of the Solver
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
	// for each Polyhedron
	for (int i=0;i<numberOfElements;i++) {
		auto& element=*mesh.element(i);
		
		// for each point ID it adds the integral to the known term
		for (int j=0;j<element.numberOfPoints;j++) {
			long pointID=element.point(j)->getPointID();
			real addTerm=solver.computeKnownTerm(mesh.element(i),element.point(j));
			this->knownTerm[pointID]+=addTerm;
		}
	
	}
	this->boundaryCondition.assignBoundaryConditionOnKnownTerm(this->knownTerm);
}

#endif /* Mesh3_Laplace_h */
