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
#include "Recoverer/ContourModeRecoverer.h"

static unsigned getContoursNumber(SupportFunctionDataPtr data)
{
	DEBUG_START;
	std::set<int> contourIDs;
	for (int i = 0; i < data->size(); ++i)
	{
		auto item = (*data)[i];
		int iContour = item.info->iContour;
		contourIDs.insert(iContour);
	}
	std::cout << "Number of shadow contours: " << contourIDs.size()
		<< std::endl;
	for (int iContour : contourIDs)
		ASSERT(iContour < int(contourIDs.size()) && "Wrong numeration");
	DEBUG_END;
	return contourIDs.size();
}

typedef std::vector<std::vector<SupportFunctionDataItem>> ContourVectorTy;
typedef std::vector<std::vector<unsigned>> SideIDsTy;

SideIDsTy getLongSidesIDs(const ContourVectorTy &contours,
		double edgeLengthLimit)
{
	DEBUG_START;
	SideIDsTy longSideIDs(contours.size());
	unsigned maxSidesNumberLocal = 0;
	for (auto &contour : contours)
	{
		maxSidesNumberLocal = std::max(maxSidesNumberLocal,
			unsigned(contour.size()));
		for (unsigned i = 0; i < contour.size(); ++i)
		{
			ASSERT(contour[i].info->iSide == int(i)
					&& "Wrong numeration");
		}
	}
	std::cout << "Maximal sides number per contour: "
		<< maxSidesNumberLocal << std::endl;

	std::vector<unsigned> numLongSidesLocal(maxSidesNumberLocal);
	for (unsigned i = 0; i < maxSidesNumberLocal; ++i)
		numLongSidesLocal[i] = 0.;

	unsigned numLongSides = 0;
	for (unsigned i = 0; i < contours.size(); ++i)
	{
		auto &contour = contours[i];
		int numLongSidesCurrent = 0;
		for (unsigned j = 0; j < contour.size(); ++j)
		{
			auto item = contour[j];
			double length = sqrt(
					item.info->segment.squared_length());
			if (length >= edgeLengthLimit)
			{
				++numLongSides;
				++numLongSidesCurrent;
				longSideIDs[i].push_back(j);
			}
		}
		std::cout << "Contour " << i << " has " << numLongSidesCurrent
			<< " long sides, and total " << contour.size()
			<< std::endl;
		++numLongSidesLocal[numLongSidesCurrent];
	}

	std::cout << "There are " << numLongSides << " contour sides, which "
		"length is greater than " << edgeLengthLimit << std::endl;
	for (unsigned i = 0; i < maxSidesNumberLocal; ++i)
	{
		std::cout << "    Number of contour with " << i <<
			" long sides: " << numLongSidesLocal[i] << std::endl;
	}
	DEBUG_END;
	return longSideIDs;
}

typedef std::vector<std::vector<double>> AnglesTy;

static AnglesTy calculateAngles(const ContourVectorTy &contours,
		const SideIDsTy &longSideIDs)
{
	DEBUG_START;
	Vector_3 ez(0., 0., 1.);
	AnglesTy angles(contours.size());
	bool fullLog = getenv("FULL_LOG") != nullptr;
	for (unsigned iContour = 0; iContour < contours.size(); ++iContour)
	{
		if (fullLog)
			std::cout << "Angles for contour #" << iContour
				<< ", which has "
				<< longSideIDs[iContour].size()
				<< " long sides from total "
				<< contours[iContour].size()
				<< " sides:" << std::endl;
		unsigned iLong = 0;
		for (unsigned iSide = 0; iSide < contours[iContour].size();
				++iSide)
		{
			SupportFunctionDataItem item
				= contours[iContour][iSide];
			Vector_3 norm = item.direction;
			double angle = acos(norm * ez);
			Vector_3 product = CGAL::cross_product(ez, norm);
			if (product * item.info->normalShadow < 0.)
				angle *= -1.;
			angles[iContour].push_back(angle);
			double length = sqrt(
				item.info->segment.squared_length());
			if (longSideIDs[iContour][iLong] == iSide)
			{
				std::cout << COLOUR_YELLOW;
				if (length > 1.)
					std::cout << COLOUR_RED;
				++iLong;
			}
			if (fullLog)
			{
				std::cout << "    Angle for side #" << iSide
					<< ":\t";
				if (angle >= 0.)
					std::cout << " ";
				std::cout << angle << ",\tlength: "
					<< length << std::endl;
			}
			std::cout << COLOUR_NORM;
		}
	}
	DEBUG_END;
	return angles;
}

void ContourModeRecoverer::run()
{
	DEBUG_START;
	std::cout << "Starting contour mode recovering..." << std::endl;
	std::cout << "There are " << data->size() << " support items "
		<< std::endl;
	ASSERT(data->size() > 0 && "The data must be non-empty");

	double edgeLengthLimit = 0.;
	if (!tryGetenvDouble("EDGE_LENGTH_LIMIT", edgeLengthLimit))
	{
		ERROR_PRINT("Failed to get EDGE_LENGTH_LIMIT");
		DEBUG_END;
		return;
	}

	ContourVectorTy contours(getContoursNumber(data));
	for (int i = 0; i < data->size(); ++i)
	{
		SupportFunctionDataItem item = (*data)[i];
		int iContour = item.info->iContour;
		contours[iContour].push_back(item);
	}

	SideIDsTy longSideIDs = getLongSidesIDs(contours, edgeLengthLimit);
	AnglesTy angles = calculateAngles(contours, longSideIDs);
	DEBUG_END;
}
