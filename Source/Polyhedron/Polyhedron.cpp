#include "PolyhedraCorrectionLibrary.h"

Polyhedron::Polyhedron() :
				numVertices(0),
				numFacets(0),
				vertices(NULL),
				facets(NULL),
				vertexInfos(NULL),
				edgeLists(NULL),
				numEdges(0),
				edges(NULL),
				numContours(0),
				contours(NULL) {
	fprintf(stdout, "Creating empty polyhedron...\n");
}

Polyhedron::Polyhedron(
		int numv_orig,
		int numf_orig) :
				numVertices(numv_orig),
				numFacets(numf_orig),
				vertices(new Vector3d[numv_orig]),
				facets(new Facet[numf_orig]),
				vertexInfos(NULL),
				edgeLists(NULL),
				numEdges(0),
				edges(NULL),
				numContours(0),
				contours(NULL) {
	fprintf(stdout, "Creating polyhedron with numv = %d, numf = %d...\n",
			numVertices, numFacets);
}

Polyhedron::Polyhedron(
		int numv_orig,
		int numf_orig,
		Vector3d* vertex_orig,
		Facet* facet_orig) :
				numVertices(numv_orig),
				numFacets(numf_orig),
				vertices(new Vector3d[numv_orig]),
				facets(new Facet[numf_orig]),
				vertexInfos(NULL),
				edgeLists(NULL),
				numEdges(0),
				edges(NULL),
				numContours(0),
				contours(NULL) {
	fprintf(stdout, "Creating polyhedron by coping...\n");

	for (int i = 0; i < numVertices; ++i)
		vertices[i] = vertex_orig[i];
	for (int i = 0; i < numFacets; ++i)
		facets[i] = facet_orig[i];

}

Polyhedron::~Polyhedron() {
//	fprintf(stdout, "Deleting polyhedron...\n");
	if (vertices)
		delete[] vertices;
	if (facets)
		delete[] facets;
	if (vertexInfos)
		delete[] vertexInfos;
	if (edgeLists)
		delete[] edgeLists;
	if (edges)
		delete[] edges;
	if (contours)
		delete[] contours;

}

int Polyhedron::signum(
		Vector3d point,
		Plane plane) {
	double d = plane % point;
	if (fabs(d) < EPS_SIGNUM)
		return 0;
	return d > 0 ?
			1 : (d < 0 ?
					-1 : 0);
}

void Polyhedron::get_boundary(
		double& xmin,
		double& xmax,
		double& ymin,
		double& ymax,
		double& zmin,
		double& zmax) {
	int i;
	double tmp;

	xmin = xmax = vertices[0].x;
	ymin = ymax = vertices[0].y;
	zmin = zmax = vertices[0].z;

	for (i = 0; i < numVertices; ++i) {
		tmp = vertices[i].x;
		if (tmp > xmax)
			xmax = tmp;
		if (tmp < xmin)
			xmin = tmp;

		tmp = vertices[i].y;
		if (tmp > ymax)
			ymax = tmp;
		if (tmp < ymin)
			ymin = tmp;

		tmp = vertices[i].z;
		if (tmp > zmax)
			zmax = tmp;
		if (tmp < zmin)
			zmin = tmp;
	}
}

void Polyhedron::delete_empty_facets() {
	int i, j;
	for (i = 0; i < numFacets; ++i) {
		if (facets[i].numVertices < 3) {
			printf("===Facet %d is empty...\n", i);
			for (j = i; j < numFacets - 1; ++j) {
				facets[j] = facets[j + 1];
				facets[j].id = j;
			}

			--numFacets;
		}
	}
}
