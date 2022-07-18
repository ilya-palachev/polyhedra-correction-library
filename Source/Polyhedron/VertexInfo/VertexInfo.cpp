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
#include "Polyhedron/VertexInfo/VertexInfo.h"

VertexInfo::VertexInfo() : id(-1), numFacets(0), indFacets()
{
}

VertexInfo::VertexInfo(const int id_orig, const int nf_orig, const Vector3d vector_orig,
					   const std::vector<int> &index_orig) :
	id(id_orig), numFacets(nf_orig), vector(vector_orig), indFacets(index_orig)
{
	if (index_orig.empty())
	{
		ERROR_PRINT("Error. index_orig is empty\n");
	}
	if (nf_orig < 3)
	{
		ERROR_PRINT("Error. nf1 < 3\n");
	}
}

VertexInfo::VertexInfo(const int id_orig, const Vector3d vector_orig) : id(id_orig), numFacets(0), vector(vector_orig)
{
}

VertexInfo::~VertexInfo()
{
}

VertexInfo &VertexInfo::operator=(const VertexInfo &orig)
{
	id = orig.id;
	vector = orig.vector;
	numFacets = orig.numFacets;

	if (numFacets > 0)
	{
		indFacets = orig.indFacets;
	}
	return *this;
}

int VertexInfo::get_nf()
{
	return numFacets;
}
