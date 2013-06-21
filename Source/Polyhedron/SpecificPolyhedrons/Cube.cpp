/*
 * Cube.cpp
 *
 *  Created on: 17.06.2013
 *      Author: ilya
 */

#include "PolyhedraCorrectionLibrary.h"

Cube::Cube() :
		Polyhedron(),
		height(0.),
		center(Vector3d(0., 0.,  0.))
{
	init();
}

Cube::Cube(double h) :
		Polyhedron(),
		height(h),
		center(Vector3d(0., 0., 0.))
{
	init();
}

Cube::Cube(double h, double x, double y, double z) :
		Polyhedron(),
		height(h),
		center(Vector3d(x, y, z))
{
	init();
}

Cube::Cube(double h, Vector3d c) :
		Polyhedron(),
		height(h),
		center(c)
{
	init();
}

Cube::~Cube()
{
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
		int nv = facets[ifacet].numVertices = 4;
		facets[ifacet].indVertices = new int[3 * nv + 1];
		facets[ifacet].id = ifacet;
		facets[ifacet].parentPolyhedron = this;
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

	for (int i = 0; i < numFacets; ++i)
	{
		facets[i].parentPolyhedron = this;
	}
	DEBUG_END;
}
