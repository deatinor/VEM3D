# README VISUALIZATION
									     
This file is about the scripts in the folder `Python/`
The folder contains some Python scripts, written in Python 2. 
	
In this folder, one can find the following 4 subfolders:
* MeshGeneration
* Output
* PlotMatplotlib
* PlotMayavi

## `MeshGeneration/`

There, one can find 4 scripts to generate different meshes. 
Those scripts have not been made to be very powerful, they have been only 
included for completeness.

* `Mesh2D` and `Mesh2DSquare` generate meshes in 2 dimension of type `ANYTHING2D`. 
`Mesh2DCube creates rectangular meshes; `Mesh2D` is similar but creates 
triangular meshes: each square is divided in 2 triangles. 
* `meshCube` generates 3D cubic meshes.
* `meshCubeWithoutPart` is the same as `meshCube` but the mesh is a cube 
without the part having a vertex in -1,-1,-1. Is is used to test Fichera-Corner.

The parameters to create such meshes can be customized at the beginning 
of each of those files, in the indicated part.
  
## `Output/`

It is the output folder of `MeshGeneration/`

## `PlotMatplotlib/`

In there, one can find 2 scripts used to plot a mesh, or a part of it. 
It uses the Matplotlib library for plotting.
* `plotMesh` plots a 3D mesh of Polyhedron. It becomes slow if the mesh is 
too large. It is only used for testing.
* `plotSinglePolyhedron` plots only the first element of a mesh. It can 
be useful for having a visual feedback in case one wants to create manually 
a complex polyhedron. This can happen for example to create a complex 
polyhedron to test the G = BD identity.
  
On Ubuntu 14.04, to make it work, install the following packages:
* `python-matplotlib`

## `PlotMayavi/`

In there, one can find 3 scripts used to plot the results of the 
simulation in 2D or in 3D.
It is important to use as input files the ones in the output folder of `VEM/`.
We used the Mayavi library in Python. The library is not installed by default.

On Ubuntu 14.04, to make it work, install the following packages:
* `python-vtk`
* `python-wxgtk2.8` (looking at the site, version 2.6 should also work)
* `python-setuptools`
* `python-numpy`
* `python-configobj`
* `python-traits`
* `mayavi2`

Please refer to [this website page](http://docs.enthought.com/mayavi/mayavi/installation.html) for more info.

The 3 scripts are the following:
* `plot2D`: it plots the mesh in 2D with the solution.
* `plot3D`: it plots the mesh in 3D with the solution. There is the possibility to see the solution
  on a section of the mesh through an apposite tool. 
* `plotPoints3D`: it only plots points with their solution.

To show the legend:
* click on the top left button.
* Colors and legend -> show legend

There are a lot of other graphic tools that the user can explore, in the same menu.










