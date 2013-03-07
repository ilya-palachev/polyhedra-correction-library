#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"

#define EPSILON 1e-7
//#define DEFORM_SCALE //Этот макрос определяет, каким методом производить деформацию:
                        // экспоненциальным ростом штрафа или масштабированием
                        // (если он определен - то масштабированием)

double Polyhedron::min_dist(int id) {
    int i, nf, *index;
    double dist, min_dist;

    nf = vertexinfo[id].nf;
    index = vertexinfo[id].index;
    for (i = 0; i < nf; ++i) {
        dist = qmod(vertex[id] - vertex[index[nf + 1 + i]]);
        dist = sqrt(dist);
        if (i == 0 || dist < min_dist)
            min_dist = dist;
    }
    return min_dist;
}

void Polyhedron::deform_linear_partial(int id, Vector3d delta, int num) {
    int i;
    double coeff;
    Vector3d delta_step;

    if (num == 1) {
        deform_linear(id, delta);
    } else {
        coeff = 1. / num;
        delta_step = delta * coeff;
        for (i = 0; i < num; ++i) {
            deform_linear(id, delta_step);
        }
    }
}

#define MAX_STEPS 1e4

//Данная функция отвечает за глобальную минимизацию (т. е. минимизируется отклонение в целом)
void Polyhedron::deform_linear(int id, Vector3d delta) {
    //    printf(".");

    int step, i;
    double err, err_eps;
    double K;
    double *x, *y, *z, *A, *B;
    double *xold, *yold, *zold;
    double norm;

    //    int i, nf, *index;
    //    double dist;
    //
    ////    vertexinfo[id].fprint_my_format(stdout);
    //
    //    nf = vertexinfo[id].nf;
    //    index = vertexinfo[id].index;
    //    for (i = 0; i < nf; ++i) {
    //        dist = qmod(vertex[id] - vertex[index[nf + 1 + i]]);
    //        dist = sqrt(dist);
    ////        printf("dist(%d, %d) = %lf\n", id, index[nf + 1 + i], dist);
    //    }
    //    //    scanf("%lf", &dist);

    x = new double[numv]; //Вспомогательный массив для вычисления в deform_linear_facets
    y = new double[numv]; //Вспомогательный массив для вычисления в deform_linear_facets
    z = new double[numv]; //Вспомогательный массив для вычисления в deform_linear_facets
    xold = new double[numv]; //Чтобы хранить начальные значения
    yold = new double[numv]; //Чтобы хранить начальные значения
    zold = new double[numv]; //Чтобы хранить начальные значения
    A = new double[9]; //Вспомогательный массив для вычисления в deform_linear_vertices
    B = new double[3]; //Вспомогательный массив для вычисления в deform_linear_vertices

    //    double a, b, c, d;
    //    for (i = 0; i < numf; ++i) {
    //        a = facet[i].plane.norm.x;
    //        b = facet[i].plane.norm.y;
    //        c = facet[i].plane.norm.z;
    //        d = facet[i].plane.dist;
    //        printf("\t%d:\t%lf\t%lf\t%lf\n", 
    //                i, 
    //                a + b + c + d,
    //                fabs(a) + fabs(b) + fabs(c) + fabs(d),
    //                a * a + b * b + c * c + d * d);
    //    }

    //    delta += vertex[id]; //Сохраняем положение деформированной точки
    vertex[id] += delta;

    //    printf("vertex[%d] = (%lf, %lf, %lf)\n", 
    //        id, vertex[id].x, vertex[id].y, vertex[id].z);

    for (i = 0; i < numv; ++i) {
        xold[i] = vertex[i].x;
        yold[i] = vertex[i].y;
        zold[i] = vertex[i].z;
    }


#ifndef DEFORM_SCALE
    step = 0;
    K = 100;
    err = deform_linear_calculate_error();
    //    K = sqrt(qmod(delta)) / err;
    err_eps = EPSILON;
    for (i = 0; err > 1e-10; ++i) {
        do {

            printf("\\hline\n %d & %d & ", i, step);
            //                    vertex[id].x = xold[id];
            //                    vertex[id].y = yold[id];
            //                    vertex[id].z = zold[id];
            //            vertex[id] = delta;
            deform_linear_facets(x, y, z);
            test_consections(true);
//            join_points();
            deform_linear_vertices(id, K, A, B, xold, yold, zold);
            err = deform_linear_calculate_error();
            norm = deform_linear_calculate_deform(xold, yold, zold);
            printf("%le & %le & %lf\\\\\n", err, norm, K);
            //                    printf("%d\t%d\terr = %le\tnorm = %le\terr_eps = %le\tK = %lf\n", i, step++, err, norm, err_eps, K);
            ++step;
            if (step > MAX_STEPS) {
                printf("Too much steps...\n");
                return;
            }
            //            } while (err > err_eps);
        } while (0);

        //            K += K;
//        K *= 1.005;
        K *= 2;
        //            K = norm / err;
        //            if (K < 100.)
        //                K = 100;
        //            K += 1.;
        err_eps *= 0.5;
    }
#endif
#ifdef DEFORM_SCALE
        step = 0;
        err = deform_linear_calculate_error();
//        K = sqrt(qmod(delta)) / err;
            K = 1. / err;
        do {
    //        vertex[id].x = xold[id];
    //        vertex[id].y = yold[id];
    //        vertex[id].z = zold[id];
            deform_linear_facets(x, y, z);
            test_consections();
            deform_linear_vertices(id, K, A, B, xold, yold, zold);
            err = deform_linear_calculate_error();
            norm = deform_linear_calculate_deform(xold, yold, zold);
            if (step % 100 == 0 || step >= 2059) {
                printf("\\hline\n %d & %le & %le & %lf\\\\\n", step, err, norm, K);
    
                //                printf("%d\t%d\terr = %le\tnorm = %le\tK = %lf\n", i, step++, err, norm, K);
            }
            ++step;
            if (step > MAX_STEPS) {
                printf("Too much steps...\n");
                return;
            }
            K = norm / err;
        } while (err > 1e-10);
#endif

    //    double xmin, xmax, ymin, ymax, zmin, zmax;
    //    this->get_boundary(xmin, xmax, ymin, ymax, zmin, zmax);
    //    fprintf(stdout, "boundary : \n");
    //    fprintf(stdout, "x [%lf,  %lf]\n", xmin, xmax);
    //    fprintf(stdout, "y [%lf,  %lf]\n", ymin, ymax);
    //    fprintf(stdout, "z [%lf,  %lf]\n", zmin, zmax);

    //    printf("vertex[%d] = (%lf, %lf, %lf)\n", 
    //        id, vertex[id].x, vertex[id].y, vertex[id].z);


    if (x != NULL) delete[] x;
    if (y != NULL) delete[] y;
    if (z != NULL) delete[] z;
    if (x != NULL) delete[] xold;
    if (y != NULL) delete[] yold;
    if (z != NULL) delete[] zold;
    if (A != NULL) delete[] A;
    if (B != NULL) delete[] B;

}

