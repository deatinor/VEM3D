# VEM3D

`VEM3D` is a project for the Master courses of Advanced Programming for Scientific 
Computing by Prof. Formaggia and of Advanced Numerical Analysis of Partial 
Differential Equations by Prof. Perotto, at the [Politecnico di Milano](http://www.polimi.it/). 

This is part of the Laurea Magistrale (equivalent of a Master) in Mathematical 
Enginnering, with a specialization in Scientific Computing.

This project has been done under the supervision of Prof. Formaggia, Prof. 
Perotto, Prof. Verani and Prof. Antonietti. 

# Team
* [Stefano Savarè](https://github.com/deatinor)
* [Ondine Chanon](https://github.com/ochanon)

# Goals
* Implementation in C++ of the linear Virtual Element Method on Laplace 
equation with Dirichlet boundary condition, in 2D and in 3D, using all 
the power of the C++ programming so that the code could be developped 
further and generalized. 
This is done in the `VEM/` folder. A Doxygen documentation is also furnished there.
* Theoretical explanation of the method, together with convergence results. 
This is done in the `Report/` folder, the pdf format of the report is called 
`main.pdf`. In this same report is also explained our implementation of VEM. 
* Convergence analysis of the linear Virtual Element Method under mesh refinement.
This is done in the `Convergence/` folder in Matlab. This also allows us 
to test the implementation. 

For a sake of completeness, the following has also been made:
* A visualization of the solutions, in 2D and in 3D, made in Python in the 
`Python/` folder.
* The generation of different meshes for testing, together with a possibility 
to visualize them. The different meshes can be found in the folder `Mesh/`. 
Some generations have been made in Python and can be found in the `Python/` 
folder, while some others have been made either in C++ of in Matlab and they 
can be directly found in their corresponding subfolder in `Mesh/`. 

# How to use our code
More detailed README are present in each folder, with all the instructions 
about compiling, running and using our code.
