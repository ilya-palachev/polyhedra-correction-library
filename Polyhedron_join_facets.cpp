#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"
#include "list_squares_method.h"
#include "Vector3d.h"
#include "array_operations.h"

int Polyhedron::join_facets_count_nv(int facet_id0, int facet_id1) {
    int nv0, nv1, nv_common, *index0, *index1, i, j, nv;

    if (facet_id0 == facet_id1) {
        fprintf(stderr, "join_facets_...: Error. facet_id0 == facet_id1.\n");
        return 0;
    }

    nv0 = facet[facet_id0].nv;
    nv1 = facet[facet_id1].nv;
    index0 = facet[facet_id0].index;
    index1 = facet[facet_id1].index;

    for (i = 0; i < nv0; ++i) {
        if (index0[nv0 + 1 + i] == facet_id1)
            break;
    }
    if (i == nv0) {
        fprintf(stderr, "join_facets_...: Error. No common vertices.\n");
        return 0;
    }

    //Подсчет количества общих вершин
    nv_common = 0;
    for (j = 0; j < nv0; i = (i < nv0 - 1 ? i + 1 : 0), ++j) {
        ++nv_common;
        if (index0[nv0 + 1 + i] != facet_id1)
            break;
    }

    nv = nv0 + nv1 - nv_common;
    return nv;
}

void Polyhedron::join_facets_create_vertex_list(int facet_id0, int facet_id1, int nv,
        int *vertex_list, int* edge_list) {
    int nv0, nv1, *index0, *index1, i, j, k;

    nv0 = facet[facet_id0].nv;
    nv1 = facet[facet_id1].nv;
    index0 = facet[facet_id0].index;
    index1 = facet[facet_id1].index;

    for (i = 0; i < nv0; ++i) {
        if (index0[nv0 + 1 + i] == facet_id1)
            break;
    }

    for (j = 0; j < nv0; i = (i < nv0 - 1 ? i + 1 : 0), ++j) {
        if (index0[nv0 + 1 + i] != facet_id1)
            break;
    }

    //Сбор вершин нулевой грани
    for (j = 0; j < nv0; i = (i < nv0 - 1 ? i + 1 : 0), ++j) {
        vertex_list[j] = index0[i];
        if (index0[nv0 + 1 + i] == facet_id1)
            break;
        edge_list[j] = index0[nv0 + 1 + i];
    }
    //Утверждение. index0[nv0 + 1 + i] == facet_id1
    i = index0[2 * nv0 + 1 + i];
    //Сбор вершин первой грани
    for (k = 0; k < nv1; i = (i < nv1 - 1 ? i + 1 : 0), ++k, ++j) {
        if (index1[nv1 + 1 + i] == facet_id0)
            break;
        vertex_list[j] = index1[i];
        edge_list[j] = index1[nv1 + 1 + i];
    }

    nv = nv;
    //debug
    //	printf("join_facets_create_*vertex_list:\n");
    //	printf("facet %d: nv = %d\n\t", facet_id0, nv0);
    //	for(i = 0; i < nv0; ++i)
    //		printf("%d ", index0[i]);
    //	printf("\n");
    //	printf("facet %d: nv = %d\n\t", facet_id1, nv1);
    //	for(i = 0; i < nv1; ++i)
    //		printf("%d ", index1[i]);
    //	printf("\n");
    //	printf("joined facet: nv = %d\n", nv);
    //	for(i = 0; i < nv; ++i)
    //		printf("%d ", vertex_list[i]);
    //	printf("\n");
    //	for(i = 0; i < nv; ++i)
    //		printf("%d ", edge_list[i]);
    //	printf("\n");
}

void Polyhedron::list_squares_method(int nv, int *vertex_list, Plane *plane) {
    int i, id;
    double *x, *y, *z, a, b, c, d;

    x = new double[nv];
    y = new double[nv];
    z = new double[nv];

    for (i = 0; i < nv; ++i) {
        id = vertex_list[i];
        x[i] = vertex[id].x;
        y[i] = vertex[id].y;
        z[i] = vertex[id].z;
    }

    aprox(nv, x, y, z, a, b, c, d);

    plane->norm.x = a;
    plane->norm.y = b;
    plane->norm.z = c;
    plane->dist = d;

    //debug
    //printf("list_squares_method: a = %lf, b = %lf, c = %lf, d = %lf\n", a, b, c, d);

    if (x != NULL) delete[] x;
    if (y != NULL) delete[] y;
    if (z != NULL) delete[] z;
}

