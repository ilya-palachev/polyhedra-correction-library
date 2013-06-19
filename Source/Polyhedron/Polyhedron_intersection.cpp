#include "PolyhedraCorrectionLibrary.h"

void Polyhedron::intersect(Plane iplane)
{
	Intersector* intersector = new Intersector(this);
	intersector->run(iplane);
	delete intersector;
}

void Polyhedron::intersectCoalesceMode(Plane iplane, int jfid)
{
	Intersector* intersector = new Intersector(this);
	intersector->runCoalesceMode(iplane, jfid);
	delete intersector;
}
