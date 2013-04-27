/*
 * Facet_area.cpp
 *
 *  Created on: 20.04.2013
 *      Author: iliya
 */

#include "PolyhedraCorrectionLibrary.h"

double Facet::area() {
	DEBUG_START;
	my_fprint_all(stderr);

	ASSERT(test_initialization());
	Vector3d A0 = parentPolyhedron->vertices[indVertices[0]];
	my_fprint_all(stderr);
	DEBUG_PRINT("  in parent polyhedron there are %d vertices",
			parentPolyhedron->numVertices);

	double areaFacet = 0.;
	for (int iVertex = 1; iVertex < numVertices - 1; ++iVertex) {
		DEBUG_PRINT("  calculating area of triangle %d : 1 begin", iVertex);
		Vector3d A1 = parentPolyhedron->vertices[indVertices[iVertex]] - A0;
		DEBUG_PRINT("  calculating area of triangle %d : 2", iVertex);
		Vector3d A2 = parentPolyhedron->vertices[indVertices[iVertex + 1]] - A0;
		DEBUG_PRINT("  calculating area of triangle %d : 3", iVertex);
		double areaTriangle = (A1 % A2) * plane.norm * 0.5;
		DEBUG_PRINT("  calculating area of triangle %d : 4", iVertex);
		areaFacet += areaTriangle;
		DEBUG_PRINT("  calculating area of triangle %d : 5 end", iVertex);
	}

	DEBUG_END;
	return areaFacet;
}
