#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"

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
    for (i = 0; i < poly->numf; ++i) {
//        printf("%d ", i);
        pos_next = poly->facet[i].preprocess_search_vertex(id, v_curr);
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
    nf = 0;
    do {
//        printf("%d ", fid_curr);
        ++nf;
        pos_curr = pos_next;
        fid_curr = fid_next;
        poly->facet[fid_curr].get_next_facet(
                pos_curr, pos_next, fid_next, v_curr);
    } while (fid_next != fid_first);

//    printf("\n\t\tTotal number is %d\n");
    index = new int[3 * nf + 1];

//    printf("\t3. Building the VECTOR : \n");
    pos_next = poly->facet[fid_first].preprocess_search_vertex(id, v_curr);
    fid_next = fid_first;
    for (i = 0; i < nf; ++i) {
        pos_curr = pos_next;
        fid_curr = fid_next;

        poly->facet[fid_curr].get_next_facet(
                pos_curr, pos_next, fid_next, v_curr);
        index[i] = fid_curr;
        index[i + nf + 1] = v_curr;
        index[i + 2 * nf + 1] = pos_curr;
//        printf("\t\t%d %d %d\n", fid_curr, v_curr, pos_curr);
    }
    index[nf] = index[0];
//    printf("\t4. End!\n");
}

void VertexInfo::find_and_replace_facet(int from, int to) {
    for (int i = 0; i < nf + 1; ++i)
        if (index[i] == from) {
            index[i] = to;
        }
}

void VertexInfo::find_and_replace_vertex(int from, int to) {
    for (int i = nf + 1; i < 2 * nf + 1; ++i)
        if (index[i] == from) {
            index[i] = to;
        }
}

