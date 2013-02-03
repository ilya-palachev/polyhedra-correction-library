#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"

Polyhedron::Polyhedron():
		vertex(NULL),
		facet(NULL),
		vertexinfo(NULL),
		numv(0),
		numf(0)
{
}

Polyhedron::Polyhedron(int numv_orig, int numf_orig, Vector3d* vertex_orig, Facet* facet_orig):
		numv(numv_orig),
		numf(numf_orig)
{
	
	vertex = new Vector3d[numv];
	facet = new Facet[numf];

	for(int i = 0; i < numv; ++i)
		vertex[i] = vertex_orig[i];
	for(int i = 0; i < numf; ++i)
		facet[i] = facet_orig[i];

}

Polyhedron::~Polyhedron()
{
	if(vertex) delete[] vertex;
	if(facet) delete[] facet;
	if(vertexinfo) delete[] vertexinfo;
}

//Оператор "равно" не работает. Выдает segfault. Так и не удалось выяснить
//почему.
Polyhedron& Polyhedron::operator=(const Polyhedron& poly)
{
	int i;

	numv = poly.numv;
	numf = poly.numf;

	if(vertex)     delete[] vertex;
	if(facet)      delete[] facet;
	if(vertexinfo) delete[] vertexinfo;

	vertex = new Vector3d[numv];
	for(i = 0; i < numv; ++i)
		vertex[i] = poly.vertex[i];

	facet = new Facet[numf];
	for(i = 0; i < numf; ++i)
		facet[i] = poly.facet[i];

//	vertexinfo = NULL;
//	if(!poly.vertexinfo)
//	{
//		printf("poly.vertexinfo != NULL\n");
//		vertexinfo = new VertexInfo[numv];
//		for(i = 0; i < numv; ++i)
//			vertexinfo[i] = poly.vertexinfo[i];
//	}

	return *this;
}

