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
#include "Constants.h"
#include "DataContainers/EdgeData/Edge/Edge.h"

Edge::Edge() :
				id(-1),
				v0(-1),
				v1(-1),
				f0(-1),
				f1(-1),
				assocList()
{
	DEBUG_START;
	DEBUG_END;
}

Edge::Edge(int v0_orig, int v1_orig) :
				id(INT_NOT_INITIALIZED),
				v0(v0_orig),
				v1(v1_orig),
				f0(INT_NOT_INITIALIZED),
				f1(INT_NOT_INITIALIZED),
				assocList()
{
	DEBUG_START;
	DEBUG_END;
}

Edge::Edge(int id_orig, int v0_orig, int v1_orig, int f0_orig, int f1_orig) :
				id(id_orig),
				v0(v0_orig),
				v1(v1_orig),
				f0(f0_orig),
				f1(f1_orig),
				assocList()
{
	DEBUG_START;
	DEBUG_END;
}

Edge::Edge(int v0_orig, int v1_orig, int f0_orig, int f1_orig) :
				id(-1),
				v0(v0_orig),
				v1(v1_orig),
				f0(f0_orig),
				f1(f1_orig),
				assocList()
{
	DEBUG_START;
	DEBUG_END;
}

Edge::Edge(int id_orig, int v0_orig, int v1_orig, int f0_orig, int f1_orig,
		std::list<EdgeContourAssociation> assocList_orig) :
				id(id_orig),
				v0(v0_orig),
				v1(v1_orig),
				f0(f0_orig),
				f1(f1_orig),
				assocList(assocList_orig)
{
	DEBUG_START;
	DEBUG_END;
}

Edge::Edge(int v0_orig, int v1_orig, int f0_orig, int f1_orig,
		std::list<EdgeContourAssociation> assocList_orig) :
				id(-1),
				v0(v0_orig),
				v1(v1_orig),
				f0(f0_orig),
				f1(f1_orig),
				assocList(assocList_orig)
{
	DEBUG_START;
	DEBUG_END;
}

Edge::Edge(const Edge& orig) :
				id(orig.id),
				v0(orig.v0),
				v1(orig.v1),
				f0(orig.f0),
				f1(orig.f1),
				assocList(orig.assocList)
{
	DEBUG_START;
	DEBUG_END;
}

Edge::~Edge()
{
	DEBUG_START;
	DEBUG_PRINT("Attention! edge #%d is deleting now\n", id);
	assocList.clear();
	DEBUG_END;
}

Edge& Edge::operator =(const Edge& orig)
{
	DEBUG_START;
	id = orig.id;
	v0 = orig.v0;
	v1 = orig.v1;
	f0 = orig.f0;
	f1 = orig.f1;
	assocList = orig.assocList;

	DEBUG_END;
	return *this;
}

bool Edge::operator ==(const Edge& e) const
{
	DEBUG_START;
	DEBUG_END;
	return (v0 == e.v0) && (v1 == e.v1) && (f0 == e.f0) && (f1 == e.f1);
}

bool Edge::operator != (const Edge& e) const
{
	DEBUG_START;
	DEBUG_END;
	return !(*this == e);
}
