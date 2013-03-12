#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "PolyhedraCorrectionLibrary.h"

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

void Polyhedron::poly_cube_cutted() {
	if (!vertex)
		delete[] vertex;
	if (!facet)
		delete[] facet;
	if (!vertexinfo)
		delete[] vertexinfo;

	vertex = new Vector3d[16];

	vertex[0] = Vector3d(3., 0., 0.);
	vertex[1] = Vector3d(3., 3., 0.);
	vertex[2] = Vector3d(0., 3., 0.);
	vertex[3] = Vector3d(0., 0., 0.);
	vertex[4] = Vector3d(3., 1., 1.);
	vertex[5] = Vector3d(3., 2., 1.);
	vertex[6] = Vector3d(0., 2., 1.);
	vertex[7] = Vector3d(0., 1., 1.);
	vertex[8] = Vector3d(3., 0., 3.);
	vertex[9] = Vector3d(3., 1., 3.);
	vertex[10] = Vector3d(0., 1., 3.);
	vertex[11] = Vector3d(0., 0., 3.);
	vertex[12] = Vector3d(3., 2., 3.);
	vertex[13] = Vector3d(3., 3., 3.);
	vertex[14] = Vector3d(0., 3., 3.);
	vertex[15] = Vector3d(0., 2., 3.);

	facet = new Facet[10];

	int index[8];
	Plane plane;

	index[0] = 0;
	index[1] = 3;
	index[2] = 2;
	index[3] = 1;
	plane = Plane(Vector3d(0., 0., -1.), 0);
	facet[0] = Facet(0, 4, plane, index, this, false);

	index[0] = 4;
	index[1] = 5;
	index[2] = 6;
	index[3] = 7;
	plane = Plane(Vector3d(0., 0., 1.), -1.);
	facet[1] = Facet(1, 4, plane, index, this, false);

	index[0] = 8;
	index[1] = 9;
	index[2] = 10;
	index[3] = 11;
	plane = Plane(Vector3d(0., 0., 1.), -3.);
	facet[2] = Facet(2, 4, plane, index, this, false);

	index[0] = 12;
	index[1] = 13;
	index[2] = 14;
	index[3] = 15;
	plane = Plane(Vector3d(0., 0., 1.), -3.);
	facet[3] = Facet(3, 4, plane, index, this, false);

	index[0] = 0;
	index[1] = 1;
	index[2] = 13;
	index[3] = 12;
	index[4] = 5;
	index[5] = 4;
	index[6] = 9;
	index[7] = 8;
	plane = Plane(Vector3d(1., 0., 0.), -3.);
	facet[4] = Facet(4, 8, plane, index, this, false);

	index[0] = 3;
	index[1] = 11;
	index[2] = 10;
	index[3] = 7;
	index[4] = 6;
	index[5] = 15;
	index[6] = 14;
	index[7] = 2;
	plane = Plane(Vector3d(-1., 0., 0.), 0.);
	facet[5] = Facet(5, 8, plane, index, this, false);

	index[0] = 1;
	index[1] = 2;
	index[2] = 14;
	index[3] = 13;
	plane = Plane(Vector3d(0., 1., 0.), -3.);
	facet[6] = Facet(6, 4, plane, index, this, false);

	index[0] = 0;
	index[1] = 8;
	index[2] = 11;
	index[3] = 3;
	plane = Plane(Vector3d(0., -1., 0.), 0.);
	facet[7] = Facet(7, 4, plane, index, this, false);

	index[0] = 4;
	index[1] = 7;
	index[2] = 10;
	index[3] = 9;
	plane = Plane(Vector3d(0., 1., 0.), -1.);
	facet[8] = Facet(8, 4, plane, index, this, false);

	index[0] = 5;
	index[1] = 12;
	index[2] = 15;
	index[3] = 6;
	plane = Plane(Vector3d(0., -1., 0.), 2.);
	facet[9] = Facet(9, 4, plane, index, this, false);




	numv = 16;
	numf = 10;
}
