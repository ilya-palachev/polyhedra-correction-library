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

void Polyhedron::deform_linear_vertex_control(
        int I, // The index of point we are going to deform
        Vector3d& v_new, // The new position of point
        Vector3d* vertex_old) // The array for saving data about old positions of points
{
    
    int *index, nf, nv;
    int i, j, k, i_next;
    int ii;
    double gamma0, gamma, gamma_loc;
    int fid;
    double d0, d1;
    
    int ii_prev, ii_next;
    int I_prev, I_next;
    
    Vector3d v, v_new_proj;
    Vector3d n, n0, n1;
    Plane plane, plane0, plane1;
    Vector3d A;
    
    printf("vertex[%d]\n", I);
    
//    index = vertexinfo[ii].index; //indexes of the facets including this point
    nf = vertexinfo[I].nf; //number of facets including this point
    
    gamma0 = 1.; //The coefficient of decreasing of the deformation (v_new - vertex[i])
    
    for (j = 0; j < nf; ++j) {
        fid = vertexinfo[I].index[j]; //index of current facet including deformed point
//        printf("\tfacet[%d]\n", fid);
        printf("\t");
        facet[fid].my_fprint(stdout);
        ii = vertexinfo[I].index[2 * nf + 1 + j]; //position of deformed point in current facet
        
        nv = facet[fid].nv;          // number of points in facet #fid
        index = facet[fid].index;    // indexes of points in facet #fid
        plane = facet[fid].plane;    // plane of facet #fid
        n = plane.norm;              // norm of this plane
        
        // Project all the points to the plane of the facet (because point may not
        // lay at the plane of the facet):
        for (i = 0; i < nv; ++i) {
            v = vertex[index[i]];
            vertex_old[i] = v;
            vertex[index[i]] = plane.project(v);
        }
        
        // Project the new position of deformed point to the plane:
        v_new_proj = plane.project(v_new);
        
        gamma = sqrt(qmod(v_new_proj - vertex[I])); // The local coefficient of decreasing of the deformation
        
        // Calculate the normal of plane pi_{0}:
        n0 = n % (v_new_proj - vertex[I]);
        
        // Calculate free coefficient of this plane:
        d0 = - n0 * vertex[I];
        
        // Calculate the plane pi_{0}:
        plane0 = Plane(n0, d0);
        
        i = ii;
        for (k = 0; k < nv - 2; ++k) {
            i = (nv + i + 1) % nv;
            i_next = (nv + i + 1) % nv;
            
            printf("\t\tTesting edges (%d, %dnew) and (%d, %d)\n", I, I, index[i], index[i_next]);

            // If the points A_{i} and A_{i + 1} lay on the different sides
            // relatively to the plane pi_{0}:
            if (signum(vertex[index[i]], plane0) == signum(vertex[index[i_next]], plane0)) {
                continue;
            }                

            // Calculate the normal pf plane pi_{1}:
            n1 = n % (vertex[index[i_next]] - vertex[index[i]]);

            // Calculate the free coefficient of this plane:
            d1 = - n1 * vertex[index[i]];

            // Claculate the plane pi_{1}:
            plane1 = Plane(n1, d1);

            // If the points A_{I} and v_new_proj lay on the different sides
            // relatively to the plane pi_{0}:
            if (signum(vertex[I], plane1) == signum(v_new_proj, plane1)) {
                continue;
            }

            // Calculate the intersection of plane pi_{j_{k}}, pi_{0} and pi_{1}
            // and save it to the vector A:
            intersection(plane, plane0, plane1, A);

            // If the length of such deformation is shorter than that was saved
            // then save this:
            gamma_loc = sqrt(qmod(A - vertex[I]));
            if (gamma_loc < gamma) {
                gamma = gamma_loc;
                printf("gamma = %lf (1 cycle) vertex[%d] facet[%d]- edges (%d, %dnew) and (%d, %d) consect\n", 
                        gamma / sqrt(qmod(v_new_proj - vertex[I])), I, fid, I, I, i, i_next);
            }
        }
        
        ii_next = (nv + ii + 1) % nv;
        ii_prev = (nv + ii - 1) % nv;
        i = ii_next;
        I_prev = index[ii_prev];
        I_next = index[ii_next];
        
        for (k = 0; k < nv - 3; ++k) {
            i = (nv + i + 1) % nv;

            
            // If A_{I - 1} and A_{i} lay on the same side relatively 
            // to the plane pi_{0}:
            if (signum(vertex[I_prev], plane0) == signum(vertex[index[i]], plane0)) {
                
                printf("\t\tTesting edges (%d, %dnew) and (%d, {%d or %d})\n", 
                        I_prev, I, index[i], index[(nv + i + 1) % nv], index[(nv + i - 1) % nv]);
                
                // Calculate the normal of the plane pi_{1}:
                n1 = n % (vertex[index[i]] - vertex[I_prev]);
                
                // Calculate the free coefficient of the plane pi_{1}:
                d1 = - n1 * vertex[index[i]];
                
                // Calculate the plane pi_{1}:
                plane1 = Plane(n1, d1);
                
                // If points A_{I} and A_{I}^{''} lay in different sides
                // relatively to the plane pi_{1}:
                if (signum(vertex[I], plane1) == signum(v_new_proj, plane1)) {
                    continue;
                }

                // Calculate the intersection of plane pi_{j_{k}}, pi_{0} and pi_{1}
                // and save it to the vector A:
                intersection(plane, plane0, plane1, A);

                // If the length of such deformation is shorter than that was saved
                // then save this:
                gamma_loc = sqrt(qmod(A - vertex[I]));
                if (gamma_loc < gamma) {
                    gamma = gamma_loc;
                    printf("gamma = %lf (2 cycle) vertex[%d] facet[%d] - edges (%d, %dnew) and (%d, {%d or %d}) consect\n", 
                            gamma / sqrt(qmod(v_new_proj - vertex[I])), 
                            I, fid, I_prev, I, index[i], index[(nv + 1 + i) % nv],
                            index[(nv - 1 + i) % nv]);
                }
            } 
            // If A_{I - 1} and A_{i} lay on the same side relatively 
            // to the plane pi_{0}:
            else if (signum(vertex[I_next], plane0) == signum(vertex[index[i]], plane0)) {

                printf("\t\tTesting edges (%d, %dnew) and (%d, {%d or %d})\n", 
                        I_next, I, index[i], index[(nv + i + 1) % nv], index[(nv + i - 1) % nv]);
                
                // Calculate the normal of the plane pi_{1}:
                n1 = n % (vertex[index[i]] - vertex[I_next]);
                
                // Calculate the free coefficient of the plane pi_{1}:
                d1 = - n1 * vertex[index[i]];
                
                // Calculate the plane pi_{1}:
                plane1 = Plane(n1, d1);
                
                // If points A_{I} and A_{I}^{''} lay in different sides
                // relatively to the plane pi_{1}:
                if (signum(vertex[I], plane1) == signum(v_new_proj, plane1)) {
                    continue;
                }

                // Calculate the intersection of plane pi_{j_{k}}, pi_{0} and pi_{1}
                // and save it to the vector A:
                intersection(plane, plane0, plane1, A);

                // If the length of such deformation is shorter than that was saved
                // then save this:
                gamma_loc = sqrt(qmod(A - vertex[I]));
                if (gamma_loc < gamma) {
                    gamma = gamma_loc;
                    printf("gamma = %lf (3 cycle) vertex[%d] facet[%d] - edges (%d, %dnew) and (%d, {%d or %d}) consect\n", 
                            gamma / sqrt(qmod(v_new_proj - vertex[I])), 
                            I, fid, I_prev, I, index[i], index[(nv + 1 + i) % nv],
                            index[(nv - 1 + i) % nv]);
                }   
            }    
        }
        if (signum(vertex[I_prev], plane) == signum(vertex[I_next], plane)) {

                // Calculate the normal of the plane pi_{1}:
                n1 = n % (vertex[I_next] - vertex[I_prev]);
                
                // Calculate the free coefficient of the plane pi_{1}:
                d1 = - n1 * vertex[I_prev];
                
                // Calculate the plane pi_{1}:
                plane1 = Plane(n1, d1);
                
                // If points A_{I} and A_{I}^{''} lay in different sides
                // relatively to the plane pi_{1}:
                if (signum(vertex[I], plane1) == signum(v_new_proj, plane1)) {
                    continue;
                }

                // Calculate the intersection of plane pi_{j_{k}}, pi_{0} and pi_{1}
                // and save it to the vector A:
                intersection(plane, plane0, plane1, A);

                // If the length of such deformation is shorter than that was saved
                // then save this:
                gamma_loc = sqrt(qmod(A - vertex[I]));
                if (gamma_loc < gamma) {
                    gamma = gamma_loc;
                    printf("gamma = %lf (4 cycle) vertex[%d] facet[%d] - edges (%d, %dnew) and (%d, %d) consect\n", 
                            gamma / sqrt(qmod(v_new_proj - vertex[I])), 
                            I, fid, I_prev, I, I_next, index[(nv + 1 + ii_next) % nv]);
                }   
            
        }

        // Calculate the relative coefficient:
        gamma /= sqrt(qmod(v_new_proj - vertex[I]));

        // If it is less than saved save this:
        if (gamma < gamma0) {
            gamma0 = gamma;
        }
        
        // Return all points to their old positions (before projecting to the plane):
        for (i = 0; i < nv; ++i) {
            vertex[index[i]] = vertex_old[i];
        }
    }
    
    if (gamma0 < SAFETY_LIMIT) {
        printf("  SAFETY_LIMIT has been reached...\n");
        printf("gamma0 = %lf --------- vertex[%d]\n", gamma0, I);
        test_consections(true);
        printf("End of deform_linear_vertex_control vertex[%d]\n", I);
        return;
    }
    
    
    // This multiplication saves from the consections which can be created 
    // during the deformation in different facets (???)
    gamma0 *= SAFETY_COEFFICIENT;

    // Deform point to the new safe position:
    vertex[I] += gamma0 * (v_new - vertex[I]);
    
    if (gamma0 < SAFETY_COEFFICIENT) {
        printf("gamma0 = %lf --------- vertex[%d]\n", gamma0, I);
        test_consections(true);
        printf("End of deform_linear_vertex_control vertex[%d]\n", I);
    }
    
    
}

