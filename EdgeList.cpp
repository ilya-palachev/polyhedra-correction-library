#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"

EdgeList::EdgeList() :
		id(-1),
		len(0),
		num(0),
		pointer(0),
		edge0(NULL),
		edge1(NULL),
		next_facet(NULL),
		next_direction(NULL),
		scalar_mult(NULL),
		id_v_new(NULL),
		isUsed(NULL),
		poly(NULL) {}

EdgeList::EdgeList(int id_orig, int len_orig, Polyhedron* poly_orig) :
		id(id_orig),
		len(len_orig),
		num(0),
		pointer(0),
		edge0(new int[len]),
		edge1(new int[len]),
		next_facet(new int[len]),
		next_direction(new int[len]),
		scalar_mult(new double[len]),
		id_v_new(new int[len]),
		isUsed(new bool[len]),
		poly(poly_orig) {}

EdgeList::~EdgeList() {
	if (edge0 != NULL)
		delete[] edge0;
	if (edge1 != NULL)
		delete[] edge1;
	if (next_facet != NULL)
		delete[] next_facet;
	if (next_direction != NULL)
		delete[] next_direction;
	if (scalar_mult != NULL)
		delete[] scalar_mult;
	if (id_v_new != NULL)
		delete[] id_v_new;
	if (isUsed != NULL)
		delete[] isUsed;
}

EdgeList& EdgeList::operator = (const EdgeList& orig) {
	int i;

	id = orig.id;
	len = orig.len;
	num = orig.num;
	pointer = orig.pointer;

	if (edge0 != NULL)
		delete [] edge0;
	if (edge1 != NULL)
		delete [] edge1;
	if (next_facet != NULL)
		delete [] next_facet;
	if (next_direction != NULL)
		delete [] next_direction;
	if (scalar_mult != NULL)
		delete[] scalar_mult;
	if (id_v_new != NULL)
		delete[] id_v_new;
	if (isUsed != NULL)
		delete[] isUsed;

	edge0 = new int[len];
	edge1 = new int[len];
	next_facet = new int[len];
	next_direction = new int[len];
	scalar_mult = new double[len];
	id_v_new = new int[len];
	isUsed = new bool[len];

	for (i = 0; i < num; ++i) {
		edge0[i] = orig.edge0[i];
		edge1[i] = orig.edge1[i];
		next_facet[i] = orig.next_facet[i];
		next_direction[i] = orig.next_direction[i];
		scalar_mult[i] = orig.scalar_mult[i];
		id_v_new[i] = orig.id_v_new[i];
		isUsed[i] = orig.isUsed[i];
	}
	return *this;
}