void Polyhedron::deform_linear_test(int id, Vector3d delta, int mode, int& num_steps, double& norm_sum) {

    int step, i;
    double err, err_eps;
    double K;
    double *x, *y, *z, *A, *B;
    double *xold, *yold, *zold;
    double norm;


    x = new double[numv];
    y = new double[numv];
    z = new double[numv];
    xold = new double[numv];
    yold = new double[numv];
    zold = new double[numv];
    A = new double[9];
    B = new double[3];

    vertex[id] += delta;

    printf("vertex[%d] = (%lf, %lf, %lf)\n",
            id, vertex[id].x, vertex[id].y, vertex[id].z);

    for (i = 0; i < numv; ++i) {
        xold[i] = vertex[i].x;
        yold[i] = vertex[i].y;
        zold[i] = vertex[i].z;
    }

    switch (mode) {
        case 0:
            step = 0;
            K = 100;
            //    err = deform_linear_calculate_error();
            //    K = sqrt(qmod(delta)) / err;
            err_eps = EPSILON;
            for (i = 0; i < 10; ++i) {
                do {

                    //                    printf("\\hline\n %d & %d & ", i, step);
                    //                    vertex[id].x = xold[id];
                    //                    vertex[id].y = yold[id];
                    //                    vertex[id].z = zold[id];
                    //            vertex[id] = delta;
                    deform_linear_facets(x, y, z);
                    deform_linear_vertices(id, K, A, B, xold, yold, zold);
                    err = deform_linear_calculate_error();
                    norm = deform_linear_calculate_deform(xold, yold, zold);
                    //                    printf("%le & %le & %lf\\\\\n", err, norm, K);
                    //                    printf("%d\t%d\terr = %le\tnorm = %le\terr_eps = %le\tK = %lf\n", i, step++, err, norm, err_eps, K);
                    ++step;
                    if (step > MAX_STEPS) {
                        printf("Too much steps...\n");
                        return;
                    }
                } while (err > err_eps);

                K *= 2.;
                //        K = norm / err;
                //        K += 1.;
                err_eps *= 0.5;
            }
            num_steps = step;
            norm_sum = norm;
            break;
        case 1:
            step = 0;
            err = deform_linear_calculate_error();
            K = sqrt(qmod(delta)) / err;
            //    K = 1. / err;
            do {
                //                vertex[id].x = xold[id];
                //                vertex[id].y = yold[id];
                //                vertex[id].z = zold[id];
                deform_linear_facets(x, y, z);
                deform_linear_vertices(id, K, A, B, xold, yold, zold);
                err = deform_linear_calculate_error();
                norm = deform_linear_calculate_deform(xold, yold, zold);
                if (step % 100 == 0 || step >= 2059) {
                    //                    printf("\\hline\n %d & %le & %le & %lf\\\\\n", step, err, norm, K);

                    //                printf("%d\t%d\terr = %le\tnorm = %le\tK = %lf\n", i, step++, err, norm, K);
                }
                ++step;
                if (step > MAX_STEPS) {
                    printf("Too much steps...\n");
                    return;
                }
                K = norm / err;
            } while (err > 1e-10);
            num_steps = step;
            norm_sum = norm;
            break;
        default:
            break;
    }

    printf("vertex[%d] = (%lf, %lf, %lf)\n",
            id, vertex[id].x, vertex[id].y, vertex[id].z);

    if (x != NULL) delete[] x;
    if (y != NULL) delete[] y;
    if (z != NULL) delete[] z;
    if (x != NULL) delete[] xold;
    if (y != NULL) delete[] yold;
    if (z != NULL) delete[] zold;
    if (A != NULL) delete[] A;
    if (B != NULL) delete[] B;

}

