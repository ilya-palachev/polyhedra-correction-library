/*
 * VertexGrouper.cpp
 *
 *  Created on: 21.06.2013
 *      Author: ilya
 */

#include "PolyhedraCorrectionLibrary.h"

#define EPS_PARALLEL 1e-15

VertexGrouper::VertexGrouper() :
		PCorrector()
{
}

VertexGrouper::VertexGrouper(Polyhedron* p) :
		PCorrector(p)
{
}

VertexGrouper::~VertexGrouper()
{
}


void swap(int& i, int& j)
{
	int tmp = i;
	i = j;
	j = tmp;
}

int VertexGrouper::run(int id)
{
	int count;
#ifdef TCPRINT
//    printf("Begin join_points...\n");
#endif

	count = 0;
	count += groupInner();
#ifdef TCPRINT
//    printf("Total: %d consections found\n", count);
#endif
	return count;
}

int VertexGrouper::groupInner()
{
	int i, count;

	double *A, *b;
	Vector3d* vertex_old;

	A = new double[4];
	b = new double[2];
	vertex_old = new Vector3d[polyhedron->numFacets];

	count = 0;
	for (i = 0; i < polyhedron->numFacets; ++i)
	{
		count += groupInnerFacet(i, A, b, vertex_old);
	}
#ifdef TCPRINT
	if (count > 0)
	printf("\tTotal: %d inner consections found\n", count);
#endif
	if (A != NULL)
		delete[] A;
	if (b != NULL)
		delete[] b;
	if (vertex_old != NULL)
		delete[] vertex_old;

	return count;
}

int VertexGrouper::groupInnerFacet(int fid, double* A, double* b,
		Vector3d* vertex_old)
{

	int i, nv, *index, count;
	Vector3d v;
	Plane plane;
	//Написано для случая, когда немного нарушена плоскостность грани
	//Перед проверкой на самопересечение все вершины грани проецируются на
	//плоскость наименьших квадратов (2012-03-12)
//    printf("\tBegin join_points_inner_facet(%d)\n", fid);

	nv = polyhedron->facets[fid].numVertices;
	index = polyhedron->facets[fid].indVertices;
	plane = polyhedron->facets[fid].plane;
//    vertex_old = new Vector3d[nv];

	for (i = 0; i < nv; ++i)
	{
		v = polyhedron->vertices[index[i]];
		vertex_old[i] = v;
		polyhedron->vertices[index[i]] = plane.project(v);
	}

	count = 0;
	for (i = 0; i < nv; ++i)
	{
		count += grouptInnerPair(fid, index[i % nv],
				index[(i + 1) % nv], index[(i + 2) % nv], index[(i + 3) % nv],
				A, b);
	}
#ifdef TCPRINT
	if (count > 0)
	printf("\t\tIn facet %d: %d inner consections found\n", fid, count);
#endif
//    if (count > 0)
//        facet[fid].my_fprint(stdout);
	for (i = 0; i < nv; ++i)
	{
		polyhedron->vertices[index[i]] = vertex_old[i];
	}
	return count;
}

