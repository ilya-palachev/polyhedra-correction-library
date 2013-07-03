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

#define STD_SC_FORMAT_HEADER_SIZE_1 1
#define STD_SC_FORMAT_HEADER_SIZE_2 4

void ShadeContourData::fscanDefault(char* fileNameContours)
{
	FILE* fd = (FILE*) fopen(fileNameContours, "r");

	if (!fd)
	{
		ERROR_PRINT("Failed to open file %s", fileNameContours);
		return;
	}

	char* scannedString = new char[255];
	for (int i = 0; i < STD_SC_FORMAT_HEADER_SIZE_1; ++i)
	{
		if (fscanf(fd, "%s", scannedString) != 1)
		{
			ERROR_PRINT("Wrong file format, in header #1");
			fclose(fd);
			return;
		}
	}

	if (fscanf(fd, "%d", &numContours) != 1)
	{
		ERROR_PRINT("Wrong file format, in number of contours");
		fclose(fd);
		return;
	}

	for (int i = 0; i < STD_SC_FORMAT_HEADER_SIZE_2; ++i)
	{
		if (fscanf(fd, "%s", scannedString) != 1)
		{
			ERROR_PRINT("Wrong file format, in header #2");
			fclose(fd);
			return;
		}
	}

	contours = new SContour[numContours];
	for (int iContour = 0; iContour < numContours; ++iContour)
	{
		if (fscanf(fd, "%s", scannedString) != 0)
		{
			ERROR_PRINT("Wrong file format, in empty line before contour #%d",
					iContour);
			fclose(fd);
			return;
		}

		SContour* currContour = &contours[iContour];

		if (fscanf(fd, "%d", &currContour->ns) != 1)
		{
			ERROR_PRINT("Wrong file format, number of sides for contour #%d",
					iContour);
			fclose(fd);
			return;
		}

		if (fscanf(fd, "%lf", &currContour->plane.norm.x) != 1
				|| fscanf(fd, "%lf", &currContour->plane.norm.y) != 1
				|| fscanf(fd, "%lf", &currContour->plane.norm.z) != 1)
		{
			ERROR_PRINT("Wrong file format, "
					"in normal to plane for contour #%d",
					iContour);
			fclose(fd);
			return;
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
				return;
			}

			if (fscanf(fd, "%d", &currSide->type) != 1)
			{
				ERROR_PRINT("Wrong file format,"
						"in type of side #%d of contour #%d",
						iSide, iContour);
				fclose(fd);
				return;
			}

			if (fscanf(fd, "%lf", &currSide->A1.x) != 1
					|| fscanf(fd, "%lf", &currSide->A1.y) != 1
					|| fscanf(fd, "%lf", &currSide->A1.z) != 1)
			{
				ERROR_PRINT("Wrong file format,"
						"in A1 for side #%d of contour #%d",
						iSide, iContour);
				fclose(fd);
				return;
			}

			if (fscanf(fd, "%lf", &currSide->A2.x) != 1
					|| fscanf(fd, "%lf", &currSide->A2.y) != 1
					|| fscanf(fd, "%lf", &currSide->A2.z) != 1)
			{
				ERROR_PRINT("Wrong file format,"
						"in A1 for side #%d of contour #%d",
						iSide, iContour);
				fclose(fd);
				return;
			}
		}
	}

	fclose(fd);
	if (scannedString != NULL)
	{
		delete[] scannedString;
		scannedString = NULL;
	}
}

#undef STD_SC_FORMAT_HEADER_SIZE_1
#undef STD_SC_FORMAT_HEADER_SIZE_2
