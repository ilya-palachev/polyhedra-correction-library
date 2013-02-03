#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"

Polyhedron::Polyhedron() :
vertex(NULL),
facet(NULL),
vertexinfo(NULL),
numv(0),
numf(0) {
//log_file(stderr) {
	fprintf(stdout, "Creating empty polyhedron...\n");
}

Polyhedron::Polyhedron(
		int numv_orig,
		int numf_orig,
		Vector3d* vertex_orig,
		Facet* facet_orig) :
//		FILE* log_file_orig = stderr) :
numv(numv_orig),
numf(numf_orig)
//log_file(log_file_orig)
{
	fprintf(stdout, "Creating polyhedron by coping...\n");

	vertex = new Vector3d[numv];
	facet = new Facet[numf];

	for (int i = 0; i < numv; ++i)
		vertex[i] = vertex_orig[i];
	for (int i = 0; i < numf; ++i)
		facet[i] = facet_orig[i];

}

Polyhedron::~Polyhedron() {
	fprintf(stdout, "Deleting polyhedron...\n");
	if (vertex)
		delete[] vertex;
	if (facet)
		delete[] facet;
	if (vertexinfo)
		delete[] vertexinfo;
//	if (log_file != stderr)
//		fclose(log_file);
}
