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

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "DataContainers/ShadowContourData/SideOfContour/SideOfContour.h"

void SideOfContour::my_fprint(FILE* file)
{
	DEBUG_START;
	REGULAR_PRINT(file, "Printing content of side of contour\n");
	REGULAR_PRINT(file, "confidence = %lf (number from 0 to 1)\n", confidence);
	REGULAR_PRINT(file, "edge type = %d (that means the following:\n", type);
	REGULAR_PRINT(file,
			"// Unknown / not calculated\n"
					"EEdgeRegular     = 0 -- Regular edges, regular confidence\n"
					"EEdgeDummy       = 1 -- Dummy edge, not existing in reality\n"
					"                        (originating e.g. from a picture crop)\n"
					"EEdgeDust        = 2 -- Low confidence because of dust on stone surface\n"
					"EEdgeCavern      = 3 -- Low confidence because of cavern in the stone\n"
					"EEdgeMaxPointErr = 4 -- The edge erroneously is lower than other edges,\n"
					"                        which define a vertex (e.g. in the culet)\n"
					"EEdgeGlare       = 5 -- Low confidence because of photo glare\n\n");

	REGULAR_PRINT(file, "A1 = (%lf, %lf, %lf)\n", A1.x, A1.y, A1.z);
	REGULAR_PRINT(file, "A2 = (%lf, %lf, %lf)\n", A2.x, A2.y, A2.z);
	DEBUG_END;
}

void SideOfContour::my_fprint_short(FILE* file)
{
	DEBUG_START;
	REGULAR_PRINT(file, "%lf\t%d\t(%lf,%lf,%lf)\t(%lf,%lf,%lf)\n", confidence, type,
			A1.x, A1.y, A1.z, A2.x, A2.y, A2.z);
	DEBUG_END;
}

void SideOfContour::fprintDefault(FILE* file)
{
	DEBUG_START;
	ALWAYS_PRINT(file, "      %lf    %d\n", confidence, type);
	ALWAYS_PRINT(file, "      %lf      %lf     %lf\n", A1.x, A1.y, A1.z);
	ALWAYS_PRINT(file, "      %lf      %lf     %lf\n", A2.x, A2.y, A2.z);
	DEBUG_END;
}
