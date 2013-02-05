#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"
#include "list_squares_method.h"
#include "Vector3d.h"
#include "array_operations.h"
#include "Gauss_string.h"

#define SAFETY_COEFFICIENT 0.9
#define SAFETY_LIMIT       0.0
#define EPSILON 1e-7
//#define DEFORM_SCALE //Этот макрос определяет, каким методом производить деформацию:
// экспоненциальным ростом штрафа или масштабированием
// (если он определен - то масштабированием)


// This function determines if point lays in the polyhedra region 
// limited with the array of planes

bool ifPointInPolyhedraRegion(Vector3d v, int n, Plane* plane) {
    double val;
    for (int i = 0; i < n; ++i) {
        // a * x + b * y + c * z + d - we determine the signum of this value
        val = plane[i] % v;
        if (val <= 0) {
            return false;
        }
    }
    return true;
}

void Polyhedron::calculate_allowed_region(int id, int& num, Plane* plane) {

    int i, j;
    int nf;
    int fid;
    int nv, *index;
    int l, r, ll, rr;

    Plane pi, pi_ll, pi_rr, pi_lr, pi_il, pi_ri;
    Vector3d n, n_ll, n_rr, n_lr, n_il, n_ri;
    double d_ll, d_rr, d_lr, d_il, d_ri;

    // Number of facets in which our point lays :
    nf = vertexinfo[id].nf;

    // Counter of added planes :
    num = 0;

    // Cycle on all these facets:
    for (j = 0; j < nf; ++j) {

        // Current facet:
        fid = vertexinfo[id].index[j];

        // Number of vertexes which lay in current facet:
        nv = facet[fid].nv;

        // Indexes of these vertexes :
        index = facet[fid].index;

        // Plane of current facet :
        pi = facet[fid].plane;

        // Normal of this plane:
        n = pi.norm;

        // Find position of our vertex in current facet :
        i = vertexinfo[id].index[2 * nf + 1 + j];

        // Find its neighbours :
        ll = index[(nv + i - 2) % nv];
        l = index[(nv + i - 1) % nv];
        r = index[(nv + i + 1) % nv];
        rr = index[(nv + i + 2) % nv];

        // If the current facet is triangle situation is very simple :
        if (nv == 3) {
            // Calculate facet pi_lr :

            // Normal:
            n_lr = n % (vertex[r] - vertex[l]);

            // Free coefficient :
            d_lr = -n_lr * vertex[l];

            // Plane itself :
            pi_lr = Plane(n_lr, d_lr);

            // Add this plane to the array of planes:
            plane[num++] = pi_lr;

        } else if (facet[fid].convexness_point(i) != -1) {
            // In point vertex[id] in facet convexness is detected

            // Calculate facet pi_ll :

            // Normal:
            n_ll = n % (vertex[l] - vertex[ll]);

            // If there is non-convexness in point Al, change direction of
            // normal:
            if (facet[fid].convexness_point(l) == -1) {
                n_ll = -n_ll;
            }

            // Free coefficient :
            d_ll = -n_ll * vertex[l];

            // Plane itself :
            pi_ll = Plane(n_ll, d_ll);

            // Calculate facet pi_rr :

            // Normal:
            n_rr = n % (vertex[rr] - vertex[r]);

            // If there is non-convexness in point Ar, change direction of
            // normal:
            if (facet[fid].convexness_point(r) == -1) {
                n_rr = -n_rr;
            }

            // Free coefficient :
            d_rr = -n_rr * vertex[r];

            // Plane itself :
            pi_rr = Plane(n_rr, d_rr);

            // Calculate facet pi_lr :

            // Normal:
            n_lr = n % (vertex[l] - vertex[ll]);

            // Free coefficient :
            d_lr = -n_ll * vertex[l];

            // Plane itself :
            pi_lr = Plane(n_lr, d_lr);

            // Add these calculated planes to the array of planes:
            plane[num++] = pi_ll;
            plane[num++] = pi_rr;
            plane[num++] = pi_lr;

        } else {
            // In point vertex[id] in facet non-convexness is detected

            // Calculate facet pi_ll :

            // Normal:
            n_ll = n % (vertex[l] - vertex[ll]);

            // If there is non-convexness in point Al, change direction of
            // normal:
            if (facet[fid].convexness_point(l) == -1) {
                n_ll = -n_ll;
            }

            // Free coefficient :
            d_ll = -n_ll * vertex[l];

            // Plane itself :
            pi_ll = Plane(n_ll, d_ll);

            // Calculate facet pi_rr :

            // Normal:
            n_rr = n % (vertex[rr] - vertex[r]);

            // If there is non-convexness in point Ar, change direction of
            // normal:
            if (facet[fid].convexness_point(r) == -1) {
                n_rr = -n_rr;
            }

            // Free coefficient :
            d_rr = -n_rr * vertex[r];

            // Plane itself :
            pi_rr = Plane(n_rr, d_rr);

            // Calculate facet pi_li :

            // Normal:
            n_il = n % (vertex[l] - vertex[id]);

            // Free coefficient :
            d_il = -n_il * vertex[id];

            // Plane itself :
            pi_il = Plane(n_il, d_il);

            // Calculate facet pi_rr :

            // Normal:
            n_ri = n % (vertex[id] - vertex[r]);

            // Free coefficient :
            d_ri = -n_ri * vertex[id];

            // Plane itself :
            pi_ri = Plane(n_ri, d_ri);

            // Add these calculated planes to the array of planes:
            plane[num++] = pi_ll;
            plane[num++] = pi_rr;
            plane[num++] = pi_il;
            plane[num++] = pi_ri;
        }
    }
}

