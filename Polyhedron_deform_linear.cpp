#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"
#include "list_squares_method.h"
#include "Vector3d.h"
#include "array_operations.h"
#include "Gauss_string.h"

#define EPSILON 1e-3

void Polyhedron::deform_linear(int id, Vector3d delta) {
    
    int step;
    double err, err_eps;
    double K;
    
    int i, nf, *index;
    double dist;
    
    vertexinfo[id].fprint_my_format(stdout);
    
    nf = vertexinfo[id].nf;
    index = vertexinfo[id].index;
    for (i = 0; i < nf; ++i){
        dist = qmod(vertex[id] - vertex[index[nf + 1 + i]]);
        dist = sqrt(dist);
        printf("dist(%d, %d) = %lf\n", id, index[nf + 1 + i], dist);
    }
    scanf("%lf", &dist);
    
    delta += vertex[id]; //Сохраняем положение деформированной точки
    
    step = 0;
    K = 1e-3;
    err_eps = EPSILON;
    for (i = 0; i < 10; ++i) {
        do {
        vertex[id] = delta;
        deform_linear_facets();
        deform_linear_vertices(K);
        err = deform_linear_calculate_error();
        printf("%d\t%d\terr = %le\terr_eps = %le\n", i, step++, err, err_eps);
        } while (err > err_eps);

        K *= 2.;
        err_eps *= 0.5;
    }

    
    double xmin, xmax, ymin, ymax, zmin, zmax;
    this->get_boundary(xmin, xmax, ymin, ymax, zmin, zmax);
    fprintf(stdout, "boundary : \n");
    fprintf(stdout, "x [%lf,  %lf]\n", xmin, xmax);
    fprintf(stdout, "y [%lf,  %lf]\n", ymin, ymax);
    fprintf(stdout, "z [%lf,  %lf]\n", zmin, zmax);

    
}

void Polyhedron::deform_linear_facets() {
    int i, j, nv, *index;
    double *x, *y, *z;
    double a0, b0, c0, d0;
    double a, b, c, d;
    double lambda;
    
    x = new double[numv];
    y = new double[numv];
    z = new double[numv];
    
    for (j = 0; j < numf; ++j) {
        nv = facet[j].nv;
        index = facet[j].index;
        for (i = 0; i < nv; ++i) {
            x[i] = vertex[index[i]].x;
            y[i] = vertex[index[i]].y;
            z[i] = vertex[index[i]].z;
        }
        aprox(nv, x, y, z, a, b, c, d);
        a0 = facet[j].plane.norm.x;
        b0 = facet[j].plane.norm.y;
        c0 = facet[j].plane.norm.z;
        d0 = facet[j].plane.dist;
        lambda = a * a0 + b * b0 + c * c0 + d * d0; // Чтобы минимизировать разницу норм плоскостей
        lambda /= a * a + b * b + c * c + d * d;
        a *= lambda;
        b *= lambda;
        c *= lambda;
        d *= lambda;
        facet[j].plane.norm.x = a;
        facet[j].plane.norm.y = b;
        facet[j].plane.norm.z = c;
        facet[j].plane.dist = d;
    }
    
    if (x != NULL) delete[] x;
    if (y != NULL) delete[] y;
    if (z != NULL) delete[] z;
}

double Polyhedron::deform_linear_calculate_error() {
    int i, j, nv, *index;
    double err, a, b, c, d, x, y, z;
    err = 0.;
    
    for (j = 0; j < numf; ++j) {
        nv = facet[j].nv;
        index = facet[j].index;
        a = facet[j].plane.norm.x;
        b = facet[j].plane.norm.y;
        c = facet[j].plane.norm.z;
        d = facet[j].plane.dist;
        for (i = 0; i < nv; ++i) {
            x = vertex[index[i]].x;
            y = vertex[index[i]].y;
            z = vertex[index[i]].z;
            err += fabs(a * x + b * y + c * z + d);
        }
    }
    
    return err;
}

void Polyhedron::deform_linear_vertices(double K) {
    int i, j, nf, *index;
    double a, b, c, d;
    double Maa, Mab, Mac, Mad, Mbb, Mbc, Mbd, Mcc, Mcd;
    double *A, *B;
    double norm;
    
    Plane pl;
    A = new double[9];
    B = new double[3];
    
    norm = 0.;
    for (i = 0; i < numv; ++i) {
        Maa = 0.;
        Mab = 0.;
        Mac = 0.;
        Mad = 0.;
        Mbb = 0.;
        Mbc = 0.;
        Mbd = 0.;
        Mcc = 0.;
        Mcd = 0.;
        nf = vertexinfo[i].nf;
        index = vertexinfo[i].index;
        for (j = 0; j < nf; ++j) {
            pl = facet[index[j]].plane;
            a = pl.norm.x;
            b = pl.norm.y;
            c = pl.norm.z;
            d = pl.dist;
            Maa += a * a;
            Mab += a * b;
            Mac += a * c;
            Mad += a * d;
            Mbb += b * b;
            Mbc += b * c;
            Mbd += b * d;
            Mcc += c * c;
            Mcd += c * d;
        }
        A[0] = 2. + 2. * K * Maa;
        A[1] = 2. * K * Mab;
        A[2] = 2. * K * Mac;
        A[3] = A[1];
        A[4] = 2. + 2. * K * Mbb;
        A[5] = 2. * K * Mbc;
        A[6] = A[2];
        A[7] = A[5];
        A[8] = 2. + 2. * K * Mcc;
        B[0] = -2. * K * Mad;
        B[1] = -2. * K * Mbd;
        B[2] = -2. * K * Mcd;
        Gauss_string(3, A, B);
        norm += (B[0] - vertex[i].x) * (B[0] - vertex[i].x);
        norm += (B[1] - vertex[i].y) * (B[1] - vertex[i].y);
        norm += (B[2] - vertex[i].z) * (B[2] - vertex[i].z);
        vertex[i].x = B[0];
        vertex[i].y = B[1];
        vertex[i].z = B[2];
        
    }
    
    printf("\t\t\tnorm = %lf\n", norm);
    if (A != NULL) delete[] A;
    if (B != NULL) delete[] B;
}
