#include "PolyhedraCorrectionLibrary.h"

#define EPS_PARALL 1e-16
#define MAX_MIN_DIST 1e+1

void Polyhedron::coalesceFacets(int fid0, int fid1)
{
	Coalescer* coalescer = new Coalescer(this);
	coalescer->run(fid0, fid1);
	delete coalescer;
}

void Polyhedron::coalesceFacets(int n, int* fid)
{
	Coalescer* coalescer = new Coalescer(this);
	coalescer->run(n, fid);
	delete coalescer;
}

void Polyhedron::delete_vertex_polyhedron(int v)
{
	printf("delete_vertex_polyhedron(%d)\n", v);
	for (int i = 0; i < numFacets; ++i)
		facets[i].delete_vertex(v);
}

int Polyhedron::appendVertex(Vector3d& vec)
{
	Vector3d* vertex1;
	vertex1 = new Vector3d[numVertices + 1];
	for (int i = 0; i < numVertices; ++i)
		vertex1[i] = vertices[i];
	vertex1[numVertices] = vec;
	if (vertices != NULL)
		delete[] vertices;
	vertices = vertex1;
	++numVertices;
	return numVertices - 1;
}

void Polyhedron::changeVertex(int position, Vector3d vec)
{
	if (position >= numVertices)
	{
		ERROR_PRINT("Cannot set %d-th vertex, because number of vertices",
				position);
		ERROR_PRINT("is bounded with %d", numVertices);
		return;
	}
	vertices[position] = vec;
}

void Polyhedron::printVertex(int i)
{
	printf("vertex %d : (%lf , %lf , %lf)\n", i, vertices[i].x, vertices[i].y,
			vertices[i].z);
}

void Polyhedron::find_and_replace_vertex(int from, int to)
{
	int i;
	for (i = 0; i < numFacets; ++i)
	{
		facets[i].find_and_replace_vertex(from, to);
	}
	for (i = 0; i < numVertices; ++i)
	{
		vertexInfos[i].find_and_replace_vertex(from, to);
	}
}

void Polyhedron::find_and_replace_facet(int from, int to)
{
	int i;
	for (i = 0; i < numFacets; ++i)
	{
		facets[i].find_and_replace_facet(from, to);
	}
	for (i = 0; i < numVertices; ++i)
	{
		vertexInfos[i].find_and_replace_facet(from, to);
	}
}

void Polyhedron::list_squares_method(int nv, int *vertex_list, Plane *plane)
{
	int i, id;
	double *x, *y, *z, a, b, c, d;

	x = new double[nv];
	y = new double[nv];
	z = new double[nv];

	for (i = 0; i < nv; ++i)
	{
		id = vertex_list[i];
		x[i] = vertices[id].x;
		y[i] = vertices[id].y;
		z[i] = vertices[id].z;
	}

	runListSquaresMethod(nv, x, y, z, a, b, c, d);

	plane->norm.x = a;
	plane->norm.y = b;
	plane->norm.z = c;
	plane->dist = d;

	//debug
	//printf("list_squares_method: a = %lf, b = %lf, c = %lf, d = %lf\n", a, b, c, d);

	if (x != NULL)
		delete[] x;
	if (y != NULL)
		delete[] y;
	if (z != NULL)
		delete[] z;
}

void Polyhedron::list_squares_method_weight(int nv, int *vertex_list,
		Plane *plane)
{
	int i, id, i_prev, i_next, id0, id1;
	double *x, *y, *z, a, b, c, d;
	double *l, *w;

	x = new double[nv];
	y = new double[nv];
	z = new double[nv];
	l = new double[nv];
	w = new double[nv];

	for (i = 0; i < nv; ++i)
	{
		i_next = (i + 1) % nv;
		id0 = vertex_list[i];
		id1 = vertex_list[i_next];
		l[i] = sqrt(qmod(vertices[id0] - vertices[id1]));
	}

	for (i = 0; i < nv; ++i)
	{
		i_prev = (i - 1 + nv) % nv;
		w[i] = 0.5 * (l[i_prev] + l[i]);
	}

	for (i = 0; i < nv; ++i)
	{
		id = vertex_list[i];
		x[i] = vertices[id].x * w[i];
		y[i] = vertices[id].y * w[i];
		z[i] = vertices[id].z * w[i];
	}

	runListSquaresMethod(nv, x, y, z, a, b, c, d);

	plane->norm.x = a;
	plane->norm.y = b;
	plane->norm.z = c;
	plane->dist = d;

	//debug
	//printf("list_squares_method: a = %lf, b = %lf, c = %lf, d = %lf\n", a, b, c, d);

	if (x != NULL)
		delete[] x;
	if (y != NULL)
		delete[] y;
	if (z != NULL)
		delete[] z;
	if (z != NULL)
		delete[] l;
	if (z != NULL)
		delete[] w;
}

int Polyhedron::test_structure()
{
	int i, res;
	res = 0;
	for (i = 0; i < numFacets; ++i)
	{
		res += facets[i].test_structure();
	}
	return res;
}
