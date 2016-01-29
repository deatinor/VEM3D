
#ifndef Mesh2_Polyhedron_h
#define Mesh2_Polyhedron_h

#include <algorithm>

/** Class to represent a Polyhedron in 3D
 *	
 *	The only way to initialize it is to call make_shared_Polygon
 *	Volume, faces orientation, normal orientation and centroid are computed during initialization and saved.
 *	Inheritance from enable_shared_from_this to obtain a shared_ptr from this.
 *
 */
template <long embedded, typename real=double>
class Polyhedron: public enable_shared_from_this<Polyhedron<embedded,real>> {
private:
	struct privateStruct {}; //!< Don't change, needed for make_shared_Polygon

protected:
	// PROPERTIES
	vector<shared_ptr<Polygon<embedded,real>>> polygonVector; //!< Stores the faces of the Polyhedron
	
	bool isBoundary;	//!< Tells if the Polygon is on the boundary
	real volume;	//!< \return the volume of the Polyhedron
	Point<embedded,real> centroid;	//!< Not the real centroid, only the mean of the vertexes \return the centroid of the Polyhedron

	vector<shared_ptr<MeshPoint<embedded,real>>> pointVector;	//!< Stores the vertexes of the Polyhedron

	
public:
	//PROPERTIES
	long numberOfPoints;	//!< Number of vertexes
	long numberOfPolygons;	//!< Number of faces
	
	// CONSTRUCTORS
	/** Constructor from a vector of shared_ptr. DO NOT USE.
	 *
	 *	Called by make_shared_Polyhedron
	 */
	Polyhedron(const privateStruct&,const vector<shared_ptr<Polygon<embedded,real>>>& inputPolygonVector):polygonVector(inputPolygonVector),numberOfPolygons(inputPolygonVector.size()),isBoundary(false),pointVector({}),volume(0) {
	};
	
	/** Empty constructor. DO NOT USE.
	 *
	 *	Called by make_shared_Polyhedron
	 */
	Polyhedron(const privateStruct&):polygonVector({}),numberOfPolygons(0),isBoundary(false),pointVector({}),volume(0){};
	/** Constructor with variadic template. DO NOT USE.
	 *
	 *	Source:
	 *	http://stackoverflow.com/questions/8158261/templates-how-to-control-number-of-constructor-args-using-template-variable
	 *	Called by make_shared_Polyhedron
	 */
	template <typename... Args>
	Polyhedron(const privateStruct&,Args... arguments):polygonVector{arguments...},pointVector({}),volume(0),isBoundary(false) {
		if (sizeof...(Args)<4) {
			cout<<endl<<endl<<"Minimum 4 polygons required"<<endl<<endl;
		}
		numberOfPolygons=sizeof...(Args);
	};
	
	/** The <b>only</b> constructor for Polyhedron
	 *
	 *	Using the variadic template it accepts as input a vector of shared_ptr to MeshPoints or a sequence of MeshPoints.
	 *	Then it calls the right constructor. It initialize the Polyhedron calling the method initialize
	 *
	 *	\return A shared pointer to the Polyhedron
	 */
	template <typename ...Args>
	static shared_ptr<Polyhedron<embedded, real>> make_shared_Polyhedron(Args... arguments) {
		auto polyhedron=make_shared<Polyhedron<embedded,real>>(privateStruct{},arguments...);
		polyhedron->initialize();
		return polyhedron;
	}
	
	// STANDARD METHODS
	void addPolygon(shared_ptr<Polygon<embedded,real>>& inputPolygon);	//!< Add a new vertex to the Polygon
	void addPoint(shared_ptr<MeshPoint<embedded,real>>& inputPoint);	//!< Add a new Polyhedron having this as face
	Point<embedded,real> computeCentroid();	//!< \return the centroid
	/** Computes the volume of the Polyhedron
	 *
	 *	It uses the Gaus formula:
	 *	The volume is the sum on all the faces of the area of the face multiplied by the x coordinate of the baricenter, multiplied by the x component of the normal to the polygon
	 *
	 */
	real computeVolume();
	real getDiameter();		//!< \return the maximum distance between 2 Points
	
	/** Makes the normal to each face pointing towards the external of the Polyhedron
	 *
	 *	Ray casting algorithm:
	 *		- Consider the first face and a random point on it
	 *		- Consider the normal starting 
	 *		- If the line intersect a vertex or an edge consider an another random
	 *
	 */
	void fixExternalNormal();
	
