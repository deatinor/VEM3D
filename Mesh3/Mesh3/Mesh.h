//
//  Mesh.h
//  Mesh2
//
//  Created by Stefano on 31/07/15.
//  Copyright (c) 2015 Stefano. All rights reserved.
//

#ifndef Mesh2_Mesh_h
#define Mesh2_Mesh_h

#include <fstream>
#include <sstream>
#include "Point.h"
#include <algorithm>

using namespace std;

template <long embedded,typename real>
class MeshPoint;

template <typename Class>
static bool lessSharedPointer(const shared_ptr<Class>& ptr1,const shared_ptr<Class>& ptr2) {
	return *ptr1<*ptr2;
}

enum OpenEnum{CLOSED,OPEN};
enum MeshType{TETRAHEDRON,TRIANGLE,ANYTHING3D,ANYTHING2D,FILETYPE1,FILETYPE2};
// FILETYPE1 e FILETYPE2 sono nel caso si volesse espandere la classe Mesh, si può ereditare da Mesh o da Mesh3D e specializzare i metodi virtual corrispondenti
// per aggiungerne altri è necessario cambiare il metodo setElementVector

////////////////////////////////////////////////////////////////
//////////                MESH                        //////////
////////////////////////////////////////////////////////////////

template <long embedded,typename baseElement,OpenEnum isOpen=OPEN,typename real=double>
class Mesh {

protected:
	
	vector<shared_ptr<baseElement>> elementVector;
	vector<shared_ptr<MeshPoint<embedded,real>>> pointVector;
	
public:
	long numberOfElements;
	long numberOfPoints;
	
	const shared_ptr<baseElement>& element(long index) const;
	const shared_ptr<MeshPoint<embedded,real>>& point(long index) const;
	const vector<shared_ptr<MeshPoint<embedded,real>>>& getPointVector() const {return pointVector;};

	virtual real hTriangle();
	
	void initialize(string pointFile,string connection,MeshType meshType=ANYTHING3D);
	// queste 4 funzioni sono chiamate in successione dal costruttore, possono essere specializzate se necessario
	virtual void setPointVector(string file);
	virtual void setElementVector(string connections, MeshType meshType); // può essere specializzato nel caso si volesse espandere la classe
	virtual void setBoundaryElements() {cout<<"fail"<<endl;};
	virtual void setRemainingThings() {}; // usato per settare i pointID, se specializzato per fare anche altro
	
	
	// metodi per settare l'elementVector, vanno implementati poi nell child class, solo quelli necessari
	virtual void setTetrahedronMesh(string connection) {};
	virtual void setTriangleMesh(string connection) {};
	virtual void setAnything3DMesh(string connection) {};
	virtual void setAnything2DMesh(string connection) {};
	virtual void setFileType1Mesh(string connection) {};
	virtual void setFileType2Mesh(string connection) {};

	virtual void sort(); // riordina il pointVector in base ai pointID
	
	void shrink_to_fit();
	virtual void write(string outputPoints="points.point",string outputConnections="connections.conn") const;
	virtual void writePoints(string outputFile="points.point") const;
	virtual void writeConnections(string outputFile="connections.conn") const;
	
protected: // costruttori protetti, non deve essere inizializzata questa classe, sebbene non possa essere definita abstract

	
	Mesh():elementVector({}),pointVector({}){};
	
	template <long embedded2,typename baseElement2,OpenEnum isOpen2,typename real2>
	friend ostream& operator<<(ostream& os,const Mesh<embedded2,baseElement2,isOpen2,real2>& mesh);
	
};

// STANDARD METHODS

// hTriangle
template <long embedded,typename baseElement,OpenEnum isOpen,typename real>
real Mesh<embedded,baseElement,isOpen,real>::hTriangle() {
	real maxH=elementVector[0]->hTriangle();
	for (long i=1; i<numberOfElements; i++) {
		real tempMax=elementVector[i]->hTriangle();
		if (tempMax>maxH) {
			maxH=tempMax;
		}
	}
	return maxH;
}

// initialize
template <long embedded,typename baseElement,OpenEnum isOpen,typename real>
void Mesh<embedded,baseElement,isOpen,real>::initialize(string pointFile,string connection,MeshType meshType) {
	
	clock_t startTime;
	double duration;
	startTime=clock();
	
	// inizializzo il pointVector
	setPointVector(pointFile);
	
	duration = ( clock() - startTime ) / (double) CLOCKS_PER_SEC;
	cout<<"PointVector time: "<<duration<<endl<<endl;
	
	startTime=clock();
	
	// inizializzo l'elementVector
	setElementVector(connection, meshType);
	
	duration = ( clock() - startTime ) / (double) CLOCKS_PER_SEC;
	cout<<"ElementVector time: "<<duration<<endl<<endl;
	
	// setto il resto
	setBoundaryElements();
	setRemainingThings();

	sort();

}

