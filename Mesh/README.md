# README MESH

In this folder `Mesh/` are found all the generated meshes that can be used 
to test our implementation of VEM, found in the `../VEM` folder:
* `Cube/`: all files in this folder are created by scripts present in 
the `../Python` folder.
	* `Cubic/`: contains the files with the vertex coordinates of different 
	cubic meshes (beginning with `point-`) and the connection files (beginning 
	with `conn-`) that represent a cube (-1,1)^3.
	* `CubeWithoutPart/`: contains the files with the vertex coordinates of different 
	cubic meshes and the connection files that represent a cube without 
	a quarter of it, i.e. (-1,1)^3\(-1,0)^3.
* `Mesh2D/`: all files in this folder have been created by scripts present 
in the `../Python` folder. This folder contains the files with the vertex 
coordinates of different triangular meshes and the connection files that 
represent a square (-1,1)^2.
* `Mesh2DSquare/`: all files in this folder have been created by scripts present 
in the `../Python` folder. This folder contains the files with the vertex 
coordinates of different rectangular meshes and the connection files that 
represent a square (-1,1)^2.
* `Sphere/`: contains the files with the vertex coordinates of different 
meshes and the connection files that represent the surface of a 
sphere centered in 0, of radius 1.
	* in `rectangularMesh`, the meshes are rectangular, and created by the
	C++ file called `sphere2d.cpp`. To generate it, it is sufficient to write
	`g++ -std=c++11 -o sphere2d sphere2d.cpp` from a terminal.
	* in `triangularMesh`, the meshes are triangular.
* `Sphere3D/`: contains the files with the vertex coordinates of different 
tetrahedral meshes and the connection files that represent a sphere centered 
in 0, of radius 1. This time, it is not only the surface that is considered, 
but the whole ball. These mesh have been created by the Matlab file 
`generateSphere3D` present in this folder. 
* `Test/`: contains some test files, generated manually.
* `Torus/`: contains the files with the vertex coordinates of different 
triangular meshes and the connection files that represents the surface 
of a torus.