	/** Makes the normal of each face pointing in the same direction (or inward or outward)
	 *
	 *	Algorithm:
	 *		- Order all the faces
	 *		- For each face check all the faces that are after it in the ordering. Check if they have common edges.
	 *		- If yes, make sure that the edge in common is in the opposite order in the 2 face. If it's in the same order invert the face. Put the face checked right in front of itself.
	 *		- If it has no common edge with any of the precedent faces do nothing
	 *		- Continue with the first face in the ordering not checked yet.
	 */
	void fixFacesOrientation();
	real hTriangle();	//!< \return The maximum distance between vertexes. Necessary for the mesh.
	void initialize();	//<! Put this Polyhedron to the polyhedronVector in each MeshPoint and Polygon. Computes volume, normal and centroid
	void linkPoints();	//<! Makes all vertexes pointing to this
	void linkPolygons();	//!< Makes all Polygons pointing to this
	void shrink_to_fit();
	real space() {return volume;} // This is a common method to Polygon and Polyhedron. In this way one of two can be passed as a template paramether and this method can be called to get area in case of Polygon and volume in case of Polyhedron
	void switchFacesOrientation();	//!< Invert the orientation of all faces
	void updatePointVector();	//!< If a new face is added, also his vertexes are added to the pointVector
	/** Output to string
	 *
	 *	\return std::string representing the Point
	 *
	 */
	string write();
	
	// GET-SET METHODS
	bool getIsBoundary() const {return isBoundary;};
	void setIsBoundary(bool inputIsBoundary) {isBoundary=inputIsBoundary;};
	real getVolume() const {return volume;};
	const Point<embedded, real>& getCentroid() const {return centroid;};
	/** Get a shared pointer to a Point in pointVector
	 */
	const shared_ptr<MeshPoint<embedded,real>>& point(long index) const {return pointVector[index%numberOfPoints];};
	
	/** Get a shared pointer to a Polygon in polygonVector
	 */
	const shared_ptr<Polygon<embedded,real>>& polygon(long index) const {return polygonVector[index%numberOfPolygons];};
	
	// OPERATORS
	/** Get the MeshPoint with a particular index
	 */
	shared_ptr<Polygon<embedded,real>> operator [](long index);


	// EXTERNAL METHODS
	template<long embedded2,typename real2>
	friend ostream& operator<<(ostream& os,const Polyhedron<embedded2,real2>&polyhedron);	//!< Output operator
};

//////////////////////
// STANDARD METHODS //
//////////////////////
		
// addPolygon
template <long embedded,typename real>
void Polyhedron<embedded,real>::addPolygon(shared_ptr<Polygon<embedded, real>>& inputPolygon) {
	polygonVector.push_back(inputPolygon);
}

// addPoint
template <long embedded,typename real>
void Polyhedron<embedded,real>::addPoint(shared_ptr<MeshPoint<embedded, real>>& inputPoint) {
	pointVector.push_back(inputPoint);
}

// centroid
template <long embedded,typename real>
Point<embedded,real> Polyhedron<embedded,real>::computeCentroid() {
	Point<embedded,real> returnPoint=*(pointVector[0]);
	for (int i=1;i<pointVector.size();i++) {
		returnPoint=returnPoint+(*(pointVector[i]));
	}
	returnPoint=returnPoint/(pointVector.size());
	return returnPoint;
}

// computeVolume
template <long embedded,typename real>
real Polyhedron<embedded,real>::computeVolume() {
	static_assert(embedded==3,"computing volume possible only for embedded=3");

	real volume=0;
	
	for (int i=0;i<numberOfPolygons;i++) {
		auto& face=*polygonVector[i];
		real normalX=face.getNormal().x();
		real x=face.getCentroid().x();
		x=x*face.getArea();
		volume+=normalX*x;
	}
	
	return abs(volume);
}
			
// diameter
template <long embedded,typename real>
real Polyhedron<embedded,real>::getDiameter() {
	real diameter=0;
	real norm=0;
	for (int i=0; i<pointVector.size();i++) {
		for (int j=i+1; j<pointVector.size();j++) {
			norm=( (*(pointVector[i])) - (*(pointVector[j]))  ).norm();
			if (norm>diameter) {
				diameter=norm;
			}
		}
	}
	return diameter;
}

