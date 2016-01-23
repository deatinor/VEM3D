// GUIDA

// Classe Point
// Memorizza la posizione del punto, la soluzione calcolata in esso.


#ifndef Mesh_Point_h
#define Mesh_Point_h



#include <array>
#include <vector>
#include <math.h>
#include <memory>
#include <cmath>
#include "Mesh.h"


using namespace std;

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

template <long embedded,typename real=double>
class Point {
	
protected:
	// PROPERTIES
	array<real,embedded> coordinates;
		
public:
	// CONSTRUCTORS
	Point(const array<real,embedded>& inputArray);
	Point(const Point<embedded,real>& inputPoint);
	Point(const MeshPoint<embedded,real>& inputPoint);
	Point():coordinates({}) {
		for (int i=0; i<embedded; i++) {
			coordinates[i]=0;
		}
	};
	//costruttore con variadic template: http://stackoverflow.com/questions/8158261/templates-how-to-control-number-of-constructor-args-using-template-variable
	template <typename... Args>
	Point(Args... arguments):coordinates{static_cast<real>(arguments)...} {
		static_assert(sizeof...(Args)==embedded,"wrong number of input paramethers");
	};
	
	
	// STANDARD METHOS
	long maxIndex() const;
	long maxAbsIndex() const;
	real norm() const;  // norma L2 del vettore
	real normL1() const; // norma L1 del vettore
	string write() const;
	
	real& x();
	real x() const;
	real& y();
	real y() const;
	real& z();
	real z() const;
	
	
	// OPERATORS
	Point<embedded,real>& operator=(Point<embedded,real> inputPoint); // attenzione, copia solo le coordinate, non tutto il resto!
	bool operator==(const Point<embedded,real>& inputPoint) const; // confronta solo le coordinate
	bool operator!=(const Point<embedded,real>& inputPoint) const; // confronta solo le coordinate
	real& operator[](long index);
	real operator[](long index) const;

	
	
	// EXTERNAL METHODS
	template<long embedded2,typename real2>
	friend ostream& operator<<(ostream& os,const Point<embedded2,real2>&point);
	template <long embedded2,typename real2>
	friend Point<embedded2,real2> operator+(const Point<embedded2,real2>& point1,const Point<embedded2,real2>& point2);
	template <long embedded2,typename real2>
	friend Point<embedded2,real2> operator-(const Point<embedded2,real2>& point1,const Point<embedded2,real2>& point2);
	template <long embedded2,typename real2>
	friend Point<embedded2,real2> operator-(const Point<embedded2,real2>& point1);
	template <long embedded2,typename real2>
	friend real2 operator*(const Point<embedded2,real2>& point1,const Point<embedded2,real2>& point2);
	template <long embedded2,typename real2>
	friend Point<embedded2,real2> operator*(long double coefficient,const Point<embedded2,real2>& point);
	template <long embedded2,typename real2>
	friend Point<embedded2,real2> operator*(const Point<embedded2,real2>& point,long double coefficient);
	template <long embedded2,typename real2>
	friend Point<embedded2,real2> operator/(const Point<embedded2,real2>& point,long double coefficient);
	template <long embedded2,typename real2>
	friend Point<embedded2,real2> cross(const Point<embedded2,real2>& point1,const Point<embedded2,real2>& point2);
	template <long embedded2,typename real2>
	friend Point<embedded2,real2> prodTermByTerm(const Point<embedded2,real2>& point1,const Point<embedded2,real2>& point2);

};




///////////////////
// CONSTRUCTORS  //
///////////////////

template <long embedded,typename real>
Point<embedded,real>::Point(const array<real,embedded>& inputArray):coordinates(inputArray) {
}

template <long embedded,typename real>
Point<embedded,real>::Point(const Point<embedded,real>& inputPoint):coordinates(inputPoint.coordinates) {
}

template <long embedded,typename real>
Point<embedded,real>::Point(const MeshPoint<embedded,real>& inputPoint):coordinates(inputPoint.coordinates) {
}


///////////////////////
// STANDARD METHODS  //
///////////////////////

// maxIndex
template <long embedded,typename real>
long Point<embedded,real>::maxIndex() const{
	long maxIndex=0;
	real maxValue=coordinates[0];
	
	for (long i=1; i<embedded; i++) {
		if (coordinates[i]>maxValue) {
			maxIndex=i;
			maxValue=coordinates[i];
		}
	}
	return maxIndex;
}

// maxAbsIndex
template <long embedded,typename real>
long Point<embedded,real>::maxAbsIndex() const{
	long maxIndex=0;
	real maxValue=abs(coordinates[0]);
	
	for (long i=1; i<embedded; i++) {
		if (abs(coordinates[i])>maxValue) {
			maxIndex=i;
			maxValue=abs(coordinates[i]);
		}
	}
}

// norm
template <long embedded,typename real>
real Point<embedded,real>::norm() const{
	double somma=0;
	for (int i=0; i<embedded; i++) {
		somma+=coordinates[i]*coordinates[i];
	}
	return sqrt(somma);
}

