from mayavi import mlab
import numpy as np

#mlab.clf()
x, y = np.mgrid[-10:10:2, -10:10:2]
r = np.sqrt(x**2 + y**2)
z = np.sin(r)/r
z=x+y
print x
print "ora y\n\n\n\n"
print y
mlab.surf(z, warp_scale='auto')
print "ora z\n\n\n\n"
print z

mlab.axes()	
mlab.colorbar()

mlab.show()


