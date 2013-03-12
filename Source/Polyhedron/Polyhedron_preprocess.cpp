#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "PolyhedraCorrectionLibrary.h"

void Polyhedron::preprocess_polyhedron() {
//    printf("\tНачинаем предобработку ...\n");
    int i;
//    printf("\t\tЧистка граней: ");
    for (i = 0; i < numf; ++i) {
        if (facet[i].nv < 1)
            continue;
//        printf("%d, ", i);
//        printf("Preprocess_free facet %d\n", i);
        facet[i].preprocess_free();
    }
//    printf("\n\t\tПредобработка граней: ");
    for (i = 0; i < numf; ++i) {
        if (facet[i].nv < 1)
            continue;
//        printf("%d ", i);
//        printf("Preprocess facet %d\n", i);
        facet[i].preprocess();
//        if (facet[i].find_total(-1) != -1)
//            facet[i].my_fprint_all(stdout);
    }
    //	if(vertexinfo != NULL)
    //		delete[] vertexinfo;
//    printf("\n\t\tПредобработка вершин: ");
    vertexinfo = new VertexInfo[numv];
    for (i = 0; i < numv; ++i) {
//        printf("%d ", i);
//        printf("Preprocess vertexinfo %d\n", i);
        vertexinfo[i] = VertexInfo(i, vertex[i], &(*this));
//        printf("\tallocated\n");
        vertexinfo[i].preprocess();
    }
//    printf("\n");
}
