#include "Polyhedron.h"

EdgeSet::EdgeSet() :
		len(0),
		num(0),
		ind(NULL),
		edge0(NULL),
		edge1(NULL)
{}

EdgeSet::EdgeSet(int len_orig) :
		len(len_orig),
		num(0),
		ind(new int[len]),
		edge0(new int[len]),
		edge1(new int[len])
{}

EdgeSet::EdgeSet(const EdgeSet& orig) :
		len(orig.len),
		num(orig.num),
		ind(new int[len]),
		edge0(new int[len]),
		edge1(new int[len]) {
	for (int i; i < num; ++i) {
		ind[i] = orig.ind[i];
		edge0[i] = orig.edge0[i];
		edge1[i] = orig.edge1[i];
	}
}

EdgeSet::~EdgeSet() {
	if (ind != NULL)
		delete[] ind;
	if (edge0 != NULL)
		delete[] edge0;
	if (edge1 != NULL)
		delete[] edge1;
}

EdgeSet& EdgeSet::operator =(const EdgeSet& orig) {
	len = orig.len;
	num = orig.num;

	if (ind != NULL)
		delete[] ind;
	if (edge0 != NULL)
		delete[] edge0;
	if (edge1 != NULL)
		delete[] edge1;

	ind = new int[len];
	edge0 = new int[len];
	edge1 = new int[len];

	for (int i; i < num; ++i) {
		ind[i] = orig.ind[i];
		edge0[i] = orig.edge0[i];
		edge1[i] = orig.edge1[i];
	}
}

int EdgeSet::get_len() {
	return len;
}

int EdgeSet::get_num() {
	return num;
}

void EdgeSet::get_edge(int id, int& v0, int& v1) {
	if (id < 0 || id > num - 1) {
		v0 = -1;
		v1 = -1;
		fprintf(stdout,
				"EdgeSet::get_edge : Unavailable id = %d, num = %d",
				id, num);
		return;
	}
	v0 = edge0[ind[id]];
	v1 = edge1[ind[id]];
}

int EdgeSet::search_edge(int v0, int v1) {

	int tmp, first, last, mid;
	int i;

	if (v0 > v1) {
		tmp = v0;
		v0 = v1;
		v1 = tmp;
	}

	first = 0; // Первый элемент в массиве
	last = num; // Последний элемент в массиве

	while (first < last) {
		mid = (first + last) / 2;
		if (v0 < edge0[mid] || ((v0 == edge0[mid]) && (v1 < edge1[mid]))) {
			last = mid;
		} else {
			first = mid + 1;
		}
	}

	if (edge0[mid] == v0 && edge1[mid] == v1) {
		for (i = 0; i < num; ++i) {
			if(ind[i] == mid)
				break;
		}
		return i;
	} else {
		return -1;
	}
}

int EdgeSet::add_edge(int v0, int v1) {

	int tmp, first, last, mid;
	int i;

	if (num >= len) {
		fprintf(stdout, "EdgeSet::add_edge : Error. Overflow.\n");
		return -1;
	}
	
	if (v0 > v1) {
		tmp = v0;
		v0 = v1;
		v1 = tmp;
	}

	first = 0; // Первый элемент в массиве
	last = num; // Последний элемент в массиве

	while (first < last) {
		mid = (first + last) / 2;
		if (v0 < edge0[mid] || ((v0 == edge0[mid]) && (v1 < edge1[mid]))) {
			last = mid;
		} else {
			first = mid + 1;
		}
	}

	if (edge0[mid] == v0 && edge1[mid] == v1) {
		for (i = 0; i < num; ++i) {
			if(ind[i] == mid)
				break;
		}
		return i;
	} else {
		ind[num] = last;
		insert_int(edge0, num, last, v0);
		insert_int(edge1, num, last, v1);
		return num++;
	}
}
