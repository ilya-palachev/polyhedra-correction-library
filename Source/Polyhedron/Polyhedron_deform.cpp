#include "PolyhedraCorrectionLibrary.h"

void Polyhedron::deform(int id, Vector3d delta)
{
	PointShifter* pShifter = new PointShifter(this);
	pShifter->run(id, delta);
	delete pShifter;
}

void Polyhedron::deform_w(int id, Vector3d delta)
{
	PointShifterWeighted* pShifter = new PointShifterWeighted(this);
	pShifter->run(id, delta);
	delete pShifter;
}

