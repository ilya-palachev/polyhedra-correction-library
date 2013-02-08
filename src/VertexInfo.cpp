#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"


VertexInfo::VertexInfo() :
index(NULL),
nf(0) {
}

VertexInfo::VertexInfo(
		const int id_orig,
		const int nf_orig,
		const Vector3d vector_orig,
		const int* index_orig,
		Polyhedron* poly_orig) :
id(id_orig),
nf(nf_orig),
vector(vector_orig),
poly(poly_orig) {

	if (!index_orig) {
		fprintf(stdout, "VertexInfo::VertexInfo. Error. index1 = NULL\n");
	}
	if (nf_orig < 3) {
		fprintf(stdout, "VertexInfo::VertexInfo. Error. nf1 < 3\n");
	}
	index = new int[3 * nf + 1];
	for (int i = 0; i < 3 * nf + 1; ++i)
		index[i] = index_orig[i];
}

VertexInfo::VertexInfo(
		const int id_orig,
		const Vector3d vector_orig,  
		Polyhedron* poly_orig):
id(id_orig),
nf(0),
vector(vector_orig),
index(NULL),
poly(poly_orig) {}

VertexInfo::~VertexInfo() {
	
	if (index != NULL && nf != 0) {
		delete[] index;
	}
	index = NULL;
	nf = 0;
}

VertexInfo& VertexInfo::operator =(const VertexInfo& orig) {
	int i;

	this->id = orig.id;
	this->vector = orig.vector;
	this->poly = orig.poly;
	this->nf = orig.nf;

	if (nf > 0) {
		if (index) delete[] index;
		index = new int[3 * nf + 1];
		for (i = 0; i < 3 * nf + 1; ++i)
			index[i] = orig.index[i];
	}
	return *this;
}

int VertexInfo::get_nf() {
    return nf;
}