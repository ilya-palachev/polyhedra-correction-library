#include "PolyhedraCorrectionLibrary.h"

void Facet::preprocess_free() {
	if (numVertices <= 0) {
		printf("\tError. nv = %d\n", numVertices);
		return;
	}
	if (indVertices == NULL) {
		printf("\tError. index == NULL\n");
		return;
	}
	for (int i = numVertices + 1; i < 3 * numVertices + 1; ++i)
		indVertices[i] = -1;
	indVertices[numVertices] = indVertices[0];
	rgb[0] = 100;
	rgb[1] = 100;
	rgb[2] = 100;
}

void Facet::preprocess() {
	int i, v0, v1;

	//	this->my_print_all();
	for (i = 0; i < numVertices; ++i) {
		v0 = indVertices[i];
		v1 = indVertices[i + 1];
		preprocess_edge(v0, v1, i);
	}
}

void Facet::preprocess_edge(
		int v0,
		int v1,
		int v0_id) {
	int i, pos;

	pos = -1;
	for (i = 0; i < parentPolyhedron->numFacets; ++i) {
		if (i == id)
			continue;
		pos = parentPolyhedron->facets[i].preprocess_search_edge(v1, v0);
		if (pos != -1) {
			indVertices[v0_id + numVertices + 1] = i;
			indVertices[v0_id + 2 * numVertices + 1] = pos;
			break;
		}
	}
	if (pos == -1) {
		printf("preprocess_edge : Error. Edge (%d %d) cannot be found anywhere\n",
				v0, v1);
	}
}

int Facet::preprocess_search_edge(
		int v0,
		int v1) {
	int i;
	for (i = 0; i < numVertices; ++i)
		if (indVertices[i] == v0 && indVertices[i + 1] == v1)
			break;
	if (i == numVertices)
		return -1;
	else
		return i < numVertices - 1 ?
				i + 1 : 0;
}

int Facet::preprocess_search_vertex(
		int v,
		int& v_next) {
	for (int i = 0; i < numVertices; ++i)
		if (indVertices[i] == v) {
			v_next = indVertices[i + 1];
			return i;
		}
	v_next = -1;
	return -1;
}

