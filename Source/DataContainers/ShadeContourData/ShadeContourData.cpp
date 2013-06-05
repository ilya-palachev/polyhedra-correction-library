/*
 * ShadeContourData.cpp
 *
 *  Created on: 17.05.2013
 *      Author: iliya
 */

#include "PolyhedraCorrectionLibrary.h"

ShadeContourData::ShadeContourData(const Polyhedron* p) :
				numContours(0),
				contours(NULL)
{
}

ShadeContourData::~ShadeContourData()
{
	if (contours != NULL)
	{
		delete[] contours;
		contours = NULL;
	}
}

void ShadeContourData::photographContours(int numContoursNeeded,
		double firstAngle)
{
	numContours = numContoursNeeded;

}
