/* 
 * Copyright (c) 2009-2013 Ilya Palachev <iliyapalachev@gmail.com>
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

#include "PolyhedraCorrectionLibrary.h"

Pyramid::Pyramid() :
		Polyhedron(),
		numVerticesBase(3),
		height(0.),
		radius(0.)
{
	DEBUG_START;
	init();
	DEBUG_END;
}

Pyramid::Pyramid(int nv, double h, double r) :
		Polyhedron(),
		numVerticesBase(nv),
		height(h),
		radius(r)
{
	DEBUG_START;
	init();
	DEBUG_END;
}

Pyramid::~Pyramid()
{
	DEBUG_START;
	DEBUG_END;
}

void Pyramid::init()
{
	DEBUG_START;
	numVertices = numFacets = numVerticesBase + 1;

	vertices = new Vector3d[numVerticesBase + 1];

	for (int iVertex = 0; iVertex < numVerticesBase; ++iVertex)
		vertices[iVertex] = Vector3d(cos(2 * M_PI * iVertex / numVerticesBase),
				sin(2 * M_PI * iVertex / numVerticesBase), 0.);
	vertices[numVerticesBase] = Vector3d(0., 0., height);

	facets = new Facet[numVerticesBase + 1];

	int *index;
	index = new int[3];

	for (int i = 0; i < numVerticesBase; ++i)
	{
		index[0] = numVerticesBase;
		index[1] = i;
		index[2] = (i + 1) % numVerticesBase;
		Plane plane = Plane(vertices[index[0]], vertices[index[1]],
				vertices[index[2]]);
		facets[i] = Facet(i, 3, plane, index, get_ptr(), false);
	}

	index = new int[numVerticesBase];

	for (int i = 0; i < numVerticesBase; ++i)
		index[i] = numVerticesBase - 1 - i;

	Plane plane = Plane(Vector3d(0., 0., -1.), 0.);
	facets[numVerticesBase] = Facet(numVerticesBase, numVerticesBase, plane,
			index, get_ptr(), false);

	if (index)
		delete[] index;
	DEBUG_END;
}

