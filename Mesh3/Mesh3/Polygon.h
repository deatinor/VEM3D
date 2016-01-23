//
//  Polygon.h
//  Mesh
//
//  Created by Stefano on 15/07/15.
//  Copyright (c) 2015 Stefano. All rights reserved.
//

#ifndef Mesh_Polygon_h
#define Mesh_Polygon_h

#include <vector>
#include <memory>
#include <math.h>
#include <cmath>
#include <algorithm>
#include "Point.h"
//#include <tr1/memory>

using namespace std;
using std::shared_ptr;

template <long embedded,typename real>
class Polyhedron;


//template<long embedded,typename real=double,typename ...Args>
//shared_ptr<Polygon<embedded,real>> make_shared_Polygon(Args... arguments) {
//	shared_ptr<Polygon<embedded,real>> polygon=make_shared<Polygon<embedded,real>>(arguments...);
//	polygon->initialize();
//	return polygon;
//}

///////////////////////////////////////////////////////////////////////////////////
//									POLYGON										 //
///////////////////////////////////////////////////////////////////////////////////

// eredito da enable_shared_from_this per poter ottenere uno shared_ptr a this
template <long embedded, typename real=double>
class Polygon: public std::enable_shared_from_this<Polygon<embedded,real>> {
private:
	struct privateStruct {};
	
protected:
	// PROPERTIES
	vector<shared_ptr<MeshPoint<embedded,real>>> pointVector;
	bool isBoundary;
	real area;
	Vector<3, real> normal;
	Point<embedded, real> centroid;

	
	vector<weak_ptr<Polyhedron<embedded,real>>> polyhedronVector;
	
public:
	// PROPERTIES
	long numberOfPoints;
	
public:
	// CONSTRUCTORS
	Polygon(const privateStruct &,const vector<shared_ptr<MeshPoint<embedded,real>>>& vertexVector);
//	Polygon(const Polygon<embedded,real>& inputPolygon);
	Polygon(const privateStruct&):pointVector({}),numberOfPoints(0),polyhedronVector({}),isBoundary(false),area(0){};
	//costruttore con variadic template: http://stackoverflow.com/questions/8158261/templates-how-to-control-number-of-constructor-args-using-template-variable
	template <typename... Args>
	Polygon(const privateStruct&,Args... arguments):pointVector{arguments...},polyhedronVector({}),area(0),isBoundary(false) {
		if (sizeof...(Args)<3) {
			cout<<endl<<endl<<"Minimum 3 points required"<<endl<<endl;
		}
		numberOfPoints=sizeof...(Args);
	};
	
public:
	template <typename... Args>
	static shared_ptr<Polygon<embedded,real>> make_shared_Polygon(Args... arguments) {
		shared_ptr<Polygon<embedded,real>> polygon=make_shared<Polygon<embedded,real>>(privateStruct{},arguments...);
		polygon->initialize();
		return polygon;
	}

	
	
public:
	// STANDARD METHODS
	void addPoint(const shared_ptr<MeshPoint<embedded,real>>& p1);
	void addPolyhedron(weak_ptr<Polyhedron<embedded,real>> polyhedron);
			
	Vector<embedded,real> computeCentroid();
	real computeArea();
	real getDiameter();
	real hTriangle();
	
	void initialize();

	bool isConflictPoint(Point<embedded,real>& point);

	shared_ptr<MeshPoint<embedded,real>> isPointAVertex(Point<embedded,real>& point);
	bool isPointInside(Point<embedded,real>& point);
	array<shared_ptr<MeshPoint<embedded,real>>,2> isPointOnBoundary(Point<embedded,real>& point);
	Vector<3,real> computeNormal();
	
	void shrink_to_fit();
	
	real space() {return area;} // serve per ritornare l'area se dall'esterno non è chiaro se è un poligono o un poliedro
	void switchPointsOrder();
	
	string write();
	
	// GET-SET METHODS
	bool getIsBoundary() const {return isBoundary;};
	void setIsBoundary(bool inputIsBoundary) {isBoundary=inputIsBoundary;};
	long numberOfPolyhedrons() const {return polyhedronVector.size();};
	real getArea() const {return area;};
	const Vector<3, real>& getNormal() const {return normal;};
	const Point<embedded, real>& getCentroid() const {return centroid;};
	const shared_ptr<MeshPoint<embedded,real>>& point(long index) {return pointVector[index%numberOfPoints];};
	const weak_ptr<Polyhedron<embedded,real>>& polyhedron(long index) {return polyhedronVector[index%numberOfPolyhedrons()];};
	
