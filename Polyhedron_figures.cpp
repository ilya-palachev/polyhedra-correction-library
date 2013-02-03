#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"

void Polyhedron::poly_cube(double h, double x, double y, double z) {
	if (!vertex)
		delete[] vertex;
	if (!facet)
		delete[] facet;
	if (!vertexinfo)
		delete[] vertexinfo;

	vertex = new Vector3d[8];

	vertex[0] = Vector3d(x - 0.5 * h, y - 0.5 * h, z - 0.5 * h);
	vertex[1] = Vector3d(x + 0.5 * h, y - 0.5 * h, z - 0.5 * h);
	vertex[2] = Vector3d(x + 0.5 * h, y + 0.5 * h, z - 0.5 * h);
	vertex[3] = Vector3d(x - 0.5 * h, y + 0.5 * h, z - 0.5 * h);
	vertex[4] = Vector3d(x - 0.5 * h, y - 0.5 * h, z + 0.5 * h);
	vertex[5] = Vector3d(x + 0.5 * h, y - 0.5 * h, z + 0.5 * h);
	vertex[6] = Vector3d(x + 0.5 * h, y + 0.5 * h, z + 0.5 * h);
	vertex[7] = Vector3d(x - 0.5 * h, y + 0.5 * h, z + 0.5 * h);

	facet = new Facet[6];

	int index[4];
	Plane plane;

	index[0] = 0; index[1] = 3; index[2] = 2; index[3] = 1;
	plane = Plane(Vector3d(0., 0., -1.), z - 0.5 * h);
	facet[0] = Facet(0, 4, plane, index, &(*this), false);
	
	index[0] = 0; index[1] = 1; index[2] = 5; index[3] = 4;
	plane = Plane(Vector3d(0., -1., 0.), y - 0.5 * h);
	facet[0] = Facet(1, 4, plane, index, &(*this), false);

	index[0] = 1; index[1] = 2; index[2] = 6; index[3] = 5;
	plane = Plane(Vector3d(1., 0., 0.), -x - 0.5 * h);
	facet[0] = Facet(2, 4, plane, index, &(*this), false);

	index[0] = 2; index[1] = 3; index[2] = 7; index[3] = 6;
	plane = Plane(Vector3d(0., 1., 0.), -y - 0.5 * h);
	facet[0] = Facet(3, 4, plane, index, &(*this), false);

	index[0] = 0; index[1] = 4; index[2] = 7; index[3] = 3;
	plane = Plane(Vector3d(-1., 0., 0.), -x + 0.5 * h);
	facet[0] = Facet(4, 4, plane, index, &(*this), false);

	index[0] = 4; index[1] = 5; index[2] = 6; index[3] = 7;
	plane = Plane(Vector3d(0., 0., 1.), -z - 0.5 * h);
	facet[0] = Facet(5, 4, plane, index, &(*this), false);

	numv = 8;
	numf = 6;

//	for(int i = 0; i < 6; ++i)
//		facet[i].set_poly(this);
}

void Polyhedron::poly_pyramid(int n, double h, double r) {
	if (vertex)
		delete[] vertex;
	if (facet)
		delete[] facet;
	if (vertexinfo)
		delete[] vertexinfo;

	vertex = new Vector3d[n + 1];

	for (int i = 0; i < n; ++i)
		vertex[i] = Vector3d(cos(2 * M_PI * i / n), sin(2 * M_PI * i / n), 0.);
	vertex[n] = Vector3d(0., 0., h);

	facet = new Facet[n + 1];

	int *index;
	index = new int[3];

	Plane plane;

	for (int i = 0; i < n; ++i) {
		index[0] = n;
		index[1] = i;
		index[2] = (i + 1) % n;
		plane = Plane(vertex[index[0]], vertex[index[1]], vertex[index[2]]);
		facet[i] = Facet(i, 3, plane, index, this, false);
	}

	index = new int[n];

	for (int i = 0; i < n; ++i)
		index[i] = n - 1 - i;
	
	plane = Plane(Vector3d(0., 0., -1.), 0.);
	facet[n] = Facet(n, n, plane, index, this, false);

	numv = numf = n + 1;

	if(index)
		delete[] index;
}

void Polyhedron::poly_prism(int n, double h, double r) {
	if (vertex)
		delete[] vertex;
	if (facet)
		delete[] facet;
	if (vertexinfo)
		delete[] vertexinfo;

	vertex = new Vector3d[2 * n];

	for (int i = 0; i < n; ++i)
		vertex[i] =
				Vector3d(cos(2 * M_PI * i / n), sin(2 * M_PI * i / n), 0.);
	for (int i = 0; i < n; ++i)
		vertex[n + i] =
				Vector3d(cos(2 * M_PI * i / n), sin(2 * M_PI * i / n), h);

	facet = new Facet[n + 2];

	int *index;
	index = new int[4];

	Plane plane;

	for (int i = 0; i < n; ++i) {
		index[0] = i;
		index[1] = (i + 1) % n;
		index[2] = index[1] + n;
		index[3] = index[0] + n;
		plane = Plane(vertex[index[0]], vertex[index[1]], vertex[index[2]]);
		facet[i] = Facet(i, 4, plane, index, this, false);
	}

	index = new int[n];

	for (int i = 0; i < n; ++i)
		index[i] = n - 1 - i;
	
	plane = Plane(Vector3d(0., 0., -1.), 0.);
	facet[n] = Facet(n, n, plane, index, this, false);

	for (int i = 0; i < n; ++i)
		index[i] = i + n;
	
	plane = Plane(Vector3d(0., 0., 1.), - h);
	facet[n + 1] = Facet(n + 1, n, plane, index, this, false);

	numv = 2 * n;
	numf = n + 2;

	if(index)
		delete[] index;
}
