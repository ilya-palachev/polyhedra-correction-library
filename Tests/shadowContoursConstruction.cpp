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

#include "PolyhedraCorrectionLibrary.h"

/* Default number of contours in this test. */
const int NUM_CONTOURS = 100;

/* Shifting first angle helps to avoid non-regular positions of projection. */
const double SHIFT_ANGLE_FIRST = 1e-3;

const char *nameFileOriginal = "./Tests/shadowContoursConstuctedForCube.txt";

int main(int argc, char **argv)
{
	DEBUG_START;

	/* Create a cube with side 1 and with center in the O = (0, 0, 0). */
	PolyhedronPtr cube(new Cube(1., 0., 0., 0.));

	ShadowContourDataPtr contourData(new ShadowContourData(cube));
	ShadowContourConstructor scConstructor(cube, contourData);
	scConstructor.run(NUM_CONTOURS, SHIFT_ANGLE_FIRST);

	/* In case when environmental variable
	 * "SHADOW_CONTOURS_CONSTRUCTION_FIRST_RUN" is defined, do printing of
	 * contour data instead of reading it. */
	char *firstRun = getenv("SHADOW_CONTOURS_CONSTRUCTION_FIRST_RUN");
	if (firstRun != NULL && sizeof(firstRun) > 0)
	{
		DEBUG_PRINT("firstRun = %s", firstRun);
		FILE *fileOriginal = (FILE *)fopen(nameFileOriginal, "w");
		contourData->fprintDefault(fileOriginal);
	}
	else
	{
		DEBUG_PRINT("polyhedron use count: %ld", cube.use_count());

		ShadowContourDataPtr contourDataOriginal(new ShadowContourData(cube));
		bool ifScanSucceeded = contourDataOriginal->fscanDefault(nameFileOriginal);
		if (!ifScanSucceeded)
		{
			ERROR_PRINT("Failed to scan contour data from file %s", nameFileOriginal);
			DEBUG_PRINT("polyhedron use count: %ld", cube.use_count());
			DEBUG_END;
			return EXIT_FAILURE;
		}

		if (*contourData != *contourDataOriginal)
		{
			ERROR_PRINT("Inequality found during comparison of obtained "
						"contour data with original one.");
			DEBUG_PRINT("polyhedron use count: %ld", cube.use_count());
			DEBUG_END;
			return EXIT_FAILURE;
		}
	}

	DEBUG_PRINT("polyhedron use count: %ld", cube.use_count());
	DEBUG_END;
	return EXIT_SUCCESS;
}
