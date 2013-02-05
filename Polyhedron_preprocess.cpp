#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"

void Polyhedron::preprocess_polyhedron() {
    printf("preprocess_polyhedron : begin ...\n");
    int i;
    for (i = 0; i < numf; ++i) {
        printf("Preprocess_free facet %d\n", i);
        facet[i].preprocess_free();
    }
    for (i = 0; i < numf; ++i) {
        printf("Preprocess facet %d\n", i);
        facet[i].preprocess();
        if (facet[i].find_total(-1) != -1)
            facet[i].my_fprint_all(stdout);
    }
    return;
    //	if(vertexinfo != NULL)
    //		delete[] vertexinfo;
    vertexinfo = new VertexInfo[numv];
    for (i = 0; i < numv; ++i) {
        printf("Preprocess vertexinfo %d\n", i);
        vertexinfo[i] = VertexInfo(i, vertex[i], &(*this));
        printf("\tallocated\n");
        vertexinfo[i].preprocess();
    }
}