// normL1
template <long embedded,typename real>
real Point<embedded,real>::normL1() const{
	double somma=0;
	for (int i=0; i<embedded; i++) {
		somma+=abs(coordinates[i]);
	}
	return somma;
}

// write
template <long embedded,typename real>
string Point<embedded,real>::write() const{
	string output="";
	for (int i=0; i<embedded-1; i++) {
		output+=(to_string(coordinates[i])+",");
	}
	output+=to_string(coordinates[embedded-1]);
	return output;
}



/////////////////////////////
// TEMPLATE SPECIALIZATION //
/////////////////////////////

// x
template <long embedded,typename real>
real& Point<embedded,real>::x() {
	static_assert(embedded>=1,"embedded lower than 1");
	return coordinates[0];
}
template <long embedded,typename real>
real Point<embedded,real>::x() const{
	static_assert(embedded>=1,"embedded lower than 1");
	return coordinates[0];
}

// y
template <long embedded,typename real>
real& Point<embedded,real>::y() {
	static_assert(embedded>=2,"embedded lower than 2");

	return coordinates[1];
}
template <long embedded,typename real>
real Point<embedded,real>::y() const{
	static_assert(embedded>=2,"embedded lower than 2");
	
	return coordinates[1];
}

// z
template <long embedded,typename real>
real& Point<embedded,real>::z() {
	static_assert(embedded>=3,"embedded lower than 3");
	return coordinates[2];
}
template <long embedded,typename real>
real Point<embedded,real>::z() const{
	static_assert(embedded>=3,"embedded lower than 3");
	return coordinates[2];
}

///////////////
// OPERATORS //
///////////////

// operator =
template <long embedded,typename real>
Point<embedded,real>& Point<embedded,real>::operator=(Point<embedded,real> inputPoint) {
	swap(coordinates,inputPoint.coordinates);
	return *this;
}

// operator ==
template <long embedded,typename real>
bool Point<embedded,real>::operator==(const Point<embedded,real>& inputPoint) const{
	real toll=1e-13;
	if (((*this)-inputPoint).norm()<toll) {
		return true;
	}
	
	return false;
}

// operator !=
template <long embedded,typename real>
bool Point<embedded,real>::operator!=(const Point<embedded,real>& inputPoint) const{
	real toll=1e-13;
	if (((*this)-inputPoint).norm()<toll) {
		return false;
	}
	
	return true;
}

// operator []
template <long embedded,typename real>
real& Point<embedded,real>::operator[](long index) {
	return coordinates[index%embedded];
}

// operator []
template <long embedded,typename real>
real Point<embedded,real>::operator[](long index) const {
	return coordinates[index%embedded];
}


//////////////////////
// EXTERNAL METHODS //
//////////////////////

// operator <<
template <long embedded,typename real>
ostream& operator <<(ostream& os,const Point<embedded,real>&point) {
	cout<<endl<<"(";
	for (int i=0; i<embedded-1; i++) {
		cout<<point.coordinates[i]<<",";
	}
	cout<<point.coordinates[embedded-1];
	cout<<")"<<endl;
	return os;
}

// operator +
template <long embedded,typename real>
Point<embedded,real> operator+(const Point<embedded,real>& point1,const Point<embedded,real>& point2) {
	Point<embedded,real> returnPoint(point1);
	for (int i=0; i<embedded; i++) {
		returnPoint.coordinates[i]+=point2.coordinates[i];
	}
	return returnPoint;
}

// operator -
template <long embedded,typename real>
Point<embedded,real> operator-(const Point<embedded,real>& point1,const Point<embedded,real>& point2) {
	Point<embedded,real> returnPoint(point1);
	for (int i=0; i<embedded; i++) {
		returnPoint.coordinates[i]-=point2.coordinates[i];
	}
	return returnPoint;
}

// operator -
template <long embedded,typename real>
Point<embedded,real> operator-(const Point<embedded,real>& point1) {
	Point<embedded,real> returnPoint;
	for (int i=0; i<embedded; i++) {
		returnPoint.coordinates[i]=-point1.coordinates[i];
	}
	return returnPoint;
}

// operator *
template <long embedded,typename real>
real operator*(const Point<embedded,real>& point1,const Point<embedded,real>& point2) {
	real result=0;
	for (int i=0; i<embedded; i++) {
		result+=point1.coordinates[i]*point2.coordinates[i];
	}
	return result;
}

// operator *
template <long embedded,typename real>
Point<embedded,real> operator*(long double coefficient,const Point<embedded,real>& point) {
	Point<embedded,real> newPoint(point);
	for (int i=0; i<embedded; i++) {
		newPoint.coordinates[i]*=coefficient;
	}
	return newPoint;
}

// operator *
template <long embedded,typename real>
Point<embedded,real> operator*(const Point<embedded,real>& point,long double coefficient) {
	return coefficient*point;
}

