#include "PolyhedraCorrectionLibrary.h"

int Polyhedron::simplify_vertex(double eps) {
    
    int i, j, *group, ndel;
    
    group = new int[numv];
    
    for (i = 0; i < numv; ++i) {
        group[i] = i;
    }
    
    ndel = 0;
    for (i = 0; i < numv; ++i) {
        for (j = 0; j < i; ++j) {
            if (dist_vertex(i, j) < eps) {
                group[i] = group[j];
                ++ndel;
                break;
            }
        }
    }
    
    if (group != NULL)
        delete[] group;
    return ndel;
}

double Polyhedron::dist_vertex(int i, int j) {
    return sqrt(qmod(vertex[i] - vertex[j]));
}
