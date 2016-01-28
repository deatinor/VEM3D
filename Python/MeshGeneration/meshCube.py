import numpy as np

##
## 	Create a 3D mesh of cubes. 
##
#		
#	
#	Output file of type ANYTHING3D. 
#	xpoints, ypoints and zpoints are the number of points on the x, y and z axys
#


##################################################################
##################################################################

## MODIFY ONLY THESE PARAMETHERS


xmin=-1.0
xmax=1.0
ymin=-1.0
ymax=1.0
zmin=-1.0
zmax=1.0

xpoints=5
ypoints=5
zpoints=5

outputPoint="point-2.txt"
outputConn="conn-2.txt"


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

		
	def write(self):
		output=str(self.x)+","+str(self.y)+","+str(self.z)+"\n"
		return output
		
	def __str__(self):
		output="\n\nx:  "+str(self.x)
		output+="\ny:  "+str(self.y)
		output+="\nz:  "+str(self.z)+"\n\n"
		return output
		
	def __eq__(self,point):
		if self.x==point.x and self.y==point.y and self.z==point.z:
			return True
		else:
			return False
			

			
		
class MeshPoints:
	listPoint=[]
	xpoints,xmin,xmax=0,-1,1
	ypoints,ymin,ymax=0,-1,1
	zpoints,zmin,zmax=0,-1,1
	stepx,stepy,stepz=0,0,0
	
	def add(self,point):
		self.listPoint.append(point)
		

	def indexOfPoint(self,point):
		for i in range(len(self.listPoint)):
			if self.listPoint[i]==point:
				return i+1
		return "null"
		
	def fastIndexOfPoint(self,point):
		span=self.ypoints*self.zpoints
		
		
		indexx=round((point.x-self.xmin)/self.stepx)
		indexy=round((point.y-self.ymin)/self.stepy)
		indexz=round((point.z-self.zmin)/self.stepz)
	
		return span*indexx+self.zpoints*indexy+indexz+1
	
	def len(self):
		return len(self.listPoint)

	def __str__(self):
		output=""
		for i in self.listPoint:
			output+=str(i)
		return output
		

def convertToString(p1,p2,p3,p4):
	output=str(p1)+","+str(p2)+","+str(p3)+","+str(p4)
	return output

def createCubeString(face1,face2,face3,face4,face5,face6):
	output=str(face1)+";"+str(face2)+";"+str(face3)+";"+str(face4)+";"+str(face5)+";"+str(face6)+"\n"
	return output
	



x=np.linspace(xmin,xmax,xpoints)
y=np.linspace(ymin,ymax,ypoints)
z=np.linspace(zmin,zmax,zpoints)

points=MeshPoints()
points.xpoints,points.xmin,points.xmax=xpoints,xmin,xmax
points.ypoints,points.ymin,points.ymax=ypoints,ymin,ymax
points.zpoints,points.zmin,points.zmax=zpoints,zmin,zmax
points.stepx=(points.xmax-points.xmin)/float(points.xpoints-1)
points.stepy=(points.ymax-points.ymin)/float(points.ypoints-1)
points.stepz=(points.zmax-points.zmin)/float(points.zpoints-1)


for i in x:
	for j in y:
		for k in z:
			point=Point(i,j,k)
			points.add(point)
			

# print points
print points.len()


# scrivo i punti
f=open("../Output/"+outputPoint,'w')

for i in points.listPoint:
	f.write(i.write())

f.close()

# scrivo le connessioni
f=open("../Output/"+outputConn,'w')
for i in range(len(x)):
	if i==xpoints-1:
		continue
	for j in range(len(y)):
		if j==ypoints-1:
			continue
		for k in range(len(z)):
			if k==zpoints-1:
				continue
				
# 			p1=points.indexOfPoint(Point(x[i],y[j],z[k]))
# 			p2=points.indexOfPoint(Point(x[i+1],y[j],z[k]))
# 			p3=points.indexOfPoint(Point(x[i+1],y[j+1],z[k]))
# 			p4=points.indexOfPoint(Point(x[i],y[j+1],z[k]))
# 
# 			p5=points.indexOfPoint(Point(x[i],y[j],z[k+1]))
# 			p6=points.indexOfPoint(Point(x[i+1],y[j],z[k+1]))
# 			p7=points.indexOfPoint(Point(x[i+1],y[j+1],z[k+1]))
# 			p8=points.indexOfPoint(Point(x[i],y[j+1],z[k+1]))


			p1=int(points.fastIndexOfPoint(Point(x[i],y[j],z[k])))
			p2=int(points.fastIndexOfPoint(Point(x[i+1],y[j],z[k])))
			p3=int(points.fastIndexOfPoint(Point(x[i+1],y[j+1],z[k])))
			p4=int(points.fastIndexOfPoint(Point(x[i],y[j+1],z[k])))
			
			p5=int(points.fastIndexOfPoint(Point(x[i],y[j],z[k+1])))
			p6=int(points.fastIndexOfPoint(Point(x[i+1],y[j],z[k+1])))
			p7=int(points.fastIndexOfPoint(Point(x[i+1],y[j+1],z[k+1])))
			p8=int(points.fastIndexOfPoint(Point(x[i],y[j+1],z[k+1])))
			
			
			face1=convertToString(p1,p2,p3,p4)
			face2=convertToString(p1,p2,p6,p5)
			face3=convertToString(p2,p3,p7,p6)
			face4=convertToString(p3,p4,p8,p7)
			face5=convertToString(p4,p1,p5,p8)
			face6=convertToString(p5,p6,p7,p8)

			cube=createCubeString(face1,face2,face3,face4,face5,face6)
			
			f.write(cube)

f.close

			




