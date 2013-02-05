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

void Polyhedron::join_facets(int fid0, int fid1) {

    int nv;
    Plane plane;
    Facet join_facet;

    if (fid0 == fid1) {
        fprintf(stderr, "join_facets: Error. Cannot join facet with itself.\n");
        return;
    }

    // I ). Вычисление средней плоскости
    printf("I ). Вычисление средней плоскости\n");
    join_facets_calculate_plane(fid0, fid1, plane, nv);

    // II ). Составление списка вершин
    printf("II ). Составление списка вершин\n");
    join_facets_build_index(fid0, fid1, plane, join_facet, nv);
    if (nv == -1)
        return;

    // III ). Дополнительная предобработка многогранника
    printf("III ). Дополнительная предобработка многогранника\n");
    facet[fid0] = join_facet;
    facet[fid1] = Facet();
    preprocess_polyhedron();

    // IV ). Алгортм поднятия вершин, лежащих ниже плоскости
    printf("IV ). Алгортм поднятия вершин, лежащих ниже плоскости\n");
    join_facets_rise(fid0, fid1);

    // V ). Предобработка многогранника после поднятия
    printf("V ). Предобработка многогранника после поднятия\n");
    preprocess_polyhedron();

    // VI). Рассечение многогранника плоскостью
    printf("VI). Рассечение многогранника плоскостью\n");
    intersect(-plane);

    printf("=====================================================\n");
    printf("=========    Грани %d и %d объединены !!!   =========\n", fid0, fid1);
    printf("=====================================================\n");
}

void Polyhedron::join_facets_calculate_plane(int fid0, int fid1, Plane& plane, int& nv) {

    int i, j;
    int nv0, *index0;
    int nv1, *index1;
    int *index;
    bool *is;


    nv0 = facet[fid0].nv;
    nv1 = facet[fid1].nv;
    index0 = facet[fid0].index;
    index1 = facet[fid1].index;

    is = new bool[numv];
    for (i = 0; i < numv; ++i)
        is[i] = false;
    for (i = 0; i < nv0; ++i)
        is[index0[i]] = true;
    for (i = 0; i < nv1; ++i)
        is[index1[i]] = true;
    nv = 0;
    for (i = 0; i < numv; ++i)
        if (is[i])
            ++nv;
    index = new int[nv];
    for (i = 0, j = 0; i < numv; ++i)
        if (is[i])
            index[j++] = i;
    list_squares_method(nv, index, &plane);

    //Проверка, что нормаль построенной плокости направлена извне многогранника
    if (plane.norm * facet[fid0].plane.norm < 0 &&
            plane.norm * facet[fid1].plane.norm < 0) {
        plane.norm *= -1.;
        plane.dist *= -1.;
    }

    if (index != NULL)
        delete[] index;
    if (is != NULL)
        delete[] is;
}

void Polyhedron::join_facets_build_index(int fid0, int fid1, Plane plane, Facet& join_facet, int& nv) {

    int i, j;
    int *index, *index0, *index1, nv0, nv1;
    bool *is, *del;

    index = new int[3 * nv + 1];
    index0 = facet[fid0].index;
    index1 = facet[fid1].index;
    nv0 = facet[fid0].nv;
    nv1 = facet[fid1].nv;

    // 1. Найдем общую вершину граней
    for (i = 0; i < nv1; ++i)
        if (facet[fid0].find_vertex(index1[i]) != -1)
            break;
    if (i == nv1) {
        printf("join_facets : Error. Facets %d and %d have no common vertexes\n",
                fid0, fid1);
        nv = -1;
        return;
    }

    // 2. Найдем последнюю против часовой стрелки (для 1-й грани) общую точку граней
    i = facet[fid0].find_vertex(index1[i]);
    while (facet[fid1].find_vertex(index0[i]) != -1) {
        i = (nv0 + i + 1) % nv0;
    }

    // 3. Если эта точка висячая, то не добавляем ее
    i = (nv0 + i - 1) % nv0;
    j = 0;
    if (vertexinfo[index0[i]].get_nf() > 3)
        index[j++] = index0[i];

    // 4. Собираем все точки 1-й грани против часовой стрелки,
    //    пока не наткнемся на 2-ю грань
    i = (nv0 + i + 1) % nv0;
    while (facet[fid1].find_vertex(index0[i]) == -1) {
        index[j++] = index0[i];
        i = (nv0 + i + 1) % nv0;
    }

    // 5. Если крайняя точка не висячая, то добавляем ее
    //    i = (nv0 + i - 1) % nv0;
    if (vertexinfo[index0[i]].get_nf() > 3)
        index[j++] = index0[i];

    // 6. Собираем все точки 2-й грани против часовой стрелки, 
    // пока не наткнемся на 1-ю грань
    i = facet[fid1].find_vertex(index0[i]);
    i = (nv1 + i + 1) % nv1;
    while (facet[fid0].find_vertex(index1[i]) == -1) {
        index[j++] = index1[i];
        i = (nv1 + i + 1) % nv1;
    }
    nv = j;

    // 7. Общие и висячие вершины объединяемых граней удаляются из
    //    многогранника
    is = new bool[numv];
    del = new bool[numv];
    for (i = 0; i < numv; ++i) {
        is[i] = false;
        del[i] = false;
    }
    printf("Resulting index: ");
    for (i = 0; i < nv; ++i) {
        is[index[i]] = true;
        printf("%d ", index[i]);
    }
    printf("\n");


    for (i = 0; i < nv0; ++i)
        if (!is[index0[i]])
            del[index0[i]] = true;
    for (i = 0; i < nv1; ++i)
        if (!is[index1[i]])
            del[index1[i]] = true;
    for (i = 0; i < numv; ++i)
        if (del[i])
            delete_vertex_polyhedron(i);

    // 8. По построенному списку создается грань
    join_facet = Facet(fid0, nv, plane, index, this, true);

    if (is != NULL)
        delete[] is;
    if (del != NULL)
        delete[] del;
    if (index != NULL)
        delete[] index;
}

