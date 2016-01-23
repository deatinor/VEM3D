//
//  Polyhedron.h
//  Mesh2
//
//  Created by Stefano on 31/07/15.
//  Copyright (c) 2015 Stefano. All rights reserved.
//

#ifndef Mesh2_Polyhedron_h
#define Mesh2_Polyhedron_h

#include <algorithm>

template <long embedded,typename real=double,typename ...Args>
shared_ptr<Polyhedron<embedded, real>> make_shared_Polyhedron(Args... arguments) {
	auto polyhedron=make_shared<Polyhedron<embedded,real>>(arguments...);
	polyhedron->initialize();
	return polyhedron;
}

//template <long embedded,typename real=double,typename ...Args>
//Polyhedron<embedded, real> make_Polyhedron(Args... arguments) {
//	Polyhedron<embedded,real> polyhedron(arguments...);
//	polyhedron->initialize();
//	return polyhedron;
//}

template <long embedded, typename real=double>
class Polyhedron: public enable_shared_from_this<Polyhedron<embedded,real>> {
protected:
	// PROPERTIES
	vector<shared_ptr<Polygon<embedded,real>>> polygonVector;
	
	bool isBoundary;
	real volume;
	Point<embedded,real> centroid;

	vector<shared_ptr<MeshPoint<embedded,real>>> pointVector;

	
public:
	//PROPERTIES
	long numberOfPoints;
	long numberOfPolygons;
	
	// CONSTRUCTORS
	Polyhedron(const vector<shared_ptr<Polygon<embedded,real>>>& inputPolygonVector):polygonVector(inputPolygonVector),numberOfPolygons(inputPolygonVector.size()),isBoundary(false),pointVector({}),volume(0) {
	};
//	Polyhedron(const Polyhedron<embedded,real>& inputPolyhedron):polygonVector(inputPolyhedron.polygonVector),numberOfPolygons(inputPolyhedron.numberOfPolygons),pointVector(inputPolyhedron.pointVector),isBoundary(inputPolyhedron.isBoundary),volume(0) {
//	};
	Polyhedron():polygonVector({}),numberOfPolygons(0),isBoundary(false),pointVector({}),volume(0){};
	//costruttore con variadic template: http://stackoverflow.com/questions/8158261/templates-how-to-control-number-of-constructor-args-using-template-variable
	template <typename... Args>
	Polyhedron(Args... arguments):polygonVector{arguments...},pointVector({}),volume(0),isBoundary(false) {
		if (sizeof...(Args)<4) {
			cout<<endl<<endl<<"Minimum 4 polygons required"<<endl<<endl;
		}
		numberOfPolygons=sizeof...(Args);
	};
	
	// STANDARD METHODS
	void addPolygon(shared_ptr<Polygon<embedded,real>>& inputPolygon);
	void addPoint(shared_ptr<MeshPoint<embedded,real>>& inputPoint);
	Point<embedded,real> computeCentroid();
	real computeVolume();
	real getDiameter();
	void fixExternalNormal();
	void fixFacesOrientation();
	real hTriangle();
	void initialize();
	void linkPoints();
	void linkPolygons();
//	void putSharedFacesInFront();
	void shrink_to_fit();
	real space() {return volume;} // serve per ritornare il volume se dall'esterno non è chiaro se è un poligono o un poliedro
	void switchFacesOrientation();
	void updatePointVector();
	string write();
	
	// GET-SET METHODS
	bool getIsBoundary() const {return isBoundary;};
	void setIsBoundary(bool inputIsBoundary) {isBoundary=inputIsBoundary;};
	real getVolume() const {return volume;};
	const Point<embedded, real>& getCentroid() const {return centroid;};
	const shared_ptr<MeshPoint<embedded,real>>& point(long index) const {return pointVector[index%numberOfPoints];};
	const shared_ptr<Polygon<embedded,real>>& polygon(long index) const {return polygonVector[index%numberOfPolygons];};
	
	// OPERATORS
	shared_ptr<Polygon<embedded,real>> operator [](long index);


	// EXTERNAL METHODS
	template<long embedded2,typename real2>
	friend ostream& operator<<(ostream& os,const Polyhedron<embedded2,real2>&polyhedron);
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
//	cout<<"volume start"<<endl;
	
	for (int i=0;i<numberOfPolygons;i++) {
//		cout<<"face: "<<i<<endl;
		auto& face=*polygonVector[i];
		real normalX=face.getNormal().x();
//		cout<<"normal and centroid: "<<endl<<face.normal<<face.centroid();
//		real x=0;
//		for (int j=0;j<face.numberOfPoints;j++) {
		real x=face.getCentroid().x();
//		cout<<"centroid x: "<<x<<endl;
//		}
		x=x*face.getArea();//face.numberOfPoints;
//		cout<<"area: "<<face.area<<endl;
		volume+=normalX*x;
	}
	
//	cout<<"volume end: "<<volume<<endl;
	return abs(volume);
}
			
