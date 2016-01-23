import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

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
		

file="/Users/stefano/Dropbox/Condivise/Ste-Nico/Progetto/Mesh/Test/point0.txt"
connection="/Users/stefano/Dropbox/Condivise/Ste-Nico/Progetto/Mesh/Test/conn0.txt"


f=open(file)

points=[]

content=f.read().splitlines()

for line in content:
	line=line.split(',')
	line=[float(j) for j in line] 
	point=Point(line)

	points.append(point)	
f.close
	
	

f=open(connection)

connections=[]

content=f.read().splitlines()

for line in content:
	line=line.split(',')
	line=[int(j) for j in line] 
	

	connections.append(line)	
f.close
	
for i in connections:
	print i
	
	
	
	
	
fig=plt.figure()
ax = fig.add_subplot(111, projection='3d')

x=[]
y=[]
z=[]


print len(points)

j=0

for i in connections:
	p1=i[0]-1
	p2=i[1]-1
	p3=i[2]-1
	p4=i[3]-1
	
	print j

	
	x=[points[p1].x, points[p2].x]
	y=[points[p1].y, points[p2].y]
	z=[points[p1].z, points[p2].z]

	ax.plot(x,y,z,'r')

	x=[points[p2].x, points[p3].x]
	y=[points[p2].y, points[p3].y]
	z=[points[p2].z, points[p3].z]

	ax.plot(x,y,z,'r')
	
	x=[points[p1].x, points[p3].x]
	y=[points[p1].y, points[p3].y]
	z=[points[p1].z, points[p3].z]

	ax.plot(x,y,z,'r')
	
	x=[points[p1].x, points[p4].x]
	y=[points[p1].y, points[p4].y]
	z=[points[p1].z, points[p4].z]

	ax.plot(x,y,z,'r')
	
	x=[points[p2].x, points[p4].x]
	y=[points[p2].y, points[p4].y]
	z=[points[p2].z, points[p4].z]

	ax.plot(x,y,z,'r')
	
	x=[points[p3].x, points[p4].x]
	y=[points[p3].y, points[p4].y]
	z=[points[p3].z, points[p4].z]

	ax.plot(x,y,z,'r')
	


x=[-0.438481,-0.438481-0.30772/10]
y=[0.346371,0.346371-0.579306/10]
z=[-0.399873,-0.399873+0.754793/10]

ax.plot(x,y,z,'b')


plt.show()	


