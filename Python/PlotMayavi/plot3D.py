from mayavi import mlab
import numpy as np

folder="../Mesh3/Mesh3/Output/"

point="point.txt"
solution="solution.txt"


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



X=np.asarray(x)
Y=np.asarray(y)
Z=np.asarray(z)
S=np.asarray(s)



pts = mlab.points3d(X, Y, Z, S)

mesh = mlab.pipeline.delaunay3d(pts)

print mesh
pts.remove()

prova=mlab.pipeline.iso_surface(mesh,contours=8,opacity=0.1)


#aggiungo il piano
mlab.pipeline.scalar_cut_plane(mesh,view_controls=True)




mlab.show()












