#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"
#include "list_squares_method.h"
#include "Vector3d.h"
#include "array_operations.h"
#include "Gauss_string.h"

#define eps 1.
#define EPSILON 1e-11


#define a(i) X[4 * (i)]
#define b(i) X[4 * (i) + 1]
#define c(i) X[4 * (i) + 2]
#define d(i) X[4 * (i) + 3]

#define x(i) X[4 * numf + 3 * (i) - ((i) > id)]
#define y(i) X[4 * numf + 3 * (i) + 1 - ((i) > id)]
#define z(i) X[4 * numf + 3 * (i) + 2 - ((i) > id)]

#define l(i) X[4 * numf + 3  * (numv - 1) + (i)]

#define na(i) (4 * (i))
#define nb(i) (4 * (i) + 1)
#define nc(i) (4 * (i) + 2)
#define nd(i) (4 * (i) + 3)

#define nx(i) (4 * numf + 3 * (i) - ((i) > id))
#define ny(i) (4 * numf + 3 * (i) + 1 - ((i) > id))
#define nz(i) (4 * numf + 3 * (i) + 2 - ((i) > id))

#define nl(i) (4 * numf + 3  * (numv - 1) + (i))


double norm_vector(int n, double* a) {
    int i;
    double max, tmp;
    max = 0.;
    for (i = 0; i < n; ++i) {
        tmp = fabs(a[i]);
        if (tmp > max)
            max = tmp;
    }
    return max;
//    int i;
//    double sum = 0.;
//    for (i = 0; i < n; ++i)
//        sum += a[i] * a[i];
//    return sqrt(sum);
}

void print_matrix(FILE* file, int n, double* A) {
    int i, j;
    
    fprintf(file, "\n");
    for (i = 0; i < n; ++i)
        fprintf(file, "--");
    fprintf(file, "\n");
    
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            if (fabs(A[i * n + j]) >= 1e-16)
                fprintf(file, "# ");
            else
                fprintf(file, "  ");
        }
        fprintf(file, "|\n");
    }
    for (i = 0; i < n; ++i)
        fprintf(file, "--");
    fprintf(file, "\n");
}

void print_vector(int n, double* v) {
    int i;
    for (i = 0; i < n; ++i)
        printf("\t%lf\n", v[i]);
}

