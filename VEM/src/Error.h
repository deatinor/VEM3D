//
//  Error.h
//  Mesh3
//
//  Created by Stefano on 17/08/15.
//  Copyright (c) 2015 Stefano. All rights reserved.
//

#ifndef Mesh3_Error_h
#define Mesh3_Error_h

#include <Eigen/Dense>

using namespace Eigen;

template <typename real=double>
using VectorX=Matrix<real, Dynamic, 1>;

/** Class to compute and display the error
 *
 *	It computes the values only in the points
 *	2 error computed
 *		- L infinite norm (maximum of the difference with absolute value in the points)
 *		- H1 discrete norm (u^T * K * u)
 */
template <long embedded,typename real=double>
class Error {
protected:
	const VectorX<real>& solution;
	VectorX<real> realSolution;
	VectorX<real> difference;
	const vector<shared_ptr<MeshPoint<embedded,real>>>& pointVector;
	std::function<real(Point<embedded,real>&)> realSolutionFunction;
	
	SparseMatrix<real>& stiffnessMatrix;
	
	void computeRealSolution(); //!< This computes the exact solution, from realSolutionFunction
	
public:
	/** Standard constructor
	 *
	 *	\param inputSolution solution computed
	 *	\param inputPointVector pointVector used before
	 *	\param inputRealSolutionFunction function that expresses the real solution
	 *	\param inputStiffnessMatrix used for H1 discrete norm
	 */
	Error(const VectorX<real>& inputSolution,const vector<shared_ptr<MeshPoint<embedded,real>>>& inputPointVector,std::function<real(Point<embedded,real>&)> inputRealSolutionFunction,SparseMatrix<real>& inputStiffnessMatrix);
	
	/** L infinite norm of the error
	 *
	 *	Maximum of the difference between points
	 */
	real LInfinity();
	
	/** H1 discrete norm of the error
	 *
	 *	u^T * K * u where u is the solution and K is the stiffness matrix
	 */
	real H1Discrete();
	
	void displayError();	//!< Print the computer error
	/** Print the error to a file
	 * 
	 * \param outputError output file
	 * \param errorAction string that can be either "append" or "override", if we want to append the file or override it with the new computed errors
	 */
	void writeError(string outputError,string errorAction);
	
};


// COSTRUTTORE
template <long embedded,typename real>
Error<embedded,real>::Error(const VectorX<real>& inputSolution,const vector<shared_ptr<MeshPoint<embedded,real>>>& inputPointVector,std::function<real(Point<embedded,real>&)> inputRealSolutionFunction,SparseMatrix<real>& inputStiffnessMatrix):solution(inputSolution),pointVector(inputPointVector),realSolutionFunction(inputRealSolutionFunction),stiffnessMatrix(inputStiffnessMatrix),realSolution(solution.size()),difference(solution.size()) {
	computeRealSolution();
	
}


// METHODS
template <long embedded,typename real>
void Error<embedded,real>::computeRealSolution() {
	for (int i=0; i<pointVector.size(); i++) {
		realSolution[i]=realSolutionFunction(*pointVector[i]);
		
//		cout<<*pointVector[i];
//		cout<<solution[i]<<"   "<<realSolution[i]<<"   "<<pointVector[i]->getIsBoundary()<<endl;
		
		
		difference[i]=abs(realSolution[i]-solution[i]);

		//cout<<*pointVector[i]<<solution[i]<<"   "<<realSolution[i]<<"   "<<difference[i]<<endl;
		
		

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
	cout<<endl<<"LInfinity norm: "<<LInfinity();
	cout<<endl<<endl<<"H1 discrete norm: "<<H1Discrete()<<endl<<endl;
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









#endif