	// OPERATORS
	shared_ptr<MeshPoint<embedded,real>> operator[](long index) {
		if (index<0) {
			index+=numberOfPoints;
		}
		return pointVector[index%numberOfPoints];
	};
	
	bool operator==(const Polygon<embedded,real>& polygon);
	bool operator!=(const Polygon<embedded,real>& polygon){return !(*this==polygon);};
	
	// EXTERNAL METHODS
	template <long embedded2, typename real2>
	friend ostream& operator<<(ostream& os,const Polygon<embedded2,real2>&polygon);
	
//	template<long embedded2,typename real2,typename ...Args>
//	friend shared_ptr<Polygon<embedded2,real2>> make_shared_Polygon(Args... arguments);
	
//	template <long embedded2,typename real2>
//	friend class std::tr1::_Ref_count_obj<Polygon>;//<embedded2,real2>>;
	
};

//////////////////
// CONSTRUCTORS //
//////////////////
			
template <long embedded, typename real>
Polygon<embedded,real>::Polygon(const privateStruct&,const vector<shared_ptr<MeshPoint<embedded,real>>>& vertexVector):pointVector(vertexVector),polyhedronVector({}),isBoundary(false) {
	numberOfPoints=(long)pointVector.size();
}

//template <long embedded, typename real>
//Polygon<embedded,real>::Polygon(const Polygon<embedded,real>& inputPolygon):pointVector(inputPolygon.pointVector),numberOfPoints(inputPolygon.numberOfPoints),polyhedronVector(inputPolygon.polyhedronVector),isBoundary(false),area(inputPolygon.area),normal(inputPolygon.normal) {
//	
//}



//////////////////////
// STANDARD METHODS //
//////////////////////

// addPoint
template <long embedded,typename real>
void Polygon<embedded,real>::addPoint(const shared_ptr<MeshPoint<embedded, real>>& p1) {
	pointVector.push_back(p1);
	numberOfPoints++;
}

// addPolyhedron
template <long embedded,typename real>
void Polygon<embedded,real>::addPolyhedron(weak_ptr<Polyhedron<embedded, real>> polyhedron) {
	polyhedronVector.push_back(polyhedron);
	
}
			
// centroid
template <long embedded,typename real>
Point<embedded,real> Polygon<embedded,real>::computeCentroid() {
	Point<embedded,real> returnPoint;
	
	// trovo gli indici per proiettare il tutto su un piano
	long maxNormalIndex=0;
	real maxNormalValue=0;
	long minNormalIndex=0;
	real minNormalValue=1;
	for (int i=0;i<3;i++) {
		if (abs(normal[i])>maxNormalValue) {
			maxNormalIndex=i;
			maxNormalValue=abs(normal[i]);
		}
		if (abs(normal[i])<minNormalValue) {
			minNormalIndex=i;
			minNormalValue=abs(normal[i]);
		}

	}
	
	long indexX=0;
	long indexY=1;
	if (maxNormalIndex==0) {
		indexX=1;
		indexY=2;
	} else if (maxNormalIndex==1){
		indexY=2;
	}
	
	if (maxNormalValue>1-1e-13) { // giace su un piano perpendicolare a x,y o z
		returnPoint[maxNormalIndex]=(*pointVector[0])[maxNormalIndex];
		
		// componente indexX e indexY
		real valueX=0;
		real valueY=0;
		real signedArea=0;
		for (int i=0; i<numberOfPoints; i++) {
			auto& point1=*(*this)[i];
			auto& point2=*(*this)[i+1];
			valueX+=( point1[indexX]+point2[indexX] ) * ( point1[indexX]*point2[indexY] - point2[indexX]*point1[indexY] );
			valueY+=( point1[indexY]+point2[indexY] ) * ( point1[indexX]*point2[indexY] - point2[indexX]*point1[indexY] );
			signedArea+=( point1[indexX]*point2[indexY] - point2[indexX]*point1[indexY] );
		}
		signedArea/=2;
		valueX/=(6*signedArea);
		valueY/=(6*signedArea);
		
		// aggiorno returnPoint
		returnPoint[indexX]=valueX;
		returnPoint[indexY]=valueY;
		return returnPoint;
	}
	
	// piano qualsiasi non perpendicolare
	real valueX=0;
	real valueY=0;
	real signedArea=0;
	for (int i=0; i<numberOfPoints; i++) {
		auto& point1=*(*this)[i];
		auto& point2=*(*this)[i+1];
		valueX+=( point1[indexX]+point2[indexX] ) * ( point1[indexX]*point2[indexY] - point2[indexX]*point1[indexY] );
		valueY+=( point1[indexY]+point2[indexY] ) * ( point1[indexX]*point2[indexY] - point2[indexX]*point1[indexY] );
		signedArea+=( point1[indexX]*point2[indexY] - point2[indexX]*point1[indexY] );
	}
	signedArea/=2;
	valueX/=(6*signedArea);
	valueY/=(6*signedArea);
	
	// aggiorno returnPoint
	returnPoint[indexX]=valueX;
	returnPoint[indexY]=valueY;

	// ripeto il procedimento per trovare indexZ (maxNormalIndex), devo fare in modo che indexX sia il minimo
	// se il minimo e' 0 deve esserci un indice su di lui! Altrimenti l'area viene 0
	if (indexX!=minNormalIndex) {
		indexX=indexY;
	}
	indexY=maxNormalIndex;
	
	valueX=0;
	valueY=0;
	signedArea=0;
	for (int i=0; i<numberOfPoints; i++) {
		auto& point1=*(*this)[i];
		auto& point2=*(*this)[i+1];
		valueX+=( point1[indexX]+point2[indexX] ) * ( point1[indexX]*point2[indexY] - point2[indexX]*point1[indexY] );
		valueY+=( point1[indexY]+point2[indexY] ) * ( point1[indexX]*point2[indexY] - point2[indexX]*point1[indexY] );
		signedArea+=( point1[indexX]*point2[indexY] - point2[indexX]*point1[indexY] );
	}
	signedArea/=2;
//	valueX/=(6*signedArea);
	valueY/=(6*signedArea);
	returnPoint[indexY]=valueY;
	
	return returnPoint;
}