void Polyhedron::join_facets_rise(int fid0, int fid1) {

    int i;
    int nv, *index, *sign_vertex;
    int ndown;
    int step;
    int imin;


    Plane plane;

    nv = facet[fid0].nv;
    index = facet[fid0].index;
    plane = facet[fid0].plane;

    // 1 ). Посчитаем знаки вершин контура и количество вершин ниже грани
    sign_vertex = new int[nv];
    ndown = 0;
    for (i = 0; i < nv; ++i) {
        sign_vertex[i] = signum(vertex[index[i]], plane);
        if (sign_vertex[i] == -1)
            ++ndown;
    }

    // 2 ). Цикл по всем низлежащим вершинам
    step = 0;
    while (ndown > 0) {
        printf("-------------------------------------------------------------\n");
        printf("-----------------     Шаг  %d         -----------------------\n", step);
        printf("-------------------------------------------------------------\n");
        printf("ndown = %d\n", ndown);
        // 2. 1). Находим, по какой вершине нужно шагать, и минимальное расстояние
        printf("\t2. 1). Находим, по какой вершине нужно шагать, и минимальное расстояние\n");
        join_facets_rise_find(fid0, fid1, imin);

        if (imin == -1) {
            printf("join_facets_rise : Ошибка. Не удалось найти вершину (imin = -1)\n");
            return;
        }

        // 2. 2). Шагаем по выбранной вершине
        printf("\t2. 2). Шагаем по выбранной вершине\n");
        join_facets_rise_point(fid0, fid1, imin);
        
        if (test_structure() > 0) {
            printf("Не пройден тест на структуру!\n");
            return;
        }

        ++step;
        nv = facet[fid0].nv;
        index = facet[fid0].index;
        ndown = 0;
        for (i = 0; i < nv; ++i) {
            if (signum(vertex[index[i]], plane) == -1)
                ++ndown;
        }
        
        preprocess_polyhedron();
    }

    if (sign_vertex != NULL)
        delete[] sign_vertex;

}

void Polyhedron::join_facets_rise_find(int fid0, int fid1, int& imin) {
    int i;
    int nv, *index;

    double d, dmin;

    Plane plane;

    fid1 = fid1;

    nv = facet[fid0].nv;
    index = facet[fid0].index;
    plane = facet[fid0].plane;

    imin = -1;
    for (i = 0; i < nv; ++i) {
        printf("\t\t2. 1. %d ). Обработка %d-й вершины : ", i, index[i]);
        if (signum(vertex[index[i]], plane) == 1) {
            printf("выше плоскости\n");
            continue;
        }
        printf("ниже плоскости");
        //        printf("\n");
        join_facets_rise_find_step(fid0, fid1, i, d);
        if (d < 0) {
            //Если перемещение отдаляет точку от плоскости
            //?????????????????????????????
            continue;
        }
        if (dmin > d || imin == -1) {
            dmin = d;
            imin = i;
        }
    }
}

void Polyhedron::join_facets_rise_find_step(int fid0, int fid1, int i, double& d) {
    int nv, *index;
    int fl2, fl1, fr1, fr2;

    double d1, d2;


    Plane plane;
    Plane pl2, pl1, pr1, pr2;
    Vector3d v1, v2;

    fid1 = fid1;

    nv = facet[fid0].nv;
    index = facet[fid0].index;
    plane = facet[fid0].plane;

    // Номера соседних с контуром граней вблизи рассматриваемой точки:
    fl2 = index[nv + 1 + (nv + i - 2) % nv];
    fl1 = index[nv + 1 + (nv + i - 1) % nv];
    fr1 = index[nv + 1 + (nv + i) % nv];
    fr2 = index[nv + 1 + (nv + i + 1) % nv];

    pl2 = facet[fl2].plane;
    pl1 = facet[fl1].plane;
    pr1 = facet[fr1].plane;
    pr2 = facet[fr2].plane;

    // Найдем точку пересечения первой тройки граней

    if (qmod(pl2.norm % pr1.norm) < EPS_PARALL) {
        // Если грани параллельны: 
        intersection(plane, pl1, pr1, v1);
    } else {
        // Если грани не параллельны:
        intersection(pl2, pl1, pr1, v1);
        if (signum(v1, plane) == 1) {
            // Если точка пересечения лежит выше плоскости:
            intersection(plane, pl1, pr1, v1);
        }
    }
    // Найдем проекцию перемещения точки на нормаль плоскости:
    d1 = (v1 - vertex[index[i]]) * plane.norm;
    printf("\td1 = %lf", d1);

    // Найдем точку пересечения второй тройки граней

    if (qmod(pl1.norm % pr2.norm) < EPS_PARALL) {
        // Если грани параллельны: 
        intersection(plane, pl1, pr1, v2);
    } else {
        // Если грани не параллельны:
        intersection(pl1, pr1, pr2, v2);
        if (signum(v2, plane) == 1) {
            // Если точка пересечения лежит выше плоскости:
            intersection(plane, pl1, pr1, v2);
        }
    }
    // Найдем проекцию перемещения точки на нормаль плоскости:
    d2 = (v2 - vertex[index[i]]) * plane.norm;
    printf("\td2 = %lf", d2);
    printf("\n");

    d = (d1 < d2) ? d1 : d2;
}

