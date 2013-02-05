#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"
#include "list_squares_method.h"
#include "Vector3d.h"
#include "array_operations.h"
#include "Gauss_string.h"

#define EPS_PARALLEL 1e-15

int Polyhedron::test_consections() {
    int count;

    count = 0;
    count += test_inner_consections();
    count += test_outer_consections();

    printf("Total: %d consections found\n", count);
    return count;
}

int Polyhedron::test_inner_consections() {
    int i, count;

    double *A, *b;
    A = new double[4];
    b = new double[2];

    count = 0;
    for (i = 0; i < numf; ++i) {
        count += test_inner_consections_facet(i, A, b);
    }
    if (count > 0)
        printf("\tTotal: %d inner consections found\n", count);

    if (A != NULL)
        delete[] A;
    if (b != NULL)
        delete[] b;

    return count;
}

int Polyhedron::test_inner_consections_facet(int fid, double* A, double* b) {
    int i, j, nv, *index, count;

    nv = facet[fid].nv;
    index = facet[fid].index;

    count = 0;
    for (i = 0; i < nv; ++i) {
        for (j = i + 1; j < nv; ++j) {
            count += test_inner_consections_pair(
                    fid,
                    index[i % nv],
                    index[(i + 1) % nv],
                    index[j % nv],
                    index[(j + 1) % nv],
                    A,
                    b
                    );
        }
    }
    if (count > 0)
        printf("\t\tIn facet %d: %d inner consections found\n", fid, count);
    if (count > 0)
        facet[fid].my_fprint(stdout);
    return count;
}

int Polyhedron::test_inner_consections_pair(int fid, int id0, int id1, int id2, int id3,
        double* A, double* b) {

    // fid - identificator of the facet
    // (id0, id1) - first edge
    // (id2, id3) - second edge
    double nx, ny, nz;
    double x0, x1, x2, x3, y0, y1, y2, y3, z0, z1, z2, z3;

    if (id0 < 0 || id1 < 0 || id2 < 0 || id2 < 0 ||
            id0 >= numv || id1 >= numv || id2 >= numv || id3 >= numv) {
        printf("\t\t\ttest_inner_consections_pair: Error. incorrect input\n");
        return 0;
    }

    if ((id0 == id3 && id1 == id2) || (id0 == id2 && id1 == id3)) {
        printf("\t\t\t(%d, %d) and (%d, %d) are equal edges\n", id0, id1, id2, id3);
        return 2;
    }

    if (id1 == id2) {
        if (qmod((vertex[id1] - vertex[id0]) % (vertex[id3] - vertex[id2])) < EPS_PARALLEL &&
                (vertex[id1] - vertex[id0]) * (vertex[id3] - vertex[id2]) < 0) {
            printf("\t\t\t(%d, %d) and (%d, %d) consect untrivially\n",
                    id0, id1, id2, id3);
            return 2;
        } else {
            return 0;
        }
    }

    if (id0 == id3) {
        if (qmod((vertex[id1] - vertex[id0]) % (vertex[id3] - vertex[id2])) < EPS_PARALLEL &&
                (vertex[id1] - vertex[id0]) * (vertex[id3] - vertex[id2]) > 0) {
            printf("\t\t\t(%d, %d) and (%d, %d) consect untrivially\n",
                    id0, id1, id2, id3);
            return 2;
        } else {
            return 0;
        }
    }

    if (qmod((vertex[id1] - vertex[id0]) % (vertex[id3] - vertex[id2])) < EPS_PARALLEL) {
        return 0;
    }

    nx = facet[fid].plane.norm.x;
    ny = facet[fid].plane.norm.y;
    nz = facet[fid].plane.norm.z;
    nx = fabs(nx);
    ny = fabs(ny);
    nz = fabs(nz);

    x0 = vertex[id0].x;
    y0 = vertex[id0].y;
    z0 = vertex[id0].z;

    x1 = vertex[id1].x;
    y1 = vertex[id1].y;
    z1 = vertex[id1].z;

    x2 = vertex[id2].x;
    y2 = vertex[id2].y;
    z2 = vertex[id2].z;

    x3 = vertex[id3].x;
    y3 = vertex[id3].y;
    z3 = vertex[id3].z;
    
    if (nz >= nx && nz >= ny) {
        A[0] = x1 - x0;
        A[1] = x2 - x3;
        A[2] = y1 - y0;
        A[3] = y2 - y3;
        b[0] = x2 - x0;
        b[1] = y2 - y0;
    }

    if (ny >= nx && ny >= nz) {
        A[0] = x1 - x0;
        A[1] = x2 - x3;
        A[2] = z1 - z0;
        A[3] = z2 - z3;
        b[0] = x2 - x0;
        b[1] = z2 - z0;
    }

    if (nx >= ny && nx >= nz) {
        A[0] = y1 - y0;
        A[1] = y2 - y3;
        A[2] = z1 - z0;
        A[3] = z2 - z3;
        b[0] = y2 - y0;
        b[1] = z2 - z0;
    }

    if (Gauss_string(2, A, b) == 0) {
        return 0;
    }    


    if (b[0] > 0. && b[0] < 1. && b[1] > 0. && b[1] < 1.) {
        printf("\t\t\ttest_inner_consections_pair: edges (%d, %d) and (%d, %d) consect\n", id0, id2, id1, id3);
        return 1;
    } else
        return 0;
}

