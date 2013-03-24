#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "PolyhedraCorrectionLibrary.h"

void VertexInfo::preprocess() {

	int i;
	int pos_curr;
	int pos_next;
	int v_curr;
	int fid_first;
	int fid_curr;
	int fid_next;

	//	pos_curr = -100;
	//	pos_next = -100;
	//	v_curr = -100;
	//	fid_first = -100;
	//	fid_curr = -100;
	//	fid_next = -100;

//    printf("\t1. Searching first facet : ");
	for (i = 0; i < parentPolyhedron->numFacets; ++i) {
//        printf("%d ", i);
		pos_next = parentPolyhedron->facets[i].preprocess_search_vertex(id, v_curr);
		if (pos_next != -1)
			break;
	}
//    printf("\n");
	if (pos_next == -1) {
		return;
	}
	fid_first = fid_next = i;

	//count nf
//    printf("\t2. Searching the number of facets : ");
	numFacets = 0;
	do {
//        printf("%d ", fid_curr);
		++numFacets;
		pos_curr = pos_next;
		fid_curr = fid_next;
		parentPolyhedron->facets[fid_curr].get_next_facet(pos_curr, pos_next,
				fid_next, v_curr);
		if (pos_next == -1 || fid_next == -1) {
			printf("\nVertxInfo::preprocess : Error. Cannot find v%d in f%d\n",
					v_curr, fid_curr);
			return;
		}

	} while (fid_next != fid_first);

//    printf("\n\t\tTotal number is %d\n");
	indFacets = new int[3 * numFacets + 1];

//    printf("\t3. Building the VECTOR : \n");
	pos_next = parentPolyhedron->facets[fid_first].preprocess_search_vertex(id,
			v_curr);
	fid_next = fid_first;
	for (i = 0; i < numFacets; ++i) {
		pos_curr = pos_next;
		fid_curr = fid_next;

		parentPolyhedron->facets[fid_curr].get_next_facet(pos_curr, pos_next,
				fid_next, v_curr);
		indFacets[i] = fid_curr;
		indFacets[i + numFacets + 1] = v_curr;
		indFacets[i + 2 * numFacets + 1] = pos_curr;
//        printf("\t\t%d %d %d\n", fid_curr, v_curr, pos_curr);
	}
	indFacets[numFacets] = indFacets[0];
//    printf("\t4. End!\n");
}

void VertexInfo::find_and_replace_facet(
		int from,
		int to) {
	for (int i = 0; i < numFacets + 1; ++i)
		if (indFacets[i] == from) {
			indFacets[i] = to;
		}
}

void VertexInfo::find_and_replace_vertex(
		int from,
		int to) {
	for (int i = numFacets + 1; i < 2 * numFacets + 1; ++i)
		if (indFacets[i] == from) {
			indFacets[i] = to;
		}
}

