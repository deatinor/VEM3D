from mayavi import mlab
import numpy

##  TODO  ##
## 	Class to plot the vertexes of a 3D mesh with the corresponding solution.
##
#		
#	It reads only the point file (the one output of the program) and the solution file.
#	Don't use the input point file since the ordering of the points may change during
#	the algorithm
#


##################################################################
##################################################################

## MODIFY ONLY THIS FILES

folder="../../VEM/VEM/Output/"

point="point2D.txt"
solution="solution2D.txt"

##################################################################
##################################################################



class Point:
	x=0
	y=0
	z=0
	
	def __init__(self,x,y,z):
		self.x=x
		self.y=y
		self.z=z
	
	def __init__(self,list):
		self.x=list[0]
		self.y=list[1]
		self.z=list[2]
		
	def __str__(self):
		output="\n\nx:  "+str(self.x)
		output+="\ny:  "+str(self.y)
		output+="\nz:  "+str(self.z)+"\n\n"
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
z=[]

for i in points:
	x.append(i.x)
	y.append(i.y)
	z.append(i.z)

# creo il vettore solutioni
f=open(solutionFile)
s=[]

content=f.read().splitlines()
for line in content:
	print line
	line=float(line)
	s.append(line)
f.close()


# traslo s
maxS=max(s)
minS=min(s)

diff=maxS-minS

for i in s:
	i=(i-minS)/diff


mlab.plot3d(x,y,z,s,vmax=maxS,vmin=minS)#,colormap="summer")

#x=[[0,1],[1,0],[0,0]]
#y=[[0,1],[1,0],[0,0]]
#z=[[0,1],[1,3],[3,0]]

#mlab.mesh(x,y,z)

mlab.show()






















		

