#include <iostream>
#include <random>
#include <stdexcept>
#include "Point.h"
#include "MeshPoint.h"
#include "Polygon.h"
#include "Polyhedron.h"
#include "Mesh.h"
#include "Problem.h"
#include "Laplace.h"
#include "Monomials.h"
#include "MonomialsPolygon.h"
#include "Solver.h"
#include "BoundaryCondition.h"
#include "Error.h"
#include "Mesh3D.h"
#include "Mesh2D.h"
#include "SolverVEM.h"
#include "SolverVEM3D.h"
#include "SolverVEM2D.h"
#include "Eigen/Dense"
#include "SolverVEM.h"
#include "Dirichlet.h"
#include "muParserInterface.h"
#include "cartesianParserInterface.h"
#include "sphericalParserInterface.h"

int main(int argc, const char * argv[]) {
	
	/* ------------  PARAMETERS TO BE READ  ----------- */
	
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
	string outputError;
	string errorAction;
	string coordinateType;
	string forceTermFunctionExpr;
	string boundaryFunctionExpr;
	string solutionFunctionExpr;
	string real;
	
	
	/* ------------ READ THE FILE  ------------ */
	string parametroInput,valore,linea;
	ifstream inputFile("Datafile/datafile.dat");
	
	try{
		if (!inputFile.is_open())
			throw std::runtime_error("Problem reading 'datafile.dat', make sure such file is in the Datafile/ folder.");
		
		while (getline(inputFile, linea)) {
			// eliminate white spaces
			std::string::iterator end_pos = std::remove(linea.begin(), linea.end(), ' ');
			linea.erase(end_pos, linea.end());
			
			if (linea=="") {
				continue;
			} else if (linea.substr(0,2)=="//") {
				continue;
			}
			
			// obtain input parameters and their values
			stringstream iss(linea);
			getline(iss, parametroInput, '=');
			getline(iss, valore, ';');
			
			// overwrite found input parameters
			if (parametroInput=="inputPoint") {
				inputPoint=valore;
			} else if (parametroInput=="inputConnection") {
				inputConnection=valore;
			} else if (parametroInput=="meshType") {
				meshType=valore;
				if (meshType=="Mesh3D") {
					embedded=3;
				} else if (meshType=="Mesh2D"){
					embedded=2;
				}
			} else if (parametroInput=="fileType") {
				fileType=valore;
			} else if (parametroInput=="solver") {
				solverType=valore;
				if (solverType!="VEM")
					throw std::runtime_error("Problem with the type of solver: behavior defined only for 'VEM'");
			} else if (parametroInput=="boundaryCondition") {
				boundaryCondition=valore;
				if (boundaryCondition!="dirichlet")
					throw std::runtime_error("Problem with the boundaryCondition: behavior defined only for 'dirichlet'.");
			} else if (parametroInput=="outputPoint") {
				outputPoint=valore;
			} else if (parametroInput=="outputConnection") {
				outputConnection=valore;
			} else if (parametroInput=="outputSolution") {
				outputSolution=valore;
			} else if (parametroInput=="outputError") {
				outputError=valore;
			} else if (parametroInput=="errorAction") {
				errorAction=valore;
			} else if (parametroInput=="coordinateType") {
				coordinateType=valore;
			} else if (parametroInput=="forceTermFunction") {
				forceTermFunctionExpr=valore;
			} else if (parametroInput=="boundaryFunction") {
				boundaryFunctionExpr=valore;
			} else if (parametroInput=="solutionFunction") {
				solutionFunctionExpr=valore;
			} else if (parametroInput=="real") {
				real=valore;
			} // any other parametroInput is ignored
		}

	/* -------  CREATE THE APPROPRIATES CLASSES  */
		// Obtain the fileType
		MeshType fileTypeMesh=TETRAHEDRON;
		if (fileType=="TETRAHEDRON") {
			fileTypeMesh=TETRAHEDRON;
		} else if (fileType=="TRIANGLE") {
			fileTypeMesh=TRIANGLE;
		} else if (fileType=="ANYTHING3D") {
			fileTypeMesh=ANYTHING3D;
		} else if (fileType=="ANYTHING2D") {
			fileTypeMesh=ANYTHING2D;
		} else if (fileType=="FILETYPE1") {
			fileTypeMesh=FILETYPE1;
		} else if (fileType=="FILETYPE2") {
			fileTypeMesh=FILETYPE2;
		} else {
			throw std::runtime_error("Wrong fileType: choose between TETRAHEDRON, TRIANGLE, ANYTHING3D, ANYTHING2D, FILETYPE1 or FILETYPE2.");
		}

		// double case
		if (real=="double") {
			
			if (meshType=="Mesh3D") {
				muParserInterface<3,double>* forceTerm;
				muParserInterface<3,double>* boundaryFunc;
				muParserInterface<3,double>* solution;
				if (coordinateType=="cartesian") {
					forceTerm = new cartesianParserInterface<3,double>(forceTermFunctionExpr);
					boundaryFunc = new cartesianParserInterface<3,double>(boundaryFunctionExpr);
					if (solutionFunctionExpr == "boundaryFunction") {
						solution = new cartesianParserInterface<3,double>(boundaryFunctionExpr);
					} else if (solutionFunctionExpr != "null") {
						solution = new cartesianParserInterface<3,double>(solutionFunctionExpr);
					}
				} else if (coordinateType=="spherical" || coordinateType=="polar") {
					forceTerm = new sphericalParserInterface<3,double>(forceTermFunctionExpr);
					boundaryFunc = new sphericalParserInterface<3,double>(boundaryFunctionExpr);
					if (solutionFunctionExpr == "boundaryFunction") {
						solution = new sphericalParserInterface<3,double>(boundaryFunctionExpr);
					} else if (solutionFunctionExpr != "null") {
						solution = new sphericalParserInterface<3,double>(solutionFunctionExpr);
					}
				} else {
					throw std::runtime_error("Wrong coordinateType: choose between cartesian, spherical or polar.");
				}
			
				Mesh3D<> newMesh(inputPoint,inputConnection,fileTypeMesh);
				
				cout<<"Mesh created"<<endl;
				
				Laplace<3, Mesh3D<>, SolverVEM3D<>, Dirichlet3D<>> problem3(newMesh,*forceTerm,*boundaryFunc,1);
				problem3();
				if (solutionFunctionExpr != "null")
					problem3.displayError(*solution,outputError,errorAction);
				
				if (outputPoint!="") {
					problem3.write(outputPoint,outputConnection,outputSolution);
				}
				
				cout<<"hTriangle: "<<newMesh.hTriangle()<<endl<<endl;

				// Clear memory
				delete forceTerm;
				delete boundaryFunc;
				if (solutionFunctionExpr != "null")
					delete solution;
					
			} else if(meshType=="Mesh2D") {
				
				muParserInterface<2,double>* forceTerm;
				muParserInterface<2,double>* boundaryFunc;
				muParserInterface<2,double>* solution;
				if (coordinateType=="cartesian") {
					forceTerm = new cartesianParserInterface<2,double>(forceTermFunctionExpr);
					boundaryFunc = new cartesianParserInterface<2,double>(boundaryFunctionExpr);
					if (solutionFunctionExpr == "boundaryFunction") {
						solution = new cartesianParserInterface<2,double>(boundaryFunctionExpr);
					} else if (solutionFunctionExpr != "null") {
						solution = new cartesianParserInterface<2,double>(solutionFunctionExpr);
					}
				} else if (coordinateType=="spherical" || coordinateType=="polar") {
					forceTerm = new sphericalParserInterface<2,double>(forceTermFunctionExpr);
					boundaryFunc = new sphericalParserInterface<2,double>(boundaryFunctionExpr);
					if (solutionFunctionExpr == "boundaryFunction") {
						solution = new sphericalParserInterface<2,double>(boundaryFunctionExpr);
					} else if (solutionFunctionExpr != "null") {
						solution = new sphericalParserInterface<2,double>(solutionFunctionExpr);
					}
				} else {
					throw std::runtime_error("Wrong coordinateType: choose between cartesian, spherical or polar.");
				}
			
				Mesh2D<> newMesh(inputPoint,inputConnection,fileTypeMesh);
				
				Laplace<2, Mesh2D<>, SolverVEM2D<>, Dirichlet2D<>> problem3(newMesh,*forceTerm,*boundaryFunc,1);
				problem3();
				if (solutionFunctionExpr != "null")
					problem3.displayError(*solution,outputError,errorAction);
				
				if (outputPoint!="") {
					problem3.write(outputPoint,outputConnection,outputSolution);
				}
				
				cout<<"hTriangle: "<<newMesh.hTriangle()<<endl<<endl;

				// Clear memory
				delete forceTerm;
				delete boundaryFunc;
				if (solutionFunctionExpr != "null")
					delete solution;
					
			} else {
				throw std::runtime_error("No predefined behaviour with this Mesh type.");
			}
			
		} else if (real=="longdouble") { // long double case
			
			if (meshType=="Mesh3D") {
				
				muParserInterface<3,long double>* forceTerm;
				muParserInterface<3,long double>* boundaryFunc;
				muParserInterface<3,long double>* solution;
				if (coordinateType=="cartesian") {
					forceTerm = new cartesianParserInterface<3,long double>(forceTermFunctionExpr);
					boundaryFunc = new cartesianParserInterface<3,long double>(boundaryFunctionExpr);
					if (solutionFunctionExpr == "boundaryFunction") {
						solution = new cartesianParserInterface<3,long double>(boundaryFunctionExpr);
					} else if (solutionFunctionExpr != "null") {
						solution = new cartesianParserInterface<3,long double>(solutionFunctionExpr);
					}
				} else if (coordinateType=="spherical" || coordinateType=="polar") {
					forceTerm = new sphericalParserInterface<3,long double>(forceTermFunctionExpr);
					boundaryFunc = new sphericalParserInterface<3,long double>(boundaryFunctionExpr);
					if (solutionFunctionExpr == "boundaryFunction") {
						solution = new sphericalParserInterface<3,long double>(boundaryFunctionExpr);
					} else if (solutionFunctionExpr != "null") {
						solution = new sphericalParserInterface<3,long double>(solutionFunctionExpr);
					}
				} else {
					throw std::runtime_error("Wrong coordinateType: choose between cartesian, spherical or polar.");
				}
				
				Mesh3D<long double> newMesh(inputPoint,inputConnection,fileTypeMesh);
				
				cout<<"Mesh created"<<endl;
				
				Laplace<3, Mesh3D<long double>, SolverVEM3D<long double>, Dirichlet3D<long double>,long double> problem3(newMesh,*forceTerm,*boundaryFunc,1);
				problem3();
				if (solutionFunctionExpr != "null")
					problem3.displayError(*solution,outputError,errorAction);
				
				if (outputPoint!="") {
					problem3.write(outputPoint,outputConnection,outputSolution);
				}
				
				cout<<"hTriangle: "<<newMesh.hTriangle()<<endl<<endl;

				// Clear memory
				delete forceTerm;
				delete boundaryFunc;
				if (solutionFunctionExpr != "null")
					delete solution;
					
			} else if(meshType=="Mesh2D") {
				
				muParserInterface<2,long double>* forceTerm;
				muParserInterface<2,long double>* boundaryFunc;
				muParserInterface<2,long double>* solution;
				if (coordinateType=="cartesian") {
					forceTerm = new cartesianParserInterface<2,long double>(forceTermFunctionExpr);
					boundaryFunc = new cartesianParserInterface<2,long double>(boundaryFunctionExpr);
					if (solutionFunctionExpr == "boundaryFunction") {
						solution = new cartesianParserInterface<2,long double>(boundaryFunctionExpr);
					} else if (solutionFunctionExpr != "null") {
						solution = new cartesianParserInterface<2,long double>(solutionFunctionExpr);
					}
				} else if (coordinateType=="spherical" || coordinateType=="polar") {
					forceTerm = new sphericalParserInterface<2,long double>(forceTermFunctionExpr);
					boundaryFunc = new sphericalParserInterface<2,long double>(boundaryFunctionExpr);
					if (solutionFunctionExpr == "boundaryFunction") {
						solution = new sphericalParserInterface<2,long double>(boundaryFunctionExpr);
					} else if (solutionFunctionExpr != "null") {
						solution = new sphericalParserInterface<2,long double>(solutionFunctionExpr);
					}
				} else {
					throw std::runtime_error("Wrong coordinateType: choose between cartesian, spherical or polar.");
				}			
				
				Mesh2D<long double> newMesh(inputPoint,inputConnection,fileTypeMesh);
				
				Laplace<2, Mesh2D<long double>, SolverVEM2D<long double>, Dirichlet2D<long double>,long double> problem3(newMesh,*forceTerm,*boundaryFunc,1);
				problem3();
				if (solutionFunctionExpr != "null")
					problem3.displayError(*solution,outputError,errorAction);
				
				if (outputPoint!="") {
					problem3.write(outputPoint,outputConnection,outputSolution);
				}
				
				cout<<"hTriangle: "<<newMesh.hTriangle()<<endl<<endl;

				// Clear memory
				delete forceTerm;
				delete boundaryFunc;
				if (solutionFunctionExpr != "null")
					delete solution;
		
			} else {
				throw std::runtime_error("No predefined behaviour with this Mesh type.");
			}
			
		} else {
			throw std::runtime_error("Program can run only with type=double or long double.");
		}

	} catch (mu::Parser::exception_type &e) {
		cerr << "Error in evaluating an expression." << endl;
		printMuException(e);
		cerr << endl;
	}
	catch (std::exception& e) {
		cerr << e.what() << endl;
	}
																																										
	return 0;
}
