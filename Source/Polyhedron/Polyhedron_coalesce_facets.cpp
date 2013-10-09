#include "PolyhedraCorrectionLibrary.h"

#define EPS_PARALL 1e-16
#define MAX_MIN_DIST 1e+1

void Polyhedron::coalesceFacets(int fid0, int fid1)
{
	DEBUG_START;
	Coalescer* coalescer = new Coalescer(get_ptr());
	coalescer->run(fid0, fid1);
	delete coalescer;
	DEBUG_END;
}

void Polyhedron::coalesceFacets(int n, int* fid)
{
	DEBUG_START;
	Coalescer* coalescer = new Coalescer(get_ptr());
	coalescer->run(n, fid);
	delete coalescer;
	DEBUG_END;
}