// computeArea
template <long embedded,typename real>
real Polygon<embedded,real>::computeArea() {
	static_assert(embedded==2 || embedded==3,"computing area possible only for embedded=2,3");
	
	real area=0;
	
	if (embedded==2) {
	for (int i=0;i<pointVector.size()-1;i++) {
		area+=pointVector[i]->x()*pointVector[i+1]->y()-pointVector[i+1]->x()*pointVector[i]->y();
	}
	area+=pointVector[0]->y()*pointVector[pointVector.size()-1]->x()-pointVector[0]->x()*pointVector[pointVector.size()-1]->y();
	return abs(area/2);
	}
	
	
	

	else if (embedded==3) {
	// ottengo lo scale factor
	long maxNormalIndex=0;
	real maxNormalValue=0;
	for (int i=0;i<3;i++) {
		if (abs(normal[i])>maxNormalValue) {
			maxNormalIndex=i;
			maxNormalValue=abs(normal[i]);
		}
	}
	real normalReduced=abs(normal[maxNormalIndex]);
	real scaleFactor=normalReduced/(normal.norm());

	
	real reducedArea=0;
	long index1=0;
	long index2=1;
	if (maxNormalIndex==0) {
		index1=1;
		index2=2;
	} else if (maxNormalIndex==1){
		index2=2;
	}
	for (int i=0;i<pointVector.size()-1;i++) {
		reducedArea+=(*pointVector[i])[index1]*(*pointVector[i+1])[index2]-(*pointVector[i+1])[index1]*(*pointVector[i])[index2];
	}
	reducedArea+=(*pointVector[0])[index2]*(*pointVector[pointVector.size()-1])[index1]-(*pointVector[0])[index1]*(*pointVector[pointVector.size()-1])[index2];
	
	// ne calcolo l'area
	area=reducedArea/scaleFactor;
	return abs(area)/2;
	}
}

// computeNormal
template <long embedded,typename real>
Vector<3,real> Polygon<embedded,real>::computeNormal(){
	
	Vector<3,real> newNormal(0,0,0);
	
	if (embedded==3) {
		
		/// NEWELL'S ALGORITHM
		// http://www.gamedev.net/topic/416131-calculating-a-polygon-normal/?p=3771628
		
		for (int i=0; i<numberOfPoints; i++) {
			auto& p1=*pointVector[i];
			auto& p2=*pointVector[(i+1)%numberOfPoints];
			
			newNormal[0]+=(p1[1]-p2[1])*(p1[2]+p2[2]);
			newNormal[1]+=(p1[2]-p2[2])*(p1[0]+p2[0]);
			newNormal[2]+=(p1[0]-p2[0])*(p1[1]+p2[1]);
			
		}
	} else if (embedded==2) {
		for (int i=0; i<numberOfPoints; i++) {
			auto& p1=*pointVector[i];
			auto& p2=*pointVector[(i+1)%numberOfPoints];
			newNormal[2]+=(p1[0]-p2[0])*(p1[1]+p2[1]);
			
			// ordino SEMPRE i vertici in senso antiorario
			
			
		}
		if (newNormal[2]<0) {
			switchPointsOrder();
			newNormal[2]=-newNormal[2];
		}

	}
	return newNormal/newNormal.norm();
}

