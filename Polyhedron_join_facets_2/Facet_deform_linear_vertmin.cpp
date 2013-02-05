#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"
#include "list_squares_method.h"
#include "Vector3d.h"
#include "array_operations.h"
#include "Gauss_string.h"

int Facet::convexness_point(int i) {
   
    int l, r;
    Vector3d Ai, Al, Ar, n;
    double value;
    
    // Normal of plane of facet:
    n = plane.norm;
    
    // Calculate indexes  of neighbours of point :
    l = (nv + i - 1) % nv;
    r = (nv + i + 1) % nv;
    
    // Points theirself :
    Ai  = poly->vertex[index[i]];
    Al  = poly->vertex[index[l]];
    Ar  = poly->vertex[index[r]];
    
    value = ((Al - Ai) % (Ar - Ai)) * n;
    
    return value > 0 ? 1 : (value < 0 ? -1 : 0);
    
}