void Polyhedron::deform(int id, Vector3d delta) {
    int i, step;
    double *X, *fx, *A, err;
    bool success;
    
    int n; // Число переменных = число условий = размерность вектора
    int *sum;
    
//    double xx, yy, zz;
//    double a, b, c, d;
//    int nv, *index;
//    for (int j = 0; j < numf; ++j) {
//        a = facet[j].plane.norm.x;
//        b = facet[j].plane.norm.y;
//        c = facet[j].plane.norm.z;
//        d = facet[j].plane.dist;
//        index = facet[j].index;
//        nv = facet[j].nv;
//        for (i = 0; i < nv; ++i) {
//            xx = vertex[index[i]].x;
//            yy = vertex[index[i]].y;
//            zz = vertex[index[i]].z;
//            printf("v%d -> f%d : %lf\n", index[i], j, a * xx + b * yy + c * zz + d);
//        }
//    }

    
    
//    vertex[id] = vertex[id] + delta;
    
//    n = 0;
//    for (i = 0; i < numf; ++i) {
//        n += facet[i].nv;
//    }
//    n += 4 * numf + 3 * (numv - 1);
  
    n = 3;
    
    sum = new int[numf];
    sum[0] = 0;
//    for (i = 0; i < numf - 1; ++i) {
//        sum[i + 1] = sum[i] + facet[i].nv;
//    }
//    
//    for (i = 0; i < numf; ++i) {
//        printf("sum[%d] = %d\n", i, sum[i]);
//    }
    
    
    X = new double[n];
    fx = new double[n];
    A  = new double[n * n];
    
//    for (i = 0; i < n; ++i) {
//        X[i] = 0.;
//    }
    X[0] = 1.01;
    X[1] = 0.01;
    X[2] = 0.01;
    f(n, X, fx, id, sum);
    err = norm_vector(n, fx);
    
    step = 0;
    while (err > EPSILON) {
        derf(n, X, A, id, sum);
        f(n, X, fx, id, sum);
        err = norm_vector(n, fx);

        printf("step %d\terr = %le\n", step++, err);
        print_vector(n, fx);
        if (err > 100) {
            printf("Ошибка. Метод Ньютона разошелся\n");
            break;
        }
        print_matrix(stdout, n, A);
        
        success = Gauss_string(n, A, fx);
        if (!success) {
            printf("Stoping deform...\n");
            return;
        }
        for (i = 0; i < n; ++i)
            X[i] -= fx[i];
    }
    
//    for (i = 0; i < numv; ++i) {
//        if (i == id)
//            continue;
//        vertex[i].x += x(i);
//        vertex[i].y += y(i);
//        vertex[i].z += z(i);
//    }
//    for (i = 0; i < numf; ++i) {
//        facet[i].plane.norm.x += a(i);
//        facet[i].plane.norm.y += b(i);
//        facet[i].plane.norm.z += c(i);
//        facet[i].plane.dist += d(i);
//    }
//    
//    for (int j = 0; j < numf; ++j) {
//        a = facet[j].plane.norm.x;
//        b = facet[j].plane.norm.y;
//        c = facet[j].plane.norm.z;
//        d = facet[j].plane.dist;
//        index = facet[j].index;
//        nv = facet[j].nv;
//        for (i = 0; i < nv; ++i) {
//            xx = vertex[index[i]].x;
//            yy = vertex[index[i]].y;
//            zz = vertex[index[i]].z;
//            printf("v%d -> f%d : %lf\n", index[i], j, a * xx + b * yy + c * zz + d);
//        }
//    }
    
    if (X != NULL) delete[] X;
    if (fx != NULL) delete[] fx;
    if (A != NULL) delete[] A;

}