double Polyhedron::calculate_functional(int id, double K, Vector3d vnew) {
    int i, nf, *index;
    double value, result;
    
    nf = vertexinfo[id].nf;
    index = vertexinfo[id].index;
    
    result = 0.;
    for (i = 0; i < nf; ++i) {
        value = facet[index[i]].plane % vnew;
        result += value * value;
    }
    result *= K;
    result += qmod(vertex[id] - vnew);
    return result;
}

void Polyhedron::deform_linear_vertex_vertmin(int id, double K, double* A, double* B, Plane* plane) {

    int i, j, k;
    int nf, *index;
    double a, b, c, d;
    double Maa, Mab, Mac, Mad, Mbb, Mbc, Mbd, Mcc, Mcd;

    Plane pl;

    int num_planes;
    double functionalValue, functionalValueMin;
    bool ifValueFound = false;
    bool ifCalculated;
    Vector3d vnew, vnewMin;
    
    double lambda0, lambda1, lambda2;

    //Find polyehdra region of allowed deformations for the current point :
    calculate_allowed_region(id, num_planes, plane);

    // Calculate the coefficients for matrix we are going to solve :
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
    if (nf == 0) {
        printf("Vertex %d cannot be found in any facet...\n", i);
        return;
    }
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

    // Case 0: All Lagrange's coefficients are = 0
    // Calculating new point :
    ifCalculated = calculate_point_case0(id, K, A, B, vnew, 
            Maa, Mab, Mac, Mad, Mbb, Mbc, Mbd, Mcc, Mcd);
            
    if (ifCalculated) {
        ifValueFound = true;
        functionalValueMin = calculate_functional(id, K, vnew);
        vnewMin = vnew;
    }
    
    // Case 1: One and only one Lagrange's coefficients is not null
    // Calculating new point :
    for (i = 0; i < num_planes; ++i) {
        ifCalculated = calculate_point_case1(id, K, A, B, vnew, 
                plane[i], lambda0,
                Maa, Mab, Mac, Mad, Mbb, Mbc, Mbd, Mcc, Mcd);

        if (ifCalculated && lambda0 > 0.) {
            functionalValue = calculate_functional(id, K, vnew);
            if (!ifValueFound || functionalValue < functionalValueMin) {
                ifValueFound = true;
                functionalValueMin = functionalValue;
                vnewMin = vnew;
            }
        }
        
    }
    
    // Case 2: Two and only two Lagrange's coefficients are not null
    // Calculating new point :
    for (i = 0; i < num_planes; ++i) for (j = i + 1; j < num_planes; ++j) {
        
        ifCalculated = calculate_point_case2(id, K, A, B, vnew, 
                plane[i], plane[j], lambda0, lambda1,
                Maa, Mab, Mac, Mad, Mbb, Mbc, Mbd, Mcc, Mcd);

        if (ifCalculated && lambda0 > 0. && lambda1 > 0.) {
            functionalValue = calculate_functional(id, K, vnew);
            if (!ifValueFound || functionalValue < functionalValueMin) {
                ifValueFound = true;
                functionalValueMin = functionalValue;
                vnewMin = vnew;
            }
        }
        
    }
    
    // Case 3: Three and only three Lagrange's coefficients are not null
    // Calculating new point :
    for (i = 0; i < num_planes; ++i) for (j = i + 1; j < num_planes; ++j) for (k = j + 1; k < num_planes; ++k) {
        
        ifCalculated = calculate_point_case3(id, K, A, B, vnew, 
                plane[i], plane[j], plane[k], lambda0, lambda1, lambda2,
                Maa, Mab, Mac, Mad, Mbb, Mbc, Mbd, Mcc, Mcd);

        if (ifCalculated && lambda0 > 0. && lambda1 > 0. && lambda2 > 0.) {
            functionalValue = calculate_functional(id, K, vnew);
            if (!ifValueFound || functionalValue < functionalValueMin) {
                ifValueFound = true;
                functionalValueMin = functionalValue;
                vnewMin = vnew;
            }
        }
        
    }
    
    vertex[id] = vnewMin;
}

