//
//  Mesh2D.h
//  Mesh3
//
//  Created by Stefano on 17/08/15.
//  Copyright (c) 2015 Stefano. All rights reserved.
//

#ifndef Mesh3_Mesh2D_h
#define Mesh3_Mesh2D_h

#include "Mesh.h"
#include "Polygon.h"


static bool lessPair(const pair<long,long>& pair1,const pair<long,long>& pair2) {
	if (pair1.first==pair2.first) {
		return pair1.second<pair2.second;
	}
	return pair1.first<pair2.first;
}

/** Specialized class for 2D Mesh
 *
 *	BaseElement is a Polygon
 *
 */
template <typename real=double>
class Mesh2D: public Mesh<2,Polygon<2,real>,OPEN,real> {
private:
	vector<pair<long,long>> pairVector; //!< This is to obtain internal and external points

public:
	long numberOfBoundaryPoints;
	
	/** Constructor with input file
	 */
	Mesh2D(string pointFile,string connectionFile,MeshType meshType=ANYTHING2D):Mesh<2,Polygon<2,real>,OPEN,real>::Mesh(),numberOfBoundaryPoints(0) {
		this->initialize(pointFile,connectionFile,meshType);
		
	};
	
	// STANDARD METHODS
	template <typename... Args >
	shared_ptr<Polygon<2,real>> newPolygon(Args... arguments); //!< Method to add a Polygon to the Mesh after having read it
	
	virtual void setAnything2DMesh(string connection);	//!< Specilization of the method to read ANYTHING2D file.
	
	
	virtual void setBoundaryElements();	//!< Specilization of the method
	virtual void setRemainingThings();	//!< Specilization of the method
	
	void shrink_to_fit();
	
};

// newPolygon
template <typename real>
template <typename... Args>
shared_ptr<Polygon<2,real>> Mesh2D<real>::newPolygon(Args ...arguments) {
	auto newPolygon=Polygon<2,real>::make_shared_Polygon(arguments...);
	return newPolygon;
}


template <typename real>
void Mesh2D<real>::setAnything2DMesh(string connection) {
	ifstream file2;
	file2.open(connection);
	string rigaFile2;

	
	//	int counter=0;
	while (getline(file2, rigaFile2)) {
		// conto il numero di punti
		long numberOfVertexes=1;
		for (int i=0; i<rigaFile2.size(); i++) {
			if (rigaFile2[i]==',') {
				numberOfVertexes++;
			}
		}
		
		
		istringstream rigaFileStream(rigaFile2);
		
		
		vector<long> vertexVector;
		vector<shared_ptr<MeshPoint<2,real>>> vertexPointerVector;
			
			
		
			
		
		// creo il vertexVector
		for (int i=0;i<numberOfVertexes;i++) {
			string pointString;
			getline(rigaFileStream, pointString, ',');
			istringstream stream3(pointString);
			
			//				cout<<pointString<<endl;
			long value=0;
			stream3>>value;
			vertexVector.push_back(value);
		}
		
		for (int i=0; i<numberOfVertexes; i++) {
			pair<long,long> pair;
			if (vertexVector[i]<vertexVector[(i+1)%numberOfVertexes]){
				pair=make_pair(vertexVector[i],vertexVector[(i+1)%numberOfVertexes]);
			} else {
				pair=make_pair(vertexVector[(i+1)%numberOfVertexes],vertexVector[i]);
			}
			pairVector.push_back(pair);
		}
			
		// creo il vertexPointerVector
		for (int i=0; i<numberOfVertexes; i++) {
			vertexPointerVector.push_back(this->pointVector[vertexVector[i]-1 ]);
		}
			
		// creo la faccia
		auto polygon=Polygon<2,real>::make_shared_Polygon(vertexPointerVector);
		
		//			cout<<face->isBoundary;
		//			cout<<*face;
		

		this->elementVector.push_back(polygon);
	}
	

	
}

template <typename real>
void Mesh2D<real>::setBoundaryElements() {
	
	// riordino il vettore di pair per effettuare un confronto tra lati più veloce
	sort(pairVector.begin(),pairVector.end(),lessPair);
	
	// pongo tutti i punti con isBoundary=false
	for (int i=0;i<this->pointVector.size();i++) {
		this->pointVector[i]->setIsBoundary(false);
	}
	

	// confronto le pair
	for (int i=0;i<pairVector.size()-1;i++) { // l'ultima la escludo
		if (pairVector[i].first!=pairVector[i+1].first || pairVector[i].second!=pairVector[i+1].second) {
			long point1=pairVector[i].first;
			long point2=pairVector[i].second;
			
			this->pointVector[point1-1]->setIsBoundary(true);
			this->pointVector[point2-1]->setIsBoundary(true);
		} else {
			i++;
			// se sono uguali salta la pair dopo
		}
	}
	// l'ultima pair che era stata scartata si può scartare, i punti al bordo sono già stati individuati tutti

	
	// imposta isBoundary sui poligoni
	auto& pointVector=this->pointVector;
	for (int i=0; i<pointVector.size(); i++) {
		auto& point=pointVector[i];
		if (point->getIsBoundary()) {
//			auto& polygonVector=pointVector[i]->polygonVector;
			for (int j=0; j<point->numberOfPolygons(); j++) {
				if (auto polygon=point->polygon(j).lock()){
					polygon->setIsBoundary(true);
				} else {
					cout<<"Polygon is expired"<<endl;
				}
			}
		}
	}

}

template <typename real>
void Mesh2D<real>::setRemainingThings() {
	// setta i pointID e le ultime proprietà
	this->numberOfElements=this->elementVector.size();
	this->numberOfPoints=this->pointVector.size();

	
	numberOfBoundaryPoints=0;
	long notBoundaryCounter=1;
	
	for (int i=0; i<this->numberOfPoints; i++) {
		if (this->pointVector[i]->getIsBoundary()) {
			this->pointVector[i]->setPointID(numberOfBoundaryPoints);
			numberOfBoundaryPoints++;
		} else {
			this->pointVector[i]->setPointID(this->numberOfPoints-notBoundaryCounter);
			notBoundaryCounter++;
		}
	}

}


template <typename real>
void Mesh2D<real>::shrink_to_fit() {
	Mesh<3,Polyhedron<3,real>,OPEN,real>::shrink_to_fit();
	for (int i=0; i<this->pointVector.size(); i++) {
		this->pointVector[i]->shrink_to_fit();
	}
	for (int i=0; i<this->elementVector.size(); i++) {
		this->elementVector[i]->shrink_to_fit();
	}
}
















#endif
