
#ifndef MeshPoint_h
#define MeshPoint_h

#include <array>
#include <vector>
#include <math.h>
#include <memory>
#include <cmath>
#include "Mesh.h"
#include "Point.h"

template <long embedded, typename real>
class Polygon;

template <long embedded, typename real>
class Polyhedron;

template <long embedded, typename real>
class MeshPoint;

template <long embedded2,typename baseElement,OpenEnum isOpen,typename real2>
class Mesh;

template <typename real>
class Mesh3D;

template <typename real>
class Mesh2D;

template <long embedded,typename real>
class Point;

/** Class to represent a Point belonging to a Mesh
 *
 *	The main differences with a simple Point are:
 *	- no copy constructor
 *	- it stores weak_ptr to the Polygons and Polyhedrons that have itself has vertex
 */
template <long embedded,typename real=double>
class MeshPoint: public Point<embedded,real> {
	
protected:
	
	// PROPERTIES
	long pointID;	//!< ID of the MeshPoint
	real value;		//!< Value in the point after the resolution of the problem
	bool isBoundary;	//<! Tells if the MeshPoint is on the boundary
	
	/** Vector of Polygon with this MeshPoint as vertex
	 *
	 *	weak_ptr necessary to not create loop pointers
	 */
	vector<weak_ptr<Polygon<embedded, real>>> polygonVector;
	
	/** Vector of Polyhedron with this MeshPoint as vertex
	 *
	 *	weak_ptr necessary to not create loop pointers
	 */
	vector<weak_ptr<Polyhedron<embedded,real>>> polyhedronVector;
	
	
public:
	// CONSTRUCTORS
	MeshPoint(const array<real,embedded>& inputArray); //!< Constructor from array of coordinates
	MeshPoint():Point<embedded,real>(),pointID(0),value(0),isBoundary(0),polygonVector({}),polyhedronVector({}) {}; //!< Empty constructor
	
	/** Constructor with variadic template
	 *
	 *  Source:
	 *  http://stackoverflow.com/questions/8158261/templates-how-to-control-number-of-constructor-args-using-template-variable
	 */
	template <typename... Args>
	MeshPoint(Args... arguments):Point<embedded,real>(arguments...),pointID(0),value(0),isBoundary(false),polygonVector({}),polyhedronVector({}) {
	};
	
	// STANDARD METHODS
	void addPolygon(weak_ptr<Polygon<embedded,real>> inputPolygon);	//!< It insert in polygon vector a new Polygon
	void addPolyhedron(weak_ptr<Polyhedron<embedded,real>> inputPolyhedron);	//!< It insert in polyhedron vector a new Polyhedron
	
	void shrink_to_fit();
	
	// GET-SET METHODS
	long getPointID() const {return pointID;};
	void setPointID(long inputPointID) {pointID=inputPointID;};
	bool getIsBoundary() const {return isBoundary;};
	void setIsBoundary(bool inputIsBoundary) {isBoundary=inputIsBoundary;};
	long numberOfPolygons() const {return polygonVector.size();};	//!< Number of Polygons with this as vertex
	long numberOfPolyhedrons() const {return polyhedronVector.size();};	//!< Number of Polyhedorns with this as vertex
	const weak_ptr<Polygon<embedded,real>>& polygon(long index) const {return polygonVector[index%numberOfPolygons()];};	//!< Get a weak pointer to a Polygon in polygonVector
	const weak_ptr<Polyhedron<embedded,real>>& polyhedron(long index) const {return polyhedronVector[index%numberOfPolyhedrons()];}; //!< Get a weak pointer to a Polyhedron in polyhedronVector

	
	// OPERATOR
	bool operator<(MeshPoint<embedded,real>& inputPoint); //!< it compares pointID
	
	
};

///////////////////
// CONSTRUCTORS  //
///////////////////

template <long embedded,typename real>
MeshPoint<embedded,real>::MeshPoint(const array<real,embedded>& inputArray):Point<embedded,real>(inputArray),pointID(0),value(0),isBoundary(0),polygonVector({}),polyhedronVector({}) {
}



///////////////////////
// STANDARD METHODS  //
///////////////////////


// addPolygon
template <long embedded, typename real>
void MeshPoint<embedded,real>::addPolygon(weak_ptr<Polygon<embedded, real>> inputPolygon) {
	polygonVector.push_back(inputPolygon);
}

// addPolyhedron
template <long embedded,typename real>
void MeshPoint<embedded,real>::addPolyhedron(weak_ptr<Polyhedron<embedded, real> > inputPolyhedron) {
	polyhedronVector.push_back(inputPolyhedron);
}

// shrink_to_fit
template <long embedded,typename real>
void MeshPoint<embedded,real>::shrink_to_fit() {
	polygonVector.shrink_to_fit();
	polyhedronVector.shrink_to_fit();
}

// operator <
template <long embedded,typename real>
bool MeshPoint<embedded,real>::operator<(MeshPoint<embedded,real>& inputPoint) {
	return this->pointID<inputPoint.pointID;
}



#endif /* MeshPoint_h */