bool Polyhedron::calculate_point_case0(int id, 
        double K,
        double* matrix, 
        double* row, 
        Vector3d& vnew, 
        double Maa, double Mab, double Mac, double Mad, 
        double Mbb, double Mbc, double Mbd, 
        double Mcc, double Mcd) {
    
    bool result;
    
    matrix[0] = 1. + K * Maa;    matrix[1] = K * Mab;       matrix[2] = K * Mac;
    matrix[3] = matrix[1];       matrix[4] = 1. + K * Mbb;  matrix[5] = K * Mbc;
    matrix[6] = matrix[2];       matrix[7] = matrix[5];     matrix[8] = 1. + K * Mcc;
    
    
    row[0] = - K * Mad + vertex[id].x;
    row[1] = - K * Mbd + vertex[id].y;
    row[2] = - K * Mcd + vertex[id].z;
    
    result = Gauss_string(3, matrix, row);
    vnew = Vector3d(row[0], row[1], row[2]);
    
    return result;
}

bool Polyhedron::calculate_point_case1(int id, 
        double K, 
        double* matrix, 
        double* row, 
        Vector3d& vnew, 
        Plane plane, 
        double& lambda, 
        double Maa, double Mab, double Mac, double Mad, 
        double Mbb, double Mbc, double Mbd, 
        double Mcc, double Mcd) {

    bool result;
    double a, b, c, d;
    
    a = plane.norm.x;
    b = plane.norm.y;
    c = plane.norm.z;
    d = plane.dist;
    
    matrix[0] = 1. + K * Maa;    matrix[1] = K * Mab;       matrix[2] = K * Mac;        matrix[3]  = - a;
    matrix[4] = matrix[1];       matrix[5] = 1. + K * Mbb;  matrix[6] = K * Mbc;        matrix[7]  = - b;
    matrix[8] = matrix[2];       matrix[9] = matrix[6];     matrix[10] = 1. + K * Mcc;  matrix[11] = - c;
    matrix[12] = matrix[3];      matrix[13] = matrix[7];    matrix[14] = matrix[11];    matrix[15] = 0.;
    
    
    row[0] = - K * Mad + vertex[id].x;
    row[1] = - K * Mbd + vertex[id].y;
    row[2] = - K * Mcd + vertex[id].z;
    row[3] = d;
    
    result = Gauss_string(4, matrix, row);
    vnew = Vector3d(row[0], row[1], row[2]);
    lambda = row[3];
    
    return result;
}

