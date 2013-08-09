#include "PolyhedraCorrectionLibrary.h"

void Polyhedron::intersect(Plane iplane)
{
	DEBUG_START;
	Intersector* intersector = new Intersector(this);
	intersector->run(iplane);
	delete intersector;
	DEBUG_END;
}

void Polyhedron::intersectCoalesceMode(Plane iplane, int jfid)
{
	DEBUG_START;
	Intersector* intersector = new Intersector(this);
	intersector->runCoalesceMode(iplane, jfid);
	delete intersector;
	DEBUG_END;
}