// setPointVector
template <long embedded,typename baseElement,OpenEnum isOpen,typename real>
void Mesh<embedded,baseElement,isOpen,real>::setPointVector(string file) {
	ifstream file1;
	file1.open(file);
	string rigaFile1;
	
	
	while (getline(file1, rigaFile1)) {
		istringstream rigaFileStream(rigaFile1);
		auto point=make_shared<MeshPoint<embedded,real>>();
		
		for (long i=0; i<embedded; i++) {
			string stringaDiSupporto;
			getline(rigaFileStream, stringaDiSupporto, ',');
			istringstream stream2(stringaDiSupporto);
			
			stream2>>(*point)[i];
			
		}
		
		
		this->pointVector.push_back(point);
	}
}

// setElementVector
template <long embedded,typename baseElement,OpenEnum isOpen,typename real>
void Mesh<embedded,baseElement,isOpen,real>::setElementVector(string connections, MeshType meshType) {
	switch (meshType) {
		case TETRAHEDRON:
			setTetrahedronMesh(connections);
			break;
		case TRIANGLE:
			setTriangleMesh(connections);
			break;
		case ANYTHING3D:
			setAnything3DMesh(connections);
			break;
		case ANYTHING2D:
			setAnything2DMesh(connections);
			break;
		case FILETYPE1:
			setFileType1Mesh(connections);
			break;
		case FILETYPE2:
			setFileType2Mesh(connections);
			break;
			
		default:
			break;
	}

}

// element
template <long embedded,typename baseElement,OpenEnum isOpen,typename real>
const shared_ptr<baseElement>& Mesh<embedded,baseElement,isOpen,real>::element(long int index) const{
	if (index>elementVector.size()) {
		cout<<endl<<endl<<"poligonVector size less than index given"<<endl<<endl;
	}
	
	return elementVector[index%numberOfElements];
}

// point
template <long embedded,typename baseElement,OpenEnum isOpen,typename real>
const shared_ptr<MeshPoint<embedded,real>>& Mesh<embedded,baseElement,isOpen,real>::point(long index) const{
	if (index>pointVector.size()) {
		cout<<endl<<endl<<"pointVector size less than index given"<<endl<<endl;
	}
	
	return pointVector[index%numberOfPoints];
}

// sort
template <long embedded,typename baseElement,OpenEnum isOpen,typename real>
void Mesh<embedded,baseElement,isOpen,real>::sort() {
	std::sort(pointVector.begin(),pointVector.end(),lessSharedPointer<MeshPoint<embedded,real>>);
}

// shrink_to_fit
template <long embedded,typename baseElement,OpenEnum isOpen,typename real>
void Mesh<embedded,baseElement,isOpen,real>::shrink_to_fit() {
	elementVector.shrink_to_fit();
	pointVector.shrink_to_fit();
}

// write
template <long embedded,typename baseElement,OpenEnum isOpen,typename real>
void Mesh<embedded,baseElement,isOpen,real>::write(string outputPoints,string outputConnections) const{
	writePoints(outputPoints);
	writeConnections(outputConnections);
}

// writePoints
template <long embedded,typename baseElement,OpenEnum isOpen,typename real>
void Mesh<embedded,baseElement,isOpen,real>::writePoints(string outputFile) const{
	ofstream file;
	file.open(outputFile);
	for (int i=0; i<pointVector.size(); i++) {
		file<<pointVector[i]->write();
		file<<"\n";
	}
	file.close();
}

// writeConnections
template <long embedded,typename baseElement,OpenEnum isOpen,typename real>
void Mesh<embedded,baseElement,isOpen,real>::writeConnections(string outputFile) const{
	ofstream file;
	file.open(outputFile);
	for (int i=0; i<elementVector.size(); i++) {
		file<<elementVector[i]->write();
		file<<"\n";
	}
	file.close();
}


// operator <<
template <long embedded,typename baseElement,OpenEnum isOpen,typename real>
ostream& operator<<(ostream& os,const Mesh<embedded,baseElement,isOpen,real>& mesh) {
	cout<<endl<<endl;
	cout<<"POINT VECTOR"<<endl<<endl;
	
	for (int i=0; i<mesh.pointVector.size(); i++) {
		cout<<*mesh.pointVector[i];
	}
	cout<<endl<<endl<<endl;
	
	cout<<"ELEMENT VECTOR"<<endl<<endl;
	
	for (int i=0; i<mesh.elementVector.size(); i++) {
		cout<<*(mesh.elementVector[i]);
	}
	
	cout<<endl<<"There are "<<mesh.pointVector.size()<<" points";
	cout<<endl<<"There are "<<mesh.elementVector.size()<<" elements"<<endl<<endl;
	
	return os;
	
	
	
}









#endif
