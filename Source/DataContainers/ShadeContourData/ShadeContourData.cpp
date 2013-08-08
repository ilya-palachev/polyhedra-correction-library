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
	DEBUG_START;
	DEBUG_END;
}

ShadeContourData::~ShadeContourData()
{
	DEBUG_START;
	DEBUG_PRINT("Shade contour data is being deleted now!");
	if (contours != NULL)
	{
		delete[] contours;
		contours = NULL;
	}
	DEBUG_END;
}

#define STD_SC_FORMAT_HEADER_SIZE_1 2
#define STD_SC_FORMAT_HEADER_SIZE_2 31

bool ShadeContourData::fscanDefault(char* fileNameContours)
{
	DEBUG_START;
	FILE* fd = (FILE*) fopen(fileNameContours, "r");

	if (!fd)
	{
		ERROR_PRINT("Failed to open file %s", fileNameContours);
		DEBUG_END;
		return false;
	}

	char* scannedString = new char[255];
	for (int i = 0; i < STD_SC_FORMAT_HEADER_SIZE_1; ++i)
	{
		if (fscanf(fd, "%s", scannedString) != 1)
		{
			ERROR_PRINT("Wrong file format, in header #1");
			fclose(fd);
			DEBUG_END;
			return false;
		}
		DEBUG_PRINT("scanned string : %s", scannedString);
	}

	if (fscanf(fd, "%d", &numContours) != 1)
	{
		ERROR_PRINT("Wrong file format, in number of contours");
		fclose(fd);
		DEBUG_END;
		return false;
	}

	for (int i = 0; i < STD_SC_FORMAT_HEADER_SIZE_2; ++i)
	{
		if (fscanf(fd, "%s", scannedString) != 1)
		{
			ERROR_PRINT("Wrong file format, in header #2");
			fclose(fd);
			DEBUG_END;
			return false;
		}
		DEBUG_PRINT("scanned string : %s", scannedString);
	}

	contours = new SContour[numContours];
	for (int iContour = 0; iContour < numContours; ++iContour)
	{
		SContour* currContour = &contours[iContour];
		currContour->id = iContour;

		if (fscanf(fd, "%d", &currContour->ns) != 1)
		{
			ERROR_PRINT("Wrong file format, number of sides for contour #%d",
					iContour);
			fclose(fd);
			DEBUG_END;
			return false;
		}

		if (fscanf(fd, "%lf", &currContour->plane.norm.x) != 1
				|| fscanf(fd, "%lf", &currContour->plane.norm.y) != 1
				|| fscanf(fd, "%lf", &currContour->plane.norm.z) != 1)
		{
			ERROR_PRINT("Wrong file format, "
					"in normal to plane for contour #%d",
					iContour);
			fclose(fd);
			DEBUG_END;
			return false;
		}

		currContour->plane.dist = 0.;

		currContour->sides = new SideOfContour[currContour->ns];

		for (int iSide = 0; iSide < currContour->ns; ++iSide)
		{
			SideOfContour* currSide = &currContour->sides[iSide];

			if (fscanf(fd, "%lf", &currSide->confidence) != 1)
			{
				ERROR_PRINT("Wrong file format,"
						"in confidence of side #%d of contour #%d",
						iSide, iContour);
				fclose(fd);
				DEBUG_END;
				return false;
			}

			if (fscanf(fd, "%d", &currSide->type) != 1)
			{
				ERROR_PRINT("Wrong file format,"
						"in type of side #%d of contour #%d",
						iSide, iContour);
				fclose(fd);
				DEBUG_END;
				return false;
			}

			if (fscanf(fd, "%lf", &currSide->A1.x) != 1
					|| fscanf(fd, "%lf", &currSide->A1.y) != 1
					|| fscanf(fd, "%lf", &currSide->A1.z) != 1)
			{
				ERROR_PRINT("Wrong file format,"
						"in A1 for side #%d of contour #%d",
						iSide, iContour);
				fclose(fd);
				DEBUG_END;
				return false;
			}

			if (fscanf(fd, "%lf", &currSide->A2.x) != 1
					|| fscanf(fd, "%lf", &currSide->A2.y) != 1
					|| fscanf(fd, "%lf", &currSide->A2.z) != 1)
			{
				ERROR_PRINT("Wrong file format,"
						"in A1 for side #%d of contour #%d",
						iSide, iContour);
				fclose(fd);
				DEBUG_END;
				return false;
			}
		}
	}

	fclose(fd);
	if (scannedString != NULL)
	{
		delete[] scannedString;
		scannedString = NULL;
	}

	fprint(stdout);
	DEBUG_END;
	return true;
}

#undef STD_SC_FORMAT_HEADER_SIZE_1
#undef STD_SC_FORMAT_HEADER_SIZE_2

void ShadeContourData::fprint(FILE* file)
{
	DEBUG_START;
	REGULAR_PRINT(file, "Dumping shade contour data. Number of contours: %d\n",
			numContours);
	for (int iContour = 0; iContour < numContours; ++iContour)
	{
		contours[iContour].my_fprint(file);
	}
	DEBUG_END;
}
