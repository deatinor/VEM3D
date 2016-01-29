import numpy as np

##
## 	Create a 2D mesh of triangles. It's like a square's mesh, with each square divided in 2 triangles
##
#		
#	
#	Output file of type ANYTHING2D. 
#	xpoints and ypoints are the number of points on the x and y axys
#


##################################################################
##################################################################

## MODIFY ONLY THESE PARAMETHERS


xmin=-1.0
xmax=1.0
ymin=-1.0
ymax=1.0

xpoints=5
ypoints=5

outputPoint="point-2.txt"
outputConn="conn-2.txt"


##################################################################
##################################################################



class Point:
	x=0
	y=0
	
	def __init__(self,x,y):
		self.x=x
		self.y=y

		
	def write(self):
		output=str(self.x)+","+str(self.y)+"\n"
		return output
		
	def __str__(self):
		output="\n\nx:  "+str(self.x)
		output+="\ny:  "+str(self.y)
		output+="\n\n"
		return output
		
	def __eq__(self,point):
		if self.x==point.x and self.y==point.y:
			return True
		else:
			return False
			

			
		
class MeshPoints:
	listPoint=[]
	
	def add(self,point):
		self.listPoint.append(point)
		

	def indexOfPoint(self,point):
		for i in range(len(self.listPoint)):
			if self.listPoint[i]==point:
				return i+1
		return "null"
	
	def len(self):
		return len(self.listPoint)

	def __str__(self):
		output=""
		for i in self.listPoint:
			output+=str(i)
		return output
		

def convertToString(p1,p2,p3):
	output=str(p1)+","+str(p2)+","+str(p3)+"\n"
	return output




x=np.linspace(xmin,xmax,xpoints)
y=np.linspace(ymin,ymax,ypoints)

points=MeshPoints()

for i in x:
	for j in y:
		point=Point(i,j)
		points.add(point)
			

print points
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
				
		p1=points.indexOfPoint(Point(x[i],y[j]))
		p2=points.indexOfPoint(Point(x[i+1],y[j]))
		p3=points.indexOfPoint(Point(x[i+1],y[j+1]))
		p4=points.indexOfPoint(Point(x[i],y[j+1]))

			

		face1=convertToString(p1,p2,p4)
		face2=convertToString(p2,p3,p4)
		

			
		f.write(face1)
		f.write(face2)

f.close

			

