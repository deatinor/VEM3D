//
//  Mesh3D.h
//  Mesh3
//
//  Created by Stefano on 17/08/15.
//  Copyright (c) 2015 Stefano. All rights reserved.
//

#ifndef Mesh3_Mesh3D_h
#define Mesh3_Mesh3D_h

#include <fstream>
#include <sstream>
#include "Mesh.h"

/** Specialized class for 3D Mesh
 *
 *	BaseElement is a Polyhedron
 *
 */
template <typename real=double>
class Mesh3D : public Mesh<3, Polyhedron<3,real>, OPEN, real> {
	
	
protected:
	
	// PROPERTIES
	vector<shared_ptr<Polygon<3,real>>> polygonVector;  //!< Vector of all the Polyhedron faces
	
public:
	long numberOfPolygons;
	long numberOfBoundaryPoints;
	
	/** Constructor with input file
	 */
	Mesh3D(string pointFile,string connectionFile,MeshType meshType=ANYTHING3D):Mesh<3,Polyhedron<3,real>,OPEN,real>::Mesh(),polygonVector({}),numberOfPolygons(0),numberOfBoundaryPoints(0) {
		this->initialize(pointFile,connectionFile,meshType);
		
	};
	
	// STANDARD METHODS
	/** Method to create a Polygon after having read it
	 */
	template <typename... Args >
	shared_ptr<Polygon<3,real>> newFace(Args... arguments);
	
	// specialization of the parent class functions
	virtual void setBoundaryElements();	//!< Specilization of the method
	virtual void setRemainingThings(); //!< Specilization of the method
	
	
	// methods to set the elementVector
	virtual void setTetrahedronMesh(string connection);	//!< Mesh of TETRAHEDRON type
	virtual void setAnything3DMesh(string connection);	//!< Mesh of ANYTHING3D type
	
	void shrink_to_fit();
	
	// GET-SET METHODS
	/** Get Polygon with index
	 */
	const shared_ptr<Polygon<3,real>>& polygon(long index) const {return polygonVector[index%numberOfPolygons];};
	
};


// STANDARD METHODS

// shared_ptr<MeshPoint<3, real>> p1, shared_ptr<MeshPoint<3, real>> p2, shared_ptr<MeshPoint<3, real>> p3

template <typename real>
template <typename... Args>
shared_ptr<Polygon<3,real>> Mesh3D<real>::newFace(Args ...arguments) {
	auto newPolygon=Polygon<3,real>::make_shared_Polygon(arguments...);
	auto& point1=*newPolygon->point(0);
	for (int i=0; i<point1.numberOfPolygons(); i++) {
		auto weakPolygon=point1.polygon(i);
		if (auto polygon=weakPolygon.lock()) {
			if (*polygon==*newPolygon && polygon!=newPolygon) { // puntatori diversi ma poligoni uguali
				polygon->setIsBoundary(false);
				newPolygon->setIsBoundary(false);
				polygonVector.push_back(newPolygon);
				return newPolygon;
			}
			
		} else {
			cout<<"Polygon is expired"<<endl;
		}
		
		
	}
	
	newPolygon->setIsBoundary(true);
	polygonVector.push_back(newPolygon);
	return newPolygon;
	
}

// setAnything3DMesh
template <typename real>
void Mesh3D<real>::setAnything3DMesh(string connection) {
	ifstream file2;
	file2.open(connection);
	string rigaFile2;
	
	//	int counter=0;
	while (getline(file2, rigaFile2)) {
		// conto il numero di facce
		long numberOfFaces=1;
		for (int i=0; i<rigaFile2.size(); i++) {
			if (rigaFile2[i]==';') {
				numberOfFaces++;
			}
		}
		
		//		cout<<numberOfFaces<<endl;
		vector<shared_ptr<Polygon<3,real>>> facesVector({});
		
		istringstream rigaFileStream(rigaFile2);
		
		// per ogni poliedro prendo le facce
		for (long i=0; i<numberOfFaces; i++) {
			string faceString;
			getline(rigaFileStream, faceString, ';');
			istringstream stream2(faceString);
			
			
			//			cout<<faceString<<endl;
			
			// vettore che contiene i punti di ogni faccia
			vector<long> vertexVector;
			vector<shared_ptr<MeshPoint<3,real>>> vertexPointerVector;
			
			
			// conto il numero di vertici per ogni faccia
			long numberOfVertexes=1;
			for (int j=0; j<faceString.size(); j++) {
				if (faceString[j]==',') {
					numberOfVertexes++;
				}
			}
			
			//			cout<<numberOfVertexes<<endl;
			
			// creo il vertexVector
			for (int j=0;j<numberOfVertexes;j++) {
				string pointString;
				getline(stream2, pointString, ',');
				istringstream stream3(pointString);
				
				//				cout<<pointString<<endl;
				long value=0;
				stream3>>value;
				vertexVector.push_back(value);
			}
			
			// creo il vertexPointerVector
			for (int j=0; j<numberOfVertexes; j++) {
				vertexPointerVector.push_back(this->pointVector[vertexVector[j]-1 ]);
			}
			
			// creo la faccia
			
			auto face=newFace(vertexPointerVector);
			
			facesVector.push_back(face);
		}
		
		auto polyhedron=Polyhedron<3,real>::make_shared_Polyhedron(facesVector);
		this->elementVector.push_back(polyhedron);
		
		
		
		
		
		
		
	}
	
}

