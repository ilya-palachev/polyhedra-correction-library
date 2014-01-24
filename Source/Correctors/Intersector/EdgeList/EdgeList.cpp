/*
 * Copyright (c) 2009-2014 Ilya Palachev <iliyapalachev@gmail.com>
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
#include "Correctors/Intersector/EdgeList/EdgeList.h"

EdgeList::EdgeList() :
				id(-1),
				len(0),
				num(0),
				pointer(0),
				edge0(NULL),
				edge1(NULL),
				ind0(NULL),
				ind1(NULL),
				next_facet(NULL),
				next_direction(NULL),
				scalar_mult(NULL),
				id_v_new(NULL),
				isUsed(NULL),
				poly(NULL)
{
	DEBUG_START;
	DEBUG_END;
}

EdgeList::EdgeList(int id_orig, int len_orig,
		shared_ptr<Polyhedron> poly_orig) :
				id(id_orig),
				len(len_orig),
				num(0),
				pointer(0),
				edge0(new int[len]),
				edge1(new int[len]),
				ind0(new int[len]),
				ind1(new int[len]),
				next_facet(new int[len]),
				next_direction(new int[len]),
				scalar_mult(new double[len]),
				id_v_new(new int[len]),
				isUsed(new bool[len]),
				poly(poly_orig)
{
	DEBUG_START;
	DEBUG_END;
}

EdgeList::~EdgeList()
{
	DEBUG_START;
	if (edge0 != NULL)
		delete[] edge0;
	if (edge1 != NULL)
		delete[] edge1;
	if (ind0 != NULL)
		delete[] ind0;
	if (ind1 != NULL)
		delete[] ind1;
	if (next_facet != NULL)
		delete[] next_facet;
	if (next_direction != NULL)
		delete[] next_direction;
	if (scalar_mult != NULL)
		delete[] scalar_mult;
	if (id_v_new != NULL)
		delete[] id_v_new;
	if (isUsed != NULL)
		delete[] isUsed;
	DEBUG_END;
}

EdgeList& EdgeList::operator =(const EdgeList& orig)
{
	DEBUG_START;
	int i;

	id = orig.id;
	len = orig.len;
	num = orig.num;
	pointer = orig.pointer;

	if (edge0 != NULL)
		delete[] edge0;
	if (edge1 != NULL)
		delete[] edge1;
	if (ind0 != NULL)
		delete[] edge0;
	if (ind1 != NULL)
		delete[] edge1;
	if (next_facet != NULL)
		delete[] next_facet;
	if (next_direction != NULL)
		delete[] next_direction;
	if (scalar_mult != NULL)
		delete[] scalar_mult;
	if (id_v_new != NULL)
		delete[] id_v_new;
	if (isUsed != NULL)
		delete[] isUsed;

	edge0 = new int[len];
	edge1 = new int[len];
	ind0 = new int[len];
	ind1 = new int[len];
	next_facet = new int[len];
	next_direction = new int[len];
	scalar_mult = new double[len];
	id_v_new = new int[len];
	isUsed = new bool[len];

	for (i = 0; i < num; ++i)
	{
		edge0[i] = orig.edge0[i];
		edge1[i] = orig.edge1[i];
		ind0[i] = orig.ind0[i];
		ind1[i] = orig.ind1[i];
		next_facet[i] = orig.next_facet[i];
		next_direction[i] = orig.next_direction[i];
		scalar_mult[i] = orig.scalar_mult[i];
		id_v_new[i] = orig.id_v_new[i];
		isUsed[i] = orig.isUsed[i];
	}
	DEBUG_END;
	return *this;
}

int EdgeList::get_num()
{
	DEBUG_START;
	DEBUG_END;
	return num;
}

void EdgeList::set_id_v_new(int id_v)
{
	DEBUG_START;
	id_v_new[pointer] = id_v;
	DEBUG_END;
}

void EdgeList::set_isUsed(bool val)
{
	DEBUG_START;
	DEBUG_PRINT("EdgeList[%d].isUsed[%d] = %d\n",
			id, pointer, val);
	isUsed[pointer] = val;
	DEBUG_END;
}

void EdgeList::set_pointer(int val)
{
	DEBUG_START;
	if (val < 0 || val > num - 1)
	{
		ERROR_PRINT("Error. num = %d, val = %d", num, val);
		return;
	}
	DEBUG_PRINT("EdgeList[%d].set_pointer(%d)\n",
			id, val);
	pointer = val;
	DEBUG_END;
}

void EdgeList::goto_header()
{
	DEBUG_START;
	set_pointer(0);
	DEBUG_END;
}

void EdgeList::go_forward()
{
	DEBUG_START;
	set_pointer(pointer + 1);
	DEBUG_END;
}