void Polyhedron::deform_linear_facets(double* x, double* y, double* z) {
    int i, j, nv, *index;
    //    double *x, *y, *z;
    double a0, b0, c0, d0;
    double a, b, c, d;
    double lambda;

    //    x = new double[numv];
    //    y = new double[numv];
    //    z = new double[numv];

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

    //    if (x != NULL) delete[] x;
    //    if (y != NULL) delete[] y;
    //    if (z != NULL) delete[] z;
}

double Polyhedron::deform_linear_calculate_error() {
    int i, j, nv, *index;
    double err, a, b, c, d, x, y, z, locerr;
    err = 0.;
    

    for (j = 0; j < numf; ++j) {
        nv = facet[j].nv;
        index = facet[j].index;
        a = facet[j].plane.norm.x;
        b = facet[j].plane.norm.y;
        c = facet[j].plane.norm.z;
        d = facet[j].plane.dist;
        if (nv < 3)
            continue;
        locerr = 0.;
        for (i = 0; i < nv; ++i) {
            x = vertex[index[i]].x;
            y = vertex[index[i]].y;
            z = vertex[index[i]].z;
            locerr += fabs(a * x + b * y + c * z + d);
        }
        err += locerr;
        printf("error[%d] = %lf\n", j, locerr);
    }

    return err;
}

double Polyhedron::deform_linear_calculate_deform(double* x, double* y, double* z) {
    int i;
    double deform, xx, yy, zz;
    deform = 0.;
    for (i = 0; i < numv; ++i) {
        xx = vertex[i].x;
        yy = vertex[i].y;
        zz = vertex[i].z;
        deform += (xx - x[i]) * (xx - x[i]);
        deform += (yy - y[i]) * (yy - y[i]);
        deform += (zz - z[i]) * (zz - z[i]);
    }
    return deform;
}

