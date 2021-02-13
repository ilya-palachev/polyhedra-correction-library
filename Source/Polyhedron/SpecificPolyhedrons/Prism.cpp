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

#define _USE_MATH_DEFINES
#undef __STRICT_ANSI__
#include <cmath>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Polyhedron/SpecificPolyhedrons/Prism.h"
#include "Polyhedron/Facet/Facet.h"


Prism::Prism() :
		Polyhedron(),
		numVerticesBase(3),
		height(0.),
		radius(0.)
{
	DEBUG_START;
	init();
	DEBUG_END;
}

Prism::Prism(int nv, double h, double r) :
		Polyhedron(),
		numVerticesBase(nv),
		height(h),
		radius(r)
{
	DEBUG_START;
	init();
	DEBUG_END;
}

Prism::~Prism()
{
	DEBUG_START;
	DEBUG_END;
}

#define NUM_VERTICES_IN_PRISM_ELEMENT 4

void Prism::init()
{
	DEBUG_START;
	vertices = new Vector3d[2 * numVerticesBase];

	for (int i = 0; i < numVerticesBase; ++i)
		vertices[i] = Vector3d(cos(2 * M_PI * i / numVerticesBase),
				sin(2 * M_PI * i / numVerticesBase), 0.);
	for (int i = 0; i < numVerticesBase; ++i)
		vertices[numVerticesBase + i] = Vector3d(
				cos(2 * M_PI * i / numVerticesBase),
				sin(2 * M_PI * i / numVerticesBase), height);

	facets = new Facet[numVerticesBase + 2];

	int *index;
	index = new int[3 * NUM_VERTICES_IN_PRISM_ELEMENT + 1];

	Plane plane;

	for (int i = 0; i < numVerticesBase; ++i)
	{
		index[0] = i;
		index[1] = (i + 1) % numVerticesBase;
		index[2] = index[1] + numVerticesBase;
		index[3] = index[0] + numVerticesBase;
		plane = Plane(vertices[index[0]], vertices[index[1]],
				vertices[index[2]]);
		facets[i] = Facet(i, NUM_VERTICES_IN_PRISM_ELEMENT, plane,
			index, NULL, false);
	}

	index = new int[3 * numVerticesBase + 1];

	for (int i = 0; i < numVerticesBase; ++i)
		index[i] = numVerticesBase - 1 - i;

	plane = Plane(Vector3d(0., 0., -1.), 0.);
	facets[numVerticesBase] = Facet(numVerticesBase, numVerticesBase, plane,
			index, NULL, false);

	for (int i = 0; i < numVerticesBase; ++i)
		index[i] = i + numVerticesBase;

	plane = Plane(Vector3d(0., 0., 1.), -height);
	facets[numVerticesBase + 1] = Facet(numVerticesBase + 1, numVerticesBase,
			plane, index, NULL, false);

	numVertices = 2 * numVerticesBase;
	numFacets = numVerticesBase + 2;

	if (index)
		delete[] index;
	DEBUG_END;
}
