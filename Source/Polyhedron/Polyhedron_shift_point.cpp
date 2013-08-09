#include "PolyhedraCorrectionLibrary.h"

void Polyhedron::shiftPoint(int id, Vector3d delta)
{
	DEBUG_START;
	PointShifter* pShifter = new PointShifter(this);
	pShifter->run(id, delta);
	delete pShifter;
	DEBUG_END;
}

void Polyhedron::shiftPointWeighted(int id, Vector3d delta)
{
	DEBUG_START;
	PointShifterWeighted* pShifter = new PointShifterWeighted(this);
	pShifter->run(id, delta);
	delete pShifter;
	DEBUG_END;
}

#define EPSILON 1e-7
//#define DEFORM_SCALE //Этот макрос определяет, каким методом производить деформацию:
// экспоненциальным ростом штрафа или масштабированием
// (если он определен - то масштабированием)

//Данная функция отвечает за глобальную минимизацию (т. е. минимизируется отклонение в целом)
void Polyhedron::shiftPointLinearGlobal(int id, Vector3d delta)
{
	DEBUG_START;
	PointShifterLinear* pShifter = new PointShifterLinear(this);
	pShifter->runGlobal(id, delta);
	delete pShifter;
	DEBUG_END;
}

//Данная функция отвечает за локальную минимизацию (т. е. минимизируется отклонение на шаге)
void Polyhedron::shiftPointLinearLocal(int id, Vector3d delta)
{
	DEBUG_START;
	PointShifterLinear* pShifter = new PointShifterLinear(this);
	pShifter->runLocal(id, delta);
	delete pShifter;
	DEBUG_END;
}

void Polyhedron::shiftPointLinearTest(int id, Vector3d delta, int mode,
		int& num_steps, double& norm_sum)
{
	DEBUG_START;
	PointShifterLinear* pShifter = new PointShifterLinear(this);
	pShifter->runTest(id, delta, mode, num_steps, norm_sum);
	delete pShifter;
	DEBUG_END;
}

void Polyhedron::shiftPointLinearPartial(int id, Vector3d delta, int num)
{
	DEBUG_START;
	PointShifterLinear* pShifter = new PointShifterLinear(this);
	pShifter->runPartial(id, delta, num);
	delete pShifter;
	DEBUG_END;
}

double Polyhedron::distToNearestNeighbour(int id)
{
	DEBUG_START;
	int i, nf, *index;
	double dist, min_dist;

	nf = vertexInfos[id].numFacets;
	index = vertexInfos[id].indFacets;
	for (i = 0; i < nf; ++i)
	{
		dist = qmod(vertices[id] - vertices[index[nf + 1 + i]]);
		dist = sqrt(dist);
		if (i == 0 || dist < min_dist)
			min_dist = dist;
	}
	DEBUG_END;
	return min_dist;
}

void Polyhedron::copyCoordinates(Polyhedron& orig)
{
	DEBUG_START;
	int i;
	for (i = 0; i < numVertices; ++i)
	{
		vertices[i] = orig.vertices[i];
	}
	for (i = 0; i < numFacets; ++i)
	{
		facets[i].plane = orig.facets[i].plane;
	}
	DEBUG_END;
}


