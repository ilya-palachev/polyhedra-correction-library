/*
 * Pyramid.cpp
 *
 *  Created on: 17.06.2013
 *      Author: ilya
 */

#include "PolyhedraCorrectionLibrary.h"

Pyramid::Pyramid() :
		Polyhedron(),
		numVerticesBase(3),
		height(0.),
		radius(0.)
{
	init();
}

Pyramid::Pyramid(int nv, double h, double r) :
		Polyhedron(),
		numVerticesBase(nv),
		height(h),
		radius(r)
{
	init();
}

Pyramid::~Pyramid()
{
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
		facets[i] = Facet(i, 3, plane, index, this, false);
	}

	index = new int[numVerticesBase];

	for (int i = 0; i < numVerticesBase; ++i)
		index[i] = numVerticesBase - 1 - i;

	Plane plane = Plane(Vector3d(0., 0., -1.), 0.);
	facets[numVerticesBase] = Facet(numVerticesBase, numVerticesBase, plane,
			index, this, false);

	if (index)
		delete[] index;
	DEBUG_END;
}