void Polyhedron::list_squares_method_weight(
        int nv,
        int *vertex_list,
        Plane *plane) {
    int i, id, i_prev, i_next, id0, id1;
    double *x, *y, *z, a, b, c, d;
    double *l, *w;

    x = new double[nv];
    y = new double[nv];
    z = new double[nv];
    l = new double[nv];
    w = new double[nv];

    for (i = 0; i < nv; ++i) {
        i_next = (i + 1) % nv;
        id0 = vertex_list[i];
        id1 = vertex_list[i_next];
        l[i] = sqrt(qmod(vertex[id0] - vertex[id1]));
    }

    for (i = 0; i < nv; ++i) {
        i_prev = (i - 1 + nv) % nv;
        w[i] = 0.5 * (l[i_prev] + l[i]);
    }

    for (i = 0; i < nv; ++i) {
        id = vertex_list[i];
        x[i] = vertex[id].x * w[i];
        y[i] = vertex[id].y * w[i];
        z[i] = vertex[id].z * w[i];
    }

    aprox(nv, x, y, z, a, b, c, d);

    plane->norm.x = a;
    plane->norm.y = b;
    plane->norm.z = c;
    plane->dist = d;

    //debug
    //printf("list_squares_method: a = %lf, b = %lf, c = %lf, d = %lf\n", a, b, c, d);

    if (x != NULL) delete[] x;
    if (y != NULL) delete[] y;
    if (z != NULL) delete[] z;
    if (z != NULL) delete[] l;
    if (z != NULL) delete[] w;
}

