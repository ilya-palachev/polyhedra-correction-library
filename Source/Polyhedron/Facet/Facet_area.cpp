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
#include "Constants.h"
#include "Polyhedron/Facet/Facet.h"

double Facet::area()
{
	DEBUG_START;
#ifndef NDEBUG
	my_fprint_all(stderr);
#endif /* NDEBUG */

	bool ifInitialized = test_initialization();
	if (!ifInitialized)
	{
		ERROR_PRINT("Trying to calculate area of facet, which indices have");
		ERROR_PRINT("  been initialized yet.");
		DEBUG_END;
		ASSERT(ifInitialized);
		return DEFAULT_ERROR_FOR_DOUBLE_FUNCTIONS;
	}

	double areaFacet = 0.;
	if (auto polyhedron = parentPolyhedron.lock())
	{
		Vector3d A0 = polyhedron->vertices[indVertices[0]];

		DEBUG_PRINT("  in parent polyhedron there are %d vertices",
				polyhedron->numVertices);

		for (int iVertex = 1; iVertex < numVertices - 1; ++iVertex)
		{
			int v0 = indVertices[iVertex];
			int v1 = indVertices[iVertex + 1];
			if (v0 >= polyhedron->numVertices || v0 < 0 ||
					v1 >= polyhedron->numVertices || v1 < 0)
			{
				ERROR_PRINT("Invalid data in facet indices:");
				ERROR_PRINT("  indVertices[%d] = %d",	iVertex, v0);
				ERROR_PRINT("  indVertices[%d] = %d",	iVertex + 1, v1);
				polyhedron->my_fprint(stderr);
				ASSERT(0);
				DEBUG_END;
				return DEFAULT_ERROR_FOR_DOUBLE_FUNCTIONS;
			}
			Vector3d A1 = polyhedron->vertices[v0] - A0;
			Vector3d A2 = polyhedron->vertices[v1] - A0;
			double areaTriangle = (A1 % A2) * plane.norm * 0.5;
			DEBUG_PRINT("\tarea of triangle # %d = %lf", iVertex - 1, areaTriangle);
			areaFacet += areaTriangle;
		}

	}


	DEBUG_PRINT("area of facet = %lf", areaFacet);
	DEBUG_END;
	return areaFacet;
}