bool Polyhedron::calculate_point_case2(int id, 
        double K, 
        double* matrix, 
        double* row, 
        Vector3d& vnew, 
        Plane plane0, Plane plane1, 
        double& lambda0, double& lambda1, 
        double Maa, double Mab, double Mac, double Mad, 
        double Mbb, double Mbc, double Mbd, 
        double Mcc, double Mcd) {

    bool result;
    double a0, b0, c0, d0;
    double a1, b1, c1, d1;
    
    a0 = plane0.norm.x;
    b0 = plane0.norm.y;
    c0 = plane0.norm.z;
    d0 = plane0.dist;

    a1 = plane1.norm.x;
    b1 = plane1.norm.y;
    c1 = plane1.norm.z;
    d1 = plane1.dist;
    
    matrix[0] = 1. + K * Maa;    matrix[1] = K * Mab;       matrix[2] = K * Mac;        matrix[3]  = - a0;  matrix[4] = - a1;
    matrix[5] = matrix[1];       matrix[6] = 1. + K * Mbb;  matrix[7] = K * Mbc;        matrix[8]  = - b0;  matrix[9] = - b1;
    matrix[10] = matrix[2];      matrix[11] = matrix[7];    matrix[12] = 1. + K * Mcc;  matrix[13] = - c0;  matrix[14] = - c1;
    matrix[15] = matrix[3];      matrix[16] = matrix[8];    matrix[17] = matrix[13];    matrix[18] = 0.;    matrix[19] = 0.;
    matrix[15] = matrix[4];      matrix[16] = matrix[9];    matrix[17] = matrix[14];    matrix[18] = 0.;    matrix[19] = 0.;
    
    row[0] = - K * Mad + vertex[id].x;
    row[1] = - K * Mbd + vertex[id].y;
    row[2] = - K * Mcd + vertex[id].z;
    row[3] = d0;
    row[4] = d1;
    
    result = Gauss_string(5, matrix, row);
    vnew = Vector3d(row[0], row[1], row[2]);
    lambda0 = row[3];
    lambda1 = row[4];
    
    return result;
    
}

bool Polyhedron::calculate_point_case3(int id, 
        double K, 
        double* matrix, 
        double* row, 
        Vector3d& vnew, 
        Plane plane0, Plane plane1, Plane plane2, 
        double& lambda0, double& lambda1, double& lambda2, 
        double Maa, double Mab, double Mac, double Mad, 
        double Mbb, double Mbc, double Mbd, 
        double Mcc, double Mcd) {

    bool result;
    double a0, b0, c0, d0;
    double a1, b1, c1, d1;
    double a2, b2, c2, d2;
    
    a0 = plane0.norm.x;
    b0 = plane0.norm.y;
    c0 = plane0.norm.z;
    d0 = plane0.dist;

    a1 = plane1.norm.x;
    b1 = plane1.norm.y;
    c1 = plane1.norm.z;
    d1 = plane1.dist;

    a2 = plane2.norm.x;
    b2 = plane2.norm.y;
    c2 = plane2.norm.z;
    d2 = plane2.dist;
    
    matrix[0] = 1. + K * Maa;    matrix[1] = K * Mab;       matrix[2] = K * Mac;        matrix[3]  = - a0;  matrix[4] = - a1;   matrix[5]  = - a2;
    matrix[6] = matrix[1];       matrix[7] = 1. + K * Mbb;  matrix[8] = K * Mbc;        matrix[9]  = - b0;  matrix[10] = - b1;  matrix[11] = - b2;
    matrix[12] = matrix[2];      matrix[13] = matrix[8];    matrix[14] = 1. + K * Mcc;  matrix[15] = - c0;  matrix[16] = - c1;  matrix[17] = - c2;
    matrix[18] = matrix[3];      matrix[19] = matrix[9];    matrix[20] = matrix[15];    matrix[21] = 0.;    matrix[22] = 0.;    matrix[23] = 0.;
    matrix[24] = matrix[4];      matrix[25] = matrix[10];   matrix[26] = matrix[16];    matrix[27] = 0.;    matrix[28] = 0.;    matrix[29] = 0.;
    matrix[30] = matrix[5];      matrix[31] = matrix[11];   matrix[32] = matrix[17];    matrix[33] = 0.;    matrix[34] = 0.;    matrix[35] = 0.;
    
    row[0] = - K * Mad + vertex[id].x;
    row[1] = - K * Mbd + vertex[id].y;
    row[2] = - K * Mcd + vertex[id].z;
    row[3] = d0;
    row[4] = d1;
    row[5] = d2;
    
    result = Gauss_string(5, matrix, row);
    vnew = Vector3d(row[0], row[1], row[2]);
    lambda0 = row[3];
    lambda1 = row[4];
    lambda2 = row[5];
    
    return result;

    
}