void Polyhedron::join_facets_rise_point(int fid0, int fid1, int imin) {

    int nv, *index;
    int fl2, fl1, fr1, fr2;
    int irep;
    int ind_new;
    int pos, what;

    double d1, d2;

    Plane plane;
    Plane pl2, pl1, pr1, pr2;
    Vector3d v1, v2;

    bool ifjoin;
    bool deg_imin_big, deg_irep_big;
    int deg_imin, deg_irep;

    int nnv;

    fid1 = fid1;

    nv = facet[fid0].nv;
    index = facet[fid0].index;
    plane = facet[fid0].plane;

    // Номера соседних с контуром граней вблизи рассматриваемой точки:
    fl2 = index[nv + 1 + (nv + imin - 2) % nv];
    fl1 = index[nv + 1 + (nv + imin - 1) % nv];
    fr1 = index[nv + 1 + (nv + imin) % nv];
    fr2 = index[nv + 1 + (nv + imin + 1) % nv];

    printf("\t\tПоднимаем %d-ю вершину\n", index[imin]);
    printf("\t\t\tКартина такая: ---%d---%d---%d---\n",
            index[(nv + imin - 1) % nv],
            index[imin],
            index[(nv + imin + 1) % nv]);
    printf("\t\t\t                   |     |     |     \n");
    printf("\t\t\t               %d  | %d  | %d  | %d  \n",
            fl2, fl1, fr1, fr2);

    pl2 = facet[fl2].plane;
    pl1 = facet[fl1].plane;
    pr1 = facet[fr1].plane;
    pr2 = facet[fr2].plane;

    ifjoin = false; // Эта константа отвечает за то, сливаются ли 2 точки или нет

    // Найдем точку пересечения первой тройки граней

    if (qmod(pl2.norm % pr1.norm) < EPS_PARALL) {
        // Если грани параллельны: 
        intersection(plane, pl1, pr1, v1);
    } else {
        // Если грани не параллельны:
        intersection(pl2, pl1, pr1, v1);
        if (signum(v1, plane) == 1) {
            // Если точка пересечения лежит выше плоскости:
            intersection(plane, pl1, pr1, v1);
        } else {
            ifjoin = true;
        }
    }
    // Найдем проекцию перемещения точки на нормаль плоскости:
    d1 = (v1 - vertex[index[imin]]) * plane.norm;
    printf("\td1 = %lf", d1);

    // Найдем точку пересечения второй тройки граней

    if (qmod(pl1.norm % pr2.norm) < EPS_PARALL) {
        // Если грани параллельны: 
        intersection(plane, pl1, pr1, v2);
    } else {
        // Если грани не параллельны:
        intersection(pl1, pr1, pr2, v2);
        if (signum(v2, plane) == 1) {
            // Если точка пересечения лежит выше плоскости:
            intersection(plane, pl1, pr1, v2);
        } else {
            ifjoin = true;
        }
    }
    // Найдем проекцию перемещения точки на нормаль плоскости:
    d2 = (v2 - vertex[index[imin]]) * plane.norm;
    printf("\td2 = %lf\n", d2);

    if (d1 < d2 || (d1 > 0 && d2 <= 0)) {
        // Движение определяет левый сосед вершины
        if (ifjoin) {
            // Если нужно объединяtellteть вершины

            irep = (nv + imin - 1) % nv;
            deg_imin = vertexinfo[index[imin]].get_nf();
            deg_imin_big = deg_imin > 3;
            deg_irep = vertexinfo[index[irep]].get_nf();
            deg_irep_big = deg_irep > 3;
            printf("\tdeg_imin = deg(%d) = %d\n", index[imin], deg_imin);
            printf("\tdeg_irep = deg(%d) = %d\n", index[irep], deg_irep);

            if (deg_imin_big && deg_irep_big) {

                // Создать новую вершину с номером ind_new
                ind_new = add_vertex(v1);

                // Добавить v1 в грань fl2 с информацией
                nnv = facet[fl2].nv;

                what = ind_new;
                pos = index[2 * nv + 1 + (nv + irep - 1) % nv];
                facet[fl2].add(what, pos);

                what = fid0;
                pos += nnv + 1;
                facet[fl2].add(what, pos);

                what = -1;
                pos += nnv;
                facet[fl2].add(what, pos);

                pos = index[2 * nv + 1 + (nv + irep - 1) % nv];
                pos = (nnv + pos - 1) % nnv;
                facet[fl2].index[nnv + 1 + pos] = fl1;

                facet[fl2].index[facet[fl2].nv] = facet[fl2].index[0];

                //Добавить v1 в грань fl1 с информацией
                nnv = facet[fl1].nv;

                what = ind_new;
                pos = index[2 * nv + 1 + irep];
                facet[fl1].add(what, pos);

                what = fl2;
                pos += nnv + 1;
                facet[fl1].add(what, pos);

                what = -1;
                pos += nnv;
                facet[fl1].add(what, pos);

                pos = index[2 * nv + 1 + irep];
                pos = (nnv + pos - 1) % nnv;
                facet[fr1].index[nnv + 1 + pos] = fr1;

                facet[fl1].index[facet[fl1].nv] = facet[fl1].index[0];

                //Добавить v1 в грань fr1 с информацией
                nnv = facet[fr1].nv;

                what = ind_new;
                pos = index[2 * nv + 1 + imin];
                facet[fr1].add(what, pos);

                what = fl1;
                pos += nnv + 1;
                facet[fr1].add(what, pos);

                what = -1;
                pos += nnv;
                facet[fr1].add(what, pos);

                facet[fr1].index[facet[fr1].nv] = facet[fr1].index[0];

                //Заменить irep в грани fid0 на ind_new
                index[irep] = ind_new;
                index[irep + nv + 1] = fr1;
                // Удалить вершину imin из главной грани (fid0) и исправим грань                
                facet[fid0].remove(imin); // с информацией!!!

                facet[fid0].update_info();
                facet[fl2].update_info();
                facet[fl1].update_info();
                facet[fr1].update_info();

            } else if (deg_imin_big && !deg_irep_big) {
                vertex[index[irep]] = v1;

                //Добавить v1 в грань fr1 с информацией
                nnv = facet[fr1].nv;

                what = index[irep];
                pos = index[2 * nv + 1 + imin];
                facet[fr1].add(what, pos);

                what = fl1;
                pos += nnv + 1;
                facet[fr1].add(what, pos);

                what = -1;
                pos += nnv;
                facet[fr1].add(what, pos);

                facet[fr1].index[facet[fr1].nv] = facet[fr1].index[0];

                //Исправить грань fl1
                nnv = facet[fl1].nv;
                pos = index[2 * nv + 1 + irep];
                pos = (nnv + pos - 1) % nnv;
                facet[fr1].index[nnv + 1 + pos] = fr1;

                //Удалить imin из главной грани fid0 и исправим грань
                index[irep + nv + 1] = fr1;
                facet[fid0].remove(imin);

                facet[fid0].update_info();
                facet[fr1].update_info();

            } else if (!deg_imin_big && deg_irep_big) {

                vertex[index[imin]] = v1;

                // Добавить v1 в грань fl2 с информацией
                nnv = facet[fl2].nv;

                what = index[imin];
                pos = index[2 * nv + 1 + (nv + irep - 1) % nv];
                facet[fl2].add(what, pos);

                what = fid0;
                pos += nnv + 1;
                facet[fl2].add(what, pos);

                what = -1;
                pos += nnv;
                facet[fl2].add(what, pos);

                pos = index[2 * nv + 1 + (nv + irep - 1) % nv];
                pos = (nnv + pos - 1) % nnv;
                facet[fl2].index[nnv + 1 + pos] = fl1;

                facet[fl2].index[facet[fl2].nv] = facet[fl2].index[0];

                //Исправить грань fl1
                nnv = facet[fl1].nv;
                pos = index[2 * nv + 1 + irep];
                facet[fr1].index[nnv + 1 + pos] = fl2;

                //Удалить irep из главной грани fid0 и исправим грань
                index[irep] = imin;
                index[irep + nv + 1] = fr1;
                facet[fid0].remove(imin);

                facet[fid0].update_info();
                facet[fl2].update_info();

            } else if (!deg_imin_big && !deg_irep_big) {

                vertex[index[irep]] = v1;

                //Удалить вершину imin из грани fl1
                nnv = facet[fl1].nv;
                pos = index[2 * nv + 1 + irep];
                pos = (nnv + pos - 1) % nnv;
                facet[fl1].remove(pos);

                //Заменить вершину imin из грани fr1
                pos = index[2 * nv + 1 + imin];
                facet[fr1].index[pos] = index[irep];

                //Удалить imin из главной грани fid0
//                index[irep] = index[imin];
                index[irep + nv + 1] = fr1;
                facet[fid0].remove(imin);

                facet[fid0].update_info();
                facet[fl1].update_info();
            }
        } else {
            // Если не нужно объединять вершины
            vertex[index[imin]] = v1;
        }
    } else if (d2 > d1 || (d2 > 0 && d1 <= 0)) {
        // Движение определяет правый сосед вершины
        if (ifjoin) {
            // Если нужно объединять вершины

            irep = (nv + imin + 1) % nv;
            deg_imin = vertexinfo[index[imin]].get_nf();
            deg_imin_big = deg_imin > 3;
            deg_irep = vertexinfo[index[irep]].get_nf();
            deg_irep_big = deg_irep > 3;
            printf("\tdeg_imin = deg(%d) = %d\n", index[imin], deg_imin);
            printf("\tdeg_irep = deg(%d) = %d\n", index[irep], deg_irep);

            if (deg_imin_big && deg_irep_big) {

                // Создать новую вершину с номером ind_new
                ind_new = add_vertex(v2);

                // Добавить вершину v2 в грань fl1 после imin и исправить грань
                nnv = facet[fl1].nv;
                what = ind_new;
                pos = index[2 * nv + 1 + (nv + imin - 1) % nv];
                facet[fl1].add(what, pos);

                what = fid0;
                pos += nnv + 1;
                facet[fl1].add(what, pos);

                what = -1;
                pos += nnv;
                facet[fl1].add(what, pos);

                pos = index[2 * nv + 1 + (nv + imin - 1) % nv];
                pos = (nnv + pos - 1) % nv;
                facet[fl1].index[nnv + 1 + pos] = fr1;

                // Добавить вершину v2 в грань fr1 до imin и исправить грань
                nnv = facet[fr1].nv;
                what = ind_new;
                pos = index[2 * nv + 1 + imin];
                facet[fr1].add(what, pos);

                what = fl1;
                pos += nnv + 1;
                facet[fr1].add(what, pos);

                what = -1;
                pos += nnv;
                facet[fr1].add(what, pos);

                pos = index[2 * nv + 1 + imin];
                pos = (nnv + pos - 1) % nnv;
                facet[fr1].index[nnv + 1 + pos] = fr2;

                //Добавить вершину v2 в грань fr2 до irep и исправить грань
                nnv = facet[fr2].nv;
                what = ind_new;
                pos = index[2 * nv + 1 + irep];
                facet[fr2].add(what, pos);

                what = fr1;
                pos += nnv + 1;
                facet[fr2].add(what, pos);

                what = -1;
                pos += nnv;
                facet[fr2].add(what, pos);

                //Удалить imin из главной грани fid0 и заменить irep на v2; и исправить грань
                index[irep] = ind_new;
                facet[fid0].remove(imin);

                facet[fid0].update_info();
                facet[fl1].update_info();
                facet[fr1].update_info();
                facet[fr2].update_info();

            } else if (deg_imin_big && !deg_irep_big) {

                vertex[index[irep]] = v2;

                //Добавить вершину irep в грань fl1 после imin и исправить грань
                nnv = facet[fl1].nv;
                what = index[irep];
                pos = index[2 * nv + 1 + (nv + imin - 1) % nv];
                facet[fl1].add(what, pos);

                what = fid0;
                pos += nnv + 1;
                facet[fl1].add(what, pos);

                what = -1;
                pos += nnv;
                facet[fl1].add(what, pos);

                pos = index[2 * nv + 1 + (nv + imin - 1) % nv];
                pos = (nnv + pos - 1) % nnv;
                facet[fl1].index[nnv + 1 + pos] = fr1;

                // Исправить грань fr1
                nnv = facet[fr1].nv;
                pos = index[2 * nv + 1 + imin];
                pos = (nnv + pos - 1) % nnv;
                facet[fr1].index[nnv + 1 + pos] = fl1;

                // Удалить вршину imin из главной грани fid0 и исправить грань
                facet[fid0].remove(imin);

                facet[fid0].update_info();
                facet[fl1].update_info();

            } else if (!deg_imin_big && deg_irep_big) {

                vertex[index[imin]] = v2;

                //Добавить вершину v2 в грань fr2 до irep и исправить грань
                nnv = facet[fr2].nv;
                what = index[imin];
                pos = index[2 * nv + 1 + irep];
                facet[fr2].add(what, pos);

                what = fr1;
                pos += nnv + 1;
                facet[fr2].add(what, pos);

                what = -1;
                pos += nnv;
                facet[fr2].add(what, pos);

                // Исправить грань fr1
                nnv = facet[fr1].nv;
                pos = index[2 * nv + 1 + imin];
                pos = (nnv + pos - 1) % nnv;
                facet[fr1].index[nnv + 1 + pos] = fr2;

                // Удалить вершину irep из главной грани fid0 и исправить грань
                index[irep] = index[imin];
                facet[fid0].remove(imin);

                facet[fid0].update_info();
                facet[fr2].update_info();

            } else if (!deg_imin_big && !deg_irep_big) {

                vertex[index[irep]] = v2;

                //Заменить вершину imin из грани fl1 на вершину irep и исправить грань
                nnv = facet[fl1].nv;
                pos = index[2 * nv + 1 + (nv + imin - 1) % nv];
                pos = (nnv + pos - 1) % nnv;
                facet[fl1].index[pos] = index[irep];

                //Удалить вершину imin из грани fr1 и исправить грань
                nnv = facet[fr1].nv;
                pos = index[2 * nv + 1 + imin];
                facet[fr1].remove(pos);
                facet[fr1].index[nv + 1 + pos] = fl1;

                //Удалить вершину imin из главной грани fid0 и исправить грань
                facet[fid0].remove(imin);

                facet[fid0].update_info();
                facet[fr1].update_info();
            }
        } else {
            // Если не нужно объединять вершины
            vertex[index[imin]] = v2;
        }
    }

}


