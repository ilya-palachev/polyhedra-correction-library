#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"
#include "array_operations.h"

void EdgeList::add_edge(int v0, int v1, int i0, int i1, double sm) {
    add_edge(v0, v1, i0, i1, -1, -2, sm);
}

void EdgeList::add_edge(int v0, int v1, int i0, int i1, int next_f, int next_d, double sm) {
#ifdef DEBUG1
    fprintf(stdout, "add_edge(v0 = %d, v1 = %d, next_f = %d, next_d = %d, \
			sm = %lf)\n", v0, v1, next_f, next_d, sm);
#endif

    if (v0 > v1) {
        int tmp = v0;
        v0 = v1;
        v1 = tmp;
    }

    int first = 0; // Первый элемент в массиве
    int last = num; // Последний элемент в массиве

    while (first < last) {
        int mid = (first + last) / 2;
        if (sm <= scalar_mult[mid]) {
            last = mid;
        } else {
            first = mid + 1;
        }
    }
    insert_int(edge0, num, last, v0);
    insert_int(edge1, num, last, v1);
    insert_int(ind0, num, last, i0);
    insert_int(ind1, num, last, i1);
    insert_int(next_facet, num, last, next_f);
    insert_int(next_direction, num, last, next_d);
    insert_double(scalar_mult, num, last, sm);
    insert_int(id_v_new, num, last, -1);
    insert_bool(isUsed, num, last, false);

    ++num;
    //	this->my_fprint(stdout);
}

void EdgeList::send(EdgeSet* edge_set) {
    int i;
    for (i = 0; i < num; ++i) {
        edge_set->add_edge(edge0[i], edge1[i]);
    }
}

void EdgeList::send_edges(EdgeSet* edge_set) {
    int i;
    for (i = 0; i < num; ++i) {
        if (edge0[i] != edge1[i])
            edge_set->add_edge(edge0[i], edge1[i]);
    }
}

void EdgeList::set_curr_info(int next_d, int next_f) {
    next_direction[pointer] = next_d;
    next_facet[pointer] = next_f;
}

void EdgeList::search_and_set_info(int v0, int v1, int next_d, int next_f) {
    int i, tmp;
    if (v0 > v1) {
        tmp = v0;
        v0 = v1;
        v1 = tmp;
    }
    for (i = 0; i < num; ++i) {
        if (edge0[i] == v0 && edge1[i] == v1) {
            next_direction[i] = next_d;
            next_facet[i] = next_f;
            return;
        }
    }
    fprintf(stdout,
            "EdgeList::search_and_set_info : Error. Edge (%d, %d) not found...\n",
            v0, v1);
    return;
}

void EdgeList::null_isUsed() {
    for (int i = 0; i < num; ++i) {
        isUsed[i] = false;
    }
}

//bool EdgeList::get_first_edge(int& v0, int& v1) {
//	if (num < 1) {
//		v0 = v1 = - 1;
//		return false;
//	}
//	for (int i = 0; i < num; ++i) {
//		if (next_direction[i] != 0 && isUsed[i] == false) {
//			v0 = edge0[i];
//			v1 = edge1[i];
////			set_pointer(i);
//			isUsed[i] = true;
//			return true;
//		}
//	}
//	v0 = v1 = - 1;
//	return false;
//}

void EdgeList::get_first_edge(int& v0, int& v1, int& next_f, int& next_d) {
    if (num < 1) {
        v0 = v1 = -1;
        return;
    }
    for (int i = 0; i < num; ++i) {
        if (next_direction[i] != 0 && isUsed[i] == false) {
            //			set_pointer(i);
            isUsed[i] = true;
            v0 = edge0[i];
            v1 = edge1[i];
            next_f = next_facet[i];
            next_d = next_direction[i];
            return;
        }
    }
    //	set_pointer(0);
    isUsed[0] = true;
    v0 = edge0[0];
    v1 = edge1[0];
    next_f = next_facet[0];
    next_d = -1;
}

void EdgeList::get_first_edge(int& v0, int& v1) {
    int next_f, next_d;
    get_first_edge(v0, v1, next_f, next_d);
}

