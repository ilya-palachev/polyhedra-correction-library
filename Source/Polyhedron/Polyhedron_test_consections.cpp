#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "PolyhedraCorrectionLibrary.h"

#define EPS_PARALLEL 1e-15

int Polyhedron::test_consections(
		bool ifPrint) {
	int count;
	if (ifPrint) {
		//    printf("Begin test_consections...\n");
	}

	count = 0;
	count += test_inner_consections(ifPrint);
	count += test_outer_consections(ifPrint);
	if (ifPrint) {
		if (count > 0) {
			printf("Total: %d consections found\n", count);
		}
	}
	return count;
}

int Polyhedron::test_inner_consections(
		bool ifPrint) {
	int i, count;

	double *A, *b;
	Vector3d* vertex_old;

	A = new double[4];
	b = new double[2];
	vertex_old = new Vector3d[numFacets];

	count = 0;
	for (i = 0; i < numFacets; ++i) {
		count += test_inner_consections_facet(ifPrint, i, A, b, vertex_old);
	}
	if (ifPrint) {
		if (count > 0)
			printf("\tTotal: %d inner consections found\n", count);
	}
	if (A != NULL)
		delete[] A;
	if (b != NULL)
		delete[] b;
	if (vertex_old != NULL)
		delete[] vertex_old;

	return count;
}

int Polyhedron::test_inner_consections_facet(
		bool ifPrint,
		int fid,
		double* A,
		double* b,
		Vector3d* vertex_old) {

	int i, j, nv, *index, count;
	Vector3d v;
	Plane plane;
	//Написано для случая, когда немного нарушена плоскостность грани
	//Перед проверкой на самопересечение все вершины грани проецируются на 
	//плоскость наименьших квадратов (2012-03-12)
	//    printf("\tBegin test_inner_consections_facet(%d)\n", fid);

	nv = facets[fid].numVertices;
	index = facets[fid].indVertices;
	plane = facets[fid].plane;
	//    vertex_old = new Vector3d[nv];

	if (nv < 4) {
		return 0;
	}

	for (i = 0; i < nv; ++i) {
		v = vertices[index[i]];
		vertex_old[i] = v;
		vertices[index[i]] = plane.project(v);
	}

	count = 0;
	for (i = 0; i < nv; ++i) {
		for (j = i + 1; j < nv; ++j) {
			count += test_inner_consections_pair(ifPrint, fid, index[i % nv],
					index[(i + 1) % nv], index[j % nv], index[(j + 1) % nv], A, b);
		}
	}
	if (ifPrint) {
		if (count > 0) {
			printf("\t\tIn facet %d: %d inner consections found\n", fid, count);
			facets[fid].my_fprint(stdout);
		}
	}
	//    if (count > 0)
	//        facet[fid].my_fprint(stdout);
	for (i = 0; i < nv; ++i) {
		vertices[index[i]] = vertex_old[i];
	}
	return count;
}

int Polyhedron::test_inner_consections_pair(
		bool ifPrint,
		int fid,
		int id0,
		int id1,
		int id2,
		int id3,
		double* A,
		double* b) {

	// fid - identificator of the facet
	// (id0, id1) - first edge
	// (id2, id3) - second edge
	double nx, ny, nz;
	double x0, x1, x2, x3, y0, y1, y2, y3, z0, z1, z2, z3;

	if (id0 < 0 || id1 < 0 || id2 < 0 || id2 < 0 || id0 >= numVertices
			|| id1 >= numVertices || id2 >= numVertices || id3 >= numVertices) {
		if (ifPrint) {
			printf("\t\t\ttest_inner_consections_pair: Error. incorrect input\n");
		}
		return 0;
	}

	if ((id0 == id3 && id1 == id2) || (id0 == id2 && id1 == id3)) {
		if (ifPrint) {
			printf("\t\t\t(%d, %d) and (%d, %d) are equal edges\n", id0, id1, id2,
					id3);
		}
		return 2;
	}

	if (id1 == id2) {
		if (qmod((vertices[id1] - vertices[id0]) % (vertices[id3] - vertices[id2]))
				< EPS_PARALLEL
				&& (vertices[id1] - vertices[id0]) * (vertices[id3] - vertices[id2])
						< 0) {
			if (ifPrint) {
				printf("\t\t\t(%d, %d) and (%d, %d) consect untrivially\n", id0, id1,
						id2, id3);
			}
			return 2;
		} else {
			return 0;
		}
	}

	if (id0 == id3) {
		if (qmod((vertices[id1] - vertices[id0]) % (vertices[id3] - vertices[id2]))
				< EPS_PARALLEL
				&& (vertices[id1] - vertices[id0]) * (vertices[id3] - vertices[id2])
						> 0) {
			if (ifPrint) {
				printf("\t\t\t(%d, %d) and (%d, %d) consect untrivially\n", id0, id1,
						id2, id3);
			}
			return 2;
		} else {
			return 0;
		}
	}

	if (qmod(
			(vertices[id1] - vertices[id0])
					% (vertices[id3] - vertices[id2])) < EPS_PARALLEL) {
		return 0;
	}

	nx = facets[fid].plane.norm.x;
	ny = facets[fid].plane.norm.y;
	nz = facets[fid].plane.norm.z;
	nx = fabs(nx);
	ny = fabs(ny);
	nz = fabs(nz);

	x0 = vertices[id0].x;
	y0 = vertices[id0].y;
	z0 = vertices[id0].z;

	x1 = vertices[id1].x;
	y1 = vertices[id1].y;
	z1 = vertices[id1].z;

	x2 = vertices[id2].x;
	y2 = vertices[id2].y;
	z2 = vertices[id2].z;

	x3 = vertices[id3].x;
	y3 = vertices[id3].y;
	z3 = vertices[id3].z;

	if (nz >= nx && nz >= ny) {
		A[0] = x1 - x0;
		A[1] = x2 - x3;
		A[2] = y1 - y0;
		A[3] = y2 - y3;
		b[0] = x2 - x0;
		b[1] = y2 - y0;
	}

	if (ny >= nx && ny >= nz) {
		A[0] = x1 - x0;
		A[1] = x2 - x3;
		A[2] = z1 - z0;
		A[3] = z2 - z3;
		b[0] = x2 - x0;
		b[1] = z2 - z0;
	}

	if (nx >= ny && nx >= nz) {
		A[0] = y1 - y0;
		A[1] = y2 - y3;
		A[2] = z1 - z0;
		A[3] = z2 - z3;
		b[0] = y2 - y0;
		b[1] = z2 - z0;
	}

	if (Gauss_string(2, A, b) == 0) {
		return 0;
	}

	if (b[0] > 0. && b[0] < 1. && b[1] > 0. && b[1] < 1.) {
		if (ifPrint) {
			printf("\t\t\tEdges (%d, %d) and (%d, %d) consect\n", id0, id1, id2, id3);
		}
		return 1;
	} else
		return 0;
}