//void Polyhedron::join_create_first_facet(int fid0, int fid1) {
//
//    int nv0, nv1, nv, nv_common, *index0, *index1, *index;
//    int *sign_vertex, i, j, k, ndown, ndown_new;
//    int fid;
//
//    
//    //IV). Алгоритм поднятия вершин, лежащих ниже плоскости
//    printf("IV). Алгоритм поднятия вершин, лежащих ниже плоскости\n");
//
//    int i_prev_prev, i_prev, i_next, i_next_next;
//    int f_prev_prev, f_prev, f_next, f_next_next;
//    Vector3d intrsct;
//    Vector3d dir;
//    Vector3d point;
//    Plane plane0;
//    Plane plane1;
//    Plane plane2;
//
//    double min_dist;
//    double dist;
//    double dist_to_plane;
//    double min_coeff;
//    double coeff;
//    double coeff_to_plane;
//    int min_i, i_replace;
//    
//    int tmp0, tmp1, tmp2;
//    int new_number;
//    int v_replace;
//    
//    double plane_dist;
//    bool ifParallel;
//
//    int step = 0;
//    while (ndown > 0) {
//        printf("-------- Шаг %d ---------\n", step++);
//        printf("\tНиже плоскости находятся %d вершин контура\n", ndown);
//        printf("\t1.Находим, по какой вершине нужно шагать, и минимальное расстояние\n");
//        min_i = -1;
//        for (i = 0; i < nv; ++i) {
//            printf("\t\t1.%d. Обработка %d-й вершины : ", i, index[i]);
//            if (sign_vertex[i] == 1) {
//                printf("выше плоскости\n");
//                continue;
//            }
//            printf("ниже плоскости");
//            if (i == 0) {
//                i_prev_prev = nv - 2;
//                i_prev = nv - 1;
//                i_next = 0;
//                i_next_next = 1;
//            } else if (i == 1) {
//                i_prev_prev = nv - 1;
//                i_prev = 0;
//                i_next = 1;
//                i_next_next = 2;
//            } else if (i == nv - 1) {
//                i_prev_prev = nv - 3;
//                i_prev = nv - 2;
//                i_next = nv - 1;
//                i_next_next = 0;
//            } else {
//                i_prev_prev = i - 2;
//                i_prev = i - 1;
//                i_next = i;
//                i_next_next = i + 1;
//            }
//            f_prev_prev = index[nv + 1 + i_prev_prev];
//            f_prev = index[nv + 1 + i_prev];
//            f_next = index[nv + 1 + i_next];
//            f_next_next = index[nv + 1 + i_next_next];
//            printf("\t\t\tЗатрагиваются грани: %d, %d, %d, %d\n", 
//                    f_prev_prev, f_prev, f_next, f_next_next);
//            
//            plane0 = facet[f_prev_prev].plane;
//            plane1 = facet[f_prev].plane;
//            plane2 = facet[f_next].plane;
//
////            printf("plane_%d : (%lf)*x + (%lf)*y + (%lf)*z + (%lf) = 0\n",
////                    f_prev_prev, plane0.norm.x, plane0.norm.y, plane0.norm.z,
////                    plane0.dist);
////            printf("plane_%d : (%lf)*x + (%lf)*y + (%lf)*z + (%lf) = 0\n",
////                    f_next, plane2.norm.x, plane2.norm.y, plane2.norm.z,
////                    plane2.dist);
////            printf("plane : (%lf)*x + (%lf)*y + (%lf)*z + (%lf) = 0\n",
////                    plane.norm.x, plane.norm.y, plane.norm.z,
////                    plane.dist);
//
//            plane0.norm.norm(1.);
//            plane2.norm.norm(1.);
//            if (plane0.norm * plane2.norm < 0.) {
//                plane2.norm *= -1.;
//                plane2.dist *= -1.;
//            }
//            
//            plane_dist = qmod(plane0.norm - plane2.norm);
////            printf("dist_plane(%d, %d) = %lf\n", f_prev_prev, f_next, plane_dist);
//            intersection(facet[f_prev].plane, facet[f_next].plane, plane, intrsct);
//            dist_to_plane = (vertex[index[i]] - intrsct)*plane.norm;
//            dist_to_plane /= sqrt(qmod(plane.norm));
//            dist_to_plane = fabs(dist_to_plane);
//            coeff_to_plane = sqrt(qmod(intrsct - vertex[index[i]]));
//            
////            dist_to_plane = fabs(plane.norm * vertex[index[i]] + plane.dist);
////            dist_to_plane /= sqrt(qmod(plane.norm));
//            printf("\t\t\tdist_to_plane(%d, plane) = %lf\n", index[i], dist_to_plane);
//            
//            if (plane_dist < EPS_PARALL) {
//                ifParallel = true;
//                dist = dist_to_plane;
//                coeff = coeff_to_plane;
//                printf("\t\t\tГрани %d и %d параллельны\n", f_prev_prev, f_next);
//            } else {   
//                ifParallel = false;
//                intersection(plane0, plane1, plane2, intrsct);
//                dist = (vertex[index[i]] - intrsct)*plane.norm;
//                dist /= sqrt(qmod(plane.norm));
//                dist = fabs(dist);
//                coeff = sqrt(qmod(intrsct - vertex[index[i]]));
////                dist = qmod(vertex[index[i]] - intrsct);
////                dist = sqrt(dist);
//                printf("\t\t\tdist = %lf\n", dist);
//            }
//            if (dist_to_plane < dist) {
//                dist = dist_to_plane;
//                coeff = coeff_to_plane;
//            }
//            if ((min_i == -1 || dist < min_dist) && dist > 0.) {
//                min_dist = dist;
//                min_coeff = coeff;
//                min_i = i;
//                i_replace = ifParallel ? min_i : i_prev;
//            }
//
//            plane0 = facet[f_prev].plane;
//            plane1 = facet[f_next].plane;
//            plane2 = facet[f_next_next].plane;
//
////            printf("plane_%d : (%lf)*x + (%lf)*y + (%lf)*z + (%lf) = 0\n",
////                    f_prev, plane0.norm.x, plane0.norm.y, plane0.norm.z,
////                    plane0.dist);
////            printf("plane_%d : (%lf)*x + (%lf)*y + (%lf)*z + (%lf) = 0\n",
////                    f_next_next, plane2.norm.x, plane2.norm.y, plane2.norm.z,
////                    plane2.dist);
////            printf("plane : (%lf)*x + (%lf)*y + (%lf)*z + (%lf) = 0\n",
////                    plane.norm.x, plane.norm.y, plane.norm.z,
////                    plane.dist);
//            
//            plane0.norm.norm(1.);
//            plane2.norm.norm(1.);
//            if (plane0.norm * plane2.norm < 0.) {
//                plane2.norm *= -1.;
//                plane2.dist *= -1.;
//            }
//            
//            plane_dist = qmod(plane0.norm - plane2.norm);
////            printf("dist_plane(%d, %d) = %lf\n", f_prev, f_next_next, plane_dist);
////            dist_to_plane = fabs(plane.norm * vertex[index[i]] + plane.dist);
////            dist_to_plane /= sqrt(qmod(plane.norm));
////            printf("\t\t\tdist_to_plane(%d, plane) = %lf\n", index[i], dist);
//            if (plane_dist < EPS_PARALL) {
//                ifParallel = true;
//                printf("\t\t\tГрани %d и %d параллельны\n", f_prev, f_next_next);
//                dist = dist_to_plane;
//                coeff = coeff_to_plane;
//            } else {   
//                ifParallel = false;
//                intersection(plane0, plane1, plane2, intrsct);
//                dist = (vertex[index[i]] - intrsct)*plane.norm;
//                dist /= sqrt(qmod(plane.norm));
//                dist = fabs(dist);
//                coeff = sqrt(qmod(intrsct - vertex[index[i]]));
////                dist = qmod(vertex[index[i]] - intrsct);
////                dist = sqrt(dist);
//                printf("\t\t\tdist = %lf\n", dist);
//            }
//            if (dist_to_plane < dist) {
//                dist = dist_to_plane;
//                coeff = coeff_to_plane;
//            }
//            if ((min_i == -1 || dist < min_dist) && dist > 0.) {
//                min_dist = dist;
//                min_coeff = coeff;
//                min_i = i;
//                i_replace = ifParallel ? min_i : i_next_next;
//            }
//        }
//        printf("\tИтак:");
//        printf("\t\tШагать нужно по %d вершине\n", min_i);
//        printf("\t\tНа расстояние %lf\n", min_dist);
//        printf("\t\tПричем эта вершина сольётся с вершиной %d\n", i_replace);
////        printf("min_i = %d, min_dist = %lf, i_replace = %d\n",
////                min_i, min_dist, i_replace);
//        printf("\t2. Подъем контура на найденное расстояние\n");
////        printf("\tMoving ");
////        print_vertex(index[min_i]);
//
//        if (min_dist > MAX_MIN_DIST) {
//            printf("\t\tОшибка! Слишком большое расстояние\n");
//            break;
////            i_replace = min_i;
////            dist = fabs(plane.norm * vertex[index[min_i]] + plane.dist);
////            dist /= sqrt(qmod(plane.norm));
////            min_dist = dist;
//        }
//            
//
//        for (i = 0; i < nv; ++i) {
//            printf("\t\t2.%d. Поднимается вершина %d", i, index[i]);
//            if (sign_vertex[i] == 1) {
//                printf("\tОна выше плоскости. Пропускаем.\n");
//                continue;
//            }
//            if (i == min_i) {
//                printf("\tВершину %d обработаем отдельно. Она ключевая.\n", index[i]);
//                continue;
//            }
//            if (i != i_replace) {
//                printf("\tБУДЕМ ОБРАБАТЫВАТЬ ТОЛЬКО ЗАМЕНЯЮЩУЮ ВЕРШИНУ %d\n", index[i_replace]);
//                continue;
//                
//            }
//            if (i == 0) {
//                i_prev = nv - 1;
//                i_next = 0;
//            } else {
//                i_prev = i - 1;
//                i_next = i;
//            }
//            f_prev = index[nv + 1 + i_prev];
//            f_next = index[nv + 1 + i_next];
//            intersection(facet[f_prev].plane, facet[f_next].plane, dir, point);
//            dir.norm(min_coeff);
//
//            // 2 случая:
//            //                tmp0 = index[2 * nv + 1 + i] > 0 ? index[2 * nv + 1 + i] - 1 : facet[f_prev].nv - 1;
//            //                facet[f_prev].get_next_facet(tmp0, tmp1, fid, tmp2);
//            facet[f_prev].find_next_facet2(index[i], fid);
//
//            // Точка сдвигается
//            if (fid == f_next) {
//                printf("\tВершина %d просто сдвигается\n", index[i]);
//                //                    print_vertex(index[i]);
//                vertex[index[i]] += dir;
//                //                    sign_vertex[i] = signum(vertex[index[i]], plane);                    
//                //                    print_vertex(index[i]);
//            }// Добавляется новая точка
//            else {
//                printf("\tСоздаем новую точку для вершины %d\n", index[i]);
//                //                    print_vertex(index[i]);
//                point = vertex[index[i]] + dir;
//                new_number = add_vertex(point);
//                printf("\t\t\tДобавляем вершину %d после %d в грани %d\n", new_number, index[i], f_next);
//                facet[f_prev].add_after_position(index[2 * nv + 1 + i_prev], new_number, f_next);
//                printf("\t\t\tДобавляем вершину %d до %d в грани %d\n", new_number, index[i], f_prev);
//                facet[f_next].add_before_position(index[2 * nv + 1 + i_next], new_number, f_prev);
//                index[i] = new_number;
//                //                    sign_vertex[i] = signum(vertex[index[i]], plane);                    
//                print_vertex(index[i]);
//            }
//            //                if (i == i_replace)
//            //                    v_replace = index[i];
//        }
//        // 3. Отдельно рассмотрим случай выбранной вершины. Эта вершина будет
//        // удалена из списка.
//        // 2011.10.01 : Исключение будет в том случае, если были параллельные 
//        // вершины и номера заменяемой (min_i) и заменяющей (i_replace) точек 
//        // совпадают...
//        printf("\t3. Подъем ключевой вершины %d\n", index[min_i]);
//        v_replace = index[i_replace];
//        i = min_i;
//        if (i == 0) {
//            i_prev = nv - 1;
//            i_next = 0;
//        } else {
//            i_prev = i - 1;
//            i_next = i;
//        }
//        f_prev = index[nv + 1 + i_prev];
//        f_next = index[nv + 1 + i_next];
//
//        // 2 случая:
//        //            facet[f_prev].my_fprint_all(stdout);
//        //            tmp0 = index[2 * nv + 1 + i] > 0 ? index[2 * nv + 1 + i] - 1 : facet[f_prev].nv - 1;
//        //            facet[f_prev].get_next_facet(tmp0, tmp1, fid, tmp2);
//        //!2
//        facet[f_prev].find_next_facet2(index[i], fid);
//                
//        // Та самая исключительная ситуация :
//        if (min_i == i_replace) {
//            intersection(facet[f_prev].plane, facet[f_next].plane, dir, point);
//            dir.norm(min_coeff);
//            if (dir * plane.norm < 0)
//                dir *= -1.;
//            printf("\t\tВершина %d просто сдвигается\n", index[i]);
//            vertex[index[i]] += dir;            
//        }
//        // Точка сдвигается
//        //            printf("f_prev = %d, f_next = %d, fid = %d\n", f_prev, f_next, fid);
//        else if (fid == f_next) {
//            printf("\t\t Вершина %d заменяется вершиной %d в грани %d\n", 
//                    index[i], v_replace, f_prev);
//            facet[f_prev].find_and_replace2(index[i], v_replace);
//            printf("\t\t Вершина %d заменяется вершиной %d в грани %d\n", 
//                    index[i], v_replace, f_next);
//            facet[f_next].find_and_replace2(index[i], v_replace);
//        }// Добавляется новая точка
//        else {
//            printf("\t\tДобавляется новая точка для вершины %d\n", index[i]);
//            //                printf("\tVertex %d is followed by %d in facet %d and %d\n", index[i], v_replace, f_prev, f_next);
//            //                printf("v_replace = %d\n", v_replace);
//            printf("\t\tДобавляем вершину %d после %d в грани %d\n", 
//                    v_replace, index[i], f_next);
//            facet[f_prev].add_after_position(index[2 * nv + 1 + i_prev], v_replace, f_next);
//            printf("\t\tДобавляем вершину %d до %d в грани %d\n", 
//                    v_replace, index[i], f_prev);
//            facet[f_next].add_before_position(index[2 * nv + 1 + i_next], v_replace, f_prev);
//        }
//
//        if (min_i != i_replace) {
//            printf("\t\tУдаляем ключевую вершину %d\n", min_i);
//            for (j = i; j < nv - 1; ++j)
//                index[2 * nv + 1 + j] = index[2 * nv + 2 + j];
//            for (j = i; j < 2 * nv - 2; ++j)
//                index[nv + 1 + j] = index[nv + 2 + j];
//            for (j = i; j < 3 * nv - 2; ++j)
//                index[j] = index[j + 1];
//            //            for (j = i; j < nv - 1; ++j)
//            //                sign_vertex[j] = sign_vertex[j + 1];
//            --nv;
//            if (i == 0)
//                index[nv] = index[0];
//        }
//        ndown_new = 0;
//        for (i = 0; i < nv; ++i) {
//            sign_vertex[i] = signum(vertex[index[i]], plane);
//            if (sign_vertex[i] == -1)
//                ++ndown_new;
//        }
//        printf("\tЗа шаг %d было поднято %d вершин\n", step, ndown - ndown_new);
//        ndown = ndown_new;
//    }
//    printf("----- Алгоритм завершил работу. Ниже плоскости осталось %d вершин-----\n", 
//            ndown);
//
//    //V). Предобработка многогранника после поднятия
//    printf("V). Предобработка многогранника после поднятия");
//    first_facet = Facet(fid0, nv, plane, index, this, true);
////    printf("Cycle 3.\n");
//    facet[fid0] = first_facet;
//    facet[fid0].my_fprint_all(stdout);
//    facet[fid1] = Facet();
//    facet[fid1].my_fprint_all(stdout);
//    preprocess_polyhedron();
//    //    my_fprint(stdout);
//
//    //    clear_unused();
//    printf("VI). Рассечение многогранника плоскостью - ВКЛЮЧЕНО\n");
//    intersect(-plane);
//    printf("VII). Грани %d и %d объединены!\n", fid0, fid1);
//    //    my_fprint(stdout);
//    //    if (index != NULL)    это не нужно, т. к. index = first_facet.index
//    //        delete[] index;   и поэтому удалится деструктором...
//    if (sign_vertex != NULL)
//        delete[] sign_vertex;
//}

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


int Polyhedron::test_structure() {
    int i, res;
    res = 0;
    for (i = 0; i < numf; ++i) {
        res += facet[i].test_structure();
    }
    return res;
}