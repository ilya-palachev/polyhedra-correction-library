#include "PolyhedraCorrectionLibrary.h"

#define EPSILON 1e-7
//#define DEFORM_SCALE //Этот макрос определяет, каким методом производить деформацию:
// экспоненциальным ростом штрафа или масштабированием
// (если он определен - то масштабированием)

//Данная функция отвечает за глобальную минимизацию (т. е. минимизируется отклонение в целом)
void Polyhedron::shiftPointLinearGlobal(int id, Vector3d delta)
{
	PointShifterLinear* pShifter = new PointShifterLinear(this);
	pShifter->runGlobal(id, delta);
	delete pShifter;
}

//Данная функция отвечает за локальную минимизацию (т. е. минимизируется отклонение на шаге)
void Polyhedron::shiftPointLinearLocal(int id, Vector3d delta)
{
	PointShifterLinear* pShifter = new PointShifterLinear(this);
	pShifter->runLocal(id, delta);
	delete pShifter;
}

void Polyhedron::shiftPointLinearTest(int id, Vector3d delta, int mode,
		int& num_steps, double& norm_sum)
{
	PointShifterLinear* pShifter = new PointShifterLinear(this);
	pShifter->runTest(id, delta, mode, num_steps, norm_sum);
	delete pShifter;
}

void Polyhedron::shiftPointLinearPartial(int id, Vector3d delta, int num)
{
	PointShifterLinear* pShifter = new PointShifterLinear(this);
	pShifter->runPartial(id, delta, num);
	delete pShifter;
}

double Polyhedron::min_dist(int id)
{
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
	return min_dist;
}

void Polyhedron::import_coordinates(Polyhedron& orig)
{
	int i;
	for (i = 0; i < numVertices; ++i)
	{
		vertices[i] = orig.vertices[i];
	}
	for (i = 0; i < numFacets; ++i)
	{
		facets[i].plane = orig.facets[i].plane;
	}
}

