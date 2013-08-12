/*
 * Facet_area.cpp
 *
 *  Created on: 20.04.2013
 *      Author: iliya
 */

#include "PolyhedraCorrectionLibrary.h"

double Facet::area()
{
	DEBUG_START;
	my_fprint_all(stderr);

	bool ifInitialized = test_initialization();
	if (!ifInitialized)
	{
		ERROR_PRINT("Trying to calculate area of facet, which indices have");
		ERROR_PRINT("  been initialized yet.");
		DEBUG_END;
		ASSERT(ifInitialized);
		return DEFAULT_ERROR_FOR_DOUBLE_FUNCTIONS;
	}

	Vector3d A0 = parentPolyhedron->vertices[indVertices[0]];
	my_fprint_all(stderr);
	DEBUG_PRINT("  in parent polyhedron there are %d vertices",
			parentPolyhedron->numVertices);

	double areaFacet = 0.;
	for (int iVertex = 1; iVertex < numVertices - 1; ++iVertex)
	{
		int v0 = indVertices[iVertex];
		int v1 = indVertices[iVertex + 1];
		if (v0 >= parentPolyhedron->numVertices || v0 < 0 ||
				v1 >= parentPolyhedron->numVertices || v1 < 0)
		{
			ERROR_PRINT("Invalid data in facet indices:");
			ERROR_PRINT("  indVertices[%d] = %d",	iVertex, v0);
			ERROR_PRINT("  indVertices[%d] = %d",	iVertex + 1, v1);
			parentPolyhedron->my_fprint(stderr);
			DEBUG_END;
			return DEFAULT_ERROR_FOR_DOUBLE_FUNCTIONS;
		}
		Vector3d A1 = parentPolyhedron->vertices[v0] - A0;
		Vector3d A2 = parentPolyhedron->vertices[v1] - A0;
		double areaTriangle = (A1 % A2) * plane.norm * 0.5;
		DEBUG_PRINT("\tarea of triangle # %d = %lf", iVertex - 1, areaTriangle);
		areaFacet += areaTriangle;
	}

	DEBUG_PRINT("area of facet = %lf", areaFacet);
	DEBUG_END;
	return areaFacet;
}
