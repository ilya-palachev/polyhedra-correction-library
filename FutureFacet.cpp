#include "Polyhedron.h"

FutureFacet::FutureFacet() :
		len(0),
		nv(0),
		edge0(NULL),
		edge1(NULL),
		src_facet(NULL)
{}

FutureFacet::~FutureFacet() {
	if (edge0 != NULL)
		delete[] edge0;
	if (edge1 != NULL)
		delete[] edge1;
	if (src_facet != NULL)
		delete[] src_facet;
}

FutureFacet::FutureFacet(int len_orig) :
		len(len_orig),
		nv(0),
		edge0(new int[len]),
		edge1(new int[len]),
		src_facet(new int[len])
{}

FutureFacet::FutureFacet(const FutureFacet& orig) :
		len(orig.len),
		nv(orig.nv) {

	int i;
	if(len > 0) {
		edge0 = new int[len];
		for (i = 0; i < nv; ++i)
			edge0[i] = orig.edge0[i];

		edge1 = new int[len];
		for (i = 0; i < nv; ++i)
			edge1[i] = orig.edge1[i];

		src_facet = new int[len];
		for (i = 0; i < nv; ++i)
			src_facet[i] = orig.src_facet[i];
	} else {
		edge0 = NULL;
		edge1 = NULL;
		src_facet = NULL;
	}
}

void FutureFacet::add_edge(int v0, int v1) {

	if (nv >= len) {
		fprintf(stdout, "FutureFacet::add_edge : Error. nv >= len\n");
		return;
	}
	edge0[nv] = v0;
	edge1[nv] = v1;
	++nv;
}










