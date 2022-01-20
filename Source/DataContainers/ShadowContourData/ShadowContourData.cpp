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

#include <iostream>
#include <fstream>
#include <sys/time.h>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "DataContainers/ShadowContourData/ShadowContourData.h"
#include "DataContainers/ShadowContourData/SContour/SContour.h"
#include "DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataConstructor.h"

ShadowContourData::ShadowContourData() : PData(), numContours(0), contours(NULL)
{
	DEBUG_START;
	DEBUG_END;
}

ShadowContourData::ShadowContourData(PolyhedronPtr p) : PData(p), numContours(0), contours(NULL)
{
	DEBUG_START;
	DEBUG_END;
}

ShadowContourData::ShadowContourData(const ShadowContourData &data) :
	PData(), numContours(data.numContours), contours(NULL)
{
	DEBUG_START;
	contours = new SContour[numContours];
	for (int i = 0; i < numContours; ++i)
	{
		contours[i] = data.contours[i];
	}
	DEBUG_END;
}

ShadowContourData::ShadowContourData(const ShadowContourDataPtr data) :
	PData(), numContours(data->numContours), contours(NULL)
{
	DEBUG_START;
	contours = new SContour[numContours];
	for (int i = 0; i < numContours; ++i)
	{
		contours[i] = data->contours[i];
	}
	DEBUG_END;
}

ShadowContourData::ShadowContourData(const int numContoursNeeded) :
	PData(), numContours(numContoursNeeded), contours(new SContour[numContoursNeeded])
{
	DEBUG_START;
	DEBUG_END;
}

ShadowContourData::~ShadowContourData()
{
	DEBUG_START;
	DEBUG_PRINT("Shadow contour data is being deleted now!");
	if (contours != NULL)
	{
		delete[] contours;
		contours = NULL;
		numContours = 0;
	}
	DEBUG_END;
}

