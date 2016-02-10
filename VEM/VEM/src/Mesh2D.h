
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
	/** This is to obtain internal and external points
	 *
	 *	There is no class Edge, to simplify the mesh creation. In this case a pair is an edge.
	 *	All the pairs that are present only one time in the vector correspond to the boundary edges.
	 */
	vector<pair<long,long>> pairVector;

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
	
	/** Mesh of ANYTHING2D type
	 *
	 *  e.g. This is a Polygon, where 120 is the 120th point in the pointVector. Points are ordered:
	 *	120,124,119,155
	 *
	 */
	virtual void setAnything2DMesh(string connection);
	
	/** Specilization of the method
	 *
	 *	It uses the pairVector to accelerate the computation
	 */
	virtual void setBoundaryElements();
	
	/** Specilization of the method
	 *
	 *	It sets pointIDs and numberOfBoundaryPoints
	 */
	virtual void setRemainingThings();
	
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
	string fileRow2;

	
	while (getline(file2, fileRow2)) {
		// it counts the number of points
 		long numberOfVertexes=1;
		for (int i=0; i<fileRow2.size(); i++) {
			if (fileRow2[i]==',') {
				numberOfVertexes++;
			}
		}
		
		istringstream rigaFileStream(fileRow2);
	
		vector<long> vertexVector; // for each vertex in the Polygon to create, in this vector is stores his position in pointVector
		vector<shared_ptr<MeshPoint<2,real>>> vertexPointerVector; // vector of pointer to vertexes of the Polygon
			
		
		// I create the vertexVector
		for (int i=0;i<numberOfVertexes;i++) {
			string pointString;
			getline(rigaFileStream, pointString, ',');
			istringstream stream3(pointString);
			
			long value=0;
			stream3>>value;
			vertexVector.push_back(value);
		}
		
		// I create the pairVector
		for (int i=0; i<numberOfVertexes; i++) {
			pair<long,long> pair;
			if (vertexVector[i]<vertexVector[(i+1)%numberOfVertexes]){
				pair=make_pair(vertexVector[i],vertexVector[(i+1)%numberOfVertexes]);
			} else {
				pair=make_pair(vertexVector[(i+1)%numberOfVertexes],vertexVector[i]);
			}
			pairVector.push_back(pair);
		}
			
		// I create the vertexPointerVector
		for (int i=0; i<numberOfVertexes; i++) {
			vertexPointerVector.push_back(this->pointVector[vertexVector[i]-1 ]);
		}
			
		// I create the Polygon
		auto polygon=Polygon<2,real>::make_shared_Polygon(vertexPointerVector);

		this->elementVector.push_back(polygon);
	}
	

	
}

template <typename real>
void Mesh2D<real>::setBoundaryElements() {
	
	// I reorder the pairVector to compare faster the edges
	sort(pairVector.begin(),pairVector.end(),lessPair);
	
	// I set all the points with isBoundary=false
	for (int i=0;i<this->pointVector.size();i++) {
		this->pointVector[i]->setIsBoundary(false);
	}
	

	// I compare the pairs
	for (int i=0;i<pairVector.size()-1;i++) { // I exclude the last pair
		if (pairVector[i].first!=pairVector[i+1].first || pairVector[i].second!=pairVector[i+1].second) {
			long point1=pairVector[i].first;
			long point2=pairVector[i].second;
			
			this->pointVector[point1-1]->setIsBoundary(true);
			this->pointVector[point2-1]->setIsBoundary(true);
		} else {
			i++;
			// if equals it jumps to the next pair
		}
	}
	// the last pair is not considered, all the boundary points are already found

	
	// set isBoundary on the polygons
	auto& pointVector=this->pointVector;
	for (int i=0; i<pointVector.size(); i++) {
		auto& point=pointVector[i];
		if (point->getIsBoundary()) {
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
	// set the pointIDs and the next properties
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