void Polyhedron::deform_linear_vertices_control(int id, double K, double* A, double* B, Vector3d* vertex_old) {
    int i, j, nf, *index;
    double a, b, c, d;
    double Maa, Mab, Mac, Mad, Mbb, Mbc, Mbd, Mcc, Mcd;
//    double *A, *B;
    double norm;

    Plane pl;
    
    Vector3d v_new;
//    A = new double[9];
//    B = new double[3];

    norm = 0.;
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
        
        if (Gauss_string(3, A, B) == 0) {
            continue;
        }
        norm += (B[0] - vertex[i].x) * (B[0] - vertex[i].x);
        norm += (B[1] - vertex[i].y) * (B[1] - vertex[i].y);
        norm += (B[2] - vertex[i].z) * (B[2] - vertex[i].z);
        
        v_new = Vector3d(B[0], B[1], B[2]);
        deform_linear_vertex_control(i, v_new, vertex_old);

    }

//        printf("\t\t\tnorm = %lf\n", norm);
    //    printf("%lf & ", norm);
//    if (A != NULL) delete[] A;
//    if (B != NULL) delete[] B;
}



#define MAX_STEPS 1e4

//Данная функция отвечает за глобальную минимизацию (т. е. минимизируется отклонение в целом)
void Polyhedron::deform_linear_control(int id, Vector3d delta) {
    //    printf(".");

    int step, i;
    double err, err_eps;
    double K;
    double *x, *y, *z, *A, *B;
    double *xold, *yold, *zold;
    double norm;

    Vector3d* vertex_old;
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
    vertex_old = new Vector3d[numv];

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
            deform_linear_vertices_control(id, K, A, B, xold, yold, zold, vertex_old);
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
            deform_linear_vertices_control(id, K, A, B, xold, yold, zold, vertex_old);
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
    if (vertex_old != NULL) delete[] vertex_old;

}

