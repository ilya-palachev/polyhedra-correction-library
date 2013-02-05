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
	
	int n_positive;

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
			el->add_edge(index[i], index[i], scalar);
			++n_intrsct;
		}

		//Если ребро пересекает плоскость
		if (sign_curr * sign_next == -1) {
			v_curr = poly->vertex[index[i]];
			v_next = poly->vertex[index[i_next]];
			intersection(iplane, v_next - v_curr, v_curr, v_intrsct);
			scalar = dir * (v_intrsct - point);
			min3scalar(scalar, s0, s1, s2, i, i0, i1, i2);
			el->add_edge(index[i], index[i_next], scalar);
			++n_intrsct;
		}
	}
#ifdef DEBUG
	this->my_fprint_all(stdout);
	fprintf(stdout, 
			"i0 = %d, s0 = %.2lf, i1 = %d, s1 = %.2lf, i2 = %d, s2 = %.2lf\n",
			i0, s0, i1, s1, i2, s2);
#endif
	return n_intrsct;
}

bool Facet::intersection(
		Plane iplane,
		FutureFacet* ff,
		int& n_components,
		int n_intrsct, 
		int n_positive,
		int i0, int i1, int i2) {

	//return true => оставить грань
	//return false => удалить грань
	
	int i, i_next, i_prev;
	int sign_curr, sign_next, sign_prev;

	int i_step, i_curr, i_help;
	int up_down, in_out;
	int next_d, next_f;
	int ii;

	int nv_new;
	int* index_new;
	int i_new;

	int n_in_comp;
	int v0, v1, src_f, id_v;
	int n_components_0 = n_components;

	EdgeList* el = &(poly->edge_list[id]);

	if (n_intrsct == 0) {
		if (n_positive != nv)
			return false;
		else
			return true;
	}
	if (n_intrsct == 1) {
		//Утв. i0 != -1
		if (n_positive != nv - 1)
			return false;
		else {
			el->goto_header();
			next_d = 0;
			next_f = poly->vertexinfo[index[i0]].
				intersection_find_next_facet(iplane, id);
			el->set_curr_info(next_d, next_f);
			return true;
		}
	}

	if (n_intrsct == 2) {
		i_step = 1;
	} else {
		//Утв. i0 != -1 && i1 != -1 && i2 != -1
		i_step = sign(i0, i1, i2);
	}
	nv_new = n_intrsct + n_positive;
	index_new = new int[3 * nv_new + 1];

	i_step = sign(i0, i1, i2);
	// Проверка, что начальным выбран задний конец ребра
	sign_curr = signum(i0, iplane);
	if (i_step == -1 && sign_curr != 0)
		i0 = (i0 + 1) % nv;

	i_curr = i0;
	i_next = (i0 + i_step + nv) % nv;
	i_prev = (i0 - i_step + nv) % nv;

	sign_prev = signum(i_prev, iplane);
	up_down = sign_prev;
	in_out = -1;

	el->goto_header();
	i_new = 0;
	n_in_comp = 0;

	FutureFacet small(2);

	for (i = 0, ii = 0; i < nv; ++i) {

		sign_curr = signum(i_curr, iplane);
		sign_next = signum(i_next, iplane);

		//Утв. На первом шаге sign_curr == 0 || sign_curr * sign_next == -1
		if (sign_curr == 1) {
			if (i > 0) {
				//Утв. n_in_comp > 0
				ff->add_edge(index[i_curr], index[i_curr], id);
				++ n_in_comp;
			} else {
				//Утв. n_in_comp == 0
				small->add_edge(index[i_curr], index[i_curr], id);
			}
		}
		if (sign_curr == 1 && sign_next == - 1) {
			if (i > 0) {
				//Утв. n_in_comp > 0
				ff->add_edge(index[i_curr], index[i_next], id);
				ff->add_edge(- 1, - 1, id);
				n_in_comp = 0;
				++ n_components;
			} else {
				//Утв. n_in_comp == 0
				small->add_edge(index[i_curr], index[i_next], id);
			}
		}
		if (sign_curr == - 1 && sign_next == 1) {
			//Утв. n_in_comp == 0
			ff->add_edge(index[i_curr], index[i_next], id);
			++ n_in_comp;
		}
		if (sign_curr == 0) {
			if (n_in_comp == 0 && sign_next == 1) {
				if (sign_next == 1) {
					ff->add_edge(index[i_curr], index[i_curr], id);
					++ n_in_comp;
				} else if (i == 0) {
					//sign_prev == 1
					small->add_edge(index[i_curr], index[i_curr], id);
					++ n_in_comp;
				}
			} else if (n_in_comp > 1) {
				ff->add_edge(index[i_curr], index[i_curr], id);
				ff->add_edge(- 1, - 1, id);
				n_in_comp = 0;
				++n_components;
			}
		}

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
			if (++ii < n_intrsct)
				el->go_forward();
		}

		i_curr = i_next;
		i_next = (i_next + i_step + nv) % nv;
	}

	int v0, v1, src_f, id_v;
	if (sign_curr == 1 && sign_next != - 1) {
		if (small.get_nv() == 1) {
			small.get_edge(0, v0, v1, src_f, id_v);
			ff->add_edge(v0, v1, id);
		} else if(small.get_nv() == 2) {
			small.get_edge(0, v0, v1, src_f, id_v);
			ff->add_edge(v0, v1, id);
			small.get_edge(1, v0, v1, src_f, id_v);
			ff->add_edge(v0, v1, id);
		}
		ff->add_edge(- 1, - 1, id);
			++ n_components;
	}

	//debug
	fprintf(stdout, "facet %d : index_new : ", id);
	for (i = 0; i < i_new; ++i)
		fprintf(stdout, "%d ", index_new[i]);
	fprintf(stdout, "\n");

}