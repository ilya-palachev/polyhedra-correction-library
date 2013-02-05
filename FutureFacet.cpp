#include "Polyhedron.h"

FutureFacet::FutureFacet() :
		id(-1),
		len(0),
		nv(0),
		edge0(NULL),
		edge1(NULL),
		src_facet(NULL),
		id_v_new(NULL)
{}

FutureFacet::~FutureFacet() {
	if (edge0 != NULL)
		delete[] edge0;
	if (edge1 != NULL)
		delete[] edge1;
	if (src_facet != NULL)
		delete[] src_facet;
	if (id_v_new != NULL)
		delete[] id_v_new;
}

FutureFacet::FutureFacet(int len_orig) :
		id(-1),
		len(len_orig),
		nv(0),
		edge0(new int[len]),
		edge1(new int[len]),
		src_facet(new int[len]),
		id_v_new(new int[len])
{}

FutureFacet::FutureFacet(const FutureFacet& orig) :
		id(orig.id),
		len(orig.len),
		nv(orig.nv) {

	int i;
	if(len > 0) {
		edge0 = new int[len];
		edge1 = new int[len];
		src_facet = new int[len];
		id_v_new = new int[len];

		for (i = 0; i < nv; ++i) {
			edge0[i] = orig.edge0[i];
			edge1[i] = orig.edge1[i];
			src_facet[i] = orig.src_facet[i];
			id_v_new[i] = orig.id_v_new[i];
		}
	} else {
		edge0 = NULL;
		edge1 = NULL;
		src_facet = NULL;
	}
}

void FutureFacet::add_edge(int v0, int v1, int src_f) {
	if (nv >= len) {
		fprintf(stdout, "FutureFacet::add_edge : Error. nv >= len\n");
		return;
	}
	edge0[nv] = v0;
	edge1[nv] = v1;
	src_facet[nv] = src_f;
	++nv;
}

FutureFacet& FutureFacet::operator =(const FutureFacet& orig) {
	int i;
	id = orig.id;
	len = orig.len;
	nv = orig.nv;
	if(len > 0) {
		edge0 = new int[len];
		edge1 = new int[len];
		src_facet = new int[len];
		id_v_new = new int[len];

		for (i = 0; i < nv; ++i) {
			edge0[i] = orig.edge0[i];
			edge1[i] = orig.edge1[i];
			src_facet[i] = orig.src_facet[i];
			id_v_new[i] = orig.id_v_new[i];
		}
	} else {
		edge0 = NULL;
		edge1 = NULL;
		src_facet = NULL;
	}
	return *this;
}

int FutureFacet::get_nv() {
	return nv;
}

void FutureFacet::get_edge(int pos, int& v0, int& v1, int& src_f, int& id_v) {
	if (pos < 0 || pos >= nv) {
		v0 = v1 = src_f = id_v = -1;
		return;
	}
	v0 = edge0[pos];
	v1 = edge1[pos];
	src_f = src_facet[pos];
	id_v = id_v_new[pos];
}

void FutureFacet::set_id(int val) {
	id = val;
}