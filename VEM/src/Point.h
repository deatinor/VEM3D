#ifndef Point_h
#define Point_h

#include <array>
#include <vector>
#include <math.h>
#include <memory>
#include <cmath>
#include "Mesh.h"
#include "MeshPoint.h"

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

/*! \brief Class to store the position of a point.
 *
 *	Virtually any dimension is allowed. In this version of the code, some methods are only available for d=2 or d=3.
 *	Any vector can be seen as a point.
 *
 *	Typedefs
 *		- <b>Point2D</b>
 *		- <b>Point3D</b>
 *		- <b>PointLong2D</b>, long double
 *		- <b>PointLong3D</b>, long double
 *		- <b>Vector<embedded,real></b>, exactly the same of Point<embedded,real>
 *		- <b>VectorD<embedded></b>, type double
 *		- <b>Vector2D</b>
 *		- <b>Vector3D</b>
 */
template <long embedded,typename real=double>
class Point {
	protected:
		// PROPERTIES
		array<real,embedded> coordinates; //!< It stores the coordinates of the point
			
	public:
		// CONSTRUCTORS
		Point(const array<real,embedded>& inputArray);	//!< Constructor from std::array
		Point(const Point<embedded,real>& inputPoint);	//!< Copy constructor
		Point(const MeshPoint<embedded,real>& inputPoint);	//!< Copy constructor from MeshPoint
		
		/** Empty constructor
		 */
		Point():coordinates({}) {
			for (int i=0; i<embedded; i++) {
				coordinates[i]=0;
			}
		};
		
		/** Constructor with variadic template
		 *
		 *  Source:
		 *  http://stackoverflow.com/questions/8158261/templates-how-to-control-number-of-constructor-args-using-template-variable
		 */
		template <typename... Args>
		Point(Args... arguments):coordinates{static_cast<real>(arguments)...} {
			static_assert(sizeof...(Args)==embedded,"wrong number of input paramethers");
		};
		
		// STANDARD METHOS
		long maxIndex() const;	//!< Maximum index of the point
		long maxAbsIndex() const;	//!< Maximum index of the point with absolute value
		real norm() const;  //!< L2 norm of the vector
		real normL1() const; //!< L1 norm of the vector
		/** Output to string
		 *
		 *	\return std::string representing the Point
		 *
		 */
		string write() const;
		
		real& x();	//!< Get the first element by reference
		real x() const; //!< Get the first element by value
		real& y();	//!< Get the second element by reference
		real y() const; //!< Get the second element by value
		real& z();	//!< Get the third element by reference
		real z() const;	//!< Get the third element by value
		
		// OPERATORS
		/** Equal operator
		 *
		 * Be careful, it copies only the coordinates, not the other things
		 */
		Point<embedded,real>& operator=(Point<embedded,real> inputPoint);
		/** Check if two Point are equal
		 *
		 * It compares only the coordinates
		 */
		bool operator==(const Point<embedded,real>& inputPoint) const;
		/** Check if two Point are different
		 *
		 * It compares only coordinates
		 */
		bool operator!=(const Point<embedded,real>& inputPoint) const;
		real& operator[](long index);	//!< Get an element by reference
		real operator[](long index) const;	//!< Get an element by value

		// EXTERNAL METHODS
		template<long embedded2,typename real2>
		friend ostream& operator<<(ostream& os,const Point<embedded2,real2>&point); //!< Output operator
		template <long embedded2,typename real2>
		friend Point<embedded2,real2> operator+(const Point<embedded2,real2>& point1,const Point<embedded2,real2>& point2); //!< Sum operator Point+Point
		template <long embedded2,typename real2>
		friend Point<embedded2,real2> operator-(const Point<embedded2,real2>& point1,const Point<embedded2,real2>& point2); //!< Difference operator Point-Point
		template <long embedded2,typename real2>
		friend Point<embedded2,real2> operator-(const Point<embedded2,real2>& point1); //!< Difference operator -Point
		template <long embedded2,typename real2>
		friend real2 operator*(const Point<embedded2,real2>& point1,const Point<embedded2,real2>& point2); //!< Product operator Point*Point
		template <long embedded2,typename real2>
		friend Point<embedded2,real2> operator*(long double coefficient,const Point<embedded2,real2>& point); //!< Product operator coefficient*Point
		template <long embedded2,typename real2>
		friend Point<embedded2,real2> operator*(const Point<embedded2,real2>& point,long double coefficient); //!< Product operator Point*coefficient
		template <long embedded2,typename real2>
		friend Point<embedded2,real2> operator/(const Point<embedded2,real2>& point,long double coefficient); //!< Dividing operator Point/coefficient
		template <long embedded2,typename real2>
		/** Cross product of 2 vectors
		 *
		 * \return The resulting vector of the cross product
		 * Here Point is intended as Vector.
		 * Cross product keeping sign into consideration
		 */
		friend Point<embedded2,real2> cross(const Point<embedded2,real2>& point1,const Point<embedded2,real2>& point2);
		/** Product term by term
		 *	\return A point with components i being the product of the i-th components of the 2 vectors
		 */
		template <long embedded2,typename real2>
		friend Point<embedded2,real2> prodTermByTerm(const Point<embedded2,real2>& point1,const Point<embedded2,real2>& point2);
};


