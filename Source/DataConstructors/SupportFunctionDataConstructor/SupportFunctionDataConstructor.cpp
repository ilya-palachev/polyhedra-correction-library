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

/**
 * @file SupportFunctionDataConstructor.cpp
 * @brief Constructor class for support function data - implementation.
 */

#include <cmath>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataConstructor.h"
#include "DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataItemExtractor.h"
#include "DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataItemExtractorByPlane.h"
#include "DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataItemExtractorByPoints.h"
#include "Analyzers/SizeCalculator/SizeCalculator.h"

/**
 * Creates shadow contours from given shadow contours and balances
 * them so that to provide assertion that the mass center lies directly
 * in the coordinate center.
 *
 * @param data	Shadow contour data
 * @return	Balanced shadow contour data
 */
static ShadowContourDataPtr balanceShadowContourData(
		ShadowContourDataPtr data);

/**
 * Creates shadow contours from given shadow contours and shift them all to the
 * constant std::vector.
 *
 * @param data	Shadow contour data
 * @return	Shifted shadow contour data
 */
static ShadowContourDataPtr shiftShadowContourData(
		ShadowContourDataPtr data, const Vector3d shift);

/**
 * Creates shadow contours from given shadow contours and convexifies
 * them so that to make them all convex.
 *
 * @param data	Shadow contour data
 * @return	Convexified shadow contour data
 */
static ShadowContourDataPtr convexifyShadowContourData(
		ShadowContourDataPtr data);

/**
 * Extracts support function data items from given shadow contours.
 *
 * @param data				Shadow contour data
 * @param ifExtractItemsByPoints	Whether to "by-points" extractor is
 * enabled
 * @return		Support fucntion data items
 */
static std::vector<SupportFunctionDataItem> extractSupportFunctionDataItems(
		ShadowContourDataPtr data, bool ifExtractItemsByPoints);

/**
 * Extracts support function data items from given shadow contour.
 *
 * @param contour	Shadow contour
 * @param extractor	The extractor that must be used.
 * enabled
 * @return		Support fucntion data items
 */
static std::vector<SupportFunctionDataItem> extractSupportFunctionDataItems(
		SContour *contour, SupportFunctionDataItemExtractor *extractor);

/**
 * Checks whether the support function data in are all pairwise unequal.
 *
 * @param items	Support function data items
 * @return True if there is no equalities.
 */
bool checkSupportFunctionDataItemsInequality(
		std::vector<SupportFunctionDataItem> items);

SupportFunctionDataConstructor::SupportFunctionDataConstructor() :
	ifBalanceShadowContours(false),
	ifConvexifyShadowContours(false),
	ifExtractItemsByPoints(false)
{
	DEBUG_START;
	DEBUG_END;
}

SupportFunctionDataConstructor::~SupportFunctionDataConstructor()
{
	DEBUG_START;
	DEBUG_END;
}

void SupportFunctionDataConstructor::enableBalanceShadowContours()
{
	DEBUG_START;
	ifBalanceShadowContours = true;
	DEBUG_END;
}


void SupportFunctionDataConstructor::enableConvexifyShadowContour()
{
	DEBUG_START;
	ifConvexifyShadowContours = true;
	DEBUG_END;
}

void SupportFunctionDataConstructor::enableExtractItemsByPoints()
{
	DEBUG_START;
	ifExtractItemsByPoints = true;
	DEBUG_END;
}

SupportFunctionDataPtr SupportFunctionDataConstructor::run(
		ShadowContourDataPtr data)
{
	DEBUG_START;
	ASSERT(data);
	ASSERT(data->numContours > 0);
	/*
	 * TODO: Dump initial version of shadow contour data to file.
	 */
	/* Balance shadow contour data if demanded. */
	if (ifBalanceShadowContours)
	{
		auto dataBalanced = balanceShadowContourData(data);
		/*
		 * TODO: Dump balanced version of shadow contour data to file.
		 */
		data = dataBalanced;
	}

	/* Convexify shadow contour data if demanded. */
	if (ifConvexifyShadowContours)
	{
		auto dataConvexified = convexifyShadowContourData(data);
		/*
		 * TODO: Dump convexified version of shadow contour data to
		 * file.
		 */
		data = dataConvexified;
	}

	/* Iterate through the array of contours and get data from each. */
	auto items = extractSupportFunctionDataItems(data,
			ifExtractItemsByPoints);

	SupportFunctionDataPtr supportFunctionData(
			new SupportFunctionData(items));
	
	DEBUG_END;
	return supportFunctionData;
}

static ShadowContourDataPtr balanceShadowContourData(ShadowContourDataPtr data)
{
	DEBUG_START;
	/* Construct polyhedron which facets are the contours. */
	PolyhedronPtr p(new Polyhedron(data));
	p->set_parent_polyhedron_in_facets();

	/* Calculate the mass center of contours. */
	SizeCalculator *sizeCalculator = new SizeCalculator(p);
	Vector3d center = sizeCalculator->calculateSurfaceCenter();
	DEBUG_PRINT("Center of contours = (%lf, %lf, %lf)",
		                center.x, center.y, center.z);

	/*
	 * Shift all contours on z component of the std::vector of mass center.
	 */
	Vector3d ez(0., 0., 1.);
	auto dataShifted = shiftShadowContourData(data, - (ez * center) * ez);
	DEBUG_END;
	return dataShifted;
}