int Polyhedron::test_outer_consections(
		bool ifPrint) {
	int i, count;

	count = 0;
	for (i = 0; i < numFacets; ++i) {
		count += test_outer_consections_facet(ifPrint, i);
	}
	if (ifPrint) {
		if (count > 0)
			printf("\tTotal: %d outer consections found\n", count);
	}
	return count;
}

int Polyhedron::test_outer_consections_facet(
		bool ifPrint,
		int fid) {
	int i, nv, *index, count;

	nv = facets[fid].numVertices;
	index = facets[fid].indVertices;

	count = 0;
	for (i = 0; i < nv; ++i) {
		count += test_outer_consections_edge(ifPrint, index[i % nv],
				index[(i + 1) % nv]);
	}
	if (ifPrint) {
		if (count > 0)
			printf("\t\tIn facet %d: %d outer consections found\n", fid, count);
	}
	return count;
}

int Polyhedron::test_outer_consections_edge(
		bool ifPrint,
		int id0,
		int id1) {

	int j, count;

	count = 0;
	for (j = 0; j < numFacets; ++j) {
		//        printf("\tj = %d\n", j);
		count += test_outer_consections_pair(ifPrint, id0, id1, j);
	}
	if (ifPrint) {
		if (count > 0)
			printf("\t\t\tFor edge (%d, %d): %d outer consections found\n", id0, id1,
					count);
	}
	return count;
}

int Polyhedron::test_outer_consections_pair(
		bool ifPrint,
		int id0,
		int id1,
		int fid) {

	int i, nv, *index;
	double AA, b, u, delta, alpha, sum;
	Vector3d A, A0, A1, normal;

	if (facets[fid].find_vertex(id0) >= 0 || facets[fid].find_vertex(id1) >= 0) {
		//        printf("contains.\n");
		return 0;
	}

	AA = facets[fid].plane.norm * (vertices[id0] - vertices[id1]);

	if (fabs(AA) < EPS_PARALLEL) {
		//        printf("parallel %le.\n", fabs(AA));
		return 0;
	}

	b = -facets[fid].plane.norm * vertices[id1] - facets[fid].plane.dist;
	u = b / AA;

	if (u > 1. || u < 0.) {
		//        printf("consection out of interval u = %lf.\n", u);
		return 0;
	}
	//    printf("\ttesting whether edge (%d, %d) consects facet %d.\n", 
	//            id0, id1, fid);
	//    printf("\t\tu = %lf\n", u);

	A = u * vertices[id0] + (1. - u) * vertices[id1];
	//    printf("\t\tA = (%.2lf, %.2lf, %.2lf)\n", A.x, A.y, A.z);

	nv = facets[fid].numVertices;
	index = facets[fid].indVertices;
	normal = facets[fid].plane.norm;
	normal.norm(1.);
	//    printf("\t\t|n| = %lf,  ", sqrt(qmod(normal)));

	sum = 0.;
	for (i = 0; i < nv; ++i) {
		//        if (i > 0)
		//            printf("+");
		A0 = vertices[index[i % nv]] - A;
		A1 = vertices[index[(i + 1) % nv]] - A;
		delta = (A0 % A1) * normal;
		delta /= sqrt(qmod(A0) * qmod(A1));
		alpha = asin(delta);
		//        printf(" %lf ", alpha / M_PI * 180);
		sum += alpha;
	}

	//    printf(" = sum = %lf*\n", sum / M_PI * 180);

	if (fabs(sum) < 2 * M_PI) {
		return 0;
	} else {
		if (ifPrint) {
			printf("\t\t\t\tEdge (%d, %d) consects facet %d\n", id0, id1, fid);
		}
		return 1;
	}
}
