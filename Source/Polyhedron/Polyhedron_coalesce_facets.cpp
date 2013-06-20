#include "PolyhedraCorrectionLibrary.h"

#define EPS_PARALL 1e-16
#define MAX_MIN_DIST 1e+1

void Polyhedron::coalesceFacets(int fid0, int fid1)
{
	Coalescer* coalescer = new Coalescer(this);
	coalescer->run(fid0, fid1);
	delete coalescer;
}

void Polyhedron::coalesceFacets(int n, int* fid)
{
	Coalescer* coalescer = new Coalescer(this);
	coalescer->run(n, fid);
	delete coalescer;
}

int Polyhedron::test_structure()
{
	int i, res;
	res = 0;
	for (i = 0; i < numFacets; ++i)
	{
		res += facets[i].test_structure();
	}
	return res;
}
