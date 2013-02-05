#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"
#include "list_squares_method.h"
#include "Vector3d.h"
#include "array_operations.h"

#define EPS_PARALL 1e-16
#define MAX_MIN_DIST 1e+1

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
    int *sign_vertex, i, j, k, ndown, ndown_new;
    int fid;

    if (fid0 == fid1) {
        fprintf(stderr, "join_facets_...: Error. fid0 == fid1.\n");
        return;
    }


    nv0 = facet[fid0].nv;
    nv1 = facet[fid1].nv;
    index0 = facet[fid0].index;
    index1 = facet[fid1].index;

    //I). Вычислим среднюю плоскость.
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
    printf("I). Вычислена средняя плоскость:\n");
    printf("\t(%lf) * x + (%lf) * y + (%lf) * z + (%lf) = 0\n",
            plane.norm.x, plane.norm.y, plane.norm.z, plane.dist);

    //II). Составим список вершин
    nv = nv - nv_common + 2;
//    printf("nv = %d, nv_common = %d\n", nv, nv_common);
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

    printf("II). Составлен список вершин :\n");
    printf("\tnv = %d, nv_common = %d\n", nv, nv_common);
    printf("\tindex : ");
    for (i = 0; i < nv; ++i) {
        printf("%d ", index[i]);
    }
    printf("\n");
    
    printf("III). Дополнительная предобработка многогранника:\n");

    //III.1). Предобработка контура
    printf("\t1). Предобработка контура\n");
    index[nv] = index[0];
    for (i = 0; i < nv; ++i) {
        k = facet[fid0].find_vertex(index[i]);
        if (k != -1 && index0[nv0 + 1 + k] != fid1) {
            index[nv + 1 + i] = index0[nv0 + 1 + k];
            index[2 * nv + 1 + i] = index0[2 * nv0 + 1 + k];
            continue;
        }
        k = facet[fid1].find_vertex(index[i]);
        if (k != -1 && index1[nv1 + 1 + k] != fid0) {
            index[nv + 1 + i] = index1[nv1 + 1 + k];
            index[2 * nv + 1 + i] = index1[2 * nv1 + 1 + k];
            continue;
        }
        printf("Unexpected error. Cannot find vertex %d\n", index[i]);
        return;

    }

    //III.2). Проверка, что нормаль построенной плокости направлена извне 
    //многогранника
    printf("\t2). Проверка, что нормаль построенной плокости направлена извне многогранника\n");
    if (plane.norm * facet[fid0].plane.norm < 0 &&
            plane.norm * facet[fid1].plane.norm < 0) {
        plane.norm *= -1.;
        plane.dist *= -1.;
    }
    
    //III.3). Создается грань для контура
    printf("\t3). Создается грань для контура\n");
    Facet first_facet;
    first_facet = Facet(-1, nv, plane, index, this, true);
    first_facet.my_fprint_all(stdout);

    //III.4) В ней удаляются лишние вершины
    printf("\t4). В ней удаляются лишние вершины\n");
    first_facet.clear_bad_vertexes();
    first_facet.my_fprint_all(stdout);
    
    //III.5) Предобработка многогранника
    printf("\t5). Предобработка многогранника\n");
    facet[fid0] = first_facet;
    facet[fid1] = Facet();
    preprocess_polyhedron();

    //III.6). Посчитаем, какие вершины находятся под и над плоскостью.
    printf("\t6). Посчитаем, какие вершины находятся под и над плоскостью\n");
    first_facet = facet[fid0];
    nv = first_facet.nv;
    if (index != NULL)
        delete[] index;
    index = first_facet.index;
    
    sign_vertex = new int[nv];
    ndown = 0;
    for (i = 0; i < nv; ++i) {
        sign_vertex[i] = signum(vertex[index[i]], plane);
        if (sign_vertex[i] == -1)
            ++ndown;
    }

    for (i = 0; i < nv; ++i)
        printf("\t\t%d\t%d\n", index[i], sign_vertex[i]);
    
    //IV). Алгоритм поднятия вершин, лежащих ниже плоскости
    printf("IV). Алгоритм поднятия вершин, лежащих ниже плоскости\n");

    int i_prev_prev, i_prev, i_next, i_next_next;
    int f_prev_prev, f_prev, f_next, f_next_next;
    Vector3d intrsct;
    Vector3d dir;
    Vector3d point;
    Plane plane0;
    Plane plane1;
    Plane plane2;

    double min_dist, dist, dist_to_plane;
    int min_i, i_replace;
    int tmp0, tmp1, tmp2;
    int new_number;
    int v_replace;
    
    double plane_dist;
    bool ifParallel;

    int step = 0;
    while (ndown > 0) {
        printf("-------- Шаг %d ---------\n", step++);
        printf("\tНиже плоскости находятся %d вершин контура\n", ndown);
        printf("\t1.Находим, по какой вершине нужно шагать, и минимальное расстояние\n");
        min_i = -1;
        for (i = 0; i < nv; ++i) {
            printf("\t\t1.%d. Обработка %d-й вершины : ", i, index[i]);
            if (sign_vertex[i] == 1) {
                printf("выше плоскости\n");
                continue;
            }
            printf("ниже плоскости");
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
            printf("\t\t\tЗатрагиваются грани: %d, %d, %d, %d\n", 
                    f_prev_prev, f_prev, f_next, f_next_next);
            
            plane0 = facet[f_prev_prev].plane;
            plane1 = facet[f_prev].plane;
            plane2 = facet[f_next].plane;

//            printf("plane_%d : (%lf)*x + (%lf)*y + (%lf)*z + (%lf) = 0\n",
//                    f_prev_prev, plane0.norm.x, plane0.norm.y, plane0.norm.z,
//                    plane0.dist);
//            printf("plane_%d : (%lf)*x + (%lf)*y + (%lf)*z + (%lf) = 0\n",
//                    f_next, plane2.norm.x, plane2.norm.y, plane2.norm.z,
//                    plane2.dist);
//            printf("plane : (%lf)*x + (%lf)*y + (%lf)*z + (%lf) = 0\n",
//                    plane.norm.x, plane.norm.y, plane.norm.z,
//                    plane.dist);

            plane0.norm.norm(1.);
            plane2.norm.norm(1.);
            if (plane0.norm * plane2.norm < 0.) {
                plane2.norm *= -1.;
                plane2.dist *= -1.;
            }
            
            plane_dist = qmod(plane0.norm - plane2.norm);
//            printf("dist_plane(%d, %d) = %lf\n", f_prev_prev, f_next, plane_dist);
            dist_to_plane = fabs(plane.norm * vertex[index[i]] + plane.dist);
            dist_to_plane /= sqrt(qmod(plane.norm));
            printf("\t\t\tdist_to_plane(%d, plane) = %lf\n", index[i], dist);
            
            if (plane_dist < EPS_PARALL) {
                ifParallel = true;
                dist = dist_to_plane;
                printf("\t\t\tГрани %d и %d параллельны\n", f_prev_prev, f_next);
            } else {   
                ifParallel = false;
                intersection(plane0, plane1, plane2, intrsct);
                dist = (vertex[index[i]] - intrsct)*plane.norm;
                dist /= sqrt(qmod(plane.norm));
//                dist = qmod(vertex[index[i]] - intrsct);
//                dist = sqrt(dist);
                printf("\t\t\tdist = %lf\n", dist);
            }
            if (dist_to_plane < dist)
                dist = dist_to_plane;
            if ((min_i == -1 || dist < min_dist) && dist > 0.) {
                min_dist = dist;
                min_i = i;
                i_replace = ifParallel ? min_i : i_prev;
            }

            plane0 = facet[f_prev].plane;
            plane1 = facet[f_next].plane;
            plane2 = facet[f_next_next].plane;

//            printf("plane_%d : (%lf)*x + (%lf)*y + (%lf)*z + (%lf) = 0\n",
//                    f_prev, plane0.norm.x, plane0.norm.y, plane0.norm.z,
//                    plane0.dist);
//            printf("plane_%d : (%lf)*x + (%lf)*y + (%lf)*z + (%lf) = 0\n",
//                    f_next_next, plane2.norm.x, plane2.norm.y, plane2.norm.z,
//                    plane2.dist);
//            printf("plane : (%lf)*x + (%lf)*y + (%lf)*z + (%lf) = 0\n",
//                    plane.norm.x, plane.norm.y, plane.norm.z,
//                    plane.dist);
            
            plane0.norm.norm(1.);
            plane2.norm.norm(1.);
            if (plane0.norm * plane2.norm < 0.) {
                plane2.norm *= -1.;
                plane2.dist *= -1.;
            }
            
            plane_dist = qmod(plane0.norm - plane2.norm);
//            printf("dist_plane(%d, %d) = %lf\n", f_prev, f_next_next, plane_dist);
            dist_to_plane = fabs(plane.norm * vertex[index[i]] + plane.dist);
            dist_to_plane /= sqrt(qmod(plane.norm));
            printf("\t\t\tdist_to_plane(%d, plane) = %lf\n", index[i], dist);
            if (plane_dist < EPS_PARALL) {
                ifParallel = true;
                printf("\t\t\tГрани %d и %d параллельны\n", f_prev, f_next_next);
                dist = dist_to_plane;
            } else {   
                ifParallel = false;
                intersection(plane0, plane1, plane2, intrsct);
                dist = (vertex[index[i]] - intrsct)*plane.norm;
                dist /= sqrt(qmod(plane.norm));
//                dist = qmod(vertex[index[i]] - intrsct);
//                dist = sqrt(dist);
                printf("\t\t\tdist = %lf\n", dist);
            }
            if (dist_to_plane < dist)
                dist = dist_to_plane;
            if ((min_i == -1 || dist < min_dist) && dist > 0.) {
                min_dist = dist;
                min_i = i;
                i_replace = ifParallel ? min_i : i_next_next;
            }
        }
        printf("\tИтак:");
        printf("\t\tШагать нужно по %d вершине\n", min_i);
        printf("\t\tНа расстояние %lf\n", min_dist);
        printf("\t\tПричем эта вершина сольётся с вершиной %d\n", i_replace);
//        printf("min_i = %d, min_dist = %lf, i_replace = %d\n",
//                min_i, min_dist, i_replace);
        printf("\t2. Подъем контура на найденное расстояние\n");
//        printf("\tMoving ");
//        print_vertex(index[min_i]);

        if (min_dist > MAX_MIN_DIST) {
            printf("\t\tОшибка! Слишком большое расстояние\n");
            break;
//            i_replace = min_i;
//            dist = fabs(plane.norm * vertex[index[min_i]] + plane.dist);
//            dist /= sqrt(qmod(plane.norm));
//            min_dist = dist;
        }
            

        for (i = 0; i < nv; ++i) {
            printf("\t\t2.%d. Поднимается вершина %d\n", i, index[i]);
            if (sign_vertex[i] == 1) {
                printf("\t\t\tОна выше плоскости. Пропускаем.\n");
                continue;
            }
            if (i == min_i) {
                printf("\t\t\tВершину %d обработаем отдельно. Она ключевая.\n", index[i]);
                continue;
            }
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
                printf("\t\t\tВершина %d просто сдвигается\n", index[i]);
                //                    print_vertex(index[i]);
                vertex[index[i]] += dir;
                //                    sign_vertex[i] = signum(vertex[index[i]], plane);                    
                //                    print_vertex(index[i]);
            }// Добавляется новая точка
            else {
                printf("\t\t\tСоздаем новую точку для вершины %d\n", index[i]);
                //                    print_vertex(index[i]);
                point = vertex[index[i]] + dir;
                new_number = add_vertex(point);
                printf("\t\t\tДобавляем вершину %d после %d в грани %d\n", new_number, index[i], f_next);
                facet[f_prev].add_after_position(index[2 * nv + 1 + i_prev], new_number, f_next);
                printf("\t\t\tДобавляем вершину %d до %d в грани %d\n", new_number, index[i], f_prev);
                facet[f_next].add_before_position(index[2 * nv + 1 + i_next], new_number, f_prev);
                index[i] = new_number;
                //                    sign_vertex[i] = signum(vertex[index[i]], plane);                    
                print_vertex(index[i]);
            }
            //                if (i == i_replace)
            //                    v_replace = index[i];
        }
        // 3. Отдельно рассмотрим случай выбранной вершины. Эта вершина будет
        // удалена из списка.
        // 2011.10.01 : Исключение будет в том случае, если были параллельные 
        // вершины и номера заменяемой (min_i) и заменяющей (i_replace) точек 
        // совпадают...
        printf("\t3. Подъем ключевой вершины %d\n", index[min_i]);
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
                
        // Та самая исключительная ситуация :
        if (min_i == i_replace) {
            intersection(facet[f_prev].plane, facet[f_next].plane, dir, point);
            dir.norm(min_dist);
            printf("\t\tВершина %d просто сдвигается\n", index[i]);
            vertex[index[i]] += dir;            
        }
        // Точка сдвигается
        //            printf("f_prev = %d, f_next = %d, fid = %d\n", f_prev, f_next, fid);
        else if (fid == f_next) {
            printf("\t\t Вершина %d заменяется вершиной %d в грани %d\n", 
                    index[i], v_replace, f_prev);
            facet[f_prev].find_and_replace2(index[i], v_replace);
            printf("\t\t Вершина %d заменяется вершиной %d в грани %d\n", 
                    index[i], v_replace, f_next);
            facet[f_next].find_and_replace2(index[i], v_replace);
        }// Добавляется новая точка
        else {
            printf("\t\tДобавляется новая точка для вершины %d\n", index[i]);
            //                printf("\tVertex %d is followed by %d in facet %d and %d\n", index[i], v_replace, f_prev, f_next);
            //                printf("v_replace = %d\n", v_replace);
            printf("\t\tДобавляем вершину %d после %d в грани %d\n", 
                    v_replace, index[i], f_next);
            facet[f_prev].add_after_position(index[2 * nv + 1 + i_prev], v_replace, f_next);
            printf("\t\tДобавляем вершину %d до %d в грани %d\n", 
                    v_replace, index[i], f_prev);
            facet[f_next].add_before_position(index[2 * nv + 1 + i_next], v_replace, f_prev);
        }

        if (min_i != i_replace) {
            printf("\t\tУдаляем ключевую вершину %d\n", min_i);
            for (j = i; j < nv - 1; ++j)
                index[2 * nv + 1 + j] = index[2 * nv + 2 + j];
            for (j = i; j < 2 * nv - 2; ++j)
                index[nv + 1 + j] = index[nv + 2 + j];
            for (j = i; j < 3 * nv - 2; ++j)
                index[j] = index[j + 1];
            //            for (j = i; j < nv - 1; ++j)
            //                sign_vertex[j] = sign_vertex[j + 1];
            --nv;
            if (i == 0)
                index[nv] = index[0];
        }
        ndown_new = 0;
        for (i = 0; i < nv; ++i) {
            sign_vertex[i] = signum(vertex[index[i]], plane);
            if (sign_vertex[i] == -1)
                ++ndown_new;
        }
        printf("\tЗа шаг %d было поднято %d вершин\n", step, ndown - ndown_new);
        ndown = ndown_new;
    }
    printf("----- Алгоритм завершил работу. Ниже плоскости осталось %d вершин-----\n", 
            ndown);

    //V). Предобработка многогранника после поднятия
    printf("V). Предобработка многогранника после поднятия");
    first_facet = Facet(fid0, nv, plane, index, this, true);
