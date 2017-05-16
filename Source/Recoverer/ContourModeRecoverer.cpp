/*
 * Copyright (c) 2009-2017 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file ContourModeRecoverer.cpp
 * @brief Recovering in so-called "contour mode" (implementation)
 */

#include <iostream>
#include "Common.h"
#include "DebugAssert.h"
#include "DebugPrint.h"
#include "DataContainers/ShadowContourData/SContour/SContour.h"
#include "Recoverer/ContourModeRecoverer.h"

void ContourModeRecoverer::run()
{
	DEBUG_START;
	std::cout << "Starting contour mode recovering..." << std::endl;
	std::cout << "There are " << data->numContours << " contours"
		<< std::endl;
	ASSERT(!data->empty() && "The data must be non-empty");

	double edgeLengthLimit = 0.;
	if (!tryGetenvDouble("EDGE_LENGTH_LIMIT", edgeLengthLimit))
	{
		ERROR_PRINT("Failed to get EDGE_LENGTH_LIMIT");
		DEBUG_END;
		return;
	}

	int numLongSides = 0;
	int numSidesTotal = 0;
	int maxSidesNumberLocal = 0;
	for (int i = 0; i < data->numContours; ++i)
		maxSidesNumberLocal = std::max(maxSidesNumberLocal,
				data->contours[i].ns);
	std::cout << "Maximal sides number per contour: "
		<< maxSidesNumberLocal << std::endl;

	std::vector<int> numLongSidesLocal(maxSidesNumberLocal);
	std::vector<std::vector<double>> angles(data->numContours);
	for (int i = 0; i < data->numContours; ++i)
	{
		SContour *contour = &data->contours[i];
		ASSERT(contour->id == i && "Wrong numeration");
		int numLongSidesCurrent = 0;
		angles[i] = std::vector<double>(contour->ns);
		for (int j = 0; j < contour->ns; ++j)
		{
			++numSidesTotal;
			SideOfContour *side = &contour->sides[j];
			Vector_3 A1 = side->A1;
			Vector_3 A2 = side->A2;
			double length = sqrt((A1-A2).squared_length());
			if (length >= edgeLengthLimit)
			{
				++numLongSides;
				++numLongSidesCurrent;
			}
		}
		++numLongSidesLocal[numLongSidesCurrent];
	}
	std::cout << "Total number of sides: " << numSidesTotal << std::endl;
	std::cout << "There are " << numLongSides << " contour sides, which "
		"length is greater than " << edgeLengthLimit << std::endl;
	for (int i = 0; i < maxSidesNumberLocal; ++i)
	{
		std::cout << "    Number of contour with " << i <<
			" long sides: " << numLongSidesLocal[i] << std::endl;
	}
	DEBUG_END;
}
