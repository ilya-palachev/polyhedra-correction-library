#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"
#include "Vector3d.h"

int Facet::signum(int i, Plane plane) {
    return poly->signum(poly->vertex[index[i]], plane);
}

//#define IF_DETECT_INCIDENT_FACETS
//#define IF_NOT_PREPARE_HANGING_FACETS

void min3scalar(double s, double& s0, double& s1, double &s2,
        int i, int& i0, int& i1, int& i2) {

    if (i0 == -1) {
        s0 = s;
        i0 = i;
        return;
    }
    if (i1 == -1) {
        if (s < s0) {
            s1 = s0;
            i1 = i0;
            s0 = s;
            i0 = i;
        } else {
            s1 = s;
            i1 = i;
        }
        return;
    }
    if (i2 == -1) {
        if (s < s0) {
            s2 = s1;
            i2 = i1;
            s1 = s0;
            i1 = i0;
            s0 = s;
            i0 = i;
        } else if (s < s1) {
            s2 = s1;
            i2 = i1;
            s1 = s;
            i1 = i;
        } else {
            s2 = s;
            i2 = i;
        }
        return;
    }
    if (s < s0) {
        s2 = s1;
        i2 = i1;
        s1 = s0;
        i1 = i0;
        s0 = s;
        i0 = i;
    } else if (s < s1) {
        s2 = s1;
        i2 = i1;
        s1 = s;
        i1 = i;
    } else if (s < s2) {
        s2 = s;
        i2 = i;
    }
}

int sign(int i0, int i1, int i2) {
    bool num_inv = (i0 > i1) + (i0 > i2) + (i1 > i2);
    return 1 - 2 * num_inv;
}