// operator /
template <long embedded,typename real>
Point<embedded,real> operator/(const Point<embedded,real>& point,long double coefficient) {
	Point<embedded,real> newPoint(point);
	for (int i=0; i<embedded; i++) {
		newPoint.coordinates[i]/=coefficient;
	}
	return newPoint;
}

// cross
template <long embedded,typename real>
Point<embedded,real> cross(const Point<embedded,real>& point1,const Point<embedded,real>& point2) {
	
	
	real component1=point1[1]*point2[2]-point1[2]*point2[1];
	real component2=point1[2]*point2[0]-point1[0]*point2[2];
	real component3=point1[0]*point2[1]-point1[1]*point2[0];
	Point<embedded,real> returnVector(component1,component2,component3);

	
	return returnVector;
	
}

//prodTermByTerm
template <long embedded,typename real>
Point<embedded,real> prodTermByTerm(const Point<embedded,real>& point1,const Point<embedded,real>& point2) {
	Point<embedded,real> returnVector;
	for (int i=0; i<embedded; i++) {
		returnVector[i]=point1[i]*point2[i];
	}
	
	return returnVector;
}


/////////////
// TYPEDEF //
/////////////

using Point2D=Point<2>;
using Point3D=Point<3>;

using PointLong2D=Point<2,long double>;
using PointLong3D=Point<3,long double>;



//un Point può essere visto come un vector (utile per le normali alle superfici per esempio)
using Vector2D=Point<2>;
using Vector3D=Point<3>;
using Vector4D=Point<4>;

template <long embedded,typename real>
using Vector =Point<embedded,real>;
template <long embedded>
using VectorD=Point<embedded,double>;



/////////////////////////////////////////////////////////////////////
//////////                MESHPOINT                        //////////
/////////////////////////////////////////////////////////////////////


template <long embedded,typename real=double>
class MeshPoint: public Point<embedded,real> {
//	template <long embedded2,typename baseElement,OpenEnum isOpen,typename real2>
//	friend class Mesh;
//	template <typename real2>
//	friend class Mesh3D;
//	template <typename real2>
//	friend class Mesh2D;
//	template <long embedded2,typename real2>
//	friend class Polygon;
//	template <long embedded2,typename real2>
//	friend class Polyhedron;
	
protected:
	
	// PROPERTIES
	long pointID;
	real value;
	bool isBoundary;
	
	
	vector<weak_ptr<Polygon<embedded, real>>> polygonVector;
	vector<weak_ptr<Polyhedron<embedded,real>>> polyhedronVector;
	
	
public:
	// CONSTRUCTORS
	MeshPoint(const array<real,embedded>& inputArray);
//	MeshPoint(const Point<embedded,real>& inputPoint);
	MeshPoint():Point<embedded,real>(),pointID(0),value(0),isBoundary(0),polygonVector({}),polyhedronVector({}) {};
	
	//costruttore con variadic template: http://stackoverflow.com/questions/8158261/templates-how-to-control-number-of-constructor-args-using-template-variable
	template <typename... Args>
	MeshPoint(Args... arguments):Point<embedded,real>(arguments...),pointID(0),value(0),isBoundary(false),polygonVector({}),polyhedronVector({}) {
	};
	
	// STANDARD METHODS
	void addPolygon(weak_ptr<Polygon<embedded,real>> inputPolygon); // inserisce in polygon vector un nuovo poligono
	void addPolyhedron(weak_ptr<Polyhedron<embedded,real>> inputPolyhedron);
	
	void shrink_to_fit();
	
	// GET-SET METHODS
	long getPointID() const {return pointID;};
	void setPointID(long inputPointID) {pointID=inputPointID;};
	bool getIsBoundary() const {return isBoundary;};
	void setIsBoundary(bool inputIsBoundary) {isBoundary=inputIsBoundary;};
	long numberOfPolygons() const {return polygonVector.size();};
	long numberOfPolyhedrons() const {return polyhedronVector.size();};
	const weak_ptr<Polygon<embedded,real>>& polygon(long index) const {return polygonVector[index%numberOfPolygons()];};
	const weak_ptr<Polyhedron<embedded,real>>& polyhedron(long index) const {return polyhedronVector[index%numberOfPolyhedrons()];};
	
//	// OPERATOR
	bool operator<(MeshPoint<embedded,real>& inputPoint); // confronta i pointID

	
};

///////////////////
// CONSTRUCTORS  //
///////////////////

template <long embedded,typename real>
MeshPoint<embedded,real>::MeshPoint(const array<real,embedded>& inputArray):Point<embedded,real>(inputArray),pointID(0),value(0),isBoundary(0),polygonVector({}),polyhedronVector({}) {
}

//template <long embedded,typename real>
//MeshPoint<embedded,real>::MeshPoint(const Point<embedded,real>& inputPoint):Point<embedded,real>(inputPoint),pointID(0),value(0),isBoundary(0),polygonVector({}),polyhedronVector({}) {
//}



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
//
// operator <
template <long embedded,typename real>
bool MeshPoint<embedded,real>::operator<(MeshPoint<embedded,real>& inputPoint) {
	return this->pointID<inputPoint.pointID;
}

#endif
















