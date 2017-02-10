import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

##
## 	Class to plot a single Polyhedron using Matplotlib
##
#		
#	
#	File of type ANYTHING3D. It reads only the first line.
#


##################################################################
##################################################################

## MODIFY ONLY THIS FILES


pointFile="../../Mesh/Test/point5.txt"
connectionsFile="../../Mesh/Test/conn5.txt"

#pointFile="../../Mesh/Sphere3D/point-2.txt"
#connectionsFile="../../Mesh/Sphere3D/conn-2.txt"


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


f=open(pointFile)

points=[]

content=f.read().splitlines()

for line in content:
	line=line.split(',')
	line=[float(j) for j in line] 
	point=Point(line)

	points.append(point)	
f.close
	
	

f=open(connectionsFile)

connections=[]

content=f.read().splitlines()
line=content[0]

line=line.split(';')
	
print(line)
	
for i in line:
	poly=i.split(',')
	poly=[int(j) for j in poly]
	connections.append(poly)

print(connections)
	

	
	
f.close
	

	
	
	
	
	
fig=plt.figure()
ax = fig.add_subplot(111, projection='3d')

x=[]
y=[]
z=[]

print(len(points))

j=0

for face in connections:
	
	
	print(i)
	
	for j in range(len(face)):

		
		x=[points[face[j]-1].x, points[face[(j+1)%len(face)]-1].x]
		y=[points[face[j]-1].y, points[face[(j+1)%len(face)]-1].y]
		z=[points[face[j]-1].z, points[face[(j+1)%len(face)]-1].z]

		ax.plot(x,y,z,'r')

	




x=[0.900627]
y=[0.317176]
z=[0.58345]

ax.plot(x,y,z,'b')


plt.show()	

	
