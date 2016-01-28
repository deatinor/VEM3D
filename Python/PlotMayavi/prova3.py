import numpy as np
from mayavi import mlab

X = np.array([0, 1, 0, 1, 0.75,3])
Y = np.array([0, 0, 1, 1, 0.75,3])
Z = np.array([1, 1, 1, 1, 2,7])

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