//void Polyhedron::f(int n, double* X, double* fx, int id, int* sum) {
//    int i, j, k, p, m;
//    int *index, nv, nf;
//    Plane plane;
//    
//    k = 0;
//    n = n;
//    
//    for (j = 0; j < numf; ++j) {
//        nv = facet[j].nv;
//        index = facet[j].index;
//        plane = facet[j].plane;
//        for (i = 0; i < nv; ++i) {
//            if (index[i] == id) {
//                fx[k]  = (vertex[index[i]].x) * (plane.norm.x + a(j));                
//                fx[k] += (vertex[index[i]].y) * (plane.norm.y + b(j));
//                fx[k] += (vertex[index[i]].z) * (plane.norm.z + c(j));
//                fx[k] += plane.dist + d(j);
//            } else {
////                printf("(%.2lf + %.2lf) * (%.2lf + %.2lf) + \n", 
////                        vertex[index[i]].x, x(index[i]), plane.norm.x, a(j));
////                printf("(%.2lf + %.2lf) * (%.2lf + %.2lf) + \n",
////                        vertex[index[i]].y, y(index[i]), plane.norm.y, b(j));
////                printf("(%.2lf + %.2lf) * (%.2lf + %.2lf) + %.2lf + %.2lf = 0  ???\n\n",
////                        vertex[index[i]].z, z(index[i]), plane.norm.z, c(j),
////                        plane.dist, d(j));
//                fx[k]  = (vertex[index[i]].x + x(index[i])) * (plane.norm.x + a(j));                
//                fx[k] += (vertex[index[i]].y + y(index[i])) * (plane.norm.y + b(j));
//                fx[k] += (vertex[index[i]].z + z(index[i])) * (plane.norm.z + c(j));
//                fx[k] += plane.dist + d(j);
//            }
//            ++k;
//        }
//    }
//    
//    for (i = 0; i < numv; ++i) {
//        if (i == id) {
//            continue;
//        }
//        fx[k]     = 2 * eps * x(i);
//        fx[k + 1] = 2 * eps * y(i);
//        fx[k + 2] = 2 * eps * z(i);
//        
//        nf = vertexinfo[i].nf;;
//        index = vertexinfo[i].index;
//        for (j = 0; j < nf; ++j) {
//            p = index[j];
//            m = index[2 * j + 1] + sum[p];
//            fx[k]     -= l(m) * (facet[p].plane.norm.x + a(p));
//            fx[k + 1] -= l(m) * (facet[p].plane.norm.y + b(p));
//            fx[k + 2] -= l(m) * (facet[p].plane.norm.z + c(p));
//        }
//        k += 3;
//    }
//    
//    for (j = 0; j < numf; ++j) {
//        fx[k]     = 2 * a(j);
//        fx[k + 1] = 2 * b(j);
//        fx[k + 2] = 2 * c(j);
//        fx[k + 3] = 2 * d(j);
//        
//        nv = facet[j].nv;
//        index = facet[j].index;
//        plane = facet[j].plane;
//        
//        for (i = 0; i < nv; ++i) {
//            if (index[i] == id) {
//                fx[k    ] -= l(sum[j] + i) * vertex[index[i]].x;
//                fx[k + 1] -= l(sum[j] + i) * vertex[index[i]].y;
//                fx[k + 2] -= l(sum[j] + i) * vertex[index[i]].z;
//                fx[k + 3] -= l(sum[j] + i);
//                
//            } else {
//                fx[k    ] -= l(sum[j] + i) * (vertex[index[i]].x + x(index[i]));
//                fx[k + 1] -= l(sum[j] + i) * (vertex[index[i]].y + x(index[i]));
//                fx[k + 2] -= l(sum[j] + i) * (vertex[index[i]].z + x(index[i]));
//                fx[k + 3] -= l(sum[j] + i);
//            }
//        }
//        k += 4;
//    }
//}
//
//void Polyhedron::derf(int n, double* X, double* A, int id, int* sum) {
//    int i, j, k, p, m;
//    int *index, nv, nf;
//    Plane plane;
//    
//    k = 0;
//    
//    for (i = 0; i < n * n; ++i) {
//        A[i] = 0.;
//    }
//    
//    for (j = 0; j < numf; ++j) {
//        nv = facet[j].nv;
//        index = facet[j].index;
//        plane = facet[j].plane;
//        for (i = 0; i < nv; ++i) {
//            if (index[i] == id) {
//                A[k * n + na(j)] = vertex[index[i]].x;
//                A[k * n + nb(j)] = vertex[index[i]].y;
//                A[k * n + nc(j)] = vertex[index[i]].z;
//                A[k * n + nd(j)] = 1.;
//                A[k * n + nx(index[i])] = plane.norm.x + a(j);
//                A[k * n + ny(index[i])] = plane.norm.y + b(j);
//                A[k * n + nz(index[i])] = plane.norm.z + c(j);
//            } else {
//                A[k * n + na(j)] = vertex[index[i]].x + x(index[i]);
//                A[k * n + nb(j)] = vertex[index[i]].y + y(index[i]);
//                A[k * n + nc(j)] = vertex[index[i]].z + z(index[i]);
//                A[k * n + nd(j)] = 1.;
//                A[k * n + nx(index[i])] = plane.norm.x + a(j);
//                A[k * n + ny(index[i])] = plane.norm.y + b(j);
//                A[k * n + nz(index[i])] = plane.norm.z + c(j);
//            }
//            ++k;
//        }
//    }
//    
//    for (i = 0; i < numv; ++i) {
//        if (i == id) {
//            continue;
//        }
//        A[k * n + nx(i)]       = 2 * eps;
//        A[(k + 1) * n + ny(i)] = 2 * eps;
//        A[(k + 2) * n + nz(i)] = 2 * eps;
//        
//        nf = vertexinfo[i].nf;;
//        index = vertexinfo[i].index;
//        for (j = 0; j < nf; ++j) {
//            p = index[j];
//            m = index[2 * j + 1] + sum[p];
//            A[k * n + na(p)]       = - l(m);
//            A[k * n + nl(m)]       = - facet[p].plane.norm.x - a(p);
//            A[(k + 1) * n + nb(p)] = - l(m);
//            A[(k + 1) * n + nl(m)] = - facet[p].plane.norm.y - b(p);
//            A[(k + 2) * n + nc(p)] = - l(m);
//            A[(k + 2) * n + nl(m)] = - facet[p].plane.norm.z - c(p);
//        }
//        k += 3;
//    }
//    
//    for (j = 0; j < numf; ++j) {
//        A[k       * n + na(j)] = 2.;
//        A[(k + 1) * n + nb(j)] = 2.;
//        A[(k + 2) * n + nc(j)] = 2.;
//        A[(k + 3) * n + nd(j)] = 2.;
//        
//        nv = facet[j].nv;
//        index = facet[j].index;
//        plane = facet[j].plane;
//        
//        for (i = 0; i < nv; ++i) {
//            if (index[i] == id) {
//                A[k       * n + nl(sum[j] + i)] = - vertex[index[i]].x;
//                A[(k + 1) * n + nl(sum[j] + i)] = - vertex[index[i]].y;
//                A[(k + 2) * n + nl(sum[j] + i)] = - vertex[index[i]].z;
//                A[(k + 3) * n + nl(sum[j] + i)] = -1.;
//            } else {
//                A[k       * n + nx(index[i])] = - l(sum[j] + i);
//                A[k       * n + nl(sum[j] + i)] = - (vertex[index[i]].x + x(index[i]));
//                A[(k + 1) * n + ny(index[i])] = - l(sum[j] + i);
//                A[(k + 1) * n + nl(sum[j] + i)] = - (vertex[index[i]].y + y(index[i]));
//                A[(k + 2) * n + nz(index[i])] = - l(sum[j] + i);
//                A[(k + 2) * n + nl(sum[j] + i)] = - (vertex[index[i]].z + z(index[i]));
//                A[(k + 3) * n + nl(sum[j] + i)] = -1.;
//            }
//        }
//        k += 4;
//    }
//    
//}



