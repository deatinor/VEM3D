#ifndef Mesh3_SolverVEM3D_h
#define Mesh3_SolverVEM3D_h


/** Specilized class to solve VEM in 3D
 *
 *	Complex way of computing B. The boundary term requires to compute an interpolating polynomial on the face.
 *	This requires to solve a VEM-like problem on the face to compute the polynomial. It is computationally the most expensive part.
 *
 */
template <typename real=double>
class SolverVEM3D: public SolverVEM<3,3,Polyhedron<3,real>,Monomial3D<real>,real> {
	public:
		/** Necessary to use tangential gradient (1-Nx*Nx)
		 *
		 *  The product of the gradient of x with the gradient of y is NOT 0. We have to project it on the plane.
		 */
		virtual Matrix<real,3,3> computeGPolygon(MonomialsPolygon<real>& monomial);
		/** Complex and computationally expensive function
		 */
		virtual Matrix<real,4,Dynamic> computeB(const shared_ptr<Polyhedron<3,real>>& polyhedron,Monomial3D<real>& monomial);
		virtual Matrix<real,3,Dynamic> computeBPolygon(const shared_ptr<Polygon<3,real>>& polyhedron,MonomialsPolygon<real>& monomial);	//!< It is an integral on the boundary of a polygon. Similar to computeB in the 2D case.
		virtual Matrix<real,Dynamic,3> computeDPolygon(MonomialsPolygon<real>& monomial); 	//!< Similar to computeD in the standard case
		virtual Matrix<real,3,Dynamic> computePIStarPolygon(Matrix<real,3,3>&G,Matrix<real,3,Dynamic>&B);
		
	public:
		/** Standard constructor
		 */
		SolverVEM3D(muParserInterface<3,real>& inputForceTerm):SolverVEM<3,3,Polyhedron<3,real>,Monomial3D<real>,real>::SolverVEM(inputForceTerm) {};
		
		/** 3D version of comuteKnownTerm
		 *
		 *	First order formula: it computes f (the known term) in the average of the vertices.
		 *	In many cases, this is the centroid, so that the formula becomes second order. 
		 *	It would be too expensive to compute the real centroid for each polyhedron. 
		 *	Moreover, we use first order VEM, so there is no reason to increase the order of this formula
		 *
		 */
		virtual real computeKnownTerm(const shared_ptr<Polyhedron<3,real>>& element,const shared_ptr<MeshPoint<3,real>>& point);
};

// computeKnownTerm
template <typename real>
real SolverVEM3D<real>::computeKnownTerm(const shared_ptr<Polyhedron<3,real>>& element,const shared_ptr<MeshPoint<3,real>>& point) {
	
	real fValue=this->forceTerm(element->getCentroid()); // it computes f not in the real centroid but in the average of the points
	
	real vhCoefficient=1.0/(element->numberOfPoints); // this is P0 of each vh, always the same for every vh
	real volume=element->getVolume(); // saved to not access it for every point
	
	return volume*vhCoefficient*fValue;
}

// G polygon
template <typename real>
Matrix<real,3,3> SolverVEM3D<real>::computeGPolygon(MonomialsPolygon<real>& monomial) {
	auto& element=monomial.element;
	real area=element->getArea();
	
	Matrix<real,3,3> GPolygon;
	
	// first column
	GPolygon(0,0)=1;
	for (int i=1;i<3;i++) {
		GPolygon(i,0)=0;
	}
	
	// first row (except first column)
	real value=0;
	for (int i=1; i<3; i++) {
		value=0;
		for (int k=0;k<element->numberOfPoints;k++) {
			value+=monomial.evaluate(*(element->point(k)),i-1);
		}
		GPolygon(0,i)=value/element->numberOfPoints;
	}
	
	// the other terms (it is not diagonal in this case)
	GPolygon(1,1)=monomial.gradientX*monomial.gradientX*area;
	GPolygon(1,2)=monomial.gradientX*monomial.gradientY*area;
	GPolygon(2,1)=GPolygon(1,2);
	GPolygon(2,2)=monomial.gradientY*monomial.gradientY*area;
	
	return GPolygon;
}

