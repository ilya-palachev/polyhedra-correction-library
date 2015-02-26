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
#include "Correctors/Intersector/FutureFacet/FutureFacet.h"

FutureFacet::FutureFacet() :
				id(-1),
				len(0),
				nv(0),
				edge0(NULL),
				edge1(NULL),
				src_facet(NULL),
				id_v_new(NULL)
{
	DEBUG_START;
	DEBUG_END;
}

FutureFacet::~FutureFacet()
{
	DEBUG_START;
	if (edge0 != NULL)
	{
		delete[] edge0;
		edge0 = NULL;
	}
	if (edge1 != NULL)
	{
		delete[] edge1;
		edge1 = NULL;
	}
	if (src_facet != NULL)
	{
		delete[] src_facet;
		src_facet = NULL;
	}
	if (id_v_new != NULL)
	{
		delete[] id_v_new;
		id_v_new = NULL;
	}
	DEBUG_END;
}

FutureFacet::FutureFacet(int len_orig) :
				id(-1),
				len(len_orig),
				nv(0),
				edge0(new int[len]),
				edge1(new int[len]),
				src_facet(new int[len]),
				id_v_new(new int[len])
{
	DEBUG_START;
	DEBUG_END;
}

FutureFacet::FutureFacet(const FutureFacet& orig) :
				id(orig.id),
				len(orig.len),
				nv(orig.nv)
{
	DEBUG_START;
	int i;
	if (len > 0)
	{
		edge0 = new int[len];
		edge1 = new int[len];
		src_facet = new int[len];
		id_v_new = new int[len];

		for (i = 0; i < nv; ++i)
		{
			edge0[i] = orig.edge0[i];
			edge1[i] = orig.edge1[i];
			src_facet[i] = orig.src_facet[i];
			id_v_new[i] = orig.id_v_new[i];
		}
	}
	else
	{
		edge0 = NULL;
		edge1 = NULL;
		src_facet = NULL;
	}
	DEBUG_END;
}

void FutureFacet::add_edge(int v0, int v1, int src_f)
{
	DEBUG_START;
	if (nv >= len)
	{
		ERROR_PRINT("Error. nv >= len  ");
		ERROR_PRINT("v0 = %d, v1 = %d, src_f = %d\n", v0, v1, src_f);
		DEBUG_END;
		return;
	}
	edge0[nv] = v0;
	edge1[nv] = v1;
	src_facet[nv] = src_f;
	++nv;
	DEBUG_END;
}

FutureFacet& FutureFacet::operator =(const FutureFacet& orig)
{
	DEBUG_START;
	int i;
	id = orig.id;
	len = orig.len;
	nv = orig.nv;
	if (len > 0)
	{
		edge0 = new int[len];
		edge1 = new int[len];
		src_facet = new int[len];
		id_v_new = new int[len];

		for (i = 0; i < nv; ++i)
		{
			edge0[i] = orig.edge0[i];
			edge1[i] = orig.edge1[i];
			src_facet[i] = orig.src_facet[i];
			id_v_new[i] = orig.id_v_new[i];
		}
	}
	else
	{
		edge0 = NULL;
		edge1 = NULL;
		src_facet = NULL;
	}
	DEBUG_END;
	return *this;
}

FutureFacet& FutureFacet::operator +=(const FutureFacet& v)
{
	DEBUG_START;
	int i, *edge0_new, *edge1_new, *src_facet_new, *id_v_new_new;
	if (v.nv > 0)
	{
		if (v.nv + nv > len)
		{
			len = v.nv + nv;
			edge0_new = new int[len];
			edge1_new = new int[len];
			src_facet_new = new int[len];
			id_v_new_new = new int[len];
			for (i = 0; i < nv; ++i)
			{
				edge0_new[i] = edge0[i];
				edge1_new[i] = edge1[i];
				src_facet_new[i] = src_facet[i];
				id_v_new_new[i] = id_v_new[i];
			}
			for (i = 0; i < v.nv; ++i)
			{
				edge0_new[nv + i] = v.edge0[i];
				edge1_new[nv + i] = v.edge1[i];
				src_facet_new[nv + i] = v.src_facet[i];
				id_v_new_new[nv + i] = v.id_v_new[i];
			}
			if (edge0 != NULL)
				delete[] edge0;
			if (edge1 != NULL)
				delete[] edge1;
			if (src_facet != NULL)
				delete[] src_facet;
			if (id_v_new != NULL)
				delete[] id_v_new;
			edge0 = edge0_new;
			edge1 = edge1_new;
			src_facet = src_facet_new;
			id_v_new = id_v_new_new;
			nv += v.nv;
		}
		else
		{
			edge0_new = edge1_new = src_facet_new = id_v_new_new = NULL;
			for (i = 0; i < v.nv; ++i)
			{
				edge0[nv + i] = v.edge0[i];
				edge1[nv + i] = v.edge1[i];
				src_facet[nv + i] = v.src_facet[i];
				id_v_new[nv + i] = v.id_v_new[i];
			}
			nv += v.nv;
		}
	}
	DEBUG_END;
	return *this;
}

void FutureFacet::free()
{
	DEBUG_START;
	nv = 0;
	DEBUG_END;
}

int FutureFacet::get_nv()
{
	DEBUG_START;
	DEBUG_END;
	return nv;
}

void FutureFacet::get_edge(int pos, int& v0, int& v1, int& src_f, int& id_v)
{
	DEBUG_START;
	if (pos < 0 || pos >= nv)
	{
		v0 = v1 = src_f = id_v = -1;
		return;
	}
	v0 = edge0[pos];
	v1 = edge1[pos];
	src_f = src_facet[pos];
	id_v = id_v_new[pos];
	DEBUG_END;
}

void FutureFacet::set_id(int val)
{
	DEBUG_START;
	id = val;
	DEBUG_END;
}
