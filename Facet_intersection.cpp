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
	int nintrsct;
	int i, i_next, i_prev;
	int sign_curr, sign_next, sign_prev;
	int edge0, edge1;
	double scalar;

	int i0, i1, i2;
	double s0, s1, s2;
	int i_step, i_curr, i_help;
	int up_down, in_out;
	int next_d, next_f;
	int ii;

	int n_positive, nv_new;
	int* index_new;

	Vector3d dir;
	Vector3d point;
	Vector3d v_curr;
	Vector3d v_next;
	Vector3d v_intrsct;

	EdgeList* el = &(poly->edge_list[id]);

	// 1. Вычисляем напр вектор пересечения грани с плоскостью
	bool if_intersect = intersection(iplane, plane, dir, point);
	if (!if_intersect) {
		return 0;
	}

	// 2. Строим упорядоченный список вершин и ребер, пересекающихся с
	//    плоскостью
	nintrsct = 0;
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
			el->add_edge(index[i], index[i], scalar);
			++nintrsct;
		}

		//Если ребро пересекает плоскость
		if (sign_curr * sign_next == -1) {
			v_curr = poly->vertex[index[i]];
			v_next = poly->vertex[index[i_next]];
			intersection(iplane, v_next - v_curr, v_curr, v_intrsct);
			scalar = dir * (v_intrsct - point);
			min3scalar(scalar, s0, s1, s2, i, i0, i1, i2);
			el->add_edge(index[i], index[i_next], scalar);
			++nintrsct;
		}
	}
#ifdef DEBUG
	this->my_fprint_all(stdout);
	fprintf(stdout, 
			"i0 = %d, s0 = %.2lf, i1 = %d, s1 = %.2lf, i2 = %d, s2 = %.2lf\n",
			i0, s0, i1, s1, i2, s2);
#endif
	if (nintrsct == 0) {
		return 0;
	}
	if (nintrsct == 1) {
		//Утв. i0 != -1
		el->goto_header();
		next_d = 0;
		next_f = poly->vertexinfo[index[i0]].
			intersection_find_next_facet(iplane, id);
		el->set_curr_info(next_d, next_f);
		return 1;
	}
//	if (nintrsct == 0) {
//		if (n_positive != nv) {
//			nv = 0;
//			if (index != NULL)
//				delete[] index;
//		}
//		return 0;
//	}
//	if (nintrsct == 1) {
//		//Утв. i0 != -1
//		el->goto_header();
//		next_d = 0;
//		next_f = poly->vertexinfo[index[i0]].
//			intersection_find_next_facet(iplane, id);
//		el->set_curr_info(next_d, next_f);
//
//		if (n_positive != nv - 1) {
//			nv = 1;
//			index_new = new int[4];
//			index_new[0] = index_new[1] = index[i0];
//			index_new[2] = index_new[3] = -1;
//			if (index != NULL)
//				delete[] index;
//			index = index_new;
//		}
//		return 1;
//	}


	if (nintrsct == 2) {
		//Утв. i0 != -1 && i1 != -1
		el->goto_header();
		sign_curr = signum(i0, iplane);
		next_d = 1;
		if (sign_curr == 0) {
			next_f = poly->vertexinfo[index[i0]].
					intersection_find_next_facet(iplane, id);
		} else {
			next_f = index[i0 + nv + 1];
		}
		el->set_curr_info(next_d, next_f);
		el->go_forward();
		sign_curr = signum(i1, iplane);
		next_d = -1;
		if (sign_curr == 0) {
			next_f = poly->vertexinfo[index[i1]].
					intersection_find_next_facet(iplane, id);
		} else {
			next_f = index[i1 + nv + 1];
		}
		el->set_curr_info(next_d, next_f);
		return 2;
	}
//	if (nintrsct == 2) {
//		i_step = 1;
//	} else {
//		i_step = sign(i0, i1, i2);
//	}
//	nv_new = nintrsct + n_positive;
//	index_new = new int[3 * nv_new + 1];

	//Утв. i0 != -1 && i1 != -1 && i2 != -1
	i_step = sign(i0, i1, i2);
	// Проверка, что начальным выбран задний конец ребра
	sign_curr = signum(i0, iplane);
	if (i_step == -1 && sign_curr != 0)
		i0 = (i0 + 1) % nv;

	i_curr = i0;
	i_next = (i0 + i_step + nv) % nv;
	i_prev = (i0 - i_step + nv) % nv;

//Старый вариант проверки:
//	sign_curr = signum(i_curr, iplane);
//	sign_next = signum(i_next, iplane);
//	if (sign_curr != 0) {
//		if (sign_curr * sign_next != -1) {
//			i_next = i_curr;
//			i_curr = i_prev;
//			i_prev = (i_curr - i_step + nv) % nv;
//		}
//	}

	sign_prev = signum(i_prev, iplane);
	up_down = sign_prev;
	in_out = -1;

	el->goto_header();
//	int i_new = 0;
	for (i = 0, ii = 0; i < nv; ++i) {

		sign_curr = signum(i_curr, iplane);
		sign_next = signum(i_next, iplane);

//		if (sign_curr != -1)
//			index_new[i_new ++] = index[i_curr];
		if (sign_curr == 0 || sign_curr * sign_next == -1) {
			if (sign_next == - up_down || 
					sign_curr == 0 && in_out == -1) {
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
				next_d = 0;
				next_f = id;
			}
			el->set_curr_info(next_d, next_f);
			if (++ii < nintrsct)
				el->go_forward();
			else
				break;
		}

		i_curr = i_next;
		i_next = (i_next + i_step + nv) % nv;

	}

//	el.goto_header();
//	el.get_curr_edge(int& edge0, int& edge1);
//	for (i = 0; i < nv; ++i)
//		if (index[i] == edge0)
//			break;
//	if (edge0 != edge1) {
//		i_next = (i + 1) % nv;
//		i_prev = (nv + i - 1) % nv;
//		if (index[i_next] == edge1) {
//			++i;
//		} else if (index[i_prev] == edge1) {
//			i = i;
//		} else {
//			fprintf(stdout,
//					"Facet::prepare_egde_list : Error 1.\n");
//		}
//	}

	return nintrsct;
}