void Polyhedron::join_create_first_facet(int fid0, int fid1) {

    int nv0, nv1, nv, nv_common, *index0, *index1, *index;
    int *sign_vertex, i, j, k, ndown;
    int fid;

    if (fid0 == fid1) {
        fprintf(stderr, "join_facets_...: Error. fid0 == fid1.\n");
        return;
    }


    nv0 = facet[fid0].nv;
    nv1 = facet[fid1].nv;
    index0 = facet[fid0].index;
    index1 = facet[fid1].index;

    //0. Вычислим среднюю плоскость.
    Plane plane;
    index = new int[nv0 + nv1];
    nv = 0;
    nv_common = 0;
    for (i = 0; i < nv0; ++i) {
        index[i] = index0[i];
        ++nv;
    }
    for (i = 0; i < nv1; ++i) {
        k = 0;
        for (j = 0; j < nv0 + i; ++j)
            if (index[j] == index1[i]) {
                ++nv_common;
                k = 1;
            }
        if (k == 1)
            continue;
        else {
            index[nv] = index1[i];
            ++nv;
        }
    }
    list_squares_method(nv, index, &plane);

    //1. Составим список вершин
    nv = nv - nv_common + 2;
    printf("nv = %d, nv_common = %d\n", nv, nv_common);
    if (index != NULL)
        delete[] index;
    index = new int[3 * nv + 1];

    for (i = 0; i < nv0; ++i)
        if (facet[fid1].find_vertex(index0[i]) == -1)
            break;

    for (j = 0; j < nv; ++j) {
        index[j] = index0[i];
        k = facet[fid1].find_vertex(index0[i]);
        if (k != -1) {
            ++j;
            break;
        }
        i = (i == nv0 - 1) ? 0 : i + 1;
    }
    i = (k == nv1 - 1) ? 0 : k + 1;
    for (; j < nv; ++j) {
        index[j] = index1[i];
        k = facet[fid0].find_vertex(index1[i]);
        if (k != -1) {
            ++j;
            break;
        }
        i = (i == nv1 - 1) ? 0 : i + 1;
    }
    i = (k == nv0 - 1) ? 0 : k + 1;
    for (; j < nv; ++j) {
        index[j] = index0[i];
        i = (i == nv0 - 1) ? 0 : i + 1;
    }

    fprintf(stdout, "nv = %d, nv_common = %d\n", nv, nv_common);
    printf("index : ");
    for (i = 0; i < nv; ++i) {
        printf("%d ", index[i]);
    }
    printf("\n");
    
    index[nv] = index[0];
    for (i = 0; i < nv; ++i) {
        k = facet[fid0].find_vertex(index[i]);
        if (k != -1) {
            index[nv + 1 + i] = index0[nv0 + 1 + k];
            index[2 * nv + 1 + i] = index0[2 * nv0 + 1 + k];

        } else {
            k = facet[fid1].find_vertex(index[i]);
            if (k == -1) {
                printf("Unexpected error. Cannot find vertex %d\n", index[i]);
                return;
            }
            index[nv + 1 + i] = index1[nv1 + 1 + k];
            index[2 * nv + 1 + i] = index1[2 * nv1 + 1 + k];            
        }
    }
    //1. Сколько общих вершин?
    //    nv = nv_common = 0;
    //    for (i = 0; i < nv0; ++i) {
    //        if (index0[nv0 + 1 + i] == fid1)
    //            ++nv_common;
    //        else
    //            ++nv;
    //    }
    //    for (i = 0; i < nv1; ++i) {
    //        if (index0[nv1 + 1 + i] == fid0)
    //            ++nv_common;
    //        else
    //            ++nv;
    //    }
    //
    //    --nv;
    //    index = new int[3 * nv + 1];
    //    int fid = fid0;
    //    for (j = 0; j < nv0; ++j) {
    //        if (index0[j] != fid1)
    //            break;
    //    }
    //    for (i = 0; i < nv; ++i) {
    //        if (fid == fid0) {
    //            index[i] = index0[j];
    //            if (index0[nv0 + 1 + j] == fid1) {
    //                fid = fid1;
    //                for (k = 0; k < nv1; ++k)
    //                    if (index1[k] == index0[j]) {
    //                        index[nv + 1 + i] = index1[nv1 + 1 + k];
    //                        index[2 * nv + 1 + i] = index1[2 * nv1 + 1 + k];
    //                        j = k + 1;
    //                        break;
    //                    }
    //            } else {
    //                index[nv + 1 + i] = index0[nv0 + 1 + j];
    //                index[2 * nv + 1 + i] = index0[2 * nv0 + 1 + j];
    //                ++j;
    //            }
    //        } else { //fid == fid1
    //            index[i] = index1[j];
    //            if (index1[nv1 + 1 + j] == fid0) {
    //                fid = fid0;
    //                for (k = 0; k < nv0; ++k)
    //                    if (index0[k] == index1[j]) {
    //                        index[nv + 1 + i] = index0[nv0 + 1 + k];
    //                        index[2 * nv + 1 + i] = index0[2 * nv0 + 1 + k];
    //                        j = k + 1;
    //                        break;
    //                    }
    //            } else {
    //                index[nv + 1 + i] = index1[nv1 + 1 + j];
    //                index[2 * nv + 1 + i] = index1[2 * nv1 + 1 + j];
    //                ++j;
    //            }
    //        }
    //        //                printf("index[%d] = %d, index[%d] = %d, index[%d] = %d, fid = %d\n",
    //        //                        i, index[i], nv + 1 + i, index[nv + 1 + i],
    //        //                        2 * nv + 1 + i, index[2 * nv + 1 + i], fid);
    //    }
    //    index[nv] = index[0];


    //Проверка, что нормаль построенной плокости направлена извне 
    //многогранника
    if (plane.norm * facet[fid0].plane.norm < 0 &&
            plane.norm * facet[fid1].plane.norm < 0) {
        plane.norm *= -1.;
        plane.dist *= -1.;
    }

    Facet first_facet;
    first_facet = Facet(-1, nv, plane, index, this, true);
    first_facet.clear_bad_vertexes();

    first_facet.my_fprint_all(stdout);
    facet[fid0] = first_facet;

    //Посчитаем, сколько вершин находится под плоскостью.
    sign_vertex = new int[nv];
    index = first_facet.index;
    ndown = 0;
    for (i = 0; i < nv; ++i) {
        sign_vertex[i] = signum(vertex[index[i]], plane);
        if (sign_vertex[i] == -1)
            ++ndown;
    }

    for (i = 0; i < nv; ++i)
        printf("%d\t%d\n", index[i], sign_vertex[i]);

    int i_prev_prev, i_prev, i_next, i_next_next;
    int f_prev_prev, f_prev, f_next, f_next_next;
    Vector3d intrsct;
    Vector3d dir;
    Vector3d point;

    double min_dist, dist;
    int min_i, i_replace;
    int tmp0, tmp1, tmp2;
    int new_number;
    int v_replace;

    int step = 0;
    while (ndown > 0) {
        printf("------ step %d -------\n", step++);
        printf("\tndown = %d\n", ndown);
        printf("Cycle 1.\n");
        min_i = -1;
        for (i = 0; i < nv; ++i) {
            if (sign_vertex[i] == 1)
                continue;
            if (i == 0) {
                i_prev_prev = nv - 2;
                i_prev = nv - 1;
                i_next = 0;
                i_next_next = 1;
            } else if (i == 1) {
                i_prev_prev = nv - 1;
                i_prev = 0;
                i_next = 1;
                i_next_next = 2;
            } else if (i == nv - 1) {
                i_prev_prev = nv - 3;
                i_prev = nv - 2;
                i_next = nv - 1;
                i_next_next = 0;
            } else {
                i_prev_prev = i - 2;
                i_prev = i - 1;
                i_next = i;
                i_next_next = i + 1;
            }
            f_prev_prev = index[nv + 1 + i_prev_prev];
            f_prev = index[nv + 1 + i_prev];
            f_next = index[nv + 1 + i_next];
            f_next_next = index[nv + 1 + i_next_next];

            intersection(facet[f_prev_prev].plane,
                    facet[f_prev].plane,
                    facet[f_next].plane,
                    intrsct);
            dist = qmod(vertex[index[i]] - intrsct);
            dist = sqrt(dist);
            //                printf("i = %d, dist = %lf", i, dist);
            if (min_i == -1 || dist < min_dist) {
                min_dist = dist;
                min_i = i;
                i_replace = i_prev;
            }

            intersection(facet[f_prev].plane,
                    facet[f_next].plane,
                    facet[f_next_next].plane,
                    intrsct);
            dist = qmod(vertex[index[i]] - intrsct);
            dist = sqrt(dist);
            //                printf(", dist = %lf\n", dist);
            if (min_i == -1 || dist < min_dist) {
                min_dist = dist;
                min_i = i;
                i_replace = i_next_next;
            }
        }
        printf("min_i = %d, min_dist = %lf, i_replace = %d\n",
                min_i, min_dist, i_replace);
        printf("\tMoving ");
        print_vertex(index[min_i]);

        if (min_dist > 1.)
            break;


        printf("Cycle 2.\n");
        for (i = 0; i < nv; ++i) {
            if (sign_vertex[i] == 1)
                continue;
            if (i == min_i)
                continue;
            if (i == 0) {
                i_prev = nv - 1;
                i_next = 0;
            } else {
                i_prev = i - 1;
                i_next = i;
            }
            f_prev = index[nv + 1 + i_prev];
            f_next = index[nv + 1 + i_next];
            intersection(facet[f_prev].plane, facet[f_next].plane, dir, point);
            dir.norm(min_dist);

            // 2 случая:
            //                tmp0 = index[2 * nv + 1 + i] > 0 ? index[2 * nv + 1 + i] - 1 : facet[f_prev].nv - 1;
            //                facet[f_prev].get_next_facet(tmp0, tmp1, fid, tmp2);
            facet[f_prev].find_next_facet2(index[i], fid);

            // Точка сдвигается
            if (fid == f_next) {
                printf("\tMoving vertex %d\n", index[i]);
                //                    print_vertex(index[i]);
                vertex[index[i]] += dir;
                //                    sign_vertex[i] = signum(vertex[index[i]], plane);                    
                //                    print_vertex(index[i]);
            }// Добавляется новая точка
            else {
                printf("\tCreating new for vertex %d\n", index[i]);
                //                    print_vertex(index[i]);
                point = vertex[index[i]] + dir;
                new_number = add_vertex(point);
                printf("\tAdding vertex %d after %d in facet %d\n", new_number, index[i], f_next);
                facet[f_prev].add_after_position(index[2 * nv + 1 + i_prev], new_number, f_next);
                printf("\tAdding vertex %d before %d in facet %d\n", new_number, index[i], f_prev);
                facet[f_next].add_before_position(index[2 * nv + 1 + i_next], new_number, f_prev);
                index[i] = new_number;
                //                    sign_vertex[i] = signum(vertex[index[i]], plane);                    
                print_vertex(index[i]);
            }
            //                if (i == i_replace)
            //                    v_replace = index[i];
        }
        // Отдельно рассмотрим случай выбранной вершины. Эта вершина будет
        // удалена из списка.
        v_replace = index[i_replace];
        i = min_i;
        if (i == 0) {
            i_prev = nv - 1;
            i_next = 0;
        } else {
            i_prev = i - 1;
            i_next = i;
        }
        f_prev = index[nv + 1 + i_prev];
        f_next = index[nv + 1 + i_next];

        // 2 случая:
        //            facet[f_prev].my_fprint_all(stdout);
        //            tmp0 = index[2 * nv + 1 + i] > 0 ? index[2 * nv + 1 + i] - 1 : facet[f_prev].nv - 1;
        //            facet[f_prev].get_next_facet(tmp0, tmp1, fid, tmp2);
        //!2
        facet[f_prev].find_next_facet2(index[i], fid);

        // Точка сдвигается
        //            printf("f_prev = %d, f_next = %d, fid = %d\n", f_prev, f_next, fid);
        if (fid == f_next) {
            printf("\tVertex %d is replaced by %d in facet %d\n", index[i], v_replace, f_prev);
            //!2
            facet[f_prev].find_and_replace2(index[i], v_replace);
            printf("\tVertex %d is replaced by %d in facet %d\n", index[i], v_replace, f_next);
            //!2
            facet[f_next].find_and_replace2(index[i], v_replace);
        }// Добавляется новая точка
        else {
            //                printf("\tVertex %d is followed by %d in facet %d and %d\n", index[i], v_replace, f_prev, f_next);
            //                printf("v_replace = %d\n", v_replace);
            printf("\tAdding vertex %d after %d in facet %d\n", v_replace, index[i], f_next);
            facet[f_prev].add_after_position(index[2 * nv + 1 + i_prev], v_replace, f_next);
            printf("\tAdding vertex %d before %d in facet %d\n", v_replace, index[i], f_prev);
            facet[f_next].add_before_position(index[2 * nv + 1 + i_next], v_replace, f_prev);
        }

        for (j = i; j < nv - 1; ++j)
            index[2 * nv + 1 + j] = index[2 * nv + 2 + j];
        for (j = i; j < 2 * nv - 2; ++j)
            index[nv + 1 + j] = index[nv + 2 + j];
        for (j = i; j < 3 * nv - 2; ++j)
            index[j] = index[j + 1];
        //            for (j = i; j < nv - 1; ++j)
        //                sign_vertex[j] = sign_vertex[j + 1];
        --nv;
        ndown = 0;
        for (i = 0; i < nv; ++i) {
            sign_vertex[i] = signum(vertex[index[i]], plane);
            if (sign_vertex[i] == -1)
                ++ndown;
        }
        if (i == 0)
            index[nv] = index[0];

    }

    first_facet = Facet(fid0, nv, plane, index, this, true);
    printf("Cycle 3.\n");
    facet[fid0] = first_facet;
    facet[fid0].my_fprint_all(stdout);
    facet[fid1] = Facet();
    facet[fid1].my_fprint_all(stdout);
    this->preprocess_polyhedron();
    //        this->intersect(plane);
    printf("End\n");
    if (index != NULL)
        delete[] index;
    if (sign_vertex != NULL)
        delete[] sign_vertex;
}

void Polyhedron::delete_vertex_polyhedron(int v) {
    printf("delete_vertex_polyhedron(%d)\n", v);
    for (int i = 0; i < numf; ++i)
        facet[i].delete_vertex(v);
}

int Polyhedron::add_vertex(Vector3d& vec) {
    Vector3d* vertex1;
    vertex1 = new Vector3d[numv + 1];
    for (int i = 0; i < numv; ++i)
        vertex1[i] = vertex[i];
    vertex1[numv] = vec;
    if (vertex != NULL)
        delete[] vertex;
    vertex = vertex1;
    ++numv;
    return numv - 1;
}

void Polyhedron::print_vertex(int i) {
    printf("vertex %d : (%lf , %lf , %lf)\n",
            i, vertex[i].x, vertex[i].y, vertex[i].z);
}
