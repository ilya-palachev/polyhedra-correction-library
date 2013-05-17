/*
 * ShadeContourConstructor.cpp
 *
 *  Created on: 17.05.2013
 *      Author: iliya
 */

#include "PolyhedraCorrectionLibrary.h"

ShadeContourConstructor::ShadeContourConstructor(const Polyhedron* p,
		const ShadeContourData* d) :
				polyhedron(p),
				data(d),
				bufferBool(NULL),
				bufferInt0(NULL),
				bufferInt1(NULL)
{
}

ShadeContourConstructor::~ShadeContourConstructor()
{
	if (bufferBool != NULL)
	{
		delete[] bufferBool;
		bufferBool = NULL;
	}
	if (bufferInt0 != NULL)
	{
		delete[] bufferInt0;
		bufferInt0 = NULL;
	}
	if (bufferInt1 != NULL)
	{
		delete[] bufferInt1;
		bufferInt1 = NULL;
	}
}

void ShadeContourConstructor::run(int numContoursNeeded, double firstAngle)
{
	DEBUG_START;
	DEBUG_PRINT("Allocating 3 arrays of length %d", numEdges);
	bool* bufferBool = new bool[numEdges];
	int* bufferInt0 = new int[numEdges];
	int* bufferInt1 = new int[numEdges];

	for (int icont = 0; icont < data->numContours; ++icont)
	{
		double angle = firstAngle + 2 * M_PI * (icont) / data->numContours;
		Vector3d nu = Vector3d(cos(angle), sin(angle), 0);
		Plane planeOfProjection = Plane(nu, 0);

		data->contours[icont] = createContour(icont, planeOfProjection,
				bufferBool, bufferInt0, bufferInt1);
	}
	DEBUG_END;
}

