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
//	fprintf(stdout, "Deleting polyhedron...\n");
	if (vertex)
		delete[] vertex;
	if (facet)
		delete[] facet;
	if (vertexinfo)
		delete[] vertexinfo;
//	if (log_file != stderr)
//		fclose(log_file);
}

int Polyhedron::signum(Vector3d point, Plane plane)
{
	double d = plane % point;
	if (fabs(d) < EPS_SIGNUM)
		return 0;
	return d > 0 ? 1 : (d < 0 ? -1 : 0);
}

void Polyhedron::get_boundary(
		double& xmin, double& xmax,
		double& ymin, double& ymax,
		double& zmin, double& zmax) {
	int i;
	double tmp;

	xmin = xmax = vertex[0].x;
	ymin = ymax = vertex[0].y;
	zmin = zmax = vertex[0].z;

	for (i = 0; i < numv; ++i) {
		tmp = vertex[i].x;
		if (tmp > xmax)
			xmax = tmp;
		if (tmp < xmin)
			xmin = tmp;

		tmp = vertex[i].y;
		if (tmp > ymax)
			ymax = tmp;
		if (tmp < ymin)
			ymin = tmp;

		tmp = vertex[i].z;
		if (tmp > zmax)
			zmax = tmp;
		if (tmp < zmin)
			zmin = tmp;
	}
}