bool ShadowContourData::fscanDefault(const char *fileNameContours)
{
	DEBUG_START;
	FILE *fd = (FILE *)fopen(fileNameContours, "r");

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

bool ShadowContourData::fscanDefault(FILE *fd)
{
	DEBUG_START;

	char *scannedString = new char[255];
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
		SContour *currContour = &contours[iContour];
		currContour->id = iContour;

		if (fscanf(fd, "%d", &currContour->ns) != 1)
		{
			ERROR_PRINT("Wrong file format, number of sides for contour #%d", iContour);
			DEBUG_END;
			return false;
		}

		if (fscanf(fd, "%lf", &currContour->plane.norm.x) != 1 || fscanf(fd, "%lf", &currContour->plane.norm.y) != 1 ||
			fscanf(fd, "%lf", &currContour->plane.norm.z) != 1)
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
			SideOfContour *currSide = &currContour->sides[iSide];

			if (fscanf(fd, "%lf", &currSide->confidence) != 1)
			{
				ERROR_PRINT("Wrong file format,"
							"in confidence of side #%d of contour #%d",
							iSide, iContour);
				DEBUG_END;
				return false;
			}

			if (fscanf(fd, "%d", (int *)&currSide->type) != 1)
			{
				ERROR_PRINT("Wrong file format,"
							"in type of side #%d of contour #%d",
							iSide, iContour);
				DEBUG_END;
				return false;
			}

			if (fscanf(fd, "%lf", &currSide->A1.x) != 1 || fscanf(fd, "%lf", &currSide->A1.y) != 1 ||
				fscanf(fd, "%lf", &currSide->A1.z) != 1)
			{
				ERROR_PRINT("Wrong file format,"
							"in A1 for side #%d of contour #%d",
							iSide, iContour);
				DEBUG_END;
				return false;
			}

			if (fscanf(fd, "%lf", &currSide->A2.x) != 1 || fscanf(fd, "%lf", &currSide->A2.y) != 1 ||
				fscanf(fd, "%lf", &currSide->A2.z) != 1)
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

void ShadowContourData::fprint(FILE *file)
{
	DEBUG_START;
	REGULAR_PRINT(file, "Dumping shadow contour data. Number of contours: %d\n", numContours);
	for (int iContour = 0; iContour < numContours; ++iContour)
	{
		contours[iContour].my_fprint(file);
	}
	DEBUG_END;
}

void ShadowContourData::fprintDefault(FILE *file)
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

void ShadowContourData::fprintDefault(const char *fileName)
{
	DEBUG_START;
	FILE *fd = (FILE *)fopen(fileName, "w");

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

std::ostream &operator<<(std::ostream &stream, ShadowContourData &data)
{
	DEBUG_START;
	char *name = tmpnam(NULL);
	data.fprintDefault(name);
	std::ifstream tmpstream;
	tmpstream.open(name, std::ifstream::in);
	stream << tmpstream.rdbuf();
	tmpstream.close();
	DEBUG_END;
	return stream;
}

bool ShadowContourData::operator==(const ShadowContourData &contourData) const
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

bool ShadowContourData::operator!=(const ShadowContourData &contourData) const
{
	DEBUG_START;
	bool returnValue = !(*this == contourData);
	DEBUG_END;
	return returnValue;
}

SupportFunctionDataPtr ShadowContourData::calculateSupportData()
{
	DEBUG_START;
	SupportFunctionDataConstructor constructor;
	auto data = constructor.run(this->shared_from_this(), IF_ANALYZE_ALL_CONTOURS);
	DEBUG_END;
	return data;
}

bool ShadowContourData::empty()
{
	DEBUG_START;
	if (numContours <= 0)
	{
		DEBUG_END;
		return false;
	}

	int numSidesTotal = 0;
	for (int i = 0; i < numContours; ++i)
	{
		int numSides = contours[i].ns;
		ASSERT(numSides >= 0);
		numSidesTotal += numSides;
	}
	DEBUG_PRINT("Total number of sides = %d", numSidesTotal);
	bool result = (numSidesTotal <= 0);
	DEBUG_PRINT("Result: %d", result);
	DEBUG_END;
	return result;
}

/**
 * Generates random number d such that |d| <= maxDelta
 *
 * @param maxDelta	maximum absolute limit of generated number
 */
static double genRandomDouble(double maxDelta)
{
	DEBUG_START;
	// srand((unsigned) time(0));
	struct timeval t1;
	gettimeofday(&t1, NULL);
	srand(t1.tv_usec * t1.tv_sec);

	int randomInteger = rand();
	double randomDouble = randomInteger;
	const double halfRandMax = RAND_MAX * 0.5;
	randomDouble -= halfRandMax;
	randomDouble /= halfRandMax;

	randomDouble *= maxDelta;

	DEBUG_END;
	return randomDouble;
}

/**
 * Shifts all points of contours on random double vectors
 *
 * @param maxDelta	Maximum delta in shift vectors' coordinates
 */
void ShadowContourData::shiftRandomly(double maxDelta)
{
	DEBUG_START;
	for (int iContour = 0; iContour < numContours; ++iContour)
	{
		SContour *contour = &contours[iContour];
		if (contour->ns <= 0)
			continue;

		for (int iSide = 0; iSide < contour->ns; ++iSide)
		{
			SideOfContour *side = &contour->sides[iSide];
			Vector3d A1_backup DEBUG_VARIABLE = side->A1;
			side->A1 += Vector3d(genRandomDouble(maxDelta), genRandomDouble(maxDelta), genRandomDouble(maxDelta));
			DEBUG_PRINT("shift: (%lf, %lf, %lf) -> "
						"(%lf, %lf, %lf)",
						A1_backup.x, A1_backup.y, A1_backup.z, side->A1.x, side->A1.y, side->A1.z);
			side->A1 = contour->plane.project(side->A1);
		}
		for (int iSide = 0; iSide < contour->ns - 1; ++iSide)
		{
			contour->sides[iSide].A2 = contour->sides[iSide + 1].A1;
		}
		contour->sides[contour->ns - 1].A2 = contour->sides[0].A1;
	}
	DEBUG_END;
}
