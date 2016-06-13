/*
 * Copyright (c) 2009-2016 Ilya Palachev <iliyapalachev@gmail.com>
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

/**
 * @file ShadowContourClusterizer.cpp
 * @brief Implementation of main clusterizing engine for the pre-processing of
 * shadow contour data.
 */

#include <cmath>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "ShadowContourClusterizer.h"
#include "DataContainers/ShadowContourData/SContour/SContour.h"

ShadowContourClusterizer::ShadowContourClusterizer() :
		PAnalyzer()
{
	DEBUG_START;
	DEBUG_END;
}

ShadowContourClusterizer::ShadowContourClusterizer(PolyhedronPtr p) :
		PAnalyzer(p)
{
	DEBUG_START;
	DEBUG_END;	
}

ShadowContourClusterizer::~ShadowContourClusterizer()
{
	DEBUG_START;
	DEBUG_END;
}

static double angleModulo2PI(double angle)
{
	while (angle < 0)
	{
		angle += 2 * M_PI;
	}

	while (angle > 2 * M_PI)
	{
		angle -= 2 * M_PI;
	}

	return angle;
}

void ShadowContourClusterizer::buildPlot(ShadowContourDataPtr contourData,
										 const char* fileNamePlot)
{
	DEBUG_START;

	FILE* file = fopen(fileNamePlot, "w");

	if (!file)
	{
		ERROR_PRINT("File %s cannot be opened for writing.", fileNamePlot);
		DEBUG_END;
		return;
	}
	
	Vector3d ex(1., 0., 0.);
	Vector3d ey(0., 1., 0.);

	/* Print the header of gnuplot file. */
	fprintf(file, "# x y\n");

	/* Iterate via the array of contours. */
	for (int iContour = 0; iContour < contourData->numContours; ++iContour)
	{
		SContour* contourCurr = &contourData->contours[iContour];
		Vector3d normal = contourCurr->plane.norm;
		normal.norm(1.);
		double angle = atan2(normal * ey, normal * ex);
		
		/* Iterate via the array of sides of the current contour. */
		for (int iSide = 0; iSide < contourCurr->ns; ++iSide)
		{
			/*
			 * TODO:
			 * 1. Process left and right halves of the shadow contour separately
			 * here (DONE).
			 * 2. How to process case when A2_{i} != A1_{i + 1} correctly?
			 */
			Vector3d A1 = contourCurr->sides[iSide].A1;
			Vector3d A2 = contourCurr->sides[iSide].A2;
			double A1_angle = A1 * ex > 0 ? angle - M_PI : angle + M_PI;
			double A2_angle = A2 * ex > 0 ? angle - M_PI : angle + M_PI;
			A1_angle = angleModulo2PI(A1_angle);
			A2_angle = angleModulo2PI(A2_angle);
			fprintf(file, "%lf %lf\n", A1_angle, A1.z);
			fprintf(file, "%lf %lf\n", A2_angle, A2.z);
		}
	}
	DEBUG_END;
}
