#include "PolyhedraCorrectionLibrary.h"

void Polyhedron::deleteContent()
{
	DEBUG_START;
	if (vertices != NULL)
	{
		delete[] vertices;
		vertices = NULL;
	}
	if (facets != NULL)
	{
		delete[] facets;
		facets = NULL;
	}
	if (vertexInfos != NULL)
	{
		delete[] vertexInfos;
		vertexInfos = NULL;
	}
	if (edgeLists != NULL)
	{
		delete[] edgeLists;
		edgeLists = NULL;
	}
	DEBUG_END;
}

void Polyhedron::makeCube(double height, double xCenter, double yCenter,
		double zCenter)
{
	DEBUG_START;
	deleteContent();

	numFacets = 6;
	numVertices = 8;

	vertices = new Vector3d[numVertices];
	facets = new Facet[numFacets];

	double halfHeight = height;

	vertices[0] = Vector3d(xCenter - halfHeight, yCenter - halfHeight,
			zCenter - halfHeight);
	vertices[1] = Vector3d(xCenter + halfHeight, yCenter - halfHeight,
			zCenter - halfHeight);
	vertices[2] = Vector3d(xCenter + halfHeight, yCenter + halfHeight,
			zCenter - halfHeight);
	vertices[3] = Vector3d(xCenter - halfHeight, yCenter + halfHeight,
			zCenter - halfHeight);
	vertices[4] = Vector3d(xCenter - halfHeight, yCenter - halfHeight,
			zCenter + halfHeight);
	vertices[5] = Vector3d(xCenter + halfHeight, yCenter - halfHeight,
			zCenter + halfHeight);
	vertices[6] = Vector3d(xCenter + halfHeight, yCenter + halfHeight,
			zCenter + halfHeight);
	vertices[7] = Vector3d(xCenter - halfHeight, yCenter + halfHeight,
			zCenter + halfHeight);

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
	facets[0].plane = Plane(Vector3d(0., 0., -1.), zCenter - halfHeight);

	facets[1].indVertices[0] = 4;
	facets[1].indVertices[1] = 5;
	facets[1].indVertices[2] = 6;
	facets[1].indVertices[3] = 7;
	facets[1].plane = Plane(Vector3d(0., 0., 1.), -zCenter - halfHeight);

	facets[2].indVertices[0] = 1;
	facets[2].indVertices[1] = 2;
	facets[2].indVertices[2] = 6;
	facets[2].indVertices[3] = 5;
	facets[2].plane = Plane(Vector3d(1., 0., 0.), -xCenter - halfHeight);

	facets[3].indVertices[0] = 2;
	facets[3].indVertices[1] = 3;
	facets[3].indVertices[2] = 7;
	facets[3].indVertices[3] = 6;
	facets[3].plane = Plane(Vector3d(0., 1., 0.), -yCenter - halfHeight);

	facets[4].indVertices[0] = 0;
	facets[4].indVertices[1] = 4;
	facets[4].indVertices[2] = 7;
	facets[4].indVertices[3] = 3;
	facets[4].plane = Plane(Vector3d(-1., 0., 0.), xCenter - halfHeight);

	facets[5].indVertices[0] = 0;
	facets[5].indVertices[1] = 1;
	facets[5].indVertices[2] = 5;
	facets[5].indVertices[3] = 4;
	facets[5].plane = Plane(Vector3d(0., -1., 0.), yCenter - halfHeight);

	for (int i = 0; i < numFacets; ++i)
	{
		facets[i].parentPolyhedron = this;
	}
	DEBUG_END;
}

void Polyhedron::makePyramid(int numVerticesBase, double height, double radius)
{
	DEBUG_START;
	deleteContent();

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

void Polyhedron::makePrism(int numVerticesBase, double height, double radius)
{
	DEBUG_START;
	deleteContent();

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

void Polyhedron::makeCubeCutted()
{
	DEBUG_START;
	deleteContent();

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
