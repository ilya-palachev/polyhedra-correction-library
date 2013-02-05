#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Polyhedron.h"
#include "array_operations.h"

void EdgeList::add_edge(int v0, int v1, int next_f, int next_d, double sm) {
	//insert_binary(num, edge0, edge1, scalar_mult, v0, v1, sm);

#ifdef DEBUG1
	fprintf(stdout, "add_edge(v0 = %d, v1 = %d, next_f = %d, next_d = %d, \
			sm = %lf)\n", v0, v1, next_f, next_d, sm);
#endif

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
	insert_int(next_facet, num, last, next_f);
	insert_int(next_direction, num, last, next_d);
	insert_double(scalar_mult, num, last, sm);

	++num;
//	this->my_fprint(stdout);
}

void EdgeList::prepare_next_direction() {
	int drctn;
	int i;
	drctn = -1;
	for (i = 0; i < num; ++i) {
		if (next_direction[i] == 2) {
			next_direction[i] = drctn;
			drctn *= -1;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//         Устаревшая идея
//
//void EdgeList::get_next_edge(int& v0, int& v1) {
//	int i;
//	for (i = 0; i < num; ++i)
//		if (edge0[i] == v0 && edge1[i] == v1 || edge0[i] == v1 && edge1[i] == v0)
//			break;
//	if (i == num) {
//		fprintf(stdout, "EdgeList::get_next_edge : Error. Cannot find \
//			the edge (%d - %d) at EdgeList\n", v0, v1);
//		v0 = v1 = -1;
//	} else {
//		//По какую сторону брать следующее ребро - зависит от четности его
//		//номера в списке...
//		if (i % 2) {
//			v0 = edge0[i - 1];
//			v1 = edge1[i - 1];
//		} else {
//			v0 = edge0[i + 1];
//			v1 = edge1[i + 1];
//		}
//	}
//}
////////////////////////////////////////////////////////////////////////////////