void Polyhedron::deform_linear_vertices_control(
        int id, double K, double* A, double* B,
        double* xold, double* yold, double* zold, Vector3d* vertex_old) {
    int i, j, nf, *index;
    double a, b, c, d;
    double Maa, Mab, Mac, Mad, Mbb, Mbc, Mbd, Mcc, Mcd;
    //    double *A, *B;
    //    double norm;

    Vector3d v_new;
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
        if (Gauss_string(3, A, B) == 0) {
            continue;
        }
        //        norm += (B[0] - vertex[i].x) * (B[0] - vertex[i].x);
        //        norm += (B[1] - vertex[i].y) * (B[1] - vertex[i].y);
        //        norm += (B[2] - vertex[i].z) * (B[2] - vertex[i].z);
        v_new = Vector3d(B[0], B[1], B[2]);
        deform_linear_vertex_control(i, v_new, vertex_old);
    }

    //        printf("\t\t\tnorm = %lf\n", norm);
    //    printf("%lf & ", norm);
    //    if (A != NULL) delete[] A;
    //    if (B != NULL) delete[] B;
}

//Данная функция отвечает за локальную минимизацию (т. е. минимизируется отклонение на шаге)
void Polyhedron::deform_linear2_control(int id, Vector3d delta) {
    //    printf(".");

    int step, i;
    double err, err_eps;
    double K;
    double *x, *y, *z, *A, *B;
    double *xold, *yold, *zold;
    double norm;
    int ncons_curr, ncons_prev;
    bool ifPrint;
    
    Vector3d* vertex_old;
    
    char* fname;
    fname = new char[255];

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
    vertex_old = new Vector3d[numv];

    //    delta += vertex[id]; //Сохраняем положение деформированной точки
    vertex[id] += delta;
//    test_consections(true);

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
        ifPrint = false;
        ncons_prev = test_consections(false);
        for (i = 0; i < 10; ++i) {
            do {
    
                vertex[id].x = xold[id];
                vertex[id].y = yold[id];
                vertex[id].z = zold[id];
//            vertex[id] = delta;
                deform_linear_facets(x, y, z);
//                join_points();
                deform_linear_vertices_control(id, K, A, B, vertex_old);
                err = deform_linear_calculate_error();
                norm = deform_linear_calculate_deform(xold, yold, zold);
                
                ncons_curr = test_consections(false);
                if (ncons_curr != ncons_prev) {
//                if (1) {
                    ifPrint = true;
                    ncons_prev = ncons_curr;
                }
//                if (ifPrint) {
                if (1) {
                    printf("\\hline\n %d & %d & ", i, step);
                    printf("%le & %le & %lf & %d\\\\\n", err, norm, K, ncons_curr);
    //                    printf("%d\t%d\terr = %le\tnorm = %le\terr_eps = %le\tK = %lf\n", i, step++, err, norm, err_eps, K);
                    if (ifPrint) {
                        sprintf(fname, "../poly-data-out/Consections - %d.ply", ncons_curr);
                        fprint_ply_scale(1000., fname, "generated-in-deform-linear");
                        ifPrint = false;
                    }
                    test_consections(true);
//                    join_points(id);
                }
                ++step;
                if (step > MAX_STEPS) {
                    printf("Too much steps...\n");
                    return;
                }
            } while (err > err_eps);
    
            K *= 2;
    //        K = norm / err;
    //        K += 1.;
            err_eps *= 0.5;
        }

#endif
#ifdef DEFORM_SCALE
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
        
        deform_linear_vertices_control(K, A, B, vertex_old);
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
    if (vertex_old != NULL) delete[] vertex_old;

}