int Facet::prepare_edge_list(Plane iplane) {
    int n_intrsct;
    int i, i_next, i_prev;
    int sign_curr, sign_next, sign_prev;
    double scalar;

    int i0, i1, i2;
    double s0, s1, s2;

    int i_step, i_curr, i_help;
    int up_down, in_out;
    int next_d, next_f;
    int ii;


    int n_positive;

    Vector3d dir;
    Vector3d point;
    Vector3d v_curr;
    Vector3d v_next;
    Vector3d v_intrsct;

    EdgeList* el = &(poly->edge_list[id]);
    
    printf("facet[%d].prepare_edge_list \n", id);

    // 1. Вычисляем напр вектор пересечения грани с плоскостью
    bool if_intersect = intersection(iplane, plane, dir, point);
    if (!if_intersect) {
//        printf("===Facet::prepare_edge_list : cannot prepare list for facet %d because planes are parallel or equal...\n", id);
        // Написано исключительно для программы слияния хорошо сопрягающихся граней.
        // Здесь предполагается, что в такой грани может быть нарушена плоскостность,
        // то есть вершины могут немного не лежать в плоскости грани.
        // В грани могут быть только отрицательные и нулевые вершины (этого мы добиваемся
        // в процессе поднятия вершин - см. функцию Polyhedron::join).
        
        printf("Facet::prepare_edge_list [%d]: special case\n", id);
        
        
        for (i = 0; i < nv; ++i) {
            i_next = (i + 1) % nv;
            i_prev = (nv + i - 1) % nv;
            sign_curr = signum(i, iplane);
            sign_next = signum(i_next, iplane);
            sign_prev = signum(i_prev, iplane);
            printf(" %d", sign_curr);
            
            if (sign_curr == 0 && sign_next == -1) {
                el->add_edge(index[i], index[i], i, i, index[nv + 1 + i], 1, (double)i);
            } else if (sign_curr == 0 && sign_next == 0) {
                el->add_edge(index[i], index[i], i, i, id, 1, (double)i);
            }
            
        }
        printf("\n");
        
        return 0;
    }

    // 2. Строим упорядоченный список вершин и ребер, пересекающихся с
    //    плоскостью
    n_intrsct = 0;
    n_positive = 0;
    i0 = i1 = i2 = -1;
    for (i = 0; i < nv; ++i) {

        i_next = (i + 1) % nv;
        i_prev = (nv + i - 1) % nv;
        sign_curr = signum(i, iplane);
        sign_next = signum(i_next, iplane);
        sign_prev = signum(i_prev, iplane);
        n_positive += sign_curr == 1;


        //Если вершина лежит на плоскости
        if (sign_curr == 0) {
            //Отбрасываем висячие вершины:
            if (sign_prev * sign_next == 1)
                continue;

            v_intrsct = poly->vertex[index[i]];
            scalar = dir * (v_intrsct - point);
            min3scalar(scalar, s0, s1, s2, i, i0, i1, i2);
            el->add_edge(index[i], index[i], i, i, scalar);
            ++n_intrsct;
        }

        //Если ребро пересекает плоскость
        if (sign_curr * sign_next == -1) {
            v_curr = poly->vertex[index[i]];
            v_next = poly->vertex[index[i_next]];
            intersection(iplane, v_next - v_curr, v_curr, v_intrsct);
            scalar = dir * (v_intrsct - point);
            min3scalar(scalar, s0, s1, s2, i, i0, i1, i2);
            el->add_edge(index[i], index[i_next], i, i_next, scalar);
            ++n_intrsct;
        }
    }
#ifdef DEBUG
    //	this->my_fprint_all(stdout);
    //	fprintf(stdout,
    //			"i0 = %d, s0 = %.2lf, i1 = %d, s1 = %.2lf, i2 = %d, s2 = %.2lf\n",
    //			i0, s0, i1, s1, i2, s2);
#endif

    //Утверждение. n_intrsct == 0 || n_intrsct > 1 , т. к. первый цикл отбрасывает
    //висячие вершины
    if (n_intrsct == 0) {
        return 0;
    }

    if (n_intrsct == 2) {
        printf("------For facet %d i_step = 1\n", id);
        i_step = 1;
    } else {
        //Утверждение. i0 != -1 && i1 != -1 && i2 != -1
        i_step = sign(i0, i1, i2);
        // Проверка, что начальным выбран задний конец ребра
        sign_curr = signum(i0, iplane);
        if (i_step == -1 && sign_curr != 0)
            i0 = (i0 + 1) % nv;
    }

    i_curr = i0;
    i_next = (i0 + i_step + nv) % nv;
    i_prev = (i0 - i_step + nv) % nv;

    sign_prev = signum(i_prev, iplane);
    up_down = sign_prev;
    in_out = -1;

    el->goto_header();
    bool ifFailed = false;

    for (i = 0, ii = 0; i < nv; ++i) {

        sign_curr = signum(i_curr, iplane);
        sign_next = signum(i_next, iplane);

        if (sign_curr == 0 || sign_curr * sign_next == -1) {
            if (sign_next == -up_down
                    || sign_curr == 0 && in_out == -1
                    || sign_curr * sign_next == -1 // 2011-04-04
                    ) {
                in_out *= -1;
                up_down *= -1;
                next_d = in_out;
                if (sign_curr == 0)
                    next_f = poly->vertexinfo[index[i_curr]].
                        intersection_find_next_facet(iplane, id);
                else {
                    i_help = i_step == 1 ? i_curr : i_next;
                    next_f = index[i_help + nv + 1];
                }
            } else {
                printf("-----For facet %d we have critical situation...\n", id);
                ifFailed = true;
                break;
//                next_d = 0;
//                next_f = id;
            }
            //			el->set_curr_info(next_d, next_f);
            //			if (++ii < n_intrsct)
            //				el->go_forward();
            if (sign_curr * sign_next == -1)
                el->search_and_set_info(index[i_curr], index[i_next],
                    next_d, next_f);
            else
                el->search_and_set_info(index[i_curr], index[i_curr],
                    next_d, next_f);
        }

        i_curr = i_next;
        i_next = (i_next + i_step + nv) % nv;
    }
    
    i_step *= -1;
    i_curr = i0;
    i_next = (i0 + i_step + nv) % nv;
    i_prev = (i0 - i_step + nv) % nv;

    sign_prev = signum(i_prev, iplane);
    up_down = sign_prev;
    in_out = -1;

    el->goto_header();

    for (i = 0, ii = 0; i < nv; ++i) {

        sign_curr = signum(i_curr, iplane);
        sign_next = signum(i_next, iplane);

        if (sign_curr == 0 || sign_curr * sign_next == -1) {
            if (sign_next == -up_down
                    || sign_curr == 0 && in_out == -1
                    || sign_curr * sign_next == -1 // 2011-04-04
                    ) {
                in_out *= -1;
                up_down *= -1;
                next_d = in_out;
                if (sign_curr == 0)
                    next_f = poly->vertexinfo[index[i_curr]].
                        intersection_find_next_facet(iplane, id);
                else {
                    i_help = i_step == 1 ? i_curr : i_next;
                    next_f = index[i_help + nv + 1];
                }
            } else {
                printf("-----For facet %d we have critical situation AGAIN!!!...\n", id);
                next_d = 0;
                next_f = id;
            }
            if (sign_curr * sign_next == -1)
                el->search_and_set_info(index[i_curr], index[i_next],
                    next_d, next_f);
            else
                el->search_and_set_info(index[i_curr], index[i_curr],
                    next_d, next_f);
        }

        i_curr = i_next;
        i_next = (i_next + i_step + nv) % nv;
    }

    return n_intrsct;
}

