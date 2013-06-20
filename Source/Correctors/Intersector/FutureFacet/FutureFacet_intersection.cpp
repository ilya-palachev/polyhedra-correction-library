#include "PolyhedraCorrectionLibrary.h"

void FutureFacet::generate_facet(Facet& facet, int fid, Plane& iplane, int numv,
		EdgeSet* es)
{
	int i, v0, v1, res_id;
	int *index;

	index = new int[nv];

	for (i = 0; i < nv; ++i)
	{
		v0 = edge0[i];
		v1 = edge1[i];
		if (v0 == v1)
			index[i] = v0;
		else
		{
			res_id = es->search_edge(v0, v1);
			if (res_id < 0)
			{
				fprintf(stdout,
						"FutureFacet::generate_facet : Error. \
						Cannot find edge %d %d in edge set\n",
						v0, v1);
				res_id = -1;
			}
			else
			{
				res_id += numv;
			}
			index[i] = res_id;
		}
	}
	facet = Facet(fid, nv, iplane, index, NULL, false);
	if (index != NULL)
		delete[] index;
}
