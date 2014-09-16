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
#include "DataContainers/ShadeContourData/ShadeContourData.h"
#include "DataContainers/ShadeContourData/SContour/SContour.h"

ShadeContourData::ShadeContourData(shared_ptr<Polyhedron> p) :
				PData(p),
				numContours(0),
				contours(NULL)
{
	DEBUG_START;
	DEBUG_END;
}

ShadeContourData::ShadeContourData(const ShadeContourData& data) :
				PData(),
				numContours(data.numContours),
				contours(NULL)
{
	DEBUG_START;
	contours = new SContour[numContours];
	for (int i = 0; i < numContours; ++i)
	{
		contours[i] = data.contours[i];
	}
	DEBUG_END;
}

ShadeContourData::ShadeContourData(const ShadeContourDataPtr data) :
				PData(),
				numContours(data->numContours),
				contours(NULL)
{
	DEBUG_START;
	contours = new SContour[numContours];
	for (int i = 0; i < numContours; ++i)
	{
		contours[i] = data->contours[i];
	}
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
		numContours = 0;
	}
	DEBUG_END;
}

bool ShadeContourData::fscanDefault(const char* fileNameContours)
{
	DEBUG_START;
	FILE* fd = (FILE*) fopen(fileNameContours, "r");

	if (!fd)
	{
		ERROR_PRINT("Failed to open file %s", fileNameContours);
		DEBUG_END;
		return false;
	}

	bool fscanReturnValue = fscanDefault(fd);
	fclose(fd);
	DEBUG_END;
	return fscanReturnValue;
}

#define STD_SC_FORMAT_HEADER_SIZE_1 2
#define STD_SC_FORMAT_HEADER_SIZE_2 31

bool ShadeContourData::fscanDefault(FILE* fd)
{
	DEBUG_START;

	char* scannedString = new char[255];
	for (int i = 0; i < STD_SC_FORMAT_HEADER_SIZE_1; ++i)
	{
		if (fscanf(fd, "%s", scannedString) != 1)
		{
			ERROR_PRINT("Wrong file format, in header #1");
			DEBUG_END;
			return false;
		}
		DEBUG_PRINT("scanned string : %s", scannedString);
	}

	if (fscanf(fd, "%d", &numContours) != 1)
	{
		ERROR_PRINT("Wrong file format, in number of contours");
		DEBUG_END;
		return false;
	}

	for (int i = 0; i < STD_SC_FORMAT_HEADER_SIZE_2; ++i)
	{
		if (fscanf(fd, "%s", scannedString) != 1)
		{
			ERROR_PRINT("Wrong file format, in header #2");
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
				DEBUG_END;
				return false;
			}

			if (fscanf(fd, "%d", (int*)&currSide->type) != 1)
			{
				ERROR_PRINT("Wrong file format,"
						"in type of side #%d of contour #%d",
						iSide, iContour);
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
				DEBUG_END;
				return false;
			}
		}
	}

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

void ShadeContourData::fprintDefault(FILE* file)
{
	DEBUG_START;
	ALWAYS_PRINT(file, "# num_shadow_contours\n");
	ALWAYS_PRINT(file, "%d\n", numContours);
	ALWAYS_PRINT(file, "# contour:  num_sides   normal_to_plane\n");
	ALWAYS_PRINT(file, "#           confidence   type\n");
	ALWAYS_PRINT(file, "#           x y z coordinates of point 1 in plane of projection\n");
	ALWAYS_PRINT(file, "#           x y z coordinates of point 2 in plane of projection\n");

	for (int i = 0; i < numContours; ++i)
	{
		ALWAYS_PRINT(file, "\n");
		contours[i].fprintDefault(file);
	}
	DEBUG_END;
}

void ShadeContourData::fprintDefault(const char* fileName)
{
	DEBUG_START;
	FILE* fd = (FILE*) fopen(fileName, "w");

	if (!fd)
	{
		ERROR_PRINT("Failed to open file %s", fileName);
		DEBUG_END;
		return;
	}

	fprintDefault(fd);
	fclose(fd);
	DEBUG_END;
}

bool ShadeContourData::operator ==(const ShadeContourData& contourData) const
{
	DEBUG_START;
	if (numContours != contourData.numContours)
	{
		DEBUG_END;
		return false;
	}

	for (int iContour = 0; iContour < numContours; ++iContour)
	{
		if (contours[iContour] != contourData.contours[iContour])
		{
			DEBUG_END;
			return false;
		}
	}
	DEBUG_END;
	return true;
}

bool ShadeContourData::operator !=(const ShadeContourData& contourData) const
{
	DEBUG_START;
	bool returnValue = !(*this == contourData);
	DEBUG_END;
	return returnValue;
}