// B
template <typename real>
Matrix<real,4,Dynamic> SolverVEM3D<real>::computeB(const shared_ptr<Polyhedron<3,real>>& polyhedron,Monomial3D<real>& monomial) {
	long numberOfPoints=polyhedron->numberOfPoints;
	Matrix<real,4,Dynamic> B(4,numberOfPoints);
	
	// in the first row of the matrix, we use P0. In this case it is 1/numberOfPoints,
	real firstLine=1.0/numberOfPoints;
	for (int i=0;i<numberOfPoints;i++) {
		B(0,i)=firstLine;
	}
	
	// initialize the rest of the matrix to 0
	for (int i=1;i<4;i++) {
		for (int j=0;j<numberOfPoints;j++) {
			B(i,j)=0;
		}
	}
	
	// in the other rows, use a boundary integral
	for (int k=0; k<polyhedron->numberOfPolygons; k++) {
		auto& face=polyhedron->polygon(k);
		long facePoints=face->numberOfPoints;
		real area=face->getArea();
		auto& normal=face->getNormal();
		
		// We have to compute the interpolation polynomial. Let's choose 2 monomials from x,y,z. We do not have to choose the one corresponding to maxNormalIndex. Suppose we choose x and y:
		// the other monomials are a basis of degree 1 in the space of the polynomials.
		// For G, we have computed three tangential gradient (1-Nx*Nx). The product of the gradient of x with the gradient of y is NOT 0. We have to project it on the plane.
		// For D, no changes with respect to the standard case
		// B is a boundary integral. We compute the edge normal to the cross product of the edge and the normal to the polygon. The boundary integral is computed with the GLL nodes (1/2*(l1+l2))
		MonomialsPolygon<real> monomialPolygon(face);
		
		// indexX and indexY are inside the monomial
		auto GPolygon=computeGPolygon(monomialPolygon);
		auto BPolygon=computeBPolygon(face,monomialPolygon);
		auto DPolygon=computeDPolygon(monomialPolygon);
		if ((GPolygon-BPolygon*DPolygon).norm()>1e-13) {
			cout << "GPolygon" << endl << endl << GPolygon << endl << endl;
			cout << "BPolygon" << endl << endl << BPolygon << endl << endl;
			cout << "DPolygon" << endl << endl << DPolygon << endl << endl;
			
			cout << "Failed" << endl << *face;
		}
		
		auto PIStarPolygon=computePIStarPolygon(GPolygon,BPolygon);
		
		// PIStar has been obtained. Now, integrate the monomials and multiply them by the normalCoefficient
		real addTerm2=0;
		
		// for every row of matrix B
		for (int i=1; i<4; i++) { // index of the monomials. The real monomial is the one of order i-1, where i is the row of the matrix
			
			// normal derivative of the monomial, always the same
			real normalCoefficient=monomial.gradient*normal[i-1];
			real integralCoefficient=normalCoefficient*area; // summing area
			
			// the point indices are not the same. m is similar to j in the next cycle, but they do not coincide, so it is necessary to use another cycle
			for (int m=0; m<facePoints; m++) {
				
				auto& point=face->point(m);
				addTerm2=0; // it changes with respect to the point considered
				
				addTerm2+=PIStarPolygon(0,m);
				addTerm2+=PIStarPolygon(1,m)*monomialPolygon.evaluate(face->getCentroid(),0);
				addTerm2+=PIStarPolygon(2,m)*monomialPolygon.evaluate(face->getCentroid(),1);
				
				addTerm2*=integralCoefficient;
				
				// searching the correct index to insert it in the matrix
				for (int j=0;j<numberOfPoints;j++) {
					if (point==polyhedron->point(j)) {
						
						B(i,j)+=addTerm2;
						break;
					}
				}
			}
		}
	}
	return B;
}

// B polygon
template <typename real>
Matrix<real,3,Dynamic> SolverVEM3D<real>::computeBPolygon(const shared_ptr<Polygon<3,real>>& polygon,MonomialsPolygon<real>& monomial) {
	long numberOfPoints=polygon->numberOfPoints;
	Matrix<real,3,Dynamic> BPolygon(3,numberOfPoints);
	
	// in the first row of the matrix, we use P0. In this case, it is 1/numberOfPoints.
	real firstLine=1.0/numberOfPoints;
	for (int i=0;i<numberOfPoints;i++) {
		BPolygon(0,i)=firstLine;
	}
	
	// initialize the rest of the matrix to 0
	for (int i=1;i<3;i++) {
		for (int j=0;j<numberOfPoints;j++) {
			BPolygon(i,j)=0;
		}
	}
	
	// in the others, use a boundary integral
	for (int j=0;j<numberOfPoints;j++) { // j in this case is the column index of the matrix
		auto edge=(*polygon->point(j+1))-(*polygon->point(j));
		auto edgeNormal=cross(edge,polygon->getNormal());
		edgeNormal=edgeNormal/edgeNormal.norm(); // normalize
		auto edgeLength=edge.norm();
		
		
		real addTermX=monomial.gradient*edgeNormal[monomial.indexX]*edgeLength/2;
		real addTermY=monomial.gradient*edgeNormal[monomial.indexY]*edgeLength/2;
		
		BPolygon(1,j)+=addTermX;
		BPolygon(2,j)+=addTermY;
		BPolygon(1,(j+1)%numberOfPoints)+=addTermX; // % numberOfPoints is used for the point n+1
		BPolygon(2,(j+1)%numberOfPoints)+=addTermY;
	}
	
	return BPolygon;
}

// D polygon
template <typename real>
Matrix<real,Dynamic,3> SolverVEM3D<real>::computeDPolygon(MonomialsPolygon<real>& monomial) {
	long numberOfPoints=monomial.element->numberOfPoints;
	auto& element=monomial.element;
	Matrix<real,Dynamic,3> DPolygon(numberOfPoints,3);
	
	// first column
	for (int j=0; j<numberOfPoints; j++) {
		DPolygon(j,0)=1;
	}
	
	// rest of the matrix
	for (int i=0; i<numberOfPoints; i++) {
		for (int j=1;j<3;j++) {
			DPolygon(i,j)=monomial.evaluate(*element->point(i),j-1);
		}
	}
	
	return DPolygon;
}

// PIStar
template <typename real>
Matrix<real,3,Dynamic> SolverVEM3D<real>::computePIStarPolygon(Matrix<real,3,3>&G,Matrix<real,3,Dynamic>&B) {
	
	BiCGSTAB<Matrix<real,3,3>> solver;
	solver.compute(G);
	auto PIStarPolygon = solver.solve(B);
	
	return PIStarPolygon;
}

#endif /* Mesh3_SolverVEM3D_h */
