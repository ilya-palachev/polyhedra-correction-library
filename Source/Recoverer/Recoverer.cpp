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

#include <cmath>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Constants.h"
#include "Recoverer/Recoverer.h"
#include "DataContainers/ShadeContourData/ShadeContourData.h"
#include "DataContainers/ShadeContourData/SContour/SContour.h"

Recoverer::Recoverer()
{
	DEBUG_START;
	DEBUG_END;
}

Recoverer::~Recoverer()
{
	DEBUG_START;
	DEBUG_END;
}

Polyhedron* Recoverer::buildNaivePolyhedron(ShadeContourDataPtr SCData)
{
	DEBUG_START;
	vector<Plane> supportPlanes = extractSupportPlanes(SCData);
	DEBUG_PRINT("Number of extracted support planes: %ld",
				supportPlanes.size());

	DEBUG_END;
	return NULL;
}

vector<Plane> Recoverer::extractSupportPlanes(ShadeContourDataPtr SCData)
{
	DEBUG_START;
	vector<Plane> supportPlanes;

	/* Iterate through the array of contours. */
	for (int iContour = 0; iContour < SCData->numContours; ++iContour)
	{
		DEBUG_PRINT("contour #%d", iContour);
		SContour* contourCurr = &SCData->contours[iContour];
		Vector3d normal = contourCurr->plane.norm;

		/* Iterate through the array of sides of current contour. */
		for (int iSide = 0; iSide < contourCurr->ns; ++iSide)
		{
			SideOfContour* sideCurr = &contourCurr->sides[iSide];
			
			/*
			 * Here the plane that is incident to points A1 and A2 of the
			 * current side and collinear to the vector of projection.
			 *
			 * TODO: Here should be the calculation of the best plane fitting
			 * these conditions. Current implementation can produce big errors.
			 */
			Vector3d supportPlaneNormal = (sideCurr->A1 - sideCurr->A2) %
				normal;
			Plane supportPlane(supportPlaneNormal,
							   - supportPlaneNormal * sideCurr->A1);

			supportPlanes.push_back(supportPlane);
			
			double error1 = supportPlane.norm * sideCurr->A1 +
				supportPlane.dist;
			double error2 = supportPlane.norm * sideCurr->A2 +
				supportPlane.dist;

			DEBUG_PRINT("   side #%d\t%le\t%le", iSide, error1, error2);
			
			/* TODO: Here should be more strict conditions. */
			ASSERT(error1 < 100 * EPS_MIN_DOUBLE);
			ASSERT(error2 < 100 * EPS_MIN_DOUBLE);
		}
	}

	DEBUG_END;
	return supportPlanes;
}