// diameter
template <long embedded,typename real>
real Polyhedron<embedded,real>::getDiameter() {
//	return 1;  // ATTENZIONE CAMBIA!!!
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
	// controlla che non ci siano altri poliedri con faccia in comune a questo: nel caso prende la normale opposta
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
//						cout<<(facePointer->normal-face2Pointer->normal).norm();
//						cout<<2<<endl;
						return;
					}
				}
			}
		}
	}
	
	
	// attenzione! questo punto dovrebbe essere selezionato casualmente nell'inviluppo convesso della faccia 1,
	// si deve controllare che sia dentro la faccia per evitare problemi al bordo
	Point<3,real> p1(0,0,0);//*(*polygonVector[0])[0];


	// guardo quante facce interseca
	// attenzione! se interseca una faccia in uno spigolo o in un vertice cambio il punto iniziale e rifaccio il procedimento
	// dal momento che il punto iniziale e' scelto a caso non dovrebbe succedere spesso
	bool isP1Correct=false;
	long numberOfIntersection=0;

	while (isP1Correct==false) {
		// genera un nuovo p1 e controlla che sia in face1

		while (isP1Correct==false) {
			isP1Correct=true;
			p1[0]=0;
			p1[1]=0;
			p1[2]=0;
			
			//crea il weightVector
			vector<real> weightVector;
			
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<> dis(0, 1);
			for (int n = 0; n < face1.numberOfPoints; ++n) {
				weightVector.push_back(dis(gen));
			}
			// lo bilancia per avere somma 1
			real sum=0;
			for (int i=0; i<weightVector.size(); i++) {
				sum+=weightVector[i];
			}
			for (int i=0; i<weightVector.size(); i++) {
				weightVector[i]/=sum;
			}
			// genera P1
			for (int i=0; i<face1.numberOfPoints; i++) {
				p1=p1+weightVector[i]*(*face1[i]);
			}
//			cout<<" genero P1"<<p1<<endl;
			
			// controlla P1
			if (face1.isConflictPoint(p1)==true || face1.isPointInside(p1)==false) {
				isP1Correct=false;
			}
			
		}

		
		isP1Correct=true;
//		cout<<"face1: "<<face1<<endl;
//		cout<<"p1: "<<p1<<endl;
//		cout<<"face1 normal: "<<face1.normal;
		numberOfIntersection=0;
		for (int i=1; i<numberOfPolygons; i++) {
			auto& face2=*polygonVector[i];
			auto normal2=face2.getNormal(); //	NON METTERE LA REFERENCE, VIENE CAMBIATA
			auto& point1=*face2[0]; // 3 punti che formano il piano
//			auto& point2=*face2[1];
//			auto& point3=*face2[2];
//			
//			cout<<"finding normal inside"<<endl;
//			cout<<"normal2 start: "<<normal2<<endl;
			// elimino il caso di facce perpendicolari
			if (abs(normal1*normal2)<1e-13) {
				continue;
			}
			
			// individuo il piano formato dai 3 punti di face 2
			// piano nella forma ax+by+cz=d
//			Vector<3,real> coefficients=normal2;
			real d=0;
		
			d=(*face2[0])*normal2;
			
//			cout<<"normal2.z(): "<<normal2.z()<<endl;
			
			// ho un piano coefficients*x=d
			
//			cout<<"coefficients: "<<coefficients<<endl;
//			normal2=coefficients/coefficients.norm();  // devo correggere il segno di normal2, è importante per il segno corretto di distance
			
			// calcolo la distanza tra un punto del polygon1 e il piano formato da face2
			real distance=(normal2*p1-d)/normal2.norm();  // p1-normal2*distance appartiene al piano
			
			auto distanceVector=distance*normal2;
			// se il prodotto con normal1 è positivo non interseca il piano, altrimenti trovo il punto di intersezione
//			cout<<"d: "<<d<<endl;
//			cout<<"normal1: "<<normal1<<endl;
//			cout<<"normal2: "<<normal2<<endl;
//			cout<<"distance: "<<distance<<endl;
//			cout<<"distanceVector: "<<distanceVector<<endl;
//			cout<<"prodotto: "<<normal1*distanceVector<<endl;
//			cout<<"face:  "<<face2<<endl;
			
			if (normal1*distanceVector>-1e-13) {
				continue;
			}

//			cout<<"p1: "<<p1<<endl;
//			cout<<"normal1: "<<normal1<<endl;
//			cout<<"distance: "<<distance<<endl;
//			cout<<"normal1*normal2: "<<normal1*normal2<<endl;
			auto intersectionPoint=p1+normal1*distance/(-(normal1*normal2));
//			cout<<"intersection point: "<<intersectionPoint<<endl;


			
//			cout<<endl<<"inizio "<<endl;
//			cout<<"coefficients"<<coefficients;
//			cout<<"normal1"<<normal1;
//			cout<<"normal2"<<normal2;
//			cout<<"distance "<<distance<<endl;
//			cout<<"distanceVector "<<distanceVector<<endl;
//			cout<<"intersectionPoint"<<p1-normal2*distance<<intersectionPoint<<endl<<endl;
//			cout<<"face "<<face2;

			
			// ho trovato il punto di intersezione con il piano, devo controllare che non sia su uno spigolo o su un vertice
			if (face2.isConflictPoint(intersectionPoint)) {
				cout<<"CONFLICT POINT"<<endl;
				isP1Correct=false;
				break;
			}
			
//			cout<<"face:  "<<face2<<endl;
//			cout<<"intersection point: "<<intersectionPoint<<endl;
			
			
			if (face2.isPointInside(intersectionPoint)) {
//				cout<<"INSIDEINSIDEINSIDEINSIDEINSIDE"<<endl;
				
				numberOfIntersection++;
//				cout<<numberOfIntersection<<endl;
			}
			
		}
	}
