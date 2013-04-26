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

	Vector3d A0 = parentPolyhedron->vertices[indVertices[0]];

	double areaFacet = 0.;
	for (int iVertex = 1; iVertex < numVertices - 1; ++iVertex) {
		Vector3d A1 = parentPolyhedron->vertices[indVertices[iVertex]] - A0;
		Vector3d A2 = parentPolyhedron->vertices[indVertices[iVertex + 1]] - A0;
		double areaTriangle = (A1 % A2) * plane.norm * 0.5;
		areaFacet += areaTriangle;
	}

	DEBUG_END;
	return areaFacet;
}
