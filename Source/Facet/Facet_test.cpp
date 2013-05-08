#include "PolyhedraCorrectionLibrary.h"

bool Facet::test_self_intersection()
{
	int i, j;
	double s;
	Vector3d vi0, vi1, vj0, vj1, tmp0, tmp1;
	for (i = 0; i < numVertices; ++i)
	{
		vi0 = parentPolyhedron->vertices[indVertices[i]];
		vi1 = parentPolyhedron->vertices[indVertices[i + 1]];
		for (j = 0; j < numVertices; ++j)
		{
			if (j == i)
				continue;
			vj0 = parentPolyhedron->vertices[indVertices[j]];
			vj1 = parentPolyhedron->vertices[indVertices[j + 1]];
			tmp0 = (vi1 - vi0) % (vj0 - vi0);
			tmp1 = (vi1 - vi0) % (vj1 - vi0);
			s = tmp0 * tmp1;
			if (s < 0)
				return true;
		}
	}
	return false;
}
