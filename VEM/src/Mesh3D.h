
#ifndef Mesh3_Mesh3D_h
#define Mesh3_Mesh3D_h

#include <fstream>
#include <sstream>
#include "Mesh.h"

/** Specialized class for 3D Mesh
 *
 *	BaseElement is a Polyhedron.
 *
 *	2 possibilities of file type: tetrahedron and anything3D. In the first for each Polyhedron only the vertex are specified in the connection file. In the second for each Polyhedron all the faces are specified.
 *
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
	/** Method to create a Polygon after having read it.
	 *
	 *	It calls make_shared_Polygon.
	 *	It also checks if the Polygon is on the boundary or not. It's faster to do it now, rather than after having read everything.
	 */
	template <typename... Args >
	shared_ptr<Polygon<3,real>> newFace(Args... arguments);
	
	/** Specilization of the method
	 *
	 *	Simple method, the main work is done in newFace
	 */
	virtual void setBoundaryElements();
	virtual void setRemainingThings(); //!< Specilization of the method
	
	
	// methods to set the elementVector
	/** Mesh of TETRAHEDRON type
	 *
	 *	e.g. This is a Polyhedron, where 10 is the 10th point in the pointVector and each ';' describe a face:
	 *	1,10,13,4;1,10,11,2;10,13,14,11;13,4,5,14;4,1,2,5;2,11,14,5
	 *
	 */
	virtual void setTetrahedronMesh(string connection);
	
	/** Mesh of ANYTHING3D type
	 *
	 *  e.g. This is a Polyhedron, where 120 is the 120th point in the pointVector:
	 *	120,124,119,155
	 *
	 */
	virtual void setAnything3DMesh(string connection);
	
	void shrink_to_fit();
	
	// GET-SET METHODS
	/** Get Polygon with index
	 */
	const shared_ptr<Polygon<3,real>>& polygon(long index) const {return polygonVector[index%numberOfPolygons];};
	
};


// STANDARD METHODS

template <typename real>
template <typename... Args>
shared_ptr<Polygon<3,real>> Mesh3D<real>::newFace(Args ...arguments) {
	auto newPolygon=Polygon<3,real>::make_shared_Polygon(arguments...);
	auto& point1=*newPolygon->point(0);
	for (int i=0; i<point1.numberOfPolygons(); i++) {
		auto weakPolygon=point1.polygon(i);
		if (auto polygon=weakPolygon.lock()) {
			if (*polygon==*newPolygon && polygon!=newPolygon) { // different pointer but equal polygons
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
	if (!file2.is_open()) {
		throw std::runtime_error("Problem reading inputConnection file.");
	}
	
	string rigaFile2;
	
	while (getline(file2, rigaFile2)) {
		// it counts the number of faces
		long numberOfFaces=1;
		for (int i=0; i<rigaFile2.size(); i++) {
			if (rigaFile2[i]==';') {
				numberOfFaces++;
			}
		}
		
		vector<shared_ptr<Polygon<3,real>>> facesVector({});
		
		istringstream rigaFileStream(rigaFile2);
		
		// for each polyhedron I take the faces
		for (long i=0; i<numberOfFaces; i++) {
			string faceString;
			getline(rigaFileStream, faceString, ';');
			istringstream stream2(faceString);
			
			
			vector<long> vertexVector; // for each vertex in the Polygon to create, in this vector is stores his position in pointVector
			vector<shared_ptr<MeshPoint<3,real>>> vertexPointerVector; // vector of pointer to vertexes of the Polygon
			
			
			// I count the number of vertexes for each face
			long numberOfVertexes=1;
			for (int j=0; j<faceString.size(); j++) {
				if (faceString[j]==',') {
					numberOfVertexes++;
				}
			}
			
			
			// I create the vertexVector
			for (int j=0;j<numberOfVertexes;j++) {
				string pointString;
				getline(stream2, pointString, ',');
				istringstream stream3(pointString);
				
				long value=0;
				stream3>>value;
				vertexVector.push_back(value);
			}
			
			// I creates vertexPointerVector
			for (int j=0; j<numberOfVertexes; j++) {
				vertexPointerVector.push_back(this->pointVector[vertexVector[j]-1 ]);
			}
			
			// I create the face
			auto face=newFace(vertexPointerVector);
			
			facesVector.push_back(face);
		}
		
		// I create the Polyhedron
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
	// It sets pointIDs and last properties
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
	if (!file2.is_open()) {
		throw std::runtime_error("Problem reading inputConnection file.");
	}
	
	string rigaFile2;
	
	int counter=0;
	while (getline(file2, rigaFile2)) {
		long numberOfVertex=0;
		for (int i=0; i<rigaFile2.size(); i++) {
			if (rigaFile2[i]==',') {
				numberOfVertex++;
			}
		}
		vector<long> vertexVector; // for each vertex in the Polygon to create, in this vector is stores his position in pointVector
		vector<shared_ptr<MeshPoint<3,real>>> vertexPointerVector; // vector of pointer to vertexes of the Polygon
		istringstream rigaFileStream(rigaFile2);
		
		for (long i=0; i<numberOfVertex+1; i++) {
			string stringaDiSupporto;
			getline(rigaFileStream, stringaDiSupporto, ',');
			istringstream stream2(stringaDiSupporto);
			
			long value;
			stream2>>value;
			vertexVector.push_back(value);
			
		}
		// I obtain vertexes of the Polyhedron
		for (int i=0; i<numberOfVertex+1; i++) {
			vertexPointerVector.push_back(this->pointVector[vertexVector[i]-1]);
		}
		
		// gets the Points
		auto p1=vertexPointerVector[0];
		auto p2=vertexPointerVector[1];
		auto p3=vertexPointerVector[2];
		auto p4=vertexPointerVector[3];
		
		// makes the Polygons
		auto poly1=newFace(p1,p2,p3);
		auto poly2=newFace(p1,p3,p4);
		auto poly3=newFace(p4,p1,p2);
		auto poly4=newFace(p2,p3,p4);
		
		
		counter++;
		// makes the Polyhedron
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