int VertexGrouper::grouptInnerPair(int fid, int id0, int id1,
		int id2, int id3, double* A, double* b)
{

	// fid - identificator of the facet
	// (id0, id1) - first edge
	// (id2, id3) - second edge
	double nx, ny, nz;
	double x0, x1, x2, x3, y0, y1, y2, y3, z0, z1, z2, z3;
	int i;
//    bool ifFind1, ifFind2;
	int pos1, pos2;

	if (id0 < 0 || id1 < 0 || id2 < 0 || id2 < 0
			|| id0 >= polyhedron->numVertices
			|| id1 >= polyhedron->numVertices
			|| id2 >= polyhedron->numVertices
			|| id3 >= polyhedron->numVertices)
	{
#ifdef TCPRINT
		printf("\t\t\tjoin_points_inner_pair: Error. incorrect input\n");
#endif
		return 0;
	}

	if ((id0 == id3 && id1 == id2) || (id0 == id2 && id1 == id3))
	{
#ifdef TCPRINT
		printf("\t\t\t(%d, %d) and (%d, %d) are equal edges\n", id0, id1, id2, id3);
#endif
		return 2;
	}

	if (id1 == id2)
	{
		if (qmod(
				(polyhedron->vertices[id1] - polyhedron->vertices[id0])
						% (polyhedron->vertices[id3] - polyhedron->vertices[id2])) < EPS_PARALLEL
				&& (polyhedron->vertices[id1] - polyhedron->vertices[id0])
						* (polyhedron->vertices[id3] - polyhedron->vertices[id2]) < 0)
		{
#ifdef TCPRINT
			printf("\t\t\t(%d, %d) and (%d, %d) consect untrivially\n",
					id0, id1, id2, id3);
#endif
			return 2;
		}
		else
		{
			return 0;
		}
	}

	if (id0 == id3)
	{
		if (qmod(
				(polyhedron->vertices[id1] - polyhedron->vertices[id0])
						% (polyhedron->vertices[id3] - polyhedron->vertices[id2])) < EPS_PARALLEL
				&& (polyhedron->vertices[id1] - polyhedron->vertices[id0])
						* (polyhedron->vertices[id3] - polyhedron->vertices[id2]) > 0)
		{
#ifdef TCPRINT
			printf("\t\t\t(%d, %d) and (%d, %d) consect untrivially\n",
					id0, id1, id2, id3);
#endif
			return 2;
		}
		else
		{
			return 0;
		}
	}

	if (qmod(
			(polyhedron->vertices[id1] - polyhedron->vertices[id0])
					% (polyhedron->vertices[id3] - polyhedron->vertices[id2])) < EPS_PARALLEL)
	{
		return 0;
	}

	nx = polyhedron->facets[fid].plane.norm.x;
	ny = polyhedron->facets[fid].plane.norm.y;
	nz = polyhedron->facets[fid].plane.norm.z;
	nx = fabs(nx);
	ny = fabs(ny);
	nz = fabs(nz);

	x0 = polyhedron->vertices[id0].x;
	y0 = polyhedron->vertices[id0].y;
	z0 = polyhedron->vertices[id0].z;

	x1 = polyhedron->vertices[id1].x;
	y1 = polyhedron->vertices[id1].y;
	z1 = polyhedron->vertices[id1].z;

	x2 = polyhedron->vertices[id2].x;
	y2 = polyhedron->vertices[id2].y;
	z2 = polyhedron->vertices[id2].z;

	x3 = polyhedron->vertices[id3].x;
	y3 = polyhedron->vertices[id3].y;
	z3 = polyhedron->vertices[id3].z;

	if (nz >= nx && nz >= ny)
	{
		A[0] = x1 - x0;
		A[1] = x2 - x3;
		A[2] = y1 - y0;
		A[3] = y2 - y3;
		b[0] = x2 - x0;
		b[1] = y2 - y0;
	}

	if (ny >= nx && ny >= nz)
	{
		A[0] = x1 - x0;
		A[1] = x2 - x3;
		A[2] = z1 - z0;
		A[3] = z2 - z3;
		b[0] = x2 - x0;
		b[1] = z2 - z0;
	}

	if (nx >= ny && nx >= nz)
	{
		A[0] = y1 - y0;
		A[1] = y2 - y3;
		A[2] = z1 - z0;
		A[3] = z2 - z3;
		b[0] = y2 - y0;
		b[1] = z2 - z0;
	}

	if (Gauss_string(2, A, b) == 0)
	{
		return 0;
	}

	if (b[0] > 0. && b[0] < 1. && b[1] > 0. && b[1] < 1.)
	{
#ifdef TCPRINT
		printf("\t\t\tEdges (%d, %d) and (%d, %d) consect\n", id0, id1, id2, id3);
#endif
		//В этом случае будем объединять вершины...
		double dist = sqrt(qmod(polyhedron->vertices[id1] - polyhedron->vertices[id2]));
		printf(
				"Принято решение сливать вершины %d и %d, расстояние между которыми равно %lf\n",
				id1, id2, dist);
		if (id2 == idSavedVertex)
		{
			swap(id1, id2);
		}
		for (i = 0; i < polyhedron->numFacets; ++i)
		{
			pos1 = polyhedron->facets[i].find_vertex(id1);
			pos2 = polyhedron->facets[i].find_vertex(id2);
			if (pos1 != -1 && pos2 != -1)
			{
				printf(
						"\tОбе вершины найдены в грани %d. Из нее удаляется вершина %d\n",
						i, id2);
				polyhedron->facets[i].my_fprint(stdout);
				polyhedron->facets[i].delete_vertex(id2);
			}
			else if (pos1 != -1)
			{
				printf(
						"\tВершина %d найдена в грани %d. Оставляем грань без изменения\n",
						id1, i);
			}
			else if (pos2 != -1)
			{
				printf(
						"\tВершина %d найдена в грани %d. Заменим здесь вершину %d на вершину %d\n",
						id2, i, id2, id1);
				polyhedron->facets[i].indVertices[pos2] = id1;
			}
		}
		printf("Слияние завершено\n");
		polyhedron->delete_empty_facets();
		polyhedron->preprocessAdjacency();
		return 1;
	}
	else
		return 0;
}

