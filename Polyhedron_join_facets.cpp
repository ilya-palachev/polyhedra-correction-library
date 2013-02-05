#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"
#include "list_squares_method.h"

#include "array_operations.h"

int Polyhedron::join_facets_count_nv(int facet_id0, int facet_id1) {
	int nv0, nv1, nv_common, *index0, *index1, i, j, nv;

	if (facet_id0 == facet_id1) {
		fprintf(stderr, "join_facets_...: Error. facet_id0 == facet_id1.\n");
		return 0;
	}

	nv0 = facet[facet_id0].nv;
	nv1 = facet[facet_id1].nv;
	index0 = facet[facet_id0].index;
	index1 = facet[facet_id1].index;

	for (i = 0; i < nv0; ++i) {
		if (index0[nv0 + 1 + i] == facet_id1)
			break;
	}
	if (i == nv0) {
		fprintf(stderr, "join_facets_...: Error. No common vertices.\n");
		return 0;
	}

	//Подсчет количества общих вершин
	nv_common = 0;
	for (j = 0; j < nv0; i = (i < nv0 - 1 ? i + 1 : 0), ++j) {
		++nv_common;
		if (index0[nv0 + 1 + i] != facet_id1)
			break;
	}

	nv = nv0 + nv1 - nv_common;
	return nv;
}

void Polyhedron::join_facets_create_vertex_list(int facet_id0, int facet_id1, int nv,
		int *vertex_list, int* edge_list) {
	int nv0, nv1, *index0, *index1, i, j, k;

	nv0 = facet[facet_id0].nv;
	nv1 = facet[facet_id1].nv;
	index0 = facet[facet_id0].index;
	index1 = facet[facet_id1].index;

	for (i = 0; i < nv0; ++i) {
		if (index0[nv0 + 1 + i] == facet_id1)
			break;
	}

	for (j = 0; j < nv0; i = (i < nv0 - 1 ? i + 1 : 0), ++j) {
		if (index0[nv0 + 1 + i] != facet_id1)
			break;
	}

	//Сбор вершин нулевой грани
	for (j = 0; j < nv0; i = (i < nv0 - 1 ? i + 1 : 0), ++j) {
		vertex_list[j] = index0[i];
		if (index0[nv0 + 1 + i] == facet_id1)
			break;
		edge_list[j] = index0[nv0 + 1 + i];
	}
	//Утверждение. index0[nv0 + 1 + i] == facet_id1
	i = index0[2 * nv0 + 1 + i];
	//Сбор вершин первой грани
	for (k = 0; k < nv1; i = (i < nv1 - 1 ? i + 1 : 0), ++k, ++j) {
		if (index1[nv1 + 1 + i] == facet_id0)
			break;
		vertex_list[j] = index1[i];
		edge_list[j] = index1[nv1 + 1 + i];
	}

	nv = nv;
	//debug
	//	printf("join_facets_create_*vertex_list:\n");
	//	printf("facet %d: nv = %d\n\t", facet_id0, nv0);
	//	for(i = 0; i < nv0; ++i)
	//		printf("%d ", index0[i]);
	//	printf("\n");
	//	printf("facet %d: nv = %d\n\t", facet_id1, nv1);
	//	for(i = 0; i < nv1; ++i)
	//		printf("%d ", index1[i]);
	//	printf("\n");
	//	printf("joined facet: nv = %d\n", nv);
	//	for(i = 0; i < nv; ++i)
	//		printf("%d ", vertex_list[i]);
	//	printf("\n");
	//	for(i = 0; i < nv; ++i)
	//		printf("%d ", edge_list[i]);
	//	printf("\n");
}

void Polyhedron::list_squares_method(int nv, int *vertex_list, Plane *plane) {
	int i, id;
	double *x, *y, *z, a, b, c, d;

	x = new double[nv];
	y = new double[nv];
	z = new double[nv];

	for (i = 0; i < nv; ++i) {
		id = vertex_list[i];
		x[i] = vertex[id].x;
		y[i] = vertex[id].y;
		z[i] = vertex[id].z;
	}

	aprox(nv, x, y, z, a, b, c, d);

	plane->norm.x = a;
	plane->norm.y = b;
	plane->norm.z = c;
	plane->dist = d;

	//debug
	//printf("list_squares_method: a = %lf, b = %lf, c = %lf, d = %lf\n", a, b, c, d);

	if (x != NULL) delete[] x;
	if (y != NULL) delete[] y;
	if (z != NULL) delete[] z;
}

void Polyhedron::list_squares_method_weight(
		int nv,
		int *vertex_list,
		Plane *plane) {
	int i, id, i_prev, i_next, id0, id1;
	double *x, *y, *z, a, b, c, d;
	double *l, *w;

	x = new double[nv];
	y = new double[nv];
	z = new double[nv];
	l = new double[nv];
	w = new double[nv];

	for (i = 0; i < nv; ++i) {
		i_next = (i + 1) % nv;
		id0 = vertex_list[i];
		id1 = vertex_list[i_next];
		l[i] = sqrt(qmod(vertex[id0] - vertex[id1]));
	}

	for (i = 0; i < nv; ++i) {
		i_prev = (i - 1 + nv) % nv;
		w[i] = 0.5 * (l[i_prev] + l[i]);
	}

	for (i = 0; i < nv; ++i) {
		id = vertex_list[i];
		x[i] = vertex[id].x * w[i];
		y[i] = vertex[id].y * w[i];
		z[i] = vertex[id].z * w[i];
	}

	aprox(nv, x, y, z, a, b, c, d);

	plane->norm.x = a;
	plane->norm.y = b;
	plane->norm.z = c;
	plane->dist = d;

	//debug
	//printf("list_squares_method: a = %lf, b = %lf, c = %lf, d = %lf\n", a, b, c, d);

	if (x != NULL) delete[] x;
	if (y != NULL) delete[] y;
	if (z != NULL) delete[] z;
	if (z != NULL) delete[] l;
	if (z != NULL) delete[] w;
}