//    printf("Cycle 3.\n");
    facet[fid0] = first_facet;
    facet[fid0].my_fprint_all(stdout);
    facet[fid1] = Facet();
    facet[fid1].my_fprint_all(stdout);
    preprocess_polyhedron();
    //    my_fprint(stdout);

    //    clear_unused();
    printf("VI). Рассечение многогранника плоскостью\n");
    intersect(-plane);
    printf("VII). Грани %d и %d объединены!\n", fid0, fid1);
    //    my_fprint(stdout);
    //    if (index != NULL)    это не нужно, т. к. index = first_facet.index
    //        delete[] index;   и поэтому удалится деструктором...
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

void Polyhedron::clear_unused() {
    int i, numf_used, numv_used, nf;
    int *index_facet, *index_vertex;

    printf("\tvertex analyse\n");

    index_vertex = new int[numv];
    numv_used = 0;
    for (i = 0; i < numv; ++i) {
        nf = vertexinfo[i].get_nf();
        printf("\tanalyze vertex %d, nf = %d\n", i, nf);
        if (nf > 0) {
            index_vertex[numv_used++] = i;
        }
    }

    printf("index_vertex : ");
    for (i = 0; i < numv_used; ++i)
        printf("%d ", index_vertex[i]);
    printf("\n");

    for (i = 0; i < numv_used; ++i) {
        if (index_vertex[i] != i) {
            printf("\treplacing vertex %d by vertex %d\n", i, index_vertex[i]);
            find_and_replace_vertex(index_vertex[i], i);
            vertex[i] = vertex[index_vertex[i]];
            vertexinfo[i] = vertexinfo[index_vertex[i]];
        }
    }
    numv = numv_used;

    index_facet = new int[numf];
    numf_used = 0;
    for (i = 0; i < numf; ++i) {
        if (facet[i].nv > 0) {
            index_facet[numf_used++] = i;
        }
    }

    printf("index_facet : ");
    for (i = 0; i < numf_used; ++i)
        printf("%d ", index_facet[i]);
    printf("\n");

    for (i = 0; i < numf_used; ++i) {
        if (index_facet[i] != i) {
            printf("\treplacing facet %d by facet %d\n", i, index_facet[i]);
            find_and_replace_facet(index_facet[i], i);
            facet[i] = facet[index_facet[i]];
        }
    }
    numf = numf_used;

    //    my_fprint(stdout);
    preprocess_polyhedron();

    if (index_vertex != NULL)
        delete[] index_vertex;
    if (index_facet != NULL)
        delete[] index_facet;
}

void Polyhedron::find_and_replace_vertex(int from, int to) {
    int i;
    for (i = 0; i < numf; ++i) {
        facet[i].find_and_replace_vertex(from, to);
    }
    for (i = 0; i < numv; ++i) {
        vertexinfo[i].find_and_replace_vertex(from, to);
    }
}

void Polyhedron::find_and_replace_facet(int from, int to) {
    int i;
    for (i = 0; i < numf; ++i) {
        facet[i].find_and_replace_facet(from, to);
    }
    for (i = 0; i < numv; ++i) {
        vertexinfo[i].find_and_replace_facet(from, to);
    }
}