static ShadowContourDataPtr shiftShadowContourData(
		ShadowContourDataPtr data, const Vector3d shift)
{
	DEBUG_START;

	/* Ensure that given shift is a std::vector with finite elements. */
	ASSERT(std::isfinite(shift.x));
	ASSERT(std::isfinite(shift.y));
	ASSERT(std::isfinite(shift.z));

	/* Copy shadow contour data to new data. */
	ShadowContourDataPtr dataShifted(new ShadowContourData(data));

	/* Go through the array of contours and balance each of them. */
	for (int iContour = 0; iContour < dataShifted->numContours; ++iContour)
	{
		SContour *contour = &dataShifted->contours[iContour];
		for (int iSide = 0; iSide < contour->ns; ++iSide)
		{
			SideOfContour *side = &contour->sides[iSide];
			side->A1 += shift;
			side->A2 += shift;
		}
		/*
		 * If we want new plane to include point
		 * (x + xt, y + yt, z + zt) for each point (x, y, z) that is
		 * included in plane
		 * a * x + b * y + c * z + d = 0
		 * then its free coefficient must become equal to
		 * d - a * xt - b * yt - c * zt
		 *
		 * TODO: For our case usually we shift points in vertical planes
		 * on a vertical vector, thus, the plane will not actually move.
		 */
		contour->plane.dist -= contour->plane.norm * shift;
	}
	DEBUG_END;
	return dataShifted;
}

static ShadowContourDataPtr convexifyShadowContourData(
		ShadowContourDataPtr data)
{
	DEBUG_START;

	/* Allocate shadow contour data for new data. */
	ShadowContourDataPtr dataConvexified(new ShadowContourData(
				data->numContours));

	/* Go through the array of contours and convexify each of them. */
	for (int iContour = 0; iContour < dataConvexified->numContours;
			++iContour)
	{
		SContour *contour = &dataConvexified->contours[iContour];
		SContour *contourConvexified = contour->convexify();
		dataConvexified->contours[iContour] = *contourConvexified;
	}
	DEBUG_END;
	return dataConvexified;
}

static std::vector<SupportFunctionDataItem> extractSupportFunctionDataItems(
		ShadowContourDataPtr data, bool ifExtractItemsByPoints)
{
	DEBUG_START;
	ASSERT(data);
	ASSERT(data->numContours > 0);

	std::vector<SupportFunctionDataItem> items;

	for (int iContour = 0; iContour < data->numContours; ++iContour)
	{
		SContour *contour = &data->contours[iContour];
		SupportFunctionDataItemExtractor *extractor = NULL;
		if (ifExtractItemsByPoints)
		{
			extractor =
				static_cast<SupportFunctionDataItemExtractor*>(
				new SupportFunctionDataItemExtractorByPoints());
		}
		else
		{
			auto extractorByPlane = new
				SupportFunctionDataItemExtractorByPlane();
			extractorByPlane->setPlane(contour->plane);
			extractor =
				static_cast<SupportFunctionDataItemExtractor*>(
						extractorByPlane);
		}
		auto itemsPortion = extractSupportFunctionDataItems(contour,
				extractor);
		items.insert(items.end(), itemsPortion.begin(),
				itemsPortion.end()); /* Collect items. */
	}

	/* Check the result. */
	ASSERT(checkSupportFunctionDataItemsInequality(items));

	return items;
}


static std::vector<SupportFunctionDataItem> extractSupportFunctionDataItems(
		SContour *contour, SupportFunctionDataItemExtractor *extractor)
{
	/* Check that the input contour is correct. */
	ASSERT(contour);
	ASSERT(contour->sides);
	ASSERT(contour->ns > 0);
	ASSERT(!!contour->plane.norm); /* Check that normal != 0 */
	ASSERT(fabs(contour->plane.norm.z) < EPS_MIN_DOUBLE);

	std::vector<SupportFunctionDataItem> items;

	/* Iterate through the array of sides of current contour. */
	for (int iSide = 0; iSide < contour->ns; ++iSide)
	{
		SideOfContour *side = &contour->sides[iSide];

		SupportFunctionDataItem item = extractor->run(side);
		items.push_back(item);
	}
	DEBUG_END;
	return items;
}

#define EPS_DATA_ITEMS_EQUAL 1e-15

bool checkSupportFunctionDataItemsInequality(
		std::vector<SupportFunctionDataItem> items)
{
	DEBUG_START;
	for (auto item = items.begin(); item != items.end(); ++item)
	{
		for (auto itemPrev = items.begin(); itemPrev != item;
				++itemPrev)
		{
			if (equal(item->value, itemPrev->value,
					EPS_DATA_ITEMS_EQUAL)
				 && equal(item->direction, itemPrev->direction,
					EPS_DATA_ITEMS_EQUAL))
			{
				return false;
			}
		}
	}
	DEBUG_END;
	return true;
}
