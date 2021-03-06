/*
 * Copyright (c) 2009-2016 Ilya Palachev <iliyapalachev@gmail.com>
 *
 * This file is part of Polyhedra Correction Library.
 *
 * Polyhedra Correction Library is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Polyhedra Correction Library is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Polyhedra Correction Library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "array_operations.h"
#include "Correctors/Intersector/EdgeSetIntersected/EdgeSetIntersected.h"

void EdgeSetIntersected::get_edge(int id, int &v0, int &v1)
{
	DEBUG_START;
	if (id < 0 || id > num - 1)
	{
		v0 = -1;
		v1 = -1;
		ERROR_PRINT("Unavailable id = %d, num = %d", id, num);
		return;
	}
	v0 = edge0[id];
	v1 = edge1[id];
	DEBUG_END;
}

void EdgeSetIntersected::get_edge(int id, int &v0, int &v1, int &id_el0,
								  int &pos_el0, int &id_el1, int &pos_el1,
								  int &id_ff, int &pos_ff)
{
	DEBUG_START;
	if (id < 0 || id > num - 1)
	{
		v0 = -1;
		v1 = -1;
		ERROR_PRINT("Unavailable id = %d, num = %d", id, num);
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
	DEBUG_END;
}

int EdgeSetIntersected::search_edge(int v0, int v1)
{
	DEBUG_START;

	int tmp, first, last, mid;

	if (v0 > v1)
	{
		tmp = v0;
		v0 = v1;
		v1 = tmp;
	}

	first = 0;	// Первый элемент в массиве
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
		DEBUG_END;
		return last;
	}
	else
	{
		DEBUG_END;
		return -1;
	}
}

void EdgeSetIntersected::add_edge(int v0, int v1, int id_el, int pos_el,
								  int id_ff, int pos_ff)
{
	DEBUG_START;

	int tmp, first, last, mid;

	if (num >= len)
	{
		ERROR_PRINT("Error. Overflow.");
		DEBUG_END;
		return;
	}

	if (v0 > v1)
	{
		tmp = v0;
		v0 = v1;
		v1 = tmp;
	}

	first = 0;	// Первый элемент в массиве
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
			ERROR_PRINT("Error. 2 different components use the same edge.");
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
	DEBUG_END;
}

void EdgeSetIntersected::add_edge(int v0, int v1)
{
	DEBUG_START;
	add_edge(v0, v1, -1, -1, -1, -1);
	DEBUG_END;
}

void EdgeSetIntersected::test_info()
{
	DEBUG_START;
	for (int i = 0; i < num; ++i)
	{
		if (id_edge_list0[i] == -1 || pos_edge_list0[i] == -1 ||
			id_edge_list1[i] == -1 || pos_edge_list1[i] == -1 ||
			id_future_facet[i] == -1 || pos_future_facet[i] == -1)
		{

			ERROR_PRINT("Error at i = %d.\n", i);
		}
	}
	DEBUG_END;
}