void Polyhedron::deform_linear_vertices(
        int id, double K, double* A, double* B,
        double* xold, double* yold, double* zold) {
    int i, j, nf, *index;
    double a, b, c, d;
    double Maa, Mab, Mac, Mad, Mbb, Mbc, Mbd, Mcc, Mcd;
    //    double *A, *B;
    //    double norm;

    Plane pl;
    //    A = new double[9];
    //    B = new double[3];

    //    norm = 0.;
    for (i = 0; i < numv; ++i) {
        if (i == id)
            continue;
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
        A[0] = 1. + K * Maa;
        A[1] = K * Mab;
        A[2] = K * Mac;
        A[3] = A[1];
        A[4] = 1. + K * Mbb;
        A[5] = K * Mbc;
        A[6] = A[2];
        A[7] = A[5];
        A[8] = 1. + K * Mcc;
        B[0] = -K * Mad + xold[i];
        B[1] = -K * Mbd + yold[i];
        B[2] = -K * Mcd + zold[i];
        Gauss_string(3, A, B);
        //        norm += (B[0] - vertex[i].x) * (B[0] - vertex[i].x);
        //        norm += (B[1] - vertex[i].y) * (B[1] - vertex[i].y);
        //        norm += (B[2] - vertex[i].z) * (B[2] - vertex[i].z);
        vertex[i].x = B[0];
        vertex[i].y = B[1];
        vertex[i].z = B[2];

    }

    //        printf("\t\t\tnorm = %lf\n", norm);
    //    printf("%lf & ", norm);
    //    if (A != NULL) delete[] A;
    //    if (B != NULL) delete[] B;
}

void Polyhedron::import_coordinates(Polyhedron& orig) {
    int i;
    for (i = 0; i < numv; ++i) {
        vertex[i] = orig.vertex[i];
    }
    for (i = 0; i < numf; ++i) {
        facet[i].plane = orig.facet[i].plane;
    }
}

