from mayavi import mlab
import numpy as np

## 
## 	Class to plot the solution in 3D.
##
#		
#	It reads only the point file (the one output of the program) and the solution file.
#	Don't use the input point file since the ordering of the points may change during
#	the algorithm
#


##################################################################
##################################################################

## MODIFY ONLY THIS FILES

folder="../../VEM/Output/"

point="point2D.txt"
solution="solution2D.txt"

##################################################################
##################################################################






class Point:
	x=0
	y=0

	
	def __init__(self,list):
		self.x=list[0]
		self.y=list[1]
		
	def __str__(self):
		output="\n\nx:  "+str(self.x)
		output+="\ny:  "+str(self.y)+"\n\n"
		return output



pointFile=folder+point
solutionFile=folder+solution

f=open(pointFile)

points=[] # vettore di punti

content=f.read().splitlines()

for line in content:
	line=line.split(',')
	line=[float(j) for j in line] 
	point=Point(line)

	points.append(point)	
f.close


x=[]
y=[]
for i in points:
	x.append(i.x)
	y.append(i.y)


# creo il vettore solutioni
f=open(solutionFile)
s=[]

content=f.read().splitlines()
for line in content:
	print line
	line=float(line)
	s.append(line)
f.close()





#X = np.array([0, 1, 0, 1, 0.75,3])
#Y = np.array([0, 0, 1, 1, 0.75,3])
#Z = np.array([1, 1, 1, 1, 2,7])
X=np.asarray(x)
Y=np.asarray(y)
Z=np.asarray(s)

# Define the points in 3D space
# including color code based on Z coordinate.
pts = mlab.points3d(X, Y, Z, Z)

# Triangulate based on X, Y with Delaunay 2D algorithm.
# Save resulting triangulation.
mesh = mlab.pipeline.delaunay2d(pts)

# Remove the point representation from the plot
pts.remove()

# Draw a surface based on the triangulation
surf = mlab.pipeline.surface(mesh)

# Simple plot.
mlab.xlabel("x")
mlab.ylabel("y")
mlab.zlabel("z")
mlab.show()

print x
print y
print s









