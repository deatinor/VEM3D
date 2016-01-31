##########################################################################################
###############                  README VISUALIZATION                      ###############
                             
												     
This file is about the script in the folder Python/
The folder contains some Python scripts.  
	

4 folder inside:
- MeshGeneration
- Output
- PlotMatplotlib
- PlotMayavi

##########################################################################################

MESHGENERATION

4 script to generate a Mesh. Not very powerful, included for completeness.

- Mesh2D and Mesh2DSquare generate a Mesh in 2 dimension of type ANYTHING2D. 
  Mesh2DCube is a Mesh of squares. Mesh2D is similar: each square is divided in 2 
  triangles
  
- meshCube generates a 3D mesh of cubes

- meshCubeWithoutPart is the same as before but the mesh is a cube without the part having
  a vertex in -1,-1,-1. Used to test Fichera-Corner.
  
##########################################################################################

OUTPUT

It's the output folder of MeshGeneration

##########################################################################################

PLOTMATPLOTLIB

2 script used to plot a mesh or part of it. It uses the Matplotlib library for plotting.

- plotMesh plot a 3D mesh of Polyhedron. It becomes slow if the mesh is too large. It's
  used for testing.
  
- plotSinglePolyhedron plot only the first element of a mesh. It can be useful for having
  a visual feedback in case one wants to create manually a complex polyhedron. 
  This can happen for example to create a complex polyhedron to test the G = BD identity.
  
##########################################################################################

PLOTMAYAVI

3 script used to plot the results of the simulation in 2D or in 3D.
It's important to use as input files the ones in the output folder of VEM.
We used the Mayavi library in Python. The library is not installed by default.

On Ubuntu 14.04, to make it work, install the following packages:
- python-vtk
- python-wxgtk2.8 (looking at the site it should work also the version 2.6)
- python-setuptools
- python-numpy
- python-configobj
- python-traits
- mayavi2

Refer to the site http://docs.enthought.com/mayavi/mayavi/installation.html for more info.


We provide 3 scripts:

- plot2D, plot the mesh in 2D with the solution.

- plot3D, plot the mesh in 3D with the solution. There is the possibility to see the solution
  on a section of the mesh through an apposite tool. 

- plotPoints3D, plot only points with their solution.

To show the legend:

- click on the top left button.
- Colors and legend -> show legend

There are a lot of other graphic tools in the same menu.