void EdgeList::get_next_edge(Plane iplane, int& v0, int& v1, int& i0, int& i1, int& next_f, int& next_d) {
    int i, tmp, i_next;
    int id0;
    int id1;
    int sign0;
    int sign1;
    int incr;
    int nv;
    Plane plane;
    plane = poly->facet[next_f].plane;

    double err;
    err = qmod(plane.norm - iplane.norm) + (plane.dist - iplane.dist) * (plane.dist - iplane.dist);
    if (num < 1 && (fabs(err) > 1e-16)) {
        err = qmod(plane.norm + iplane.norm) + (plane.dist + iplane.dist) * (plane.dist + iplane.dist);
    }
    
    if (num < 1 && fabs(err) <= 0.0000000000000001) {
        fprintf(stdout, "\t\t --- SPECIAL FACET ---\n\n");
        
        sign0 = poly->signum(poly->vertex[v0], iplane);
        sign1 = poly->signum(poly->vertex[v1], iplane);
        if (1) {
            id0 = poly->facet[next_f].find_vertex(v0);
            id1 = poly->facet[next_f].find_vertex(v1);
            if (id0 == -1 || id1 == -1) {
                fprintf(stdout, "\tError: cannot find vertexes %d (%d) and %d (%d) facet %d.\n",
                        v0, id0, v1, id1, next_f);
                return;
            }
            nv = poly->facet[next_f].nv;
            if (sign0 >= 0 && sign1 <= 0) {
                if (id1 == id0 + 1 || (id1 == 0 && id0 == nv - 1))
                    incr = 1;
                else if (id1 == id0 - 1 || (id0 == 0 && id1 == nv - 1))
                    incr = -1;
                id0 = id1;
                v0 = v1;
                id1 = (id0 + incr + nv) % nv;
                v1 = poly->facet[next_f].index[id1];
            } else if (sign0 <= 0 && sign1 >= 0){
                if (id1 == id0 + 1 || (id1 == 0 && id0 == nv - 1))
                    incr = -1;
                else if (id1 == id0 - 1 || (id0 == 0 && id1 == nv - 1))
                    incr = 1;
                id0 = id0;
                v0 = v0;
                id1 = (id0 + incr + nv) % nv;
                v1 = poly->facet[next_f].index[id1];
            } else if (sign0 <= 0 && sign1 <= 0 && (next_d == -1 || next_d == 1)) {
                incr = next_d;
                if ((id1 - id0 + nv) % nv == next_d) {
                    id0 = id1;
                    v0 = v1;
                    id1 = (id0 + incr + nv) % nv;
                    v1 = poly->facet[next_f].index[id1];                
                } else {
                    id0 = id0;
                    v0 = v0;
                    id1 = (id0 + incr + nv) % nv;
                    v1 = poly->facet[next_f].index[id1];                
                }
            } else {
                fprintf(stdout, "Error. Cannot define the direction. sign(%d) = %d, sign(%d) = %d, drctn = %d\n",
                        v0, sign0, v1, sign1, next_d);
                return;
            }
            
            sign0 = poly->signum(poly->vertex[v0], iplane);
            sign1 = poly->signum(poly->vertex[v1], iplane);
            if (sign1 == 1) {
                //next_f = :
                poly->facet[next_f].find_next_facet(incr == 1 ? v0 : v1, next_f);
                next_d = 0;
            } else {
                next_f = next_f;
                next_d = incr;
            }
            
        }
        v1 = v0;
#ifdef DEBUG1
        fprintf(stdout, "\tRESULTING NEXT EDGE : %d %d - GO TO FACET %d\n",
                v0, v1, next_f);
#endif

        return;
    } else if (num < 1) {
        fprintf(stdout, "Error. num < 1,  err = %.16lf, if = %d\n", err, fabs(err) < 1e-16);
        return;
    }
        if (v0 > v1) {
            tmp = v0;
            v0 = v1;
            v1 = tmp;
        }
#ifdef DEBUG1
        fprintf(stdout,
                "EdgeList::get_next_edge %d\n",
                id);
//        this->my_fprint(stdout);
#endif
        for (i = 0; i < num; ++i) {
            //#ifdef DEBUG1
            //        fprintf(stdout,
            //                "   edge0[%d] = %d, edge1[%d] = %d\n",
            //                i, edge0[i], i, edge1[i]);
            //#endif
            if (edge0[i] == v0 && edge1[i] == v1) {
                //			set_pointer(i);
                isUsed[i] = true;
                switch (next_direction[i]) {
                    case 1:
                        i_next = i < num - 1 ? i + 1 : 0;
                        break;
                    case -1:
                        i_next = i > 0 ? i - 1 : num - 1;
                        break;
                    case 0:
                        switch (next_d) {
                            case 1:
                                i_next = i < num - 1 ? i + 1 : 0;
                                break;
                            case -1:
                                i_next = i > 0 ? i - 1 : num - 1;
                                break;
                            default:
                                break;
                        }
                        break;
                    default:
                        break;
                }
                v0 = edge0[i_next];
                v1 = edge1[i_next];
                i0 = ind0[i_next];
                i1 = ind1[i_next];
                next_f = next_facet[i_next];
                if (next_direction[i] != 0)
                    next_d = next_direction[i];
                isUsed[i_next] = true;
#ifdef DEBUG1
                fprintf(stdout, "\tRESULTING NEXT EDGE : %d %d - GO TO FACET %d\n",
                        v0, v1, next_f);
#endif

                return;
            }
        }
        //	set_pointer(0);
        v0 = v1 = next_f = -1;
        next_d = -2;
    }

    void EdgeList::get_next_edge(Plane iplane, int& v0, int& v1, int& next_f, int& next_d) {
        int i0, i1;
        get_next_edge(iplane, v0, v1, i0, i1, next_f, next_d);
    }

    int EdgeList::get_pointer() {
        return pointer;
    }

    void EdgeList::set_poly(Polyhedron* poly_orig) {
        poly = poly_orig;
    }