// getDiameter
template <long embedded,typename real>
real Polygon<embedded,real>::getDiameter(){
	return 1;
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

// hTriangle
template <long embedded,typename real>
real Polygon<embedded,real>::hTriangle() {
	real maxH=((*pointVector[0])-(*pointVector[1])).norm();
	for (long i=0; i<numberOfPoints; i++) {
		for (long j=i+1;j<numberOfPoints;j++) {
			real tempMax=((*pointVector[i])-(*pointVector[j])).norm();
			if (tempMax>maxH) {
				maxH=tempMax;
			}
		}
	}
	return maxH;
}

// initialize
template <long embedded,typename real>
void Polygon<embedded,real>::initialize() {
	//link points
	weak_ptr<Polygon<embedded,real>> pointer=enable_shared_from_this<Polygon<embedded,real>>::shared_from_this();
	for (int i=0;i<pointVector.size();i++) {
		pointVector[i]->addPolygon(pointer);
	}
//	for (int i=0;i<polyhedronVector.size();i++) {
//		polyhedronVector[i]->addPolygon(pointer);
//	}
	normal=computeNormal();
	area=computeArea();
	centroid=computeCentroid();
}

// isPointAVertex
template <long embedded,typename real>
shared_ptr<MeshPoint<embedded,real>> Polygon<embedded,real>::isPointAVertex(Point<embedded, real> &point) {
	for (int i=0; i<numberOfPoints; i++) {
		if (point==*pointVector[i]) {
			return pointVector[i];
		}
	}
	return nullptr;
}

// isPointInside
template <long embedded,typename real>
bool Polygon<embedded,real>::isPointInside(Point<embedded, real> &point) {
	static_assert(embedded==2 || embedded==3,"computing isPointInside possible only for embedded=2,3");

	// ottengo il maxNormalIndex
	long maxNormalIndex=0;
	real maxNormalValue=0;
	for (int i=0;i<3;i++) {
		if (abs(normal[i])>maxNormalValue) {
			maxNormalIndex=i;
			maxNormalValue=abs(normal[i]);
		}
	}
	long indexX=0;
	long indexY=1;
	if (maxNormalIndex==1) {
		indexY=2;
	}
	if (maxNormalIndex==0) {
		indexX=1;
		indexY=2;
	}
	
	real pointX=point[indexX];
	real pointY=point[indexY];
	bool intersectionCounter=false;
	for (int i=0; i<numberOfPoints; i++) {
		auto point1=*pointVector[i];
		auto point2=*pointVector[i];
		if (i==numberOfPoints-1) {
			point2=*pointVector[0];
		} else point2=*pointVector[i+1];
		// primo if per vedere se viene intersecato da una retta, secondo if per vedere la semiretta
		if ( (point1[indexY]<pointY && point2[indexY]>pointY) || (point1[indexY]>pointY && point2[indexY]<pointY)) {
//			cout<<"if1"<<endl;
			
			if (point1[indexX]<pointX && point2[indexX]<pointX) {
//				cout<<"primo"<<endl;
				intersectionCounter=!intersectionCounter;
				continue;
			}
			if (point1[indexX]>pointX && point2[indexX]>pointX) {
//				cout<<"secondo"<<endl;
				continue;
			}
			
			// lineX è l'ascissa del segmento tra point1 e point2 con ordinata pointY
			real lineX=point1[indexX]+(pointY-point1[indexY])/(point2[indexY]-point1[indexY])*(point2[indexX]-point1[indexX]);
//			cout<<point1<<point2;
//			cout<<point1[indexX]<<endl;
//			cout<<lineX<<endl;
			if (lineX<pointX) {
//				cout<<"if2 "<<pointX<<" "<<lineX<< endl;
				intersectionCounter=!intersectionCounter;
			}
		}
	}
	
	return intersectionCounter;
}

// isPointOnBoundary
template <long embedded,typename real>
array<shared_ptr<MeshPoint<embedded,real>>, 2> Polygon<embedded,real>::isPointOnBoundary(Point<embedded, real> &point) {
	static_assert(embedded==2 || embedded==3,"computing isPointOnBoundary possible only for embedded=2,3");

	// ottengo il maxNormalIndex
	long maxNormalIndex=0;
	real maxNormalValue=0;
	for (int i=0;i<3;i++) {
		if (abs(normal[i])>maxNormalValue) {
			maxNormalIndex=i;
			maxNormalValue=abs(normal[i]);
		}
	}
	// l'index1 e' quello di confronto
	long indexX=0;
	long indexY=1;
	if (maxNormalIndex==1) {
		indexY=2;
	}
	if (maxNormalIndex==0) {
		indexX=1;
		indexY=2;
	}
	
	real pointX=point[indexX];
	real pointY=point[indexY];
	for (int i=0; i<numberOfPoints; i++) {
		auto point1=pointVector[i];
		auto point2=pointVector[i];
		if (i==numberOfPoints-1) {
			point2=pointVector[0];
		} else point2=pointVector[i+1];
		
		if ( ((*point1)[indexY]<pointY && (*point2)[indexY]>pointY) || ((*point1)[indexY]>pointY && (*point2)[indexY]<pointY)) {
			real lineX=(*point1)[indexX]+(pointY-(*point1)[indexY])/((*point2)[indexY]-(*point1)[indexY])*((*point2)[indexX]-(*point1)[indexX]);
			if (abs(lineX-pointX)<1e-13) {
				array<shared_ptr<MeshPoint<embedded,real>>, 2> returnArray({point1,point2});
				return returnArray;
			}
			
		}
	}
	
	array<shared_ptr<MeshPoint<embedded,real>>, 2> returnArray({nullptr,nullptr});
	return returnArray;
}

// isConflictPoint
template <long embedded,typename real>
bool Polygon<embedded,real>::isConflictPoint(Point<embedded, real> &point) {
	static_assert(embedded==2 || embedded==3,"computing isConflictPoint possible only for embedded=2,3");
	
	if (isPointAVertex(point)!=nullptr || isPointOnBoundary(point)[0]!=nullptr ) {
		return true;
	}
	
	// ottengo il maxNormalIndex
	long maxNormalIndex=0;
	real maxNormalValue=0;
	for (int i=0;i<3;i++) {
		if (abs(normal[i])>maxNormalValue) {
			maxNormalIndex=i;
			maxNormalValue=abs(normal[i]);
		}
	}
	// l'index1 e' quello di confronto
	long indexX=0;
	long indexY=1;
	if (maxNormalIndex==1) {
		indexY=2;
	}
	if (maxNormalIndex==0) {
		indexX=1;
		indexY=2;
	}
	
	real pointX=point[indexX];
	real pointY=point[indexY];
	
	bool isConflict=false;
	for (int i=0; i<numberOfPoints; i++) {
		auto& vertex=*pointVector[i];
		if (abs(pointX-vertex[indexX])<1e-13 || abs(pointY-vertex[indexY])<1e-13 ) {
			isConflict=true;
			break;
		}
	}
	return isConflict;
}

// shrink_to_fit
template <long embedded,typename real>
void Polygon<embedded, real>::shrink_to_fit() {
	pointVector.shrink_to_fit();
	polyhedronVector.shrink_to_fit();
}
			
// switchPointsOrder
template <long embedded,typename real>
void Polygon<embedded,real>::switchPointsOrder() {
	std::reverse(pointVector.begin(),pointVector.end());
	normal=-normal;
}

// write
template <long embedded,typename real>
string Polygon<embedded,real>::write() {
	string output="";
	for (int i=0; i<numberOfPoints-1; i++) {
		output+=(to_string(pointVector[i]->getPointID()+1)+",");
	}
	output+=to_string(pointVector[numberOfPoints-1]->getPointID()+1);
	return output;
}
			
			

///////////////
// OPERATORS //
///////////////
			
template <long embedded, typename real>
bool Polygon<embedded,real>::operator==(const Polygon<embedded, real> &polygon) {
	if (numberOfPoints!=polygon.numberOfPoints) return false;

	for (int i=0;i<numberOfPoints;i++) {
		auto& p1=*pointVector[i];
		
		bool rilevatore=false;
		for (int j=0;j<numberOfPoints;j++) {
			if(p1==*polygon.pointVector[j]) {
				rilevatore=true;
				break;
			}
		}
		if (rilevatore==false) return false;
		
	}
	return true;
	
}
			
	
//////////////////////
// EXTERNAL METHODS //
//////////////////////
			
template <long embedded, typename real>
ostream& operator<<(ostream& os,const Polygon<embedded,real>&polygon) {
	cout<<endl<<endl;
	for (int i=0; i<polygon.numberOfPoints; i++) {
		cout<<"P"<<i+1<<":"<<*polygon.pointVector[i];
	}
	cout<<endl;
	return os;
}



			

			

			
			
			
			
			
#endif
