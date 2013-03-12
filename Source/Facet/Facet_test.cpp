#include "PolyhedraCorrectionLibrary.h"

bool Facet::test_self_intersection() {
	int i, j;
	double s;
	Vector3d vi0, vi1, vj0, vj1, tmp0, tmp1;
	for (i = 0; i < nv; ++i) {
		vi0 = poly->vertex[index[i]];
		vi1 = poly->vertex[index[i + 1]];
		for (j = 0; j < nv; ++j) {
			if (j == i)
				continue;
			vj0 = poly->vertex[index[j]];
			vj1 = poly->vertex[index[j + 1]];
			tmp0 = (vi1 - vi0) % (vj0 - vi0);
			tmp1 = (vi1 - vi0) % (vj1 - vi0);
			s = tmp0 * tmp1;
			if (s < 0)
				return true;
		}
	}
	return false;
}