//	cout<<numberOfIntersection<<endl;
	
	
	
	if (numberOfIntersection%2==1) {
		for (int i=0; i<numberOfPolygons; i++) {
			polygonVector[i]->switchPointsOrder();
		}
	}
	
//	cout<<*this;
	
}

// fixFacesOrientation
template <long embedded,typename real>
void Polyhedron<embedded,real>::fixFacesOrientation() {
	
	int nextJ=1;
	for (int i=0; i<numberOfPolygons-1; i++) { // l'ultimo poligono è per forza orientato giusto
		if (i==nextJ) {
			nextJ++;
		}
		
		// per ogni poligono controlla tutti quelli dopo, quelli che ha trovato che sono a lui adiacenti (quindi li ha sistemati) li mette subito dopo di lui
		auto& polygon1=*polygonVector[i];
		for (int j=nextJ; j<numberOfPolygons; j++) {
//			bool flag1=false;
			auto& polygon2=*polygonVector[j];
			
			for (int k=0;k<polygon1.numberOfPoints;k++) {
				bool flag2=false;
				auto point1=polygon1[k];
				auto point2=polygon1[k+1];
				for (int m=0;m<polygon2.numberOfPoints;m++) {
					if (point1==polygon2[m] && point2==polygon2[m+1]) {
//						flag1=true;
						flag2=true;
						polygon2.switchPointsOrder();
						iter_swap(polygonVector.begin()+nextJ,polygonVector.begin()+j);
						nextJ++;
						break;
					}
					if (point1==polygon2[m] && point2==polygon2[m-1]) {
//						flag1=true;
						flag2=true;
						iter_swap(polygonVector.begin()+nextJ,polygonVector.begin()+j);
						nextJ++;
						break;
					}
				}
				if (flag2) break;
			}
//			if (flag1)break;
		}

		
	}
		
		

	
	
	
//	for (int i=1;i<numberOfPolygons;i++) {
//		auto& polygon1=*polygonVector[i];
//		
//		
//		for (int j=0;j<i;j++) {
//			bool flag1=false;
//			auto& polygon2=*polygonVector[j];
//			for (int k=0;k<polygon1.numberOfPoints;k++) {
//				bool flag2=false;
//				auto point1=polygon1[k];
//				auto point2=polygon1[k+1];
//				for (int m=0;m<polygon2.numberOfPoints;m++) {
//					if (point1==polygon2[m] && point2==polygon2[m+1]) {
//						flag1=true;
//						flag2=true;
////						cout<<"face fixed: "<<i<<polygon1<<polygon2<<"Fine faceFixed"<<endl<<endl<<endl<<endl;
////						cout<<"	ORIENTATION"<<endl;
//						polygon1.switchPointsOrder();
//						break;
//					}
//				}
//				if (flag2) break;
//			}
//			if (flag1) break;
//		}
//		
//		
//		
//	}
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
//	cout<<*this;
//	switchFacesOrientation();
	
	fixExternalNormal();
	
//	cout<<*this<<endl;
	
//	for (int i=0;i<numberOfPolygons; i++) {
//		cout<<polygonVector[i]->normal;
//	}
//	cout<<*polygonVector[0];
//	cout<<polygonVector[0]->computeNormal()<<endl;
//	cout<<*polygonVector[1];
//	cout<<polygonVector[1]->normal;
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
			
//// putSharedFacesInFront
//template <long embedded,typename real>
//void Polyhedron<embedded,real>::putSharedFacesInFront() {
//	int firstIndexAvailable=0;
//	for (int i=0;i<numberOfPolygons;i++) {
//		if (polygonVector[i]->polyhedronVector.size()==2) {
//			iter_swap(polygonVector.begin()+firstIndexAvailable, polygonVector.begin()+i);
//			firstIndexAvailable++;
////			cout<<1<<endl;
//		}
//	}
//}

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
void Polyhedron<embedded,real>::updatePointVector() { // il costruttore considera solo i poligoni non anche i punti.
	for (int i=0;i<numberOfPolygons;i++) {
//		auto& polygonPointVector=polygonVector[i]->pointVector;
		
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
