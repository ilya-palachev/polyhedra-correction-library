/*
 * CubeCutted.cpp
 *
 *  Created on: 17.06.2013
 *      Author: ilya
 */

#include "PolyhedraCorrectionLibrary.h"

CubeCutted::CubeCutted() :
		Polyhedron()
{
	DEBUG_START;
	init();
	DEBUG_END;
}

CubeCutted::~CubeCutted()
{
	DEBUG_START;
	DEBUG_END;
}

void CubeCutted::init()
{
	DEBUG_START;
	vertices = new Vector3d[16];

	vertices[0] = Vector3d(3., 0., 0.);
	vertices[1] = Vector3d(3., 3., 0.);
	vertices[2] = Vector3d(0., 3., 0.);
	vertices[3] = Vector3d(0., 0., 0.);
	vertices[4] = Vector3d(3., 1., 1.);
	vertices[5] = Vector3d(3., 2., 1.);
	vertices[6] = Vector3d(0., 2., 1.);
	vertices[7] = Vector3d(0., 1., 1.);
	vertices[8] = Vector3d(3., 0., 3.);
	vertices[9] = Vector3d(3., 1., 3.);
	vertices[10] = Vector3d(0., 1., 3.);
	vertices[11] = Vector3d(0., 0., 3.);
	vertices[12] = Vector3d(3., 2., 3.);
	vertices[13] = Vector3d(3., 3., 3.);
	vertices[14] = Vector3d(0., 3., 3.);
	vertices[15] = Vector3d(0., 2., 3.);

	facets = new Facet[10];

	int index[8];
	Plane plane;

	index[0] = 0;
	index[1] = 3;
	index[2] = 2;
	index[3] = 1;
	plane = Plane(Vector3d(0., 0., -1.), 0);
	facets[0] = Facet(0, 4, plane, index, this, false);

	index[0] = 4;
	index[1] = 5;
	index[2] = 6;
	index[3] = 7;
	plane = Plane(Vector3d(0., 0., 1.), -1.);
	facets[1] = Facet(1, 4, plane, index, this, false);

	index[0] = 8;
	index[1] = 9;
	index[2] = 10;
	index[3] = 11;
	plane = Plane(Vector3d(0., 0., 1.), -3.);
	facets[2] = Facet(2, 4, plane, index, this, false);

	index[0] = 12;
	index[1] = 13;
	index[2] = 14;
	index[3] = 15;
	plane = Plane(Vector3d(0., 0., 1.), -3.);
	facets[3] = Facet(3, 4, plane, index, this, false);

	index[0] = 0;
	index[1] = 1;
	index[2] = 13;
	index[3] = 12;
	index[4] = 5;
	index[5] = 4;
	index[6] = 9;
	index[7] = 8;
	plane = Plane(Vector3d(1., 0., 0.), -3.);
	facets[4] = Facet(4, 8, plane, index, this, false);

	index[0] = 3;
	index[1] = 11;
	index[2] = 10;
	index[3] = 7;
	index[4] = 6;
	index[5] = 15;
	index[6] = 14;
	index[7] = 2;
	plane = Plane(Vector3d(-1., 0., 0.), 0.);
	facets[5] = Facet(5, 8, plane, index, this, false);

	index[0] = 1;
	index[1] = 2;
	index[2] = 14;
	index[3] = 13;
	plane = Plane(Vector3d(0., 1., 0.), -3.);
	facets[6] = Facet(6, 4, plane, index, this, false);

	index[0] = 0;
	index[1] = 8;
	index[2] = 11;
	index[3] = 3;
	plane = Plane(Vector3d(0., -1., 0.), 0.);
	facets[7] = Facet(7, 4, plane, index, this, false);

	index[0] = 4;
	index[1] = 7;
	index[2] = 10;
	index[3] = 9;
	plane = Plane(Vector3d(0., 1., 0.), -1.);
	facets[8] = Facet(8, 4, plane, index, this, false);

	index[0] = 5;
	index[1] = 12;
	index[2] = 15;
	index[3] = 6;
	plane = Plane(Vector3d(0., -1., 0.), 2.);
	facets[9] = Facet(9, 4, plane, index, this, false);

	numVertices = 16;
	numFacets = 10;
	DEBUG_END;
}
