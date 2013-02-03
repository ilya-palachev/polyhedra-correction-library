#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"


void Polyhedron::preprocess_polyhedron()
{
	int i;
	for (i = 0; i < numf; ++i)
		facet[i].preprocess_free();

	for(i = 0; i < numf; ++i)
		facet[i].preprocess();

//	if(vertexinfo != NULL)
//		delete[] vertexinfo;
	vertexinfo = new VertexInfo[numv];
	for(i = 0; i < numv; ++i) {
		vertexinfo[i] = VertexInfo(i, vertex[i], &(*this));
		vertexinfo[i].preprocess();
	}
}
