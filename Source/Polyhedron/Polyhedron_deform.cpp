#include "PolyhedraCorrectionLibrary.h"

void Polyhedron::shiftPoint(int id, Vector3d delta)
{
	PointShifter* pShifter = new PointShifter(this);
	pShifter->run(id, delta);
	delete pShifter;
}

void Polyhedron::shiftPointWeighted(int id, Vector3d delta)
{
	PointShifterWeighted* pShifter = new PointShifterWeighted(this);
	pShifter->run(id, delta);
	delete pShifter;
}

