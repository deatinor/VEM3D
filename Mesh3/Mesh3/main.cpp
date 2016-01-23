//
//  main.cpp
//  Mesh3
//
//  Created by Stefano on 01/08/15.
//  Copyright (c) 2015 Stefano. All rights reserved.
//


#include <iostream>
#include <random>
#include "Point.h"
#include "Polygon.h"
#include "Polyhedron.h"
#include "Mesh.h"
#include "Laplace.h"
#include "Monomials.h"
#include "Solver.h"
#include "BoundaryCondition.h"
#include "Error.h"
#include "Mesh3D.h"
#include "Mesh2D.h"
#include "SolverVEM3D.h"
#include "SolverVEM2D.h"
#include "Eigen/Dense"

// FUNZIONI
template <long embedded,typename real=double>
real forceTermSphere(Point<embedded,real>& inputPoint) {
	auto ro=inputPoint.norm();
	
//	return -2*exp(ro)/ro+5*exp(ro)+6*ro*exp(ro)+ro*ro*exp(ro);
	
	return 6;
	
}

template <long embedded,typename real=double>
real forceTermSquare(Point<embedded,real>& inputPoint) {
	if (embedded==2) {
		auto x=inputPoint[0];
		auto y=inputPoint[1];
		
		return -2*(y*y-1)-2*(x*x-1);
	}
	else {
		auto x=inputPoint[0];
		auto y=inputPoint[1];
		auto z=inputPoint[2];
		
		return -2*(y*y-1)*(z*z-1)-2*(x*x-1)*(z*z-1)-2*(x*x-1)*(y*y-1);
	}
}

template <long embedded,typename real=double>
real boundaryFunction(Point<embedded,real>& inputPoint) {
	auto x=inputPoint[0];
	auto y=inputPoint[1];
	auto z=inputPoint[2];
	
	return 0;
//	return -2*(y*y-1)*(z*z-1)-2*(x*x-1)*(z*z-1)-2*(x*x-1)*(y*y-1);
}

template <long embedded,typename real=double>
real solutionSphere(Point<embedded,real>& inputPoint) {
	auto ro=inputPoint.norm();
	
//	return (1-ro*ro)*exp(ro);
	
	return 1-ro*ro;
	
}

template <long embedded,typename real=double>
real solutionSquare(Point<embedded,real>& inputPoint) {
	if (embedded==2) {
		auto x=inputPoint[0];
		auto y=inputPoint[1];
		
		return (x*x-1)*(y*y-1);
	}
	else {
		auto x=inputPoint[0];
		auto y=inputPoint[1];
		auto z=inputPoint[2];
	
		return (x*x-1)*(y*y-1)*(z*z-1);
	}
}

