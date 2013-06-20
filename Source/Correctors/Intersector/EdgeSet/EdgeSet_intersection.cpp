#include "PolyhedraCorrectionLibrary.h"

void EdgeSet::get_edge(int id, int& v0, int& v1)
{
	if (id < 0 || id > num - 1)
	{
		v0 = -1;
		v1 = -1;
		fprintf(stdout, "EdgeSet::get_edge : Unavailable id = %d, num = %d", id,
				num);
		return;
	}
	v0 = edge0[id];
	v1 = edge1[id];
}

void EdgeSet::get_edge(int id, int& v0, int& v1, int& id_el0, int& pos_el0,
		int& id_el1, int& pos_el1, int& id_ff, int& pos_ff)
{
	if (id < 0 || id > num - 1)
	{
		v0 = -1;
		v1 = -1;
		fprintf(stdout, "EdgeSet::get_edge : Unavailable id = %d, num = %d", id,
				num);
		return;
	}
	v0 = edge0[id];
	v1 = edge1[id];
	id_el0 = id_edge_list0[id];
	pos_el0 = pos_edge_list0[id];
	id_el1 = id_edge_list1[id];
	pos_el1 = pos_edge_list1[id];
	id_ff = id_future_facet[id];
	pos_ff = pos_future_facet[id];
}

int EdgeSet::search_edge(int v0, int v1)
{

	int tmp, first, last, mid;

	if (v0 > v1)
	{
		tmp = v0;
		v0 = v1;
		v1 = tmp;
	}

	first = 0; // Первый элемент в массиве
	last = num; // Последний элемент в массиве

	while (first < last)
	{
		mid = (first + last) / 2;
		if (v0 < edge0[mid] || ((v0 == edge0[mid]) && (v1 <= edge1[mid])))
		{
			last = mid;
		}
		else
		{
			first = mid + 1;
		}
	}

	if (edge0[last] == v0 && edge1[last] == v1)
	{
		return last;
	}
	else
	{
		return -1;
	}
}

void EdgeSet::add_edge(int v0, int v1, int id_el, int pos_el, int id_ff,
		int pos_ff)
{

	int tmp, first, last, mid;

	if (num >= len)
	{
		fprintf(stdout, "EdgeSet::add_edge : Error. Overflow.\n");
		return;
	}

	if (v0 > v1)
	{
		tmp = v0;
		v0 = v1;
		v1 = tmp;
	}

	first = 0; // Первый элемент в массиве
	last = num; // Последний элемент в массиве

	while (first < last)
	{
		mid = (first + last) / 2;
		if (v0 < edge0[mid] || ((v0 == edge0[mid]) && (v1 <= edge1[mid])))
		{
			last = mid;
		}
		else
		{
			first = mid + 1;
		}
	}

	if (edge0[last] == v0 && edge1[last] == v1)
	{
		id_edge_list1[last] = id_el;
		pos_edge_list1[last] = pos_el;
		if (id_future_facet[last] != id_ff || pos_future_facet[last] != pos_ff)
		{
			fprintf(stdout,
					"EdgeSet : Error. 2 different components use the same edge.\n");
		}
	}
	else
	{
		insert_int(edge0, num, last, v0);
		insert_int(edge1, num, last, v1);
		insert_int(id_edge_list0, num, last, id_el);
		insert_int(pos_edge_list0, num, last, pos_el);
		insert_int(id_edge_list1, num, last, -1);
		insert_int(pos_edge_list1, num, last, -1);
		insert_int(id_future_facet, num, last, id_ff);
		insert_int(pos_future_facet, num, last, pos_ff);
		++num;
	}
}

void EdgeSet::add_edge(int v0, int v1)
{
	add_edge(v0, v1, -1, -1, -1, -1);
}

void EdgeSet::test_info()
{
	for (int i; i < num; ++i)
	{
		if (id_edge_list0[i] == -1 || pos_edge_list0[i] == -1
				|| id_edge_list1[i] == -1 || pos_edge_list1[i] == -1
				|| id_future_facet[i] == -1 || pos_future_facet[i] == -1)
		{

			fprintf(stdout, "EdgeSet::test_info : Error at i = %d.\n", i);
		}
	}
}

//void EdgeSet::informate_about_new_vertex(
//		int numv,
//		EdgeList* edge_list,
//		FutureFacet* future_facet) {
//	int i;
//	for (i = 0; i < num; ++i) {
//		edge_list[id_edge_list0[i]].
//			informate_about_new_vertex(pos_edge_list0[i], numv + i);
//		edge_list[id_edge_list1[i]].
//			informate_about_new_vertex(pos_edge_list1[i], numv + i);
//		future_facet[id_future_facet[i]].
//			informate_about_new_vertex(pos_future_facet[i], numv + i);
//	}
//}
