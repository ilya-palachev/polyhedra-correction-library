#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"

int Facet::signum(int i, Plane plane) {
	return signum(poly->vertex[index[i]], plane);
}

bool Facet::prepare_edge_list(Plane iplane) {
	int nintrsct;
	int i, i_next, i_prev;
	int sign_curr, sign_prev, sign_next;
	double scalar;
	int next_f;

	Vector3d dir;
	Vector3d point;
	Vector3d v0;
	Vector3d v1;
	Vector3d v_intrsct;

	//Вычисляем напр вектор пересечения грани с плоскостью
	bool if_intersect = intersection(iplane, plane, dir, point);
	if (!if_intersect)
		return false;

	nintrsct = 0;
	for (i = 0; i < nv; ++i) {
		i_prev = (nv + i - 1) % nv;
		i_next = (i + 1) % nv;

		sign_curr = signum(i, iplane);
		sign_next = signum(i_next, iplane);
		sign_prev = signum(i_prev, iplane);

		//Если вершина лежит на плоскости
		if (sign_curr == 0) {
			if (sign_prev == 0 && sign_next == 0) {
				//Вершина лежит внутри лежащего на плоскости ребра
				continue;
			}
			//Шагаем вперед или назад, чтобы выяснить, является ли вершина
			//вершиной входа, вершиной выхода или "висячей" вершиной
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
				//"Висячая" вершина
				continue;
			}

			v_intrsct = poly->vertex[index[i]];
			scalar = dir * (v_intrsct - point);
			//TODO: Перенести сюда нахождение следующей плоскости в этом случае
			//
			//      next_f = ?
			//
			edge_list.add_edge(index[i], index[i], next_f, scalar);
			++nintrsct;
		}

		//Если ребро пересекает плоскость
		if (sign_curr * sign_next == -1) {
			v0 = poly->vertex[index[i]];
			v1 = poly->vertex[index[i_next]];
			intersection(iplane, v1 - v0, v0, v_intrsct);
			scalar = dir * (v_intrsct - point);
			next_f = index[i + nv + 1];
			edge_list.add_edge(index[i], index[i_next], next_f,  scalar);
			++nintrsct;
		}
	}

	return nintrsct > 0;
}

