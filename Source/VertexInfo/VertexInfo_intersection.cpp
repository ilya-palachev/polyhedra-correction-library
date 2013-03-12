#include "PolyhedraCorrectionLibrary.h"

int VertexInfo::intersection_find_next_facet(Plane iplane, int facet_id) {
	int i, sgn_prev, sgn_curr;
//	my_fprint_all(stdout);
	sgn_curr = poly->signum(poly->vertex[index[2 * nf]], iplane);
	for (i = 0; i < nf; ++i) {
		sgn_prev = sgn_curr;
		sgn_curr = poly->signum(poly->vertex[index[i + nf + 1]], iplane);
		if (sgn_curr != sgn_prev)
			if (index[i] != facet_id)
				return index[i];
	}
//	fprintf(stdout, "VertexInfo::intersection_find_next_facet : Error.\n");
	return -1;
}