void Polyhedron::f(int n, double* x, double* fx, int id, int* sum) {
//    fx[0] = x[0] * x[0] + x[1] * x[1] - 1.;
//    fx[1] = (x[0] - 2.) * (x[0] - 2.) + x[1] * x[1] - 1.;

//    fx[0] = x[3] - x[0] * x[0] - x[1] * x[1];
//    fx[1] = x[0] * x[0] + x[1] * x[1] + x[2] * x[2] - 1.;
//    fx[2] = (x[0] - 1.) * (x[0] - 1.) + (x[1] - 1.) * (x[1] - 1.) - 2.;
    
    fx[0] = (x[0] - 2.) * (x[0]- 2.) + x[1] * x[1] + x[2] * x[2] - 1.;
    fx[1] = (x[0] - 1.) * (x[0] - 1.) + (x[1] - 1.) * (x[1] - 1.) + x[2] * x[2] - 1.;
//    fx[2] = (x[0] - 3.) * (x[0]- 3.) + x[1] * x[1] + x[2] * x[2] - 1.;
    fx[2] = x[0] * x[0] + x[1] * x[1] + x[2] * x[2] - 1.;
    
    n = n;
    id = id;
    sum = sum;
}

void Polyhedron::derf(int n, double* x, double* A, int id, int* sum) {
//    A[0] = 2. * x[0];
//    A[1] = 2. * x[1];
//    A[2] = 2. * x[0] - 4.;
//    A[3] = 2. * x[1];
    
//    A[0] = 1.;
//    A[1] = -2. * x[0];
//    A[2] = 2. * x[1];
//    A[3] = 2. * x[0];
//    A[4] = 2. * x[1];
//    A[5] = 2. * x[2];
//    A[6] = 2. * x[0] - 2.;
//    A[7] = 2. * x[1] - 2.;
//    A[8] = 0.;
    
    A[0] = 2 * x[0] - 4;
    A[1] = 2 * x[1];
    A[2] = 2 * x[2];
    A[3] = 2 * x[0] - 2;
    A[4] = 2 * x[1] - 2;
    A[5] = 2 * x[2];
    A[6] = 2 * x[0];
    A[7] = 2 * x[1];
    A[8] = 2 * x[2];
    
    n = n;
    id = id;
    sum = sum;
    
}