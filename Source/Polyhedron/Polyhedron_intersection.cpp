#include "PolyhedraCorrectionLibrary.h"

void Polyhedron::intersect(Plane iplane)
{
	Intersector* intersector = new Intersector(this);
	intersector->run(iplane);
	delete intersector;
}

void Polyhedron::intersect_j(Plane iplane, int jfid)
{
	Intersector* intersector = new Intersector(this);
	intersector->runJoinMode(iplane, jfid);
	delete intersector;
}
