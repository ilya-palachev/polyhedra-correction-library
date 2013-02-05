#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"

EdgeList::EdgeList() :
num(0),
edge0(NULL),
edge1(NULL),
next_facet(NULL),
next_direction(NULL),
scalar_mult(NULL),
facet(NULL) {
#ifdef DEBUG1
	fprintf(stdout, "EdgeList::EdgeList()\n");
#endif
}

EdgeList::EdgeList(Facet* facet_orig) :
num(0),
edge0(new int[num]),
edge1(new int[num]),
next_facet(new int[num]),
next_direction(new int[num]),
scalar_mult(new double[num]),
facet(facet_orig) {
#ifdef DEBUG1
	fprintf(stdout, "EdgeList::EdgeList(Facet* facet_orig)\n");
#endif
}

EdgeList::~EdgeList() {
#ifdef DEBUG1
	fprintf(stdout, "EdgeList::~EdgeList()\n");
#endif

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
}

EdgeList& EdgeList::operator=(const EdgeList& orig) {
	int i;
#ifdef DEBUG1
	fprintf(stdout, "EdgeList::operator=(const EdgeList& orig)\n");
#endif


	num = orig.num;

	if (edge0 != NULL)
		delete [] edge0;
	edge0 = new int[orig.facet->get_nv()];
	for (i = 0; i < num; ++i)
		edge0[i] = orig.edge0[i];

	if (edge1 != NULL)
		delete [] edge1;
	edge1 = new int[orig.facet->get_nv()];
	for (i = 0; i < num; ++i)
		edge1[i] = orig.edge1[i];

	if (next_facet != NULL)
		delete [] next_facet;
	next_facet = new int[orig.facet->get_nv()];
	for (i = 0; i < num; ++i)
		next_facet[i] = orig.next_facet[i];

	if (next_direction != NULL)
		delete [] next_direction;
	next_direction = new int[orig.facet->get_nv()];
	for (i = 0; i < num; ++i)
		next_direction[i] = orig.next_direction[i];

	if (scalar_mult != NULL)
		delete[] scalar_mult;
	scalar_mult = new double[orig.facet->get_nv()];
	for (i = 0; i < num; ++i)
		scalar_mult[i] = orig.scalar_mult[i];

	facet = orig.facet;

}

void EdgeList::set_facet(Facet* facet_orig) {
	facet = facet_orig;
	int len = facet_orig->get_nv();
	num = 0;
	
	if (len > 0) {
		if (edge0 != NULL)
			delete [] edge0;
		edge0 = new int[len];

		if (edge1 != NULL)
			delete [] edge1;
		edge1 = new int[len];

		if (next_facet != NULL)
			delete [] next_facet;
		next_facet = new int[len];

		if (next_direction != NULL)
			delete [] next_direction;
		next_direction = new int[len];

		if (scalar_mult != NULL)
			delete[] scalar_mult;
		scalar_mult = new double[len];
	} else {
		edge0 = edge1 = next_facet = next_direction = NULL;
		scalar_mult = NULL;
	}

}
