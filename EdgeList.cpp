#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"
#include "array_operations.h"

EdgeList::EdgeList() :
num(0),
edge0(NULL),
edge1(NULL),
next_facet(NULL),
scalar_mult(NULL),
facet(NULL) {
}

EdgeList::EdgeList(Facet* facet_orig) :
num(facet_orig->get_nv()),
edge0(new int[num]),
edge1(new int[num]),
next_facet(new int[num]),
scalar_mult(new double[num]),
facet(facet_orig) {
}

EdgeList::~EdgeList() {
	if (edge0 != NULL)
		delete[] edge0;
	if (edge1 != NULL)
		delete[] edge1;
	if (next_facet != NULL)
		delete[] next_facet;
	if (scalar_mult != NULL)
		delete[] scalar_mult;
}

EdgeList& EdgeList::operator=(const EdgeList& orig) {
	int i;
	num = orig.num;

	if (edge0 != NULL)
		delete [] edge0;
	edge0 = new int[num];
	for (i = 0; i < num; ++i)
		edge0 = orig.edge0[i];

	if (edge1 != NULL)
		delete [] edge1;
	edge1 = new int[num];
	for (i = 0; i < num; ++i)
		edge1 = orig.edge1[i];

	if (next_facet != NULL)
		delete [] next_facet;
	next_facet = new int[num];
	for (i = 0; i < num; ++i)
		next_facet = orig.next_facet[i];

	if (scalar_mult != NULL)
		delete[] scalar_mult;
	scalar_mult = new double[num];
	for (i = 0; i < num; ++i)
		scalar_mult[i] = orig.scalar_mult[i];

	facet = orig.facet;
}

void EdgeList::add_edge(int v0, int v1, int next_f, double sm) {
	//insert_binary(num, edge0, edge1, scalar_mult, v0, v1, sm);
	int first = 0; // Первый элемент в массиве
	int last = num; // Последний элемент в массиве

	while (first < last) {
		int mid = (first + last) / 2;
		if (sm <= scalar_mult[mid]) {
			last = mid;
		} else {
			first = mid + 1;
		}
	}
	insert_int(edge0, num, last, v0);
	insert_int(edge1, num, last, v1);
	insert_int(next_facet, num, last, next_f);
	insert_double(scalar_mult, num, last, sm);

	++num;
}

void EdgeList::get_next_edge(int& v0, int& v1) {
	int i;
	for (i = 0; i < num; ++i)
		if (edge0[i] == v0 && edge1[i] == v1 || edge0[i] == v1 && edge1[i] == v0)
			break;
	if (i == num) {
		fprintf(stdout, "EdgeList::get_next_edge : Error. Cannot find \
			the edge (%d - %d) at EdgeList\n", v0, v1);
		v0 = v1 = -1;
	} else {
		//По какую сторону брать следующее ребро - зависит от четности его
		//номера в списке...
		if (i % 2) {
			v0 = edge0[i - 1];
			v1 = edge1[i - 1];
		} else {
			v0 = edge0[i + 1];
			v1 = edge1[i + 1];
		}
	}
}