int Polyhedron::test_outer_consections() {
    int i, count;

    count = 0;
    for (i = 0; i < numf; ++i) {
        count += test_outer_consections_facet(i);
    }
    if (count > 0)
        printf("\tTotal: %d outer consections found\n", count);
    return count;
}

int Polyhedron::test_outer_consections_facet(int fid) {
    int i, j, nv, *index, count;

    nv = facet[fid].nv;
    index = facet[fid].index;

    count = 0;
    for (i = 0; i < nv; ++i) {
        count += test_outer_consections_edge(index[i % nv], index[(i + 1) % nv]);
    }
    if (count > 0)
        printf("\t\tIn facet %d: %d outer consections found\n", fid, count);
    return count;
}

int Polyhedron::test_outer_consections_edge(int id0, int id1) {

    int j, count;

    count = 0;
    for (j = 0; j < numf; ++j) {
        count += test_outer_consections_pair(id0, id1, j);
    }
    if (count > 0)
        printf("\t\t\tFor edge (%d, %d): %d outer consections found\n", id0, id1, count);
    return count;
}

int Polyhedron::test_outer_consections_pair(int id0, int id1, int fid) {

    int i, nv, *index;
    double AA, b, u, delta, alpha, sum;
    Vector3d A, A0, A1, normal;

    if (facet[fid].find_vertex(id0) >= 0 || facet[fid].find_vertex(id1) >= 0) {
        return 0;
    }

    AA = facet[fid].plane.norm * (vertex[id0] - vertex[id1]);

    if (fabs(AA) < EPS_PARALLEL) {
        return 0;
    }

    b = -facet[fid].plane.norm * vertex[id1];
    u = b / AA;

    if (u > 1. || u < 0.) {
        return 0;
    }

    A = u * vertex[id0] + (1. - u) * vertex[id1];

    nv = facet[fid].nv;
    index = facet[fid].index;
    normal = facet[fid].plane.norm;
    normal.norm(1.);

    sum = 0.;
    for (i = 0; i < nv; ++i) {
        A0 = vertex[index[i % nv]] - A;
        A1 = vertex[index[(i + 1) % nv]] - A;
        delta = (A0 % A1) * normal;
        delta /= sqrt(qmod(A0) * qmod(A1));
        alpha = asin(delta);
        sum += alpha;
    }

    if (fabs(sum) < 2 * M_PI) {
        return 0;
    } else {
        return 1;
    }
}