//Данная функция отвечает за локальную минимизацию (т. е. минимизируется отклонение на шаге)
void Polyhedron::deform_linear2(int id, Vector3d delta) {
    //    printf(".");

    int step, i;
    double err, err_eps;
    double K;
    double *x, *y, *z, *A, *B;
    double *xold, *yold, *zold;
    double norm;
    int ncons_curr, ncons_prev;
    

    //    int i, nf, *index;
    //    double dist;
    //
    ////    vertexinfo[id].fprint_my_format(stdout);
    //
    //    nf = vertexinfo[id].nf;
    //    index = vertexinfo[id].index;
    //    for (i = 0; i < nf; ++i) {
    //        dist = qmod(vertex[id] - vertex[index[nf + 1 + i]]);
    //        dist = sqrt(dist);
    ////        printf("dist(%d, %d) = %lf\n", id, index[nf + 1 + i], dist);
    //    }
    //    //    scanf("%lf", &dist);

    x = new double[numv];
    y = new double[numv];
    z = new double[numv];
    xold = new double[numv];
    yold = new double[numv];
    zold = new double[numv];
    A = new double[9];
    B = new double[3];

    //    delta += vertex[id]; //Сохраняем положение деформированной точки
    vertex[id] += delta;

    for (i = 0; i < numv; ++i) {
        xold[i] = vertex[i].x;
        yold[i] = vertex[i].y;
        zold[i] = vertex[i].z;
    }


#ifndef DEFORM_SCALE
        step = 0;
        K = 100;
    //    err = deform_linear_calculate_error();
    //    K = sqrt(qmod(delta)) / err;
        err_eps = EPSILON;
        ncons_prev = test_consections(false);
        for (i = 0; i < 10; ++i) {
            do {
    
                vertex[id].x = xold[id];
                vertex[id].y = yold[id];
                vertex[id].z = zold[id];
//            vertex[id] = delta;
                deform_linear_facets(x, y, z);
//                join_points();
                deform_linear_vertices(K, A, B);
                err = deform_linear_calculate_error();
                norm = deform_linear_calculate_deform(xold, yold, zold);
                
                ncons_curr = test_consections(false);
                if (ncons_curr != ncons_prev) {
                    ncons_prev = ncons_curr;
                }

				printf("\\hline\n %d & %d & ", i, step);
				printf("%le & %le & %lf & %d\\\\\n", err, norm, K, ncons_curr);
				test_consections(true);
				join_points(id);

                ++step;
                if (step > MAX_STEPS) {
                    printf("Too much steps...\n");
                    return;
                }
            } while (err > err_eps);
    
            K *= 1.005;
    //        K = norm / err;
    //        K += 1.;
            err_eps *= 0.5;
        }

#endif
#ifdef DEFORM_SCALE
	char* fname;
	fname = new char[255];

    step = 0;
    err = deform_linear_calculate_error();
    K = sqrt(qmod(delta)) / err;
    
    ncons_prev = test_consections();
    ifPrint = false;
    
    //    K = 1. / err;
    do {
        vertex[id].x = xold[id];
        vertex[id].y = yold[id];
        vertex[id].z = zold[id];
        deform_linear_facets(x, y, z);
        
        ncons_curr = test_consections();
//        if (ncons_curr != ncons_prev) {
        if (1) {
            ifPrint = true;
            ncons_prev = ncons_curr;
        }
        
        deform_linear_vertices(K, A, B);
        err = deform_linear_calculate_error();
        norm = deform_linear_calculate_deform(xold, yold, zold);
//        if (step % 100 == 0 || step >= 2059) {
        if (ifPrint) {
            printf("\\hline\n %d & %le & %le & %lf & %d\\\\\n", step, err, norm, K, ncons_curr);
            sprintf(fname, "../poly-data-out/Consections - %d.ply", ncons_curr);
            fprint_ply_scale(1000., fname, "generated-in-deform-linear");
            ifPrint = false;

            //                printf("%d\t%d\terr = %le\tnorm = %le\tK = %lf\n", i, step++, err, norm, K);
        }
        ++step;
        if (step > MAX_STEPS) {
            printf("Too much steps...\n");
            return;
        }
        K = norm / err;
    } while (err > 1e-10);
#endif

    //    double xmin, xmax, ymin, ymax, zmin, zmax;
    //    this->get_boundary(xmin, xmax, ymin, ymax, zmin, zmax);
    //    fprintf(stdout, "boundary : \n");
    //    fprintf(stdout, "x [%lf,  %lf]\n", xmin, xmax);
    //    fprintf(stdout, "y [%lf,  %lf]\n", ymin, ymax);
    //    fprintf(stdout, "z [%lf,  %lf]\n", zmin, zmax);

    this->delete_empty_facets();
//    my_fprint(stdout);
    
    if (x != NULL) delete[] x;
    if (y != NULL) delete[] y;
    if (z != NULL) delete[] z;
    if (x != NULL) delete[] xold;
    if (y != NULL) delete[] yold;
    if (z != NULL) delete[] zold;
    if (A != NULL) delete[] A;
    if (B != NULL) delete[] B;

}

void Polyhedron::deform_linear_vertices(double K, double* A, double* B) {
    int i, j, nf, *index;
    double a, b, c, d;
    double Maa, Mab, Mac, Mad, Mbb, Mbc, Mbd, Mcc, Mcd;
//    double *A, *B;
    double norm;

    Plane pl;
//    A = new double[9];
//    B = new double[3];

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
        if (nf == 0) {
            printf("Vertex %d cannot be found in any facet...\n", i);
            continue;
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
        A[0] = 1. + K * Maa;
        A[1] = K * Mab;
        A[2] = K * Mac;
        A[3] = A[1];
        A[4] = 1. + K * Mbb;
        A[5] = K * Mbc;
        A[6] = A[2];
        A[7] = A[5];
        A[8] = 1. + K * Mcc;
        B[0] = -K * Mad + vertex[i].x;
        B[1] = -K * Mbd + vertex[i].y;
        B[2] = -K * Mcd + vertex[i].z;
        Gauss_string(3, A, B);
        norm += (B[0] - vertex[i].x) * (B[0] - vertex[i].x);
        norm += (B[1] - vertex[i].y) * (B[1] - vertex[i].y);
        norm += (B[2] - vertex[i].z) * (B[2] - vertex[i].z);
        vertex[i].x = B[0];
        vertex[i].y = B[1];
        vertex[i].z = B[2];

    }

//        printf("\t\t\tnorm = %lf\n", norm);
    //    printf("%lf & ", norm);
//    if (A != NULL) delete[] A;
//    if (B != NULL) delete[] B;
}
