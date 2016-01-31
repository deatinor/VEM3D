##########################################################################################
###############                  README VISUALIZATION                      ###############
                             
												     
This file is about the c++ program in the folder VEM/
The folder contains the header files and a Makefile
The classes are all template classes, so there are not .cpp files.  

3 folder inside:
- Datafile
- Doxygen
- Output

##########################################################################################

REQUIREMENTS FOR COMPILATION

- g++ version 4.8 or higher

- Eigen library installed. Version 3.2.7 tested.
  
##########################################################################################

TO COMPILE AND RUN

To compile and run it's sufficient to run:

make clean
make
./vem

##########################################################################################

DATAFILE - PARAMETHERS TO SET

We provide a datafile to set the main paramethers of the program without the need to 
recompile it.
We didn't use any library for it, but we implemented a simple parser in the main.cpp.
We used the c++ syntax for comments "//".

The Datafile is composed in 5 main sections:
- input files, in which the user choose the inputPoint and inputConnection files.
- mesh type
- the force term and solution functions
- output files. It's advised to leave the output folder to Output/
- typename. Wether to choose double or long double.

If changes are restricted to these properties there is no need to recompile the code.
It's sufficient to change the datafile and then recompile.
If the user wants to change the force term or solution functions he has to change the 
first lines of main.cpp, where there are all the functions, and then recompile.

##########################################################################################

DOXYGEN

We used Doxygen to provide the documentation.
To get it under the VEM folder run:

doxygen Doxyfile

The documentation will be produced in the Doxygen folder.
  












