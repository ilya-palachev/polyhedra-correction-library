#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"

#define DEFAULT_NV 1000

Facet::Facet() :
		id(-1),
		index(new int[3 * DEFAULT_NV + 1]),
		nv(DEFAULT_NV),
		plane() {
	rgb[0] = 100;
	rgb[1] = 100;
	rgb[2] = 100;
}

Facet::Facet(
		const int id_orig,
		const int nv_orig,
		const Plane plane_orig,
		const int* index_orig,
		Polyhedron* poly_orig,
		const bool ifLong = false) :

		id(id_orig),
		nv(nv_orig),
		plane(plane_orig),
		poly(poly_orig) {

	rgb[0] = 100;
	rgb[1] = 100;
	rgb[2] = 100;



	if (!poly) {
		fprintf(stdout, "Facet::Facet. Error. poly = NULL\n");
	}
	if (!index_orig) {
		fprintf(stdout, "Facet::Facet. Error. index_orig = NULL\n");
	}
	if (nv_orig < 3) {
		fprintf(stdout, "Facet::Facet. Error. nv_orig < 3\n");
	}
	index = new int[3 * nv + 1];
	if (ifLong) {
		for (int i = 0; i < 3 * nv + 1; ++i)
			index[i] = index_orig[i];
	} else {
		for (int i = 0; i < nv + 1; ++i)
			index[i] = index_orig[i];
		for (int i = nv + 1; i < 3 * nv + 1; ++i)
			index[i] = -1;
	}
}

Facet& Facet::operator =(const Facet& facet1) {
	int i;

	id = facet1.id;
	nv = facet1.nv;
	plane = facet1.plane;

	if (index) delete[] index;
	index = new int[3 * nv + 1];
	for (i = 0; i < 3 * nv + 1; ++i)
		index[i] = facet1.index[i];

	poly = facet1.poly;

	rgb[0] = facet1.rgb[0];
	rgb[1] = facet1.rgb[1];
	rgb[2] = facet1.rgb[2];

	return *this;
}

Facet::~Facet() {
#ifdef DEBUG1
	fprintf(stdout, "Deleting facet[%d]\n", id);
#endif
	if (index) delete[] index;
}

int Facet::get_id() {
	return id;
}

int Facet::get_nv() {
	return nv;
}

Plane Facet::get_plane() {
	return plane;
}

int Facet::get_index(int pos) {
	return index[pos];
}

char Facet::get_rgb(int pos) {
	return rgb[pos];
}

void Facet::set_poly(Polyhedron* poly_new) {
	poly = poly_new;
}


void Facet::get_next_facet(
		int pos_curr,
		int& pos_next,
		int& fid_next,
		int& v_curr) {

	fid_next = index[pos_curr + nv + 1];
	pos_next = index[pos_curr + 2 * nv + 1];
	v_curr = index[pos_curr + 1];
}
