//
// Adapted from Oscar Sebio Cajaraville 2015
//
// Created on 9/02/17
//

#include <cmath>
#include <vector>
#include <random>
#include <string>
#include <iostream>
#include <fstream>

struct Vector3
{
	double x, y, z;
	Vector3(double v) : x(v), y(v), z(v) {}
	Vector3(double ix, double iy, double iz) : x(ix), y(iy), z(iz) {}
	Vector3 operator +(const Vector3 &other) const { return Vector3(x + other.x, y + other.y, z + other.z); }
	Vector3 operator -(const Vector3 &other) const { return Vector3(x - other.x, y - other.y, z - other.z); }
	Vector3 operator *(const Vector3 &other) const { return Vector3(x * other.x, y * other.y, z * other.z); }
};

double dot(const Vector3 &a, const Vector3 &b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3 cross(const Vector3 &a, const Vector3 &b)
{
	return Vector3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x);
}

double length(const Vector3 &a)
{
	return std::sqrt(dot(a, a));
}

Vector3 normalize(const Vector3 &a)
{
	const double lrcp = 1.0 / std::sqrt(dot(a, a));
	return Vector3(a.x * lrcp, a.y * lrcp, a.z * lrcp);
}

struct Mesh
{
	std::vector<Vector3> vertices;
	std::vector<uint32_t> triangles;

	uint32_t triangleCount() const { return triangles.size() / 3; }

	void addTriangle(uint32_t a, uint32_t b, uint32_t c)
	{
		triangles.emplace_back(a);
		triangles.emplace_back(b);
		triangles.emplace_back(c);
	}

	void addQuad(uint32_t a, uint32_t b, uint32_t c, uint32_t d)
	{
		triangles.emplace_back(a);
		triangles.emplace_back(b);
		triangles.emplace_back(c);
		triangles.emplace_back(a);
		triangles.emplace_back(c);
		triangles.emplace_back(d);
	}

	void clear()
	{
		vertices.clear();
		triangles.clear();
	}

	double distance(const Vector3 &p, uint32_t tidx) const
	{
		const uint32_t idx0 = triangles[tidx];
		const uint32_t idx1 = triangles[tidx + 1];
		const uint32_t idx2 = triangles[tidx + 2];
		const Vector3 v0 = vertices[idx0];
		const Vector3 v1 = vertices[idx1];
		const Vector3 v2 = vertices[idx2];
		const Vector3 bv = v0;
		const Vector3 e0 = v1 - v0;
		const Vector3 e1 = v2 - v0;
		const Vector3 dv = bv - p;
		const double a = dot(e0, e0);
		const double b = dot(e0, e1);
		const double c = dot(e1, e1);
		const double d = dot(e0, dv);
		const double e = dot(e1, dv);
		//const double f = dot(dv, dv);

		const double det = a*c - b*b;
		double s = b*e - c*d;
		double t = b*d - a*e;

		if (s + t <= det)
		{
			if (s < 0.0)
			{
				if (t < 0.0)
				{
					// region 4
					if (d < 0.0)
					{
						t = 0.0;
						s = -d >= a ? 1.0 : -d / a;
					}
					else
					{
						s = 0.0;
						t = e >= 0.0 ? 0.0 : (-e >= c ? 1.0 : -e / c);
					}
				}
				else
				{
					// region 3
					s = 0.0;
					t = e >= 0.0 ? 0.0 : (-e >= c ? 1.0 : -e / c);
				}
			}
			else if (t < 0.0)
			{
				// region 5
				s = d >= 0.0 ? 0.0 : (-d >= a ? 1.0 : -d / a);
				t = 0.0;
			}
			else
			{
				// region 0
				const double invDet = 1.0 / det;
				s *= invDet;
				t *= invDet;
			}
		}
		else
		{
			if (s < 0.0)
			{
				// region 2
				const double tmp0 = b + d;
				const double tmp1 = c + e;
				if (tmp1 > tmp0)
				{
					const double numer = tmp1 - tmp0;
					const double denom = a - 2.0 * b + c;
					s = numer >= denom ? 1.0 : numer / denom;
					t = 1.0 - s;
				}
				else
				{
					s = 0.0;
					t = (tmp1 <= 0.0 ? 1.0 : (e >= 0.0 ? 0.0 : -e / c));
				}
			}
			else if (t < 0.0)
			{
				// region 6
				const double tmp0 = b + e;
				const double tmp1 = a + d;
				if (tmp1 > tmp0)
				{
					const double numer = tmp1 - tmp0;
					const double denom = a - 2.0 * b + c;
					t = numer >= denom ? 1.0 : numer / denom;
					s = 1.0 - t;
				}
				else
				{
					s = (tmp1 <= 0.0 ? 1.0 : (d >= 0.0 ? 0.0 : -d / a));
					t = 0.0;
				}
			}
			else
			{
				// region 1
				const double numer = c + e - b - d;
				if (numer <= 0)
				{
					s = 0.0;
				}
				else
				{
					const double denom = a - 2.0 * b + c;
					s = numer >= denom ? 1.0 : numer / denom;
				}
				t = 1.0 - s;
			}
		}

		return length(p - (v0 + Vector3(s) * e0 + Vector3(t) * e1));
	}

