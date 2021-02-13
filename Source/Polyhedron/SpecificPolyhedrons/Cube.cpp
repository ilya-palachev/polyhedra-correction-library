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

#include <memory>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Polyhedron/SpecificPolyhedrons/Cube.h"
#include "Polyhedron/Facet/Facet.h"

Cube::Cube() :
		height(0.),
		center(Vector3d(0., 0.,  0.))
{
	DEBUG_START;
	init();
	DEBUG_END;
}

Cube::Cube(double h) :
		height(h),
		center(Vector3d(0., 0., 0.))
{
	DEBUG_START;
	init();
	DEBUG_END;
}

Cube::Cube(double h, double x, double y, double z) :
		height(h),
		center(Vector3d(x, y, z))
{
	DEBUG_START;
	init();
	DEBUG_END;
}

Cube::Cube(double h, Vector3d c) :
		height(h),
		center(c)
{
	DEBUG_START;
	init();
	DEBUG_END;
}

Cube::~Cube()
{
	DEBUG_START;
	DEBUG_END;
}

void Cube::init()
{
	DEBUG_START;

	numFacets = 6;
	numVertices = 8;

	vertices = new Vector3d[numVertices];
	facets = new Facet[numFacets];

	double halfHeight = height;

	vertices[0] = Vector3d(center.x - halfHeight, center.y - halfHeight,
			center.z - halfHeight);
	vertices[1] = Vector3d(center.x + halfHeight, center.y - halfHeight,
			center.z - halfHeight);
	vertices[2] = Vector3d(center.x + halfHeight, center.y + halfHeight,
			center.z - halfHeight);
	vertices[3] = Vector3d(center.x - halfHeight, center.y + halfHeight,
			center.z - halfHeight);
	vertices[4] = Vector3d(center.x - halfHeight, center.y - halfHeight,
			center.z + halfHeight);
	vertices[5] = Vector3d(center.x + halfHeight, center.y - halfHeight,
			center.z + halfHeight);
	vertices[6] = Vector3d(center.x + halfHeight, center.y + halfHeight,
			center.z + halfHeight);
	vertices[7] = Vector3d(center.x - halfHeight, center.y + halfHeight,
			center.z + halfHeight);

	for (int ifacet = 0; ifacet < numFacets; ++ifacet)
	{
		int nv = 4;
		facets[ifacet].numVertices = nv;
		facets[ifacet].indVertices = new int[3 * nv + 1];
		facets[ifacet].id = ifacet;
	}

	facets[0].indVertices[0] = 0;
	facets[0].indVertices[1] = 3;
	facets[0].indVertices[2] = 2;
	facets[0].indVertices[3] = 1;
	facets[0].plane = Plane(Vector3d(0., 0., -1.), center.z - halfHeight);

	facets[1].indVertices[0] = 4;
	facets[1].indVertices[1] = 5;
	facets[1].indVertices[2] = 6;
	facets[1].indVertices[3] = 7;
	facets[1].plane = Plane(Vector3d(0., 0., 1.), -center.z - halfHeight);

	facets[2].indVertices[0] = 1;
	facets[2].indVertices[1] = 2;
	facets[2].indVertices[2] = 6;
	facets[2].indVertices[3] = 5;
	facets[2].plane = Plane(Vector3d(1., 0., 0.), -center.x - halfHeight);

	facets[3].indVertices[0] = 2;
	facets[3].indVertices[1] = 3;
	facets[3].indVertices[2] = 7;
	facets[3].indVertices[3] = 6;
	facets[3].plane = Plane(Vector3d(0., 1., 0.), -center.y - halfHeight);

	facets[4].indVertices[0] = 0;
	facets[4].indVertices[1] = 4;
	facets[4].indVertices[2] = 7;
	facets[4].indVertices[3] = 3;
	facets[4].plane = Plane(Vector3d(-1., 0., 0.), center.x - halfHeight);

	facets[5].indVertices[0] = 0;
	facets[5].indVertices[1] = 1;
	facets[5].indVertices[2] = 5;
	facets[5].indVertices[3] = 4;
	facets[5].plane = Plane(Vector3d(0., -1., 0.), center.y - halfHeight);

	DEBUG_END;
}
