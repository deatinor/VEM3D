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

template <long embedded,typename real=double>
class Error {
protected:
	const VectorX<real>& solution;
	VectorX<real> realSolution;
	VectorX<real> difference;
	const vector<shared_ptr<MeshPoint<embedded,real>>>& pointVector;
	std::function<real(Point<embedded,real>&)> realSolutionFunction;
	
	SparseMatrix<real>& stiffnessMatrix;
	
	void computeRealSolution();
	
public:
	Error(const VectorX<real>& inputSolution,const vector<shared_ptr<MeshPoint<embedded,real>>>& inputPointVector,std::function<real(Point<embedded,real>&)> inputRealSolutionFunction,SparseMatrix<real>& inputStiffnessMatrix);
	
	real LInfinity();
	real H1Discrete();
	
	void displayError();
	
};


// COSTRUTTORE
template <long embedded,typename real>
Error<embedded,real>::Error(const VectorX<real>& inputSolution,const vector<shared_ptr<MeshPoint<embedded,real>>>& inputPointVector,std::function<real(Point<embedded,real>&)> inputRealSolutionFunction,SparseMatrix<real>& inputStiffnessMatrix):solution(inputSolution),pointVector(inputPointVector),realSolutionFunction(inputRealSolutionFunction),stiffnessMatrix(inputStiffnessMatrix),realSolution(solution.size()),difference(solution.size()) {
	computeRealSolution();
	
	
	
	// non c'entra
//	vector<real> diffVector({});
//	for (int i=0; i<pointVector.size(); i++) {
//		diffVector.push_back(difference[i]);
////		cout<<diffVector[i]<<"   "<<difference[i]<<endl;
//		
//	}
//	
//	sort(diffVector.begin(),diffVector.end());
//	for (int i=0;i<pointVector.size();i++) {
//		cout<<diffVector[i]<<endl;
//	}
}


// METHODS
template <long embedded,typename real>
void Error<embedded,real>::computeRealSolution() {
	for (int i=0; i<pointVector.size(); i++) {
		realSolution[i]=realSolutionFunction(*pointVector[i]);
		
//		cout<<*pointVector[i];
//		cout<<solution[i]<<"   "<<realSolution[i]<<"   "<<pointVector[i]->getIsBoundary()<<endl;
		
		
		difference[i]=abs(realSolution[i]-solution[i]);

		cout<<*pointVector[i]<<solution[i]<<"   "<<realSolution[i]<<"   "<<difference[i]<<endl;
		
		

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
	cout<<endl<<endl<<"H1 discete norm: "<<H1Discrete()<<endl<<endl;
}











#endif
