/*
 * shadowContoursConstruction.cpp
 *
 *  Created on: Sep 10, 2013
 *      Author: ilya
 */

#include "PolyhedraCorrectionLibrary.h"

/* Default number of contours in this test. */
const int NUM_CONTOURS = 100;

/* Shifting first angle helps to avoid non-regular positions of projection. */
const double SHIFT_ANGLE_FIRST = 1e-3;

const char* nameFileOriginal = "./Tests/shadowContoursConstuctedForCube.txt";

int main(int argc, char** argv)
{
	DEBUG_START;

	/* Create a cube with side 1 and with center in the O = (0, 0, 0). */
	std::shared_ptr<Polyhedron> polyhedron(new Cube(1., 0., 0., 0.));

	std::shared_ptr<ShadeContourData> contourData(new
			ShadeContourData(polyhedron));
	std::shared_ptr<ShadeContourConstructor> scConstructor(new
			ShadeContourConstructor(polyhedron, contourData));
	scConstructor->run(NUM_CONTOURS, SHIFT_ANGLE_FIRST);

	/* In case when environmental variable
	 * "SHADOW_CONTOURS_CONSTRUCTION_FIRST_RUN" is defined, do printing of
	 * contour data instead of reading it. */
	char* firstRun = getenv("SHADOW_CONTOURS_CONSTRUCTION_FIRST_RUN");
	if ( firstRun != NULL && sizeof(firstRun) > 0)
	{
		DEBUG_PRINT("firstRun = %s", firstRun);
		FILE* fileOriginal = (FILE*) fopen(nameFileOriginal, "w");
		contourData->fprintDefault(fileOriginal);
	}
	else
	{
		std::shared_ptr<ShadeContourData> contourDataOriginal(new
				ShadeContourData(polyhedron));
		bool ifScanSucceeded =
				contourDataOriginal->fscanDefault(nameFileOriginal);
		if (!ifScanSucceeded)
		{
			ERROR_PRINT("Failed to scan contour data from file %s",
					nameFileOriginal);
			DEBUG_END;
			return EXIT_FAILURE;
		}

		if(*contourData != *contourDataOriginal)
		{
			ERROR_PRINT("Inequality found during comparison of obtained "
					"contour data with original one.");
			DEBUG_END;
			return EXIT_FAILURE;
		}
	}

	DEBUG_END;
	return EXIT_SUCCESS;
}