// fixExternalNormal
template <long embedded,typename real>
void Polyhedron<embedded,real>::fixExternalNormal() {
	static_assert(embedded==3,"computing fixExternalNormal possible only for embedded=2,3");
	
	auto& face1=*polygonVector[0];
	auto& normal1=face1.getNormal();
	// check if there are other Polyhedrons with a common face: in case it takes the opposite normal
	for (int j=0; j<polygonVector.size(); j++) {
		auto facePointer=polygonVector[j];
		auto& facePoint=*(*facePointer)[0];
		for (int i=0; i<facePoint.numberOfPolygons(); i++) {
			if (auto face2Pointer=facePoint.polygon(i).lock()) {
				if (*facePointer==*face2Pointer && facePointer!=face2Pointer) {
					if (facePointer->getNormal()==face2Pointer->getNormal()) {
						switchFacesOrientation();
						return;
					} else {
						return;
					}
				}
			}
		}
	}
	
	
	// Be careful! This point has to be selected inside the convex envelop of the face,
	// One has to check if it's inside the face to avoid boundary problems
	Point<3,real> p1(0,0,0);


	// look at how many faces it intersects
	// Be careful! If intersect a face in an edge or in a vertex I change the initial point and I redo the procedure.
	// Since I chosed a random point it should not be frequent.
	bool isP1Correct=false;
	long numberOfIntersection=0;

	while (isP1Correct==false) {
		// It generates a new p1 and checks if it's inside face1
		
		while (isP1Correct==false) {
			isP1Correct=true;
			p1[0]=0;
			p1[1]=0;
			p1[2]=0;
			
			// it creates the weightVector
			vector<real> weightVector;
			
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<> dis(0, 1);
			for (int n = 0; n < face1.numberOfPoints; ++n) {
				weightVector.push_back(dis(gen));
			}
			// It balances it to have sum 1
			real sum=0;
			for (int i=0; i<weightVector.size(); i++) {
				sum+=weightVector[i];
			}
			for (int i=0; i<weightVector.size(); i++) {
				weightVector[i]/=sum;
			}
			// generate P1
			for (int i=0; i<face1.numberOfPoints; i++) {
				p1=p1+weightVector[i]*(*face1[i]);
			}
			
			// check P1
			if (face1.isConflictPoint(p1)==true || face1.isPointInside(p1)==false) {
				isP1Correct=false;
			}
			
		}

		
		isP1Correct=true;

		numberOfIntersection=0;
		for (int i=1; i<numberOfPolygons; i++) {
			auto& face2=*polygonVector[i];
			auto normal2=face2.getNormal(); //	DON'T PUT REFERENCE HERE! IT CHANGES
			auto& point1=*face2[0]; // 3 points that generate the plane
//			auto& point2=*face2[1];
//			auto& point3=*face2[2];
			
			// eliminate the case of perpendicular faces
			if (abs(normal1*normal2)<1e-13) {
				continue;
			}
			
			// I get the plan generated by the 3 points of face2
			// plane in the form ax+by+cz=d
			real d=0;
		
			d=(*face2[0])*normal2;
			
			
			// I have a plane coefficients*x=d
			
			// I compute the distance between a point of polygon1 and the plane of face2
			real distance=(normal2*p1-d)/normal2.norm();  // p1-normal2*distance belongs to the plane
			
			auto distanceVector=distance*normal2;
			// If the product with normal1 is positive it does not intersect the plane. Otherwise I find the intersection point
			if (normal1*distanceVector>-1e-13) {
				continue;
			}

			auto intersectionPoint=p1+normal1*distance/(-(normal1*normal2));

			
			// I found the point of intersection with the plane, I have to check if it's not on an edge or a vertex
			if (face2.isConflictPoint(intersectionPoint)) {
				cout<<"CONFLICT POINT"<<endl;
				isP1Correct=false;
				break;
			}
			
			
			if (face2.isPointInside(intersectionPoint)) {
				numberOfIntersection++;
			}
			
		}
	}
	
	if (numberOfIntersection%2==1) {
		for (int i=0; i<numberOfPolygons; i++) {
			polygonVector[i]->switchPointsOrder();
		}
	}
}

