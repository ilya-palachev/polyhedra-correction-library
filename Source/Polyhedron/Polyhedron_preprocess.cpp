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
#include "Polyhedron/Polyhedron.h"
#include "Polyhedron/Facet/Facet.h"
#include "Polyhedron/VertexInfo/VertexInfo.h"

void Polyhedron::preprocessAdjacency()
{
	DEBUG_START;
	int i;
	DEBUG_PRINT("Cleaning facets");
	for (i = 0; i < numFacets; ++i)
	{
		if (facets[i].numVertices < 1)
			continue;
		facets[i].preprocess_free();
	}

	DEBUG_PRINT("Facets preprocessing");
	for (i = 0; i < numFacets; ++i)
	{
		if (facets[i].numVertices < 1)
			continue;
		facets[i].preprocess();
	}

	DEBUG_PRINT("VertexInfos preprocessing");
	if (vertexInfos != NULL)
	{
		delete[] vertexInfos;
	}
	vertexInfos = new VertexInfo[numVertices];
	for (i = 0; i < numVertices; ++i)
	{
		vertexInfos[i] = VertexInfo(i, vertices[i], get_ptr());
		vertexInfos[i].preprocess();
	}
	DEBUG_END;
}
