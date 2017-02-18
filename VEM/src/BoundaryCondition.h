//
//  BoundaryCondition.h
//  Mesh3
//
//  Created by Stefano on 16/08/15.
//  Copyright (c) 2015 Stefano. All rights reserved.
//

#ifndef Mesh3_BoundaryCondition_h
#define Mesh3_BoundaryCondition_h

using namespace Eigen;

template <typename real>
using VectorX=Matrix<real, Dynamic, 1>;

/** Abstract class for BoundaryCondition
 *
 *  This class does 3 main things:
 *	- It processes the Kloc matrix after the creation from Solver (e.g in Dirichlet condition the upper part of the stiffness matrix is diagonal and there is no need to add these terms)
 *	- Once the K matrix is completed it processes the full matrix to assign boundary condition (e.g in the same example it makes the upper part diagonal)
 *	- It imposes boundary condition also on the known term
 *
 *	\param embedded Dimension of the space
 *	\param MeshType the kind of Mesh I have
 *	\param MeshElement Polygon or Polyhedron
 *	\param real double or long double
 */
template <long embedded,typename MeshType,typename MeshElement,typename real=double>
class BoundaryCondition {
protected:
	const MeshType& mesh;
	muParserInterface<embedded,real>& boundaryFunction;
	
	/** Standard constructor
	 */
	BoundaryCondition(const MeshType& inputMesh,muParserInterface<embedded,real>& inputBoundaryFunction):mesh(inputMesh),boundaryFunction(inputBoundaryFunction) {};
public:
	
	/** This is to decide if to add the Kloc computed to the matrix. It depends on the boundary condition.
	 *
	 *	For Dirichlet for example the upper part of the matrix is diagonal
	 */
	virtual void addToTripletList(Matrix<real,Dynamic,Dynamic>& Kloc,MeshElement& element,vector<Triplet<real>>& tripletList)=0;
	
	/** Changes the stiffnessMatrix to keep into account the boundary condition.
	 */
	virtual void assignBoundaryConditionOnStiffnessMatrix(vector<Triplet<real>>& tripletList)=0;
	
	/** Changes the known term to keep into account the boundary condition
	 */
	virtual void assignBoundaryConditionOnKnownTerm(VectorX<real>& knownTerm)=0; 	
	
};





#endif