// CONSTRUCTORS
template <long embedded,typename real>
Point<embedded,real>::Point(const array<real,embedded>& inputArray):coordinates(inputArray) {
}

template <long embedded,typename real>
Point<embedded,real>::Point(const Point<embedded,real>& inputPoint):coordinates(inputPoint.coordinates) {
}

template <long embedded,typename real>
Point<embedded,real>::Point(const MeshPoint<embedded,real>& inputPoint):coordinates(inputPoint.coordinates) {
}


// STANDARD METHODS
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
	double sum=0;
	for (int i=0; i<embedded; i++) {
		sum+=coordinates[i]*coordinates[i];
	}
	return sqrt(sum);
}

// normL1
template <long embedded,typename real>
real Point<embedded,real>::normL1() const{
	double sum=0;
	for (int i=0; i<embedded; i++) {
		sum+=abs(coordinates[i]);
	}
	return sum;
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


// TEMPLATE SPECIALIZATION
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


// OPERATORS
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

// operator [] (by reference)
template <long embedded,typename real>
real& Point<embedded,real>::operator[](long index) {
	return coordinates[index%embedded];
}

// operator [] (by value)
template <long embedded,typename real>
real Point<embedded,real>::operator[](long index) const {
	return coordinates[index%embedded];
}


// EXTERNAL METHODS
// operator <<
template <long embedded,typename real>
ostream& operator <<(ostream& os,const Point<embedded,real>&point) {
	cout << endl << "(";
	for (int i=0; i<embedded-1; i++) {
		cout << point.coordinates[i] << ",";
	}
	cout << point.coordinates[embedded-1];
	cout << ")" << endl;
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

// operator - (Point-Point)
template <long embedded,typename real>
Point<embedded,real> operator-(const Point<embedded,real>& point1,const Point<embedded,real>& point2) {
	Point<embedded,real> returnPoint(point1);
	for (int i=0; i<embedded; i++) {
		returnPoint.coordinates[i]-=point2.coordinates[i];
	}
	return returnPoint;
}

// operator - (-Point)
template <long embedded,typename real>
Point<embedded,real> operator-(const Point<embedded,real>& point1) {
	Point<embedded,real> returnPoint;
	for (int i=0; i<embedded; i++) {
		returnPoint.coordinates[i]=-point1.coordinates[i];
	}
	return returnPoint;
}

// operator * (Point*Point)
template <long embedded,typename real>
real operator*(const Point<embedded,real>& point1,const Point<embedded,real>& point2) {
	real result=0;
	for (int i=0; i<embedded; i++) {
		result+=point1.coordinates[i]*point2.coordinates[i];
	}
	return result;
}

// operator * (coefficient*Point)
template <long embedded,typename real>
Point<embedded,real> operator*(long double coefficient,const Point<embedded,real>& point) {
	Point<embedded,real> newPoint(point);
	for (int i=0; i<embedded; i++) {
		newPoint.coordinates[i]*=coefficient;
	}
	return newPoint;
}

// operator * (Point*coefficient)
template <long embedded,typename real>
Point<embedded,real> operator*(const Point<embedded,real>& point,long double coefficient) {
	return coefficient*point;
}

// operator / (Point/coefficient)
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


// TYPEDEF
using Point2D=Point<2>;
using Point3D=Point<3>;

using PointLong2D=Point<2,long double>;
using PointLong3D=Point<3,long double>;

// A Point can be seen as a vector (this is useful for surfaces' normal vectors)
using Vector2D=Point<2>;
using Vector3D=Point<3>;

template <long embedded,typename real>
using Vector =Point<embedded,real>;
template <long embedded>
using VectorD=Point<embedded,double>;

#endif /* Point_h */


