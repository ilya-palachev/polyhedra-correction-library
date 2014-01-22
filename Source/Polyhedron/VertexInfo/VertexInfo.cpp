/*
 * Copyright (c) 2009-2013 Ilya Palachev <iliyapalachev@gmail.com>
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

VertexInfo::VertexInfo() :
				id(-1),
				numFacets(0),
				indFacets(NULL)
{
}

VertexInfo::VertexInfo(const int id_orig, const int nf_orig,
		const Vector3d vector_orig, const int* index_orig,
		shared_ptr<Polyhedron> poly_orig) :
				id(id_orig),
				numFacets(nf_orig),
				vector(vector_orig),
				parentPolyhedron(poly_orig)
{

	if (!index_orig)
	{
		ERROR_PRINT("Error. index1 = NULL\n");
	}
	if (nf_orig < 3)
	{
		ERROR_PRINT("Error. nf1 < 3\n");
	}
	indFacets = new int[3 * numFacets + 1];
	for (int i = 0; i < 3 * numFacets + 1; ++i)
		indFacets[i] = index_orig[i];
}

VertexInfo::VertexInfo(const int id_orig, const Vector3d vector_orig,
		shared_ptr<Polyhedron> poly_orig) :
				id(id_orig),
				numFacets(0),
				vector(vector_orig),
				indFacets(NULL),
				parentPolyhedron(poly_orig)
{
}

VertexInfo::~VertexInfo()
{

	if (indFacets != NULL && numFacets != 0)
	{
		delete[] indFacets;
	}
	indFacets = NULL;
	numFacets = 0;
}

VertexInfo& VertexInfo::operator =(const VertexInfo& orig)
{
	int i;

	this->id = orig.id;
	this->vector = orig.vector;
	this->parentPolyhedron = orig.parentPolyhedron;
	this->numFacets = orig.numFacets;

	if (numFacets > 0)
	{
		if (indFacets)
			delete[] indFacets;
		indFacets = new int[3 * numFacets + 1];
		for (i = 0; i < 3 * numFacets + 1; ++i)
			indFacets[i] = orig.indFacets[i];
	}
	return *this;
}

int VertexInfo::get_nf()
{
	return numFacets;
}
