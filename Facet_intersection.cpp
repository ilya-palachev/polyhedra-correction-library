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
#define IF_NOT_PREPARE_HANGING_FACETS

int Facet::prepare_edge_list(Plane iplane) {
	int nintrsct;
	int i, i_next, i_prev;
	int sign_curr, sign_prev, sign_next;
	double scalar;
	int next_f;
	int num_null;
	int num_positive;
	int num_negative;

	Vector3d dir;
	Vector3d point;
	Vector3d v0;
	Vector3d v1;
	Vector3d v_intrsct;

	//Вычисляем напр вектор пересечения грани с плоскостью
	bool if_intersect = intersection(iplane, plane, dir, point);
	if (!if_intersect) {
#ifdef IF_DETECT_INCIDENT_FACETS
		const Vector3d v = plane.norm % iplane.norm;
		const double s1 = v * v;
		const double s2 = plane.dist - iplane.dist;
		if ( s1 + s2 * s2 < 1e-16) {
			for (i = 0; i < nv; ++i) {
				poly->edge_list[id].add_edge(index[i], index[i], id, 0, scalar);
			}
			return nv;
		} else {
//			fprintf(stdout,
//					"Facet::prepare_edge_list : Error if_intersect = false\n");
			return 0;
		}
#endif
		return 0;
	}

	nintrsct = 0;
	num_null = num_positive = num_negative = 0;
	for (i = 0; i < nv; ++i) {
#ifdef DEBUG1
		fprintf(stdout, "prepare_edge_list i = %d\n", i);
#endif

		i_prev = (nv + i - 1) % nv;
		i_next = (i + 1) % nv;

		sign_curr = signum(i, iplane);
		sign_next = signum(i_next, iplane);
		sign_prev = signum(i_prev, iplane);

		switch (sign_curr) {
			case 0 :
				++num_null;
				break;
			case 1 :
				++num_positive;
				break;
			case -1 :
				++num_negative;
				break;
			default :
				break;
		}

		//Если вершина лежит на плоскости
		if (sign_curr == 0) {
			//Нулевую вершину нельзя терять!!!

			v_intrsct = poly->vertex[index[i]];
			scalar = dir * (v_intrsct - point);

			if (sign_prev != 0) {
				while (sign_next == 0) {
					i_next = (i_next + 1) % nv;
					sign_next = signum(i_next, iplane);
				}
			}

			if (sign_next != 0) {
				while (sign_prev == 0) {
					i_prev = (nv + i_prev - 1) % nv;
					sign_prev = signum(i_prev, iplane);
				}
			}

			if (sign_prev == sign_next) {
				// Вершина лежит между лежащими на плоскости вершинами
				// (если sign_prev == sign_next == 0)
				// или является висячей
				// (если sign_prev == sign_next != 0 для непосредственных
				// соседей)
				// или лежит на висячем отрезке
				// (если sign_prev == sign_next != 0 для тех ближайших к ней
				// по списку вершин, которые не лежат на плоскости
				// соседей)

				poly->edge_list[id].add_edge(index[i], index[i], id, 0, scalar);
				continue;
			}


			//Находим следующую плоскость
			next_f = poly->vertexinfo[index[i]].
					intersection_find_next_facet(iplane, id);
			if (next_f == -1) {
				fprintf(stdout,
						"Facet::prepare_edge_list : Error. Vertex step. \
						next_f = -1 (id = %d)\n", id);
				break;
			}

			poly->edge_list[id].
				add_edge(index[i], index[i], next_f, 2, scalar);
			++nintrsct;
		}

		//Если ребро пересекает плоскость
		if (sign_curr * sign_next == -1) {
			v0 = poly->vertex[index[i]];
			v1 = poly->vertex[index[i_next]];
			intersection(iplane, v1 - v0, v0, v_intrsct);
			scalar = dir * (v_intrsct - point);
			next_f = index[i + nv + 1];
			poly->edge_list[id].
				add_edge(index[i], index[i_next], next_f, 2, scalar);
			++nintrsct;
		}
	}
//	edge_list.my_fprint(stdout);
	poly->edge_list[id].prepare_next_direction();

//	fprintf(stdout, "num_not_in_plane = %d\n", num_not_in_plane);

#ifdef IF_NOT_PREPARE_HANGING_FACETS
	if (num_positive + num_null == nv || num_negative + num_null == nv) {
		poly->edge_list[id] = EdgeList();
		return 0;
	}
#endif
	return nintrsct;
}

