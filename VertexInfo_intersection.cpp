#include "Polyhedron.h"

int VertexInfo::intersection_find_next_facet(Plane iplane, int facet_id) {
	int i, sgn_prev, sgn;
	sgn_prev = poly->signum(poly->vertex[index[2 * nf]], iplane);
	for (i = 0; i < nf; ++i) {
		sgn_prev = sgn;
		sgn = poly->signum(poly->vertex[index[i + nf + 1]], iplane);
		if (sgn == 0 || sgn != sgn_prev)
			if (index[i] != facet_id)
				return index[i];
	}
	return -1;
}
