/*
 * Copyright (c) 2009-2015 Ilya Palachev <iliyapalachev@gmail.com>
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
#include "Correctors/Intersector/EdgeSetIntersected/EdgeSetIntersected.h"

EdgeSetIntersected::EdgeSetIntersected() :
				len(0),
				num(0),
				edge0(NULL),
				edge1(NULL),
				id_edge_list0(NULL),
				pos_edge_list0(NULL),
				id_edge_list1(NULL),
				pos_edge_list1(NULL),
				id_future_facet(NULL),
				pos_future_facet(NULL)
{
	DEBUG_START;
	DEBUG_END;
}

EdgeSetIntersected::EdgeSetIntersected(int len_orig) :
				len(len_orig),
				num(0),
				edge0(new int[len]),
				edge1(new int[len]),
				id_edge_list0(new int[len]),
				pos_edge_list0(new int[len]),
				id_edge_list1(new int[len]),
				pos_edge_list1(new int[len]),
				id_future_facet(new int[len]),
				pos_future_facet(new int[len])
{
	DEBUG_START;
	DEBUG_END;
}

EdgeSetIntersected::EdgeSetIntersected(const EdgeSetIntersected& orig) :
				len(orig.len),
				num(orig.num),
				edge0(new int[len]),
				edge1(new int[len]),
				id_edge_list0(new int[len]),
				pos_edge_list0(new int[len]),
				id_edge_list1(new int[len]),
				pos_edge_list1(new int[len]),
				id_future_facet(new int[len]),
				pos_future_facet(new int[len])
{
	DEBUG_START;
	for (int i = 0; i < num; ++i)
	{
		edge0[i] = orig.edge0[i];
		edge1[i] = orig.edge1[i];
		id_edge_list0[i] = orig.id_edge_list0[i];
		pos_edge_list0[i] = orig.pos_edge_list0[i];
		id_edge_list1[i] = orig.id_edge_list1[i];
		pos_edge_list1[i] = orig.pos_edge_list1[i];
		id_future_facet[i] = orig.id_future_facet[i];
		pos_future_facet[i] = orig.pos_future_facet[i];
	}
	DEBUG_END;
}

EdgeSetIntersected::~EdgeSetIntersected()
{
	DEBUG_START;
	if (edge0 != NULL)
		delete[] edge0;
	if (edge1 != NULL)
		delete[] edge1;
	if (id_edge_list0 != NULL)
		delete[] id_edge_list0;
	if (pos_edge_list0 != NULL)
		delete[] pos_edge_list0;
	if (id_edge_list1 != NULL)
		delete[] id_edge_list1;
	if (pos_edge_list1 != NULL)
		delete[] pos_edge_list1;
	if (id_future_facet != NULL)
		delete[] id_future_facet;
	if (pos_future_facet != NULL)
		delete[] pos_future_facet;
	DEBUG_END;
}

EdgeSetIntersected& EdgeSetIntersected::operator =(const EdgeSetIntersected& orig)
{
	DEBUG_START;
	len = orig.len;
	num = orig.num;

	if (edge0 != NULL)
		delete[] edge0;
	if (edge1 != NULL)
		delete[] edge1;
	if (id_edge_list0 != NULL)
		delete[] id_edge_list0;
	if (pos_edge_list0 != NULL)
		delete[] pos_edge_list0;
	if (id_edge_list1 != NULL)
		delete[] id_edge_list1;
	if (pos_edge_list1 != NULL)
		delete[] pos_edge_list1;
	if (id_future_facet != NULL)
		delete[] id_future_facet;
	if (pos_future_facet != NULL)
		delete[] pos_future_facet;

	edge0 = new int[len];
	edge1 = new int[len];
	id_edge_list0 = new int[len];
	pos_edge_list0 = new int[len];
	id_edge_list1 = new int[len];
	pos_edge_list1 = new int[len];
	id_future_facet = new int[len];
	pos_future_facet = new int[len];

	for (int i = 0; i < num; ++i)
	{
		edge0[i] = orig.edge0[i];
		edge1[i] = orig.edge1[i];
		id_edge_list0[i] = orig.id_edge_list0[i];
		pos_edge_list0[i] = orig.pos_edge_list0[i];
		id_edge_list1[i] = orig.id_edge_list1[i];
		pos_edge_list1[i] = orig.pos_edge_list1[i];
		id_future_facet[i] = orig.id_future_facet[i];
		pos_future_facet[i] = orig.pos_future_facet[i];
	}
	DEBUG_END;
	return *this;
}

int EdgeSetIntersected::get_len()
{
	DEBUG_START;
	DEBUG_END;
	return len;
}

int EdgeSetIntersected::get_num()
{
	DEBUG_START;
	DEBUG_END;
	return num;
}