// fixFacesOrientation
template <long embedded,typename real>
void Polyhedron<embedded,real>::fixFacesOrientation() {
	
	int nextJ=1;
	for (int i=0; i<numberOfPolygons-1; i++) { // the last polygon is surely correctly orientated
		if (i==nextJ) {
			nextJ++;
		}
		
		// for each polygon it checks all the ones after, it puts right in front of it the ones he found adjacent to it (so they are correct after the check)
		auto& polygon1=*polygonVector[i];
		for (int j=nextJ; j<numberOfPolygons; j++) {
			auto& polygon2=*polygonVector[j];
			
			for (int k=0;k<polygon1.numberOfPoints;k++) {
				bool flag2=false;
				auto point1=polygon1[k];
				auto point2=polygon1[k+1];
				for (int m=0;m<polygon2.numberOfPoints;m++) {
					if (point1==polygon2[m] && point2==polygon2[m+1]) {
						flag2=true;
						polygon2.switchPointsOrder();
						iter_swap(polygonVector.begin()+nextJ,polygonVector.begin()+j);
						nextJ++;
						break;
					}
					if (point1==polygon2[m] && point2==polygon2[m-1]) {
						flag2=true;
						iter_swap(polygonVector.begin()+nextJ,polygonVector.begin()+j);
						nextJ++;
						break;
					}
				}
				if (flag2) break;
			}
		}
	}
}

// hTriangle
template <long embedded,typename real>
real Polyhedron<embedded,real>::hTriangle() {
	real hMax=polygonVector[0]->hTriangle();
	for (long i=1; i<numberOfPolygons; i++) {
		real tempMax=polygonVector[i]->hTriangle();
		if (tempMax>hMax) {
			hMax=tempMax;
		}
	}
	return hMax;
}
			
// initialize
template <long embedded,typename real>
void Polyhedron<embedded,real>::initialize()  {
	updatePointVector();
	numberOfPoints=pointVector.size();
	linkPolygons();
	linkPoints();
	fixFacesOrientation();
	
	fixExternalNormal();
	
	volume=computeVolume();
	centroid=computeCentroid();
}

// linkPolygons
template <long embedded,typename real>
void Polyhedron<embedded,real>::linkPolygons() {
	weak_ptr<Polyhedron<embedded,real>> pointer=enable_shared_from_this<Polyhedron<embedded,real>>::shared_from_this();
	for (int i=0;i<polygonVector.size();i++) {
		polygonVector[i]->addPolyhedron(pointer);
	}
}
			
// linkPoints
template <long embedded,typename real>
void Polyhedron<embedded,real>::linkPoints() {
	weak_ptr<Polyhedron<embedded,real>> pointer=enable_shared_from_this<Polyhedron<embedded,real>>::shared_from_this();
	for (int i=0;i<pointVector.size();i++) {
		pointVector[i]->addPolyhedron(pointer);
	}
}

// switchFacesOrientation
template <long embedded,typename real>
void Polyhedron<embedded,real>::switchFacesOrientation() {
	for (int i=0; i<numberOfPolygons; i++) {
		polygonVector[i]->switchPointsOrder();
	}
}

// shrink_to_fit
template <long embedded,typename real>
void Polyhedron<embedded,real>::shrink_to_fit() {
	pointVector.shrink_to_fit();
	polygonVector.shrink_to_fit();
}

// updatePointVector
template <long embedded,typename real>
void Polyhedron<embedded,real>::updatePointVector() { // the constructor consider only the polygons, not also the points
	for (int i=0;i<numberOfPolygons;i++) {
		
		for (int j=0;j<polygonVector[i]->numberOfPoints;j++) {
			auto& point=polygonVector[i]->point(j);
			bool flag=false;
			for (int k=0;k<pointVector.size();k++) {
				if (point==pointVector[k]) {
					flag=true;
					break;
				}
			}
			if (!flag) {
				pointVector.push_back(point);
			}
		}
	}
}
			

///////////////
// OPERATORS //
///////////////
			
template <long embedded,typename real>
shared_ptr<Polygon<embedded,real>> Polyhedron<embedded,real>::operator[](long int index) {
	if (index<0) {
		index+=numberOfPolygons;
	}
	return polygonVector[index%numberOfPolygons];
}
			
// write
template <long embedded,typename real>
string Polyhedron<embedded,real>::write() {
	string output="";
	for (int i=0; i<numberOfPolygons-1; i++) {
		output+=(polygonVector[i]->write()+";");
	}
	output+=polygonVector[numberOfPolygons-1]->write();
	return output;
}


//////////////////////
// EXTERNAL METHODS //
//////////////////////
			
template <long embedded,typename real>
ostream& operator <<(ostream& os,const Polyhedron<embedded,real>&polyhedron) {
	cout<<endl;
	for (int i=0; i<polyhedron.numberOfPolygons; i++) {
		cout<<"Face "<<i<<":"<<*polyhedron.polygonVector[i]<<polyhedron.polygonVector[i]->getNormal()<<endl;
	}

	cout<<endl;
	
	return os;
}


			








			
			

			
			
			
			
			
			

#endif
