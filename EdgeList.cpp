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
scalar_mult(NULL),
facet(NULL) {}

EdgeList::EdgeList(Facet* facet_orig) :
num(facet_orig->get_nv()),
edge0(new int[num]),
edge1(new int[num]),
scalar_mult(new double[num]),
facet(facet_orig) {}

EdgeList::~EdgeList() {
	if(edge0 != NULL)
		delete[] edge0;
	if(edge1 != NULL)
		delete[] edge1;
	if(scalar_mult != NULL)
		delete[] scalar_mult;
}

void EdgeList::add_edge(int v0, int v1, double sm) {
	insert_binary(num, edge0, edge1, scalar_mult, v0, v1, sm);
	++num;
}

void EdgeList::get_next_edge(int& v0, int& v1) {
	int i;
	for(i = 0; i < num; ++i)
		if(edge0[i] == v0 && edge1[i] == v1 || edge0[i] == v1 && edge1[i] == v0)
			break;
	if(i == num) {
		fprintf(stdout, "EdgeList::get_next_edge : Error. Cannot find \
			the edge (%d - %d) at EdgeList\n", v0, v1);
		v0 = v1 = -1;
	} else {
		//По какую сторону брать следующее ребро - зависит от четности его
		//номера в списке...
		if(i % 2) {
			v0 = edge0[i - 1];
			v1 = edge1[i - 1];
		} else {
			v0 = edge0[i + 1];
			v1 = edge1[i + 1];
		}
	}
}