// setBoundaryElements
template <typename real>
void Mesh3D<real>::setBoundaryElements() {
	
	for (int i=0; i<polygonVector.size(); i++) {
		if (polygonVector[i]->getIsBoundary()) {
			for (int j=0; j<polygonVector[i]->numberOfPoints; j++) {
				polygonVector[i]->point(j)->setIsBoundary(true);
			}
			if (auto polyhedron=polygonVector[i]->polyhedron(0).lock()){
				polyhedron->setIsBoundary(true);
			} else {
				cout<<"Polyhedron is expired"<<endl;
			}
		}
		
	}
}

// setRemainingThings
template <typename real>
void Mesh3D<real>::setRemainingThings() {
	// setta i pointID e le ultime proprietà
	this->numberOfElements=this->elementVector.size();
	this->numberOfPoints=this->pointVector.size();
	numberOfPolygons=polygonVector.size();
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
	
	shrink_to_fit();
}

// setTetrahedronMesh
template <typename real>
void Mesh3D<real>::setTetrahedronMesh(string connection) {
	
	ifstream file2;
	file2.open(connection);
	string rigaFile2;
	
	int counter=0;
	while (getline(file2, rigaFile2)) {
		long numberOfVertex=0;
		for (int i=0; i<rigaFile2.size(); i++) {
			if (rigaFile2[i]==',') {
				numberOfVertex++;
			}
		}
		vector<long> vertexVector;
		vector<shared_ptr<MeshPoint<3,real>>> vertexPointerVector;
		istringstream rigaFileStream(rigaFile2);
		
		for (long i=0; i<numberOfVertex+1; i++) {
			string stringaDiSupporto;
			getline(rigaFileStream, stringaDiSupporto, ',');
			istringstream stream2(stringaDiSupporto);
			
			long value;
			stream2>>value;
			vertexVector.push_back(value);
			
		}
		// ho ottenuto i vertici del poliedro
		
		for (int i=0; i<numberOfVertex+1; i++) {
			vertexPointerVector.push_back(this->pointVector[vertexVector[i]-1]);
		}
		
		auto p1=vertexPointerVector[0];
		auto p2=vertexPointerVector[1];
		auto p3=vertexPointerVector[2];
		auto p4=vertexPointerVector[3];
		
		
		auto poly1=newFace(p1,p2,p3);
		auto poly2=newFace(p1,p3,p4);
		auto poly3=newFace(p4,p1,p2);
		auto poly4=newFace(p2,p3,p4);
		
		
		counter++;
		auto polyhedron=Polyhedron<3,real>::make_shared_Polyhedron(poly1,poly2,poly3,poly4);
		
		
		this->elementVector.push_back(polyhedron);
		
		
	}
	
}


template <typename real>
void Mesh3D<real>::shrink_to_fit() {
	Mesh<3,Polyhedron<3,real>,OPEN,real>::shrink_to_fit();
	polygonVector.shrink_to_fit();
	for (int i=0; i<this->pointVector.size(); i++) {
		this->pointVector[i]->shrink_to_fit();
	}
	for (int i=0; i<this->elementVector.size(); i++) {
		this->elementVector[i]->shrink_to_fit();
	}
	for (int i=0; i<polygonVector.size(); i++) {
		polygonVector[i]->shrink_to_fit();
	}
}












#endif
