/*
 * Copyright (c) 2009-2016 Ilya Palachev <iliyapalachev@gmail.com>
 *
 * This file is part of Polyhedra Correction Library.
 *
 * Polyhedra Correction Library is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Polyhedra Correction Library is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Polyhedra Correction Library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstdlib>
#include <cmath>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Polyhedron/Polyhedron.h"
#include "Correctors/PointShifter/PointShifter.h"
#include "Correctors/PointShifterLinear/PointShifterLinear.h"
#include "Correctors/PointShifterWeighted/PointShifterWeighted.h"
#include "Polyhedron/Facet/Facet.h"
#include "Polyhedron/VertexInfo/VertexInfo.h"

void Polyhedron::shiftPoint(int id, Vector3d delta)
{
	DEBUG_START;
	PointShifter *pShifter = new PointShifter(get_ptr());
	pShifter->run(id, delta);
	delete pShifter;
	DEBUG_END;
}

void Polyhedron::shiftPointWeighted(int id, Vector3d delta)
{
	DEBUG_START;
	PointShifterWeighted *pShifter = new PointShifterWeighted(get_ptr());
	pShifter->run(id, delta);
	delete pShifter;
	DEBUG_END;
}

#define EPSILON 1e-7
//#define DEFORM_SCALE //Этот макрос определяет, каким методом производить
//деформацию:
// экспоненциальным ростом штрафа или масштабированием
// (если он определен - то масштабированием)

//Данная функция отвечает за глобальную минимизацию (т. е. минимизируется
//отклонение в целом)
void Polyhedron::shiftPointLinearGlobal(int id, Vector3d delta)
{
	DEBUG_START;
	PointShifterLinear *pShifter = new PointShifterLinear(get_ptr());
	pShifter->runGlobal(id, delta);
	delete pShifter;
	DEBUG_END;
}

//Данная функция отвечает за локальную минимизацию (т. е. минимизируется
//отклонение на шаге)
void Polyhedron::shiftPointLinearLocal(int id, Vector3d delta)
{
	DEBUG_START;
	PointShifterLinear *pShifter = new PointShifterLinear(get_ptr());
	pShifter->runLocal(id, delta);
	delete pShifter;
	DEBUG_END;
}

void Polyhedron::shiftPointLinearTest(int id, Vector3d delta, int mode, int &num_steps, double &norm_sum)
{
	DEBUG_START;
	PointShifterLinear *pShifter = new PointShifterLinear(get_ptr());
	pShifter->runTest(id, delta, mode, num_steps, norm_sum);
	delete pShifter;
	DEBUG_END;
}

void Polyhedron::shiftPointLinearPartial(int id, Vector3d delta, int num)
{
	DEBUG_START;
	PointShifterLinear *pShifter = new PointShifterLinear(get_ptr());
	pShifter->runPartial(id, delta, num);
	delete pShifter;
	DEBUG_END;
}

double Polyhedron::distToNearestNeighbour(int id)
{
	DEBUG_START;
	double dist = RAND_MAX;
	double min_dist = RAND_MAX;

	int nf = vertexInfos[id].numFacets;
	auto &index = vertexInfos[id].indFacets;
	for (int i = 0; i < nf; ++i)
	{
		dist = qmod(vertices[id] - vertices[index[nf + 1 + i]]);
		dist = sqrt(dist);
		if (i == 0 || dist < min_dist)
			min_dist = dist;
	}
	DEBUG_END;
	return min_dist;
}

void Polyhedron::copyCoordinates(Polyhedron &orig)
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
