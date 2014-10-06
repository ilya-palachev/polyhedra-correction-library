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
#include "DataContainers/ShadowContourData/SContour/SContour.h"

void SContour::my_fprint(FILE* file)
{
	DEBUG_START;
	REGULAR_PRINT(file, "Printing content of shadow contour #%d\n", id);
	REGULAR_PRINT(file, "id = %d (id of the contour)\n", id);
	REGULAR_PRINT(file, "ns = %d (number of sides)\n", ns);
	REGULAR_PRINT(file, "plane = ((%lf) * x + (%lf) * y + (%lf) * z + (%lf) = 0\n",
			plane.norm.x, plane.norm.y, plane.norm.z, plane.dist);
	REGULAR_PRINT(file, "These are that sides:\n");
	REGULAR_PRINT(file, "confidence\t"
			"type\t"
			"vector A1\t"
			"vector A2\t\n");
	for (int i = 0; i < ns; ++i)
	{
		sides[i].my_fprint_short(file);
	}
	DEBUG_END;
}

void SContour::fprintDefault(FILE* file)
{
	DEBUG_START;
	ALWAYS_PRINT(file, "   %d        %lf      %lf      %lf\n", ns,
		plane.norm.x, plane.norm.y, plane.norm.z);
	for (int i = 0; i < ns; ++i)
	{
		sides[i].fprintDefault(file);
	}
	DEBUG_END;
}
