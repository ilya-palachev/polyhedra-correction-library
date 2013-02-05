#include "Polyhedron.h"

void Polyhedron::intersection(Plane iplane) {

	int i;
	int res, total_edges;
	int* ifIntersFacet;
	int fid_curr;

	ifIntersFacet = new int[numf];

	// 1. Подготовка списков ребер
	total_edges = 0;
	for (i = 0; i < numf; ++i) {
		res = facet[i].prepare_edge_list(iplane);
		ifIntersFacet[i] = res;
		total_edges += res;
	}

	// 2. Подсчет числа компонент сечения
	while (total_edges > 0) {
		for (i = 0; i < numf; ++i) {
			if (ifIntersFacet[i] > 0)
				break;
		}
		fid_curr = i;
	}
}