bool Facet::intersect(Plane iplane, FutureFacet& ff, int& n_components) {

    int i;
    int nintrsct;
    int i0, i1;
    int v0_first, v1_first;
    int v0, v1;
    int next_f, next_d;
    int pointer;
    int i_curr, i_next, i_prev;
    int sign_curr, sign_next, sign_prev;
    int sign0, sign1;
    int i_step;

    	fprintf(stdout, "**************Пересечение грани %d*************\n", id);
    
    
    double err;
    err = qmod(plane.norm - iplane.norm) + (plane.dist - iplane.dist) * (plane.dist - iplane.dist);
    if (fabs(err) > 1e-16) {
        err = qmod(plane.norm + iplane.norm) + (plane.dist + iplane.dist) * (plane.dist + iplane.dist);
    }
    if (fabs(err) < 1e-16) {
        return false;
//        return true; //2012-03-10
    }
    if (id == -1)
        return false;


    EdgeList* el = &(poly->edge_list[id]);
    FutureFacet curr_component(2 * nv);

    el->null_isUsed();
    //	el->my_fprint(stdout);

    n_components = 0;
    nintrsct = el->get_num();
    if (nintrsct == 0) {
        n_components = 0;
        for (i = 0; i < nv; ++i) {
            sign_curr = signum(i, iplane);
            if (sign_curr != 0)
                break;
        }
        if (sign_curr == 1) {
            return true;
        } else {
            return false;
        }
    }

    while (nintrsct > 0) {
        //		fprintf(stdout, "Поиск компоненты %d\n", n_components);
        el->get_first_edge(v0, v1);
        //		fprintf(stdout, "\tПервое ребро : %d %d\n", v0, v1);
        v0_first = v0;
        v1_first = v1;
        --nintrsct;

        do {
            next_d = 1;
            do {
                curr_component.add_edge(v0, v1, id);
                //				fprintf(stdout, "\tДобавлено ребро : %d %d\n", v0, v1);
                next_f = id; //2012-03-10
                el->get_next_edge(iplane, v0, v1, i0, i1, next_f, next_d);
                //				fprintf(stdout, "\t\t За ним следует ребро : %d %d\n", v0, v1);
                --nintrsct;
                if (i0 != i1)
                    break;
                i_next = (i0 + 1) % nv;
                i_prev = (nv + i0 - 1) % nv;
                sign_next = signum(i_next, iplane);
                sign_prev = signum(i_prev, iplane);
            } while (sign_prev == 0 && sign_next == 0);
            curr_component.add_edge(v0, v1, id);
            //			fprintf(stdout, "\tДобавлено ребро : %d %d\n", v0, v1);


            if (i0 == i1) {
                i_next = (i0 + 1) % nv;
                i_prev = (nv + i0 - 1) % nv;
                sign_next = signum(i_next, iplane);
                sign_prev = signum(i_prev, iplane);
                //Утверждение. sign_next != 0 || sign_prev != 0
                if (sign_next <= 0 && sign_prev <= 0) {
                    // Построение компоненты прекращается
                    v0 = v0_first;
                    v1 = v1_first;
                    curr_component.free();
                    continue;
                }
                //Утверждение. sign_next == 1 || sign_prev == 1
                //Утверждение. sign_next == 1 && sign_prev == 1 не может быть
                i_step = sign_next == 1 ? 1 : -1;
                i_curr = (i0 + i_step + nv) % nv;
                sign_curr = signum(i_curr, iplane);
            } else {
                sign0 = signum(i0, iplane);
                sign1 = signum(i1, iplane);
                //Утверждение. sign0 == 1 || sign1 == 1
                if (sign0 == 1) {
                    i_curr = i0;
                    sign_curr = sign0;
                    i_step = (i1 + 1) % nv == i0 ? 1 : -1;
                } else {
                    i_curr = i1;
                    sign_curr = sign0;
                    i_step = (i0 + 1) % nv == i1 ? 1 : -1;
                }
            }

            do {
                curr_component.add_edge(index[i_curr], index[i_curr], id);
                //				fprintf(stdout,
                //						"\tДобавлено ребро : %d %d\n",
                //						index[i_curr], index[i_curr]);
                i_curr = (i_curr + i_step + nv) % nv;
                sign_curr = signum(i_curr, iplane);
            } while (sign_curr == 1);
            //Утверждение. sign_curr == 0 || sign_curr == -1
            if (sign_curr == 0)
                v0 = v1 = index[i_curr];
            else {
                v0 = index[(i_curr - i_step + nv) % nv];
                v1 = index[i_curr];
                if (v0 > v1) {
                    int tmp = v0;
                    v0 = v1;
                    v1 = tmp;
                }
            }
        } while (!(v0 == v0_first && v1 == v1_first));


        if (curr_component.get_nv() > 0) {
            //			fprintf(stdout, "Facet %d. Component %d : \n", id, n_components);
            //			curr_component.my_fprint(stdout);
            curr_component.add_edge(-1, -1, id);
            ff += curr_component;
            curr_component.free();
            ++n_components;
        }
    }
    el->null_isUsed();
    return false;
}

void Facet::find_and_replace_vertex(int from, int to) {
    for (int i = 0; i < nv + 1; ++i)
        if (index[i] == from) {
            index[i] = to;
        }
}

void Facet::find_and_replace_facet(int from, int to) {
    for (int i = nv + 1; i < 2 * nv + 1; ++i)
        if (index[i] == from) {
            index[i] = to;
        }
}







