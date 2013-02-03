#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"

void VertexInfo::preprocess() {

	int i;
	int pos_curr = -100, pos_next = -100;
	int v_curr = -100;
	int fid_first = -100, fid_curr = -100, fid_next = -100;

	for(i = 0; i < poly->numf; ++i) {
		pos_next = poly->facet[i].preprocess_search_vertex(id, v_curr);
		if(pos_next != -1)
			break;
	}
	if(pos_next == -1) {
		return;
	}
	fid_first = fid_next = i;

	//count nf
	nf = 0;
	do {
		++nf;
		pos_curr = pos_next;
		fid_curr = fid_next;
		poly->facet[fid_curr].get_next_facet(
			pos_curr, pos_next, fid_next, v_curr);
	} while (fid_next != fid_first);

	index = new int[2 * nf + 1];

	pos_next = poly->facet[fid_first].preprocess_search_vertex(id, v_curr);
	fid_next = fid_first;
	for(i = 0; i < nf; ++i) {
		pos_curr = pos_next;
		fid_curr = fid_next;

		index[i] = fid_curr;
		index[i + nf + 1] = pos_curr;

		poly->facet[fid_curr].get_next_facet(
			pos_curr, pos_next, fid_next, v_curr);
	}
	index[nf] = index[0];
	this->my_fprint_all(stdout);
}

