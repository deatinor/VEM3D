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

			cout<<"mesh finita"<<endl;
			
			Laplace<3, Mesh3D<>, SolverVEM3D<>, Dirichlet3D<>> problem3(newMesh,forceTermSquare3D,solutionSquare3D,1);
			problem3();
			problem3.displayError(solutionSquare3D);
			
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
	
	
	
	

	
	
	
	return 0;
}
