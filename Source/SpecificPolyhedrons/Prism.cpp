/*
 * Prism.cpp
 *
 *  Created on: 17.06.2013
 *      Author: ilya
 */

#include "PolyhedraCorrectionLibrary.h"

Prism::Prism() :
		Polyhedron(),
		numVerticesBase(3),
		height(0.),
		radius(0.)
{
	init();
}

Prism::Prism(int nv, double h, double r) :
		Polyhedron(),
		numVerticesBase(nv),
		height(h),
		radius(r)
{
	init();
}

Prism::~Prism()
{
}

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
	index = new int[4];

	Plane plane;

	for (int i = 0; i < numVerticesBase; ++i)
	{
		index[0] = i;
		index[1] = (i + 1) % numVerticesBase;
		index[2] = index[1] + numVerticesBase;
		index[3] = index[0] + numVerticesBase;
		plane = Plane(vertices[index[0]], vertices[index[1]],
				vertices[index[2]]);
		facets[i] = Facet(i, 4, plane, index, this, false);
	}

	index = new int[numVerticesBase];

	for (int i = 0; i < numVerticesBase; ++i)
		index[i] = numVerticesBase - 1 - i;

	plane = Plane(Vector3d(0., 0., -1.), 0.);
	facets[numVerticesBase] = Facet(numVerticesBase, numVerticesBase, plane,
			index, this, false);

	for (int i = 0; i < numVerticesBase; ++i)
		index[i] = i + numVerticesBase;

	plane = Plane(Vector3d(0., 0., 1.), -height);
	facets[numVerticesBase + 1] = Facet(numVerticesBase + 1, numVerticesBase,
			plane, index, this, false);

	numVertices = 2 * numVerticesBase;
	numFacets = numVerticesBase + 2;

	if (index)
		delete[] index;
	DEBUG_END;
}
