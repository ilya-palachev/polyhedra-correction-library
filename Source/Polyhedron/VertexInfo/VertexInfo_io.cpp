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
#include "Polyhedron/VertexInfo/VertexInfo.h"

void VertexInfo::fprint_my_format(FILE* file)
{
	REGULAR_PRINT(file, "%d ", numFacets);
	for (int i = 0; i < 3 * numFacets + 1; ++i)
	{
		REGULAR_PRINT(file, " %d", indFacets[i]);
	}
	REGULAR_PRINT(file, "\n");
}

void VertexInfo::my_fprint_all(FILE* file)
{
	int i;
	REGULAR_PRINT(file, "\n------------ VertexInfo %d: ------------\n", id);
//	REGULAR_PRINT(file, "id = %d\nnf = %d\n", id, nf);
//	REGULAR_PRINT(file, "vector = (%.2lf, %.2lf, %.2lf)\n",
//			std::vector.x, std::vector.y, std::vector.z);

	REGULAR_PRINT(file, "facets :          ");
	for (i = 0; i < numFacets; ++i)
		REGULAR_PRINT(file, "%d ", indFacets[i]);

	REGULAR_PRINT(file, "\nnext vertexes : ");
	for (i = numFacets + 1; i < 2 * numFacets + 1; ++i)
		REGULAR_PRINT(file, "%d ", indFacets[i]);

	REGULAR_PRINT(file, "\npositions :     ");
	for (i = 2 * numFacets + 1; i < 3 * numFacets + 1; ++i)
		REGULAR_PRINT(file, "%d ", indFacets[i]);
}