	double distance(const Vector3 &p) const
	{
		double min = 10e10;
		for (uint32_t i = 0; i < triangles.size(); i += 3)
		{
			min = std::fmin(min, distance(p, i));
		}
		return min;
	}
};

namespace CubeToSphere
{
	static const Vector3 origins[6] =
	{
		Vector3(-1.0, -1.0, -1.0),
		Vector3(1.0, -1.0, -1.0),
		Vector3(1.0, -1.0, 1.0),
		Vector3(-1.0, -1.0, 1.0),
		Vector3(-1.0, 1.0, -1.0),
		Vector3(-1.0, -1.0, 1.0)
	};
	static const Vector3 rights[6] =
	{
		Vector3(2.0, 0.0, 0.0),
		Vector3(0.0, 0.0, 2.0),
		Vector3(-2.0, 0.0, 0.0),
		Vector3(0.0, 0.0, -2.0),
		Vector3(2.0, 0.0, 0.0),
		Vector3(2.0, 0.0, 0.0)
	};
	static const Vector3 ups[6] =
	{
		Vector3(0.0, 2.0, 0.0),
		Vector3(0.0, 2.0, 0.0),
		Vector3(0.0, 2.0, 0.0),
		Vector3(0.0, 2.0, 0.0),
		Vector3(0.0, 0.0, 2.0),
		Vector3(0.0, 0.0, -2.0)
	};
};

void Sphere3D(uint32_t divisions, Mesh &mesh)
{
	const double step = 1.0 / double(divisions);
	const Vector3 step3(step, step, step);

	for (uint32_t face = 0; face < 6; ++face)
	{
		const Vector3 origin = CubeToSphere::origins[face];
		const Vector3 right = CubeToSphere::rights[face];
		const Vector3 up = CubeToSphere::ups[face];
		for (uint32_t j = 0; j < divisions + 1; ++j)
		{
			const Vector3 j3(j, j, j);
			for (uint32_t i = 0; i < divisions + 1; ++i)
			{
				const Vector3 i3(i, i, i);
				const Vector3 p = origin + step3 * (i3 * right + j3 * up);
				const Vector3 p2 = p * p;
				const Vector3 n
				(
					p.x * std::sqrt(1.0 - 0.5 * (p2.y + p2.z) + p2.y*p2.z / 3.0),
					p.y * std::sqrt(1.0 - 0.5 * (p2.z + p2.x) + p2.z*p2.x / 3.0),
					p.z * std::sqrt(1.0 - 0.5 * (p2.x + p2.y) + p2.x*p2.y / 3.0)
				);
				mesh.vertices.emplace_back(n);
			}
		}
	}

	const uint32_t k = divisions + 1;
	for (uint32_t face = 0; face < 6; ++face)
	{
		for (uint32_t j = 0; j < divisions; ++j)
		{
			for (uint32_t i = 0; i < divisions; ++i)
			{
				const uint32_t a = (face * k + j) * k + i;
				const uint32_t b = (face * k + j) * k + i + 1;
				const uint32_t c = (face * k + j + 1) * k + i;
				const uint32_t d = (face * k + j + 1) * k + i + 1;
				mesh.addQuad(a, b, d, c);
			}
		}
	}
}


void exportObj(const Mesh &mesh, const std::string &namePts, const std::string &nameConn)
{
	std::fstream fs;
	
	fs.open(namePts.c_str(), std::fstream::out | std::fstream::trunc);
	for (const auto &v : mesh.vertices)
	{
		fs << v.x << "," << v.y << "," << v.z << std::endl;
	}
	fs.close();
	
	fs.open(nameConn.c_str(), std::fstream::out | std::fstream::trunc);
	for (uint32_t i = 0; i < mesh.triangleCount(); ++i)
	{
		fs << (mesh.triangles[i * 3] + 1) << "," 
		   << (mesh.triangles[i * 3 + 1] + 1) << "," 
		   << (mesh.triangles[i * 3 + 2] + 1) << std::endl;
	}
	fs.close();
}

int main(int argc, char *argv[])
{

	if (argc != 2) {
		std::cerr 
			<< "sphere3d.cpp needs only one argument corresponding to the number of iterations to create the sphere." 
			<< std::endl << "Example: ./sphere3d 6" << std::endl;
	} else {
		int it(atoi(argv[1]));
		std::cout << "Number of iterations: " << it << "." << std::endl;
		
		Mesh m;
		Sphere3D(it, m);
		
		std::string fileConnections("conn-"+std::to_string(it)+".txt");
		std::string filePoints("point-"+std::to_string(it)+".txt");
		exportObj(m, filePoints, fileConnections);
	}

	return 0;
}