int main(int argc, const char * argv[]) {
	
	
	// PUNTI DI TEST
//	
//	auto p1=make_shared<MeshPoint<3>>(0,0,0);
//	auto p2=make_shared<MeshPoint<3>>(2,0,0);
//	auto p3=make_shared<MeshPoint<3>>(3,0,0);
//	auto p4=make_shared<MeshPoint<3>>(1.5,3,0);
//	auto p5=make_shared<MeshPoint<3>>(0,0,1);
//	auto p6=make_shared<MeshPoint<3>>(1,0,1);
//	auto p7=make_shared<MeshPoint<3>>(1,1,1);
//	auto p8=make_shared<MeshPoint<3>>(0,1,1);
//
//
//	
//	
//	clock_t startTime;
//	double duration;
//	startTime=clock();

	
	// TIPI DIVERSI DI MESH, LASCIARE SCOMMENTATA SOLO QUELLA CHE SI VUOLE, PER LAPLACE TUTTO UGUALE
	
//	Mesh3D<> mesh("/Users/stefano/Dropbox/Condivise/Ste-Nico/Progetto/Mesh/Sphere3D/p_0_10.txt","/Users/stefano/Dropbox/Condivise/Ste-Nico/Progetto/Mesh/Sphere3D/t_0_10.txt",TETRAHEDRON);
//	Mesh3D<> mesh("/Users/stefano/Dropbox/Condivise/Ste-Nico/Progetto/Mesh3/Mesh3/points.point","/Users/stefano/Dropbox/Condivise/Ste-Nico/Progetto/Mesh3/Mesh3/connections.conn",ANYTHING3D);

	
//	Mesh3D<> mesh("/Users/stefano/Dropbox/Condivise/Ste-Nico/Progetto/Mesh/Test/point0.txt","/Users/stefano/Dropbox/Condivise/Ste-Nico/Progetto/Mesh/Test/conn0.txt",TETRAHEDRON);

//	Mesh3D<> mesh("/Users/stefano/Dropbox/Condivise/Ste-Nico/Progetto/Mesh/Test/point1.txt","/Users/stefano/Dropbox/Condivise/Ste-Nico/Progetto/Mesh/Test/conn1.txt",ANYTHING3D);

//	Mesh3D<> mesh("/Users/stefano/Dropbox/Condivise/Ste-Nico/Progetto/Mesh/Test/point5.txt","/Users/stefano/Dropbox/Condivise/Ste-Nico/Progetto/Mesh/Test/conn5.txt",ANYTHING3D);
	
//	mesh.writePoints();
//	mesh.writeConnections();
	
	
	
	
	
//	duration = ( clock() - startTime ) / (double) CLOCKS_PER_SEC;
//	cout<<"Completed: "<<duration<<endl<<endl;

	// TEST SULLA MESH
	
//	auto& pointVector=mesh.pointVector;
//	for (int i=0; i<pointVector.size(); i++) {
////		if (pointVector[i]->isBoundary==true) {
//		
//			cout<<pointVector[i]->pointID<<"     "<<pointVector[i]->isBoundary<<endl;
////		}
//	}

	// CREO UN POLIEDRO, ORA POSSO FARLO ANCHE USANDO UN .txt E CREANDO LA MESH
	
//	shared_ptr<Polygon<3>> pp1=make_shared_Polygon<3>(p1,p2,p3,p4);
//	auto pp2=make_shared_Polygon<3>(p1,p2,p6,p5);
//	auto pp3=make_shared_Polygon<3>(p2,p3,p7,p6);
//	auto pp4=make_shared_Polygon<3>(p3,p4,p8,p7);
//	auto pp5=make_shared_Polygon<3>(p4,p1,p5,p8);
//	auto pp6=make_shared_Polygon<3>(p5,p6,p7,p8);
////	cout<<"ok";
//	auto pp7=make_shared_Polygon<3>(p1,p2,p3);
//	auto pp8=make_shared_Polygon<3>(p1,p2,p4);
//	auto pp9=make_shared_Polygon<3>(p1,p3,p4);
//	auto pp10=make_shared_Polygon<3>(p2,p3,p4);



//	auto poly=make_shared_Polyhedron<3>(pp1,pp2,pp3,pp4,pp5,pp6);
	
	
//	auto poly2=make_shared_Polyhedron<3>(pp7,pp8,pp9,pp10);

	
	
	
	/// PROBLEMA SUL POLIEDRO CREATO
	
	//	Monomials<3, Polyhedron<3>> monomial(poly2);
	//	auto G=problem.computeG(monomial);
	////	cout<<"G"<<endl<<endl<<G<<endl<<endl;
	//	auto D=problem.computeD(monomial);
	////	cout<<"D"<<endl<<endl<<D<<endl<<endl;
	//	auto B=problem.computeB(poly2, monomial);
	////	cout<<"B"<<endl<<endl<<B<<endl<<endl;
	//	auto PIStar=problem.computePIStar(G, B);
	////	cout<<"PIStar"<<endl<<endl<<PIStar<<endl<<endl;
	//	auto PI=problem.computePI(PIStar, D);
	////	cout<<"PI"<<endl<<endl<<PI<<endl<<endl;
	//	auto K=problem.computeK(G, PIStar, PI, poly2->diameter());
	////	cout<<"K"<<endl<<endl<<K<<endl<<endl;
	
	//	cout<<"test"<<endl<<endl;
	//	cout<<B*D<<endl<<endl;
	
	
	// PROBLEM LAPLACE
//	
//	
//	Laplace<3,Mesh3D<>,SolverVEM3D<>,Dirichlet3D<>> problem(mesh,forceTerm<3>,boundaryFunction<3>,1);
//	problem();
//	
	
//	cout<<problem.stiffnessMatrix;
	
//	problem.computeKnownTerm();
	
	
	

	
//	problem();
//	problem.displayError(realSolution<3>);
	
//	cout<<mesh;
	
	
//	Mesh2D<> mesh2("/Users/stefano/Dropbox/Condivise/Ste-Nico/Progetto/Mesh/Mesh2D/point2.txt","/Users/stefano/Dropbox/Condivise/Ste-Nico/Progetto/Mesh/Mesh2D/conn2.txt",ANYTHING2D);
//	Mesh2D<> mesh2("/Users/stefano/Dropbox/Condivise/Ste-Nico/Progetto/Mesh3/Mesh3/points.point","/Users/stefano/Dropbox/Condivise/Ste-Nico/Progetto/Mesh3/Mesh3/connections.conn",ANYTHING2D);


//	cout<<mesh2;
	
//	mesh2.writePoints();
//	mesh2.writeConnections();
	
//	Laplace<2, Mesh2D<>, SolverVEM2D<>, Dirichlet2D<>> problem2(mesh2,forceTerm2D<2,double>,boundaryFunction<2,double>,1);
//	problem2();
//	problem2.displayError(realSolution2D<2,double>);
//	mesh.writeConnections();

	// ANALISI FINALI
//	auto& pointVector3=mesh2.pointVector;
//	for (int i=0;i<pointVector3.size();i++) {
//		cout<<*pointVector3[i];
//		cout<<pointVector3[i]->pointID<<"   "<<pointVector3[i]->isBoundary<<endl<<endl<<endl;
//	}
//
//	cout<<mesh.numberOfBoundaryPoints<<endl;


//		SolverVEM3D<double> prova(forceTerm<3>);
//	prova.computeLocalK(mesh.elementVector[0]);
	
//	MonomialsPolygon<double> monomial(pp1);
	
//	auto G=prova.computeGPolygon(monomial);
//	cout<<"G: "<<endl<<G<<endl;
	
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	
	
	
	
	
	////////////  PARAMETRI SETTABILI  /////////////
	
	long embedded=3;
	string inputPoint;
	string inputConnection;
	string meshType;
	string fileType;
	string solverType;
	string boundaryCondition;
	string outputPoint;
	string outputConnection;
	string outputSolution;
	string forceTermFunction;
	string solutionFunction;
	string real;
	
	
	
	////////////  LEGGO IL FILE  ///////////////////
	string parametroInput,valore,linea;
	ifstream inputFile("Datafile/datafile");
	
	
	
	while (getline(inputFile, linea)) {
		// elimino gli spazi bianchi
		std::string::iterator end_pos = std::remove(linea.begin(), linea.end(), ' ');
		linea.erase(end_pos, linea.end());
		
		if (linea=="") {
			continue;
		}
		if (linea.substr(0,2)=="//") {
			continue;
		}
		
		// ottengo parametri di input e il loro valore
		stringstream iss(linea);
		getline(iss, parametroInput, '=');
		getline(iss, valore, ';');
		
		// sovrascrivo i parametri di input trovati
		if (parametroInput=="inputPoint") {
			inputPoint=valore;
		}
		if (parametroInput=="inputConnection") {
			inputConnection=valore;
		}
		if (parametroInput=="meshType") {
			meshType=valore;
			if (meshType=="Mesh3D") {
				embedded=3;
			} else if (meshType=="Mesh2D"){
				embedded=2;
			}
		}
		if (parametroInput=="fileType") {
			fileType=valore;
		}
		if (parametroInput=="solverType") {
			solverType=valore;
		}
		if (parametroInput=="boundaryCondition") {
			boundaryCondition=valore;
		}
		if (parametroInput=="outputPoint") {
			outputPoint=valore;
		}
		if (parametroInput=="outputConnection") {
			outputConnection=valore;
		}
		if (parametroInput=="outputSolution") {
			outputSolution=valore;
		}
		if (parametroInput=="forceTermFunction") {
			forceTermFunction=valore;
		}
		if (parametroInput=="solutionFunction") {
			solutionFunction=valore;
		}
		if (parametroInput=="real") {
			real=valore;
		}
	}

	//////////////  CREO LE CLASSI APPROPRIATE  ///////////////////
	
	// ottengo il fileType
	MeshType fileTypeMesh=TETRAHEDRON;
	if (fileType=="TETRAHEDRON") {
		fileTypeMesh=TETRAHEDRON;
	}
	if (fileType=="TRIANGLE") {
		fileTypeMesh=TRIANGLE;
	}
	if (fileType=="ANYTHING3D") {
		fileTypeMesh=ANYTHING3D;
	}
	if (fileType=="ANYTHING2D") {
		fileTypeMesh=ANYTHING2D;
	}
	if (fileType=="FILETYPE1") {
		fileTypeMesh=FILETYPE1;
	}
	if (fileType=="FILETYPE1") {
		fileTypeMesh=FILETYPE2;
	}
	
	auto forceTermSphere3D=forceTermSphere<3,double>;
	auto forceTermSphere2D=forceTermSphere<2,double>;
	auto forceTermSquare3D=forceTermSquare<3,double>;
	auto forceTermSquare2D=forceTermSquare<2,double>;
	auto solutionSphere3D=solutionSphere<3,double>;
	auto solutionSphere2D=solutionSphere<2,double>;
	auto solutionSquare3D=solutionSquare<3,double>;
	auto solutionSquare2D=solutionSquare<2,double>;
	
	// caso double
	if (real=="double") {
		if (meshType=="Mesh3D") {
			
			Mesh3D<> newMesh(inputPoint,inputConnection,fileTypeMesh);
		
//				auto& pointVector=newMesh.pointVector;
//				for (int i=0; i<pointVector.size(); i++) {
//			//		if (pointVector[i]->isBoundary==true) {
//			
//						cout<<pointVector[i]->pointID<<"     "<<pointVector[i]->isBoundary<<*pointVector[i]<<endl;
//			//		}
//				}

			cout<<"mesh finita"<<endl;
			
			Laplace<3, Mesh3D<>, SolverVEM3D<>, Dirichlet3D<>> problem3(newMesh,forceTermSphere3D,boundaryFunction<3>,1);
			problem3();
			problem3.displayError(solutionSphere3D);
			
			if (outputPoint!="") {
				problem3.write(outputPoint,outputConnection,outputSolution);
			}
			
			cout<<"hTriangle: "<<newMesh.hTriangle()<<endl<<endl;
		}
		else if(meshType=="Mesh2D") {
			cout<<222312414<<endl;
			Mesh2D<> newMesh(inputPoint,inputConnection,fileTypeMesh);
			
			Laplace<2, Mesh2D<>, SolverVEM2D<>, Dirichlet2D<>> problem3(newMesh,forceTermSquare2D,boundaryFunction<2>,1);
			problem3();
			problem3.displayError(solutionSquare2D);
			
			if (outputPoint!="") {
				problem3.write(outputPoint,outputConnection,outputSolution);
			}
		}
		
	}
//	else if (real=="long double") {
//		if (meshType=="Mesh3D") {
//			Mesh3D<long double> newMesh(inputPoint,inputConnection,fileTypeMesh);
//			
//			Laplace<3, Mesh3D<long double>, SolverVEM3D<long double>, Dirichlet3D<long double>,long double> problem3(newMesh,forceTerm<3,long double>,boundaryFunction<3,long double>,1);
//			problem3();
//			problem3.displayError(realSolution<3,long double>);
//			if (outputPoint!="") {
//				problem3.write(outputPoint,outputConnection,outputSolution);
//			}
//			
//		} else if(meshType=="Mesh2D") {
//			Mesh2D<long double> newMesh(inputPoint,inputConnection,fileTypeMesh);
//			
//			Laplace<2, Mesh2D<long double>, SolverVEM2D<long double>, Dirichlet2D<long double>,long double> problem3(newMesh,forceTerm<2,long double>,boundaryFunction<2,long double>,1);
//			problem3();
//			problem3.displayError(realSolution<2,long double>);
//			if (outputPoint!="") {
//				problem3.write(outputPoint,outputConnection,outputSolution);
//			}
//		}
//	}
	
	
	
	
	
	
//	const Point<3> ciao(1,1,1);
//	const Point<3> ciao2(2,2,2);
//	
//	cout<<ciao[0]+ciao[1];
	
	
	
	
	
	
	
	
	
	
	
	
	

	
	
	
	return 0;
}
