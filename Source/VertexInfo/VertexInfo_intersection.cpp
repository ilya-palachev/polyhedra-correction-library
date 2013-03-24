#include "PolyhedraCorrectionLibrary.h"

int VertexInfo::intersection_find_next_facet(
		Plane iplane,
		int facet_id) {
	int i, sgn_prev, sgn_curr;
	sgn_curr = parentPolyhedron->signum(
			parentPolyhedron->vertices[indFacets[2 * numFacets]], iplane);
	for (i = 0; i < numFacets; ++i) {
		sgn_prev = sgn_curr;
		sgn_curr = parentPolyhedron->signum(
				parentPolyhedron->vertices[indFacets[i + numFacets + 1]], iplane);
		if (sgn_curr != sgn_prev)
			if (indFacets[i] != facet_id)
				return indFacets[i];
	}
	return -1;
}
