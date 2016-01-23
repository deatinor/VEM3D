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
		

file="point-2.txt"
# connection="/Users/stefano/Dropbox/Condivise/Ste-Nico/Progetto/Mesh/Test/dodecaedro.txt"


f=open(file)

points=[]

content=f.read().splitlines()

for line in content:
	line=line.split(',')
	line=[float(j) for j in line] 
	point=Point(line)

	points.append(point)	
f.close

	
	
fig=plt.figure()
ax = fig.add_subplot(111, projection='3d')

x=[]
y=[]
z=[]
	
for i in points:
	x=[i.x]
	y=[i.y]
	z=[i.z]
	
	ax.plot(x,y,z,'r.')
	
plt.show()
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
