/*
 * ShadeContourData.cpp
 *
 *  Created on: 17.05.2013
 *      Author: iliya
 */

#include "PolyhedraCorrectionLibrary.h"

ShadeContourData::ShadeContourData(const Polyhedron* p) :
				PData(p),
				numContours(0),
				contours(NULL)
{
}

ShadeContourData::~ShadeContourData()
{
	DEBUG_PRINT("Shade contour data is being deleted now!");
	if (contours != NULL)
	{
		delete[] contours;
		contours = NULL;
	}
}
