#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"

void Facet::preprocess_free() {
    if (nv <= 0) {
        printf("\tError. nv = %d\n", nv);
        return;
    }
    if (index == NULL) {
        printf("\tError. index == NULL\n");
        return;
    }
    for (int i = nv + 1; i < 3 * nv + 1; ++i)
        index[i] = -1;
    index[nv] = index[0];
    rgb[0] = 255;
    rgb[1] = 255;
    rgb[2] = 255;
}

void Facet::preprocess() {
    int i, v0, v1;

    //	this->my_print_all();
    for (i = 0; i < nv; ++i) {
        v0 = index[i];
        v1 = index[i + 1];
        preprocess_edge(v0, v1, i);
    }
}

void Facet::preprocess_edge(int v0, int v1, int v0_id) {
    int i, pos;
    
    pos = -1;
    for (i = 0; i < poly->numf; ++i) {
        if (i == id)
            continue;
        pos = poly->facet[i].preprocess_search_edge(v1, v0);
        if (pos != -1) {
            index[v0_id + nv + 1] = i;
            index[v0_id + 2 * nv + 1] = pos;
            break;
        }
    }
    if (pos == -1) {
        printf("preprocess_edge : Error. Edge (%d %d) cannot be found anywhere\n", v0, v1);
    }
}

int Facet::preprocess_search_edge(int v0, int v1) {
    int i;
    for (i = 0; i < nv; ++i)
        if (index[i] == v0 && index[i + 1] == v1)
            break;
    if (i == nv)
        return -1;
    else
        return i < nv - 1 ? i + 1 : 0;
}

int Facet::preprocess_search_vertex(int v, int& v_next) {
    for (int i = 0; i < nv; ++i)
        if (index[i] == v) {
            v_next = index[i + 1];
            return i;
        }
    v_next = -1;
    return -1;
}

