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
	insert_int(next_facet, num, last, next_f);
	insert_int(next_direction, num, last, next_d);
	insert_double(scalar_mult, num, last, sm);
	insert_int(id_v_new, num, last, -1);
	insert_bool(isUsed, num, last, false);

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

void EdgeList::get_next_edge(int& v0, int& v1, int& next_f, int& next_d) {
	int i, tmp, i_next;
	if (num < 1) {
		v0 = v1 = -1;
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
	this->my_fprint(stdout);
#endif
	for (i = 0; i < num; ++i) {
#ifdef DEBUG1
	fprintf(stdout,
			"   edge0[%d] = %d, edge1[%d] = %d\n",
			i, edge0[i], i, edge1[i]);
#endif
		if (edge0[i] == v0 && edge1[i] == v1) {
//			set_pointer(i);
			isUsed[i] = true;
			switch (next_direction[i]) {
				case -1:
					i_next = i < num - 1 ? i + 1 : 0;
					break;
				case 1:
					i_next = i > 0 ? i - 1 : num - 1;
					break;
				case 0:
					switch (next_d) {
						case -1:
							i_next = i < num - 1 ? i + 1 : 0;
							break;
						case 1:
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
			next_f = next_facet[i_next];
			if (next_direction[i] != 0)
				next_d = next_direction[i];
			isUsed[i_next] = true;

			return;
		}
	}
//	set_pointer(0);
	v0 = v1 = next_f = -1;
	next_d = -2;
}

void EdgeList::set_id_v_new(int id_v) {
	id_v_new[pointer] = id_v;
}

void EdgeList::set_isUsed(bool val) {
#ifdef DEBUG
	fprintf(stdout,
			"EdgeList[%d].isUsed[%d] = %d\n",
			id, pointer, val);
#endif
	isUsed[pointer] = val;
}

void EdgeList::set_pointer(int val) {
	if (val < 0 || val > num - 1) {
		fprintf(stdout,
				"EdgeList::set_pointer : Error. num = %d, val = %d",
				num, val);
		return;
	}
#ifdef DEBUG
	fprintf(stdout,
			"EdgeList[%d].set_pointer(%d)\n",
			id, val);
#endif
	pointer = val;
}

void EdgeList::send_to_edge_set(EdgeSet* edge_set) {
	int i;
	for (i = 0; i < num; ++i) {
		edge_set->add_edge(edge0[i], edge1[i]);
	}
}