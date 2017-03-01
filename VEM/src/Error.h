#ifndef Mesh3_Error_h
#define Mesh3_Error_h

#include <Eigen/Dense>
#include "muParserInterface.h"

using namespace Eigen;

template <typename real=double>
using VectorX=Matrix<real, Dynamic, 1>;

/** Class to compute and display the error
 *
 *	It computes the values only in the mesh points
 *	2 errors are computed:
 *		- L-infinite norm (maximum of the difference with absolute value in the mesh points)
 *		- H1 discrete norm (u^T * K * u)
 */
template <long embedded,typename real=double>
class Error {
	protected:
		const VectorX<real>& solution;
		VectorX<real> realSolution;
		VectorX<real> difference;
		const vector<shared_ptr<MeshPoint<embedded,real>>>& pointVector;
		muParserInterface<embedded,real>& realSolutionFunction;
		
		SparseMatrix<real>& stiffnessMatrix;
		
		void computeRealSolution(); //!< This computes the exact solution, from realSolutionFunction
		
	public:
		/** Standard constructor
		 *
		 *	\param inputSolution Computed solution
		 *	\param inputPointVector pointVector previously used
		 *	\param inputRealSolutionFunction muParserInterface that contains the expression of the real solution
		 *	\param inputStiffnessMatrix Needed for the H1 discrete norm
		 */
		Error(const VectorX<real>& inputSolution,const vector<shared_ptr<MeshPoint<embedded,real>>>& inputPointVector,muParserInterface<embedded,real>& inputRealSolutionFunction,SparseMatrix<real>& inputStiffnessMatrix);
		
		/** L-infinite norm of the error
		 *
		 *	Maximum of the difference (in absolute value) between mesh points
		 */
		real LInfinity();
		
		/** H1 discrete norm of the error
		 *
		 *	u^T * K * u where u is the solution and K is the stiffness matrix
		 */
		real H1Discrete();
		
		void displayError();	//!< Print the computed error
		
		/** Print the error to a file
		 * 
		 * \param outputError Output file
		 * \param errorAction String that can be either "append" or "override", if we want to append the file or override it with the new computed errors
		 */
		void writeError(string outputError,string errorAction);
};


// CONSTRUCTOR
template <long embedded,typename real>
Error<embedded,real>::Error(const VectorX<real>& inputSolution,const vector<shared_ptr<MeshPoint<embedded,real>>>& inputPointVector,muParserInterface<embedded,real>& inputRealSolutionFunction,SparseMatrix<real>& inputStiffnessMatrix):solution(inputSolution),pointVector(inputPointVector),realSolutionFunction(inputRealSolutionFunction),stiffnessMatrix(inputStiffnessMatrix),realSolution(solution.size()),difference(solution.size()) {
	computeRealSolution();
}


// METHODS
template <long embedded,typename real>
void Error<embedded,real>::computeRealSolution() {
	for (int i=0; i<pointVector.size(); i++) {
		realSolution[i]=realSolutionFunction(*pointVector[i]);
		difference[i]=abs(realSolution[i]-solution[i]);
	}
}

template <long embedded,typename real>
real Error<embedded,real>::LInfinity() {
	return difference.maxCoeff();
}

template <long embedded,typename real>
real Error<embedded,real>::H1Discrete() {
	real error=difference.transpose()*stiffnessMatrix*difference;
	return sqrt(error);
	
}

template <long embedded,typename real>
void Error<embedded,real>::displayError() {
	cout << endl << "LInfinity norm: " << LInfinity();
	cout << endl << endl << "H1 discrete norm: " << H1Discrete() << endl << endl;
}

template <long embedded,typename real>
void Error<embedded,real>::writeError(string outputError,string errorAction) {
	std::ofstream outfile;
 
 	if (errorAction == "override")
 		outfile.open(outputError);
 	else {
 		if (errorAction != "append") {
 			std::cout << "The error is written is the file " << outputError << " (append, no override)." << std::endl;
 		}
 		outfile.open(outputError, std::ios_base::app);
 	}

	outfile << LInfinity() << "," << H1Discrete() << endl; 
	outfile.close(); 
}

#endif /* Mesh3_Error_h */
