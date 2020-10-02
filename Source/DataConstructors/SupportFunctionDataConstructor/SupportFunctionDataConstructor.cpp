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
#include "PCLDumper.h"

/**
 * Creates shadow contours from given shadow contours and balances
 * them so that to provide assertion that the mass center lies directly
 * in the coordinate center.
 *
 * @param data	Shadow contour data
 * @return	Balanced shadow contour data
 */
static std::pair<ShadowContourDataPtr, Vector3d> balanceShadowContourData(
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
 * @param ifExtractItemsByPoints_	Whether to "by-points" extractor is
 * 					enabled
 * @param numMaxContours		The number of contours to be analyzed.
 * @return				Support fucntion data items
 */
static std::vector<SupportFunctionDataItem> extractSupportFunctionDataItems(
		ShadowContourDataPtr data, bool ifExtractItemsByPoints_,
		int numMaxContours);

/**
 * Extracts support function data items from given shadow contour.
 *
 * @param contour		Shadow contour
 * @param extractor		The extractor that must be used.
 * 				enabled
 * @return			Support fucntion data items.
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
	ifBalanceShadowContours_(false),
	ifConvexifyShadowContours_(false),
	ifExtractItemsByPoints_(false),
	balancingVector_(0., 0., 0.),
	tangientIDs_()
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
	ifBalanceShadowContours_ = true;
	DEBUG_END;
}


void SupportFunctionDataConstructor::enableConvexifyShadowContour()
{
	DEBUG_START;
	ifConvexifyShadowContours_ = true;
	DEBUG_END;
}

void SupportFunctionDataConstructor::enableExtractItemsByPoints()
{
	DEBUG_START;
	ifExtractItemsByPoints_ = true;
	DEBUG_END;
}

SupportFunctionDataPtr SupportFunctionDataConstructor::run(
		ShadowContourDataPtr data, int numMaxContours)
{
	DEBUG_START;
	ASSERT(data);
	ASSERT(data->numContours > 0);
	ASSERT(!data->empty());

	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
		"initial-contours.dat") << *data;
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
		"initial-contours.ply") << Polyhedron(data);
	/* Balance shadow contour data if demanded. */
	if (ifBalanceShadowContours_)
	{
		auto pair = balanceShadowContourData(data);
		auto dataBalanced = pair.first;
		balancingVector_ = pair.second;
		data = dataBalanced;
		globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
			"balanced-contours.dat") << *data;
		globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
			"balanced-contours.ply") << Polyhedron(data);
	}

	/* Convexify shadow contour data if demanded. */
	if (ifConvexifyShadowContours_)
	{
		auto dataConvexified = convexifyShadowContourData(data);
		data = dataConvexified;
		globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
			"convexified-contours.dat") << *data;
		globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
			"convexified-contours.ply") << Polyhedron(data);
	}

	/* Iterate through the array of contours and get data from each. */
	auto items = extractSupportFunctionDataItems(data,
			ifExtractItemsByPoints_, numMaxContours);

	SupportFunctionDataPtr supportFunctionData(
			new SupportFunctionData(items));
	
	DEBUG_END;
	return supportFunctionData;
}

static std::pair<ShadowContourDataPtr, Vector3d> balanceShadowContourData(ShadowContourDataPtr data)
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
	Vector3d shiftingVector = - (ez * center) * ez;
	auto dataShifted = shiftShadowContourData(data, shiftingVector);
	DEBUG_END;
	return std::pair<ShadowContourDataPtr, Vector3d>(dataShifted,
			shiftingVector);
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
		ASSERT(!!contour->plane.norm);
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
	ShadowContourDataPtr dataConvexified(new ShadowContourData(data));

	/* Go through the array of contours and convexify each of them. */
	for (int iContour = 0; iContour < dataConvexified->numContours;
			++iContour)
	{
		SContour *contour = &dataConvexified->contours[iContour];
		ASSERT(!!contour->plane.norm);
		SContour *contourConvexified = contour->convexify();
		dataConvexified->contours[iContour] = *contourConvexified;
	}
	DEBUG_END;
	return dataConvexified;
}

static std::vector<SupportFunctionDataItem> extractSupportFunctionDataItems(
		ShadowContourDataPtr data, bool ifExtractItemsByPoints_,
		int numMaxContours)
{
	DEBUG_START;
	ASSERT(data);
	ASSERT(data->numContours > 0);

	std::vector<SupportFunctionDataItem> items;

	double factor = (double) data->numContours / (double) numMaxContours;
	int numContoursAnalyzed = 0;
	for (int iContour = 0; iContour < data->numContours; ++iContour)
	{
		if (numMaxContours != IF_ANALYZE_ALL_CONTOURS
			&& numMaxContours < data->numContours
			&& iContour != (int) floor(factor * numContoursAnalyzed))
		{
			continue;
		}
		++numContoursAnalyzed;

		SContour *contour = &data->contours[iContour];
		SupportFunctionDataItemExtractor *extractor = NULL;
		if (ifExtractItemsByPoints_)
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
	ALWAYS_PRINT(stdout, "We are analyzing only %d contours.\n",
			numContoursAnalyzed - 1);

	int iPrevZero = INT_NOT_INITIALIZED;
	int iCurr = 0;
	for (auto &item : items)
	{
		ASSERT(item.info->iContour != INT_NOT_INITIALIZED);
		ASSERT(item.info->numSidesContour != INT_NOT_INITIALIZED);
		ASSERT(item.info->iSide != INT_NOT_INITIALIZED);
		ASSERT(item.info->iNext == INT_NOT_INITIALIZED);
		if (item.info->iSide == 0)
			iPrevZero = iCurr;
		item.info->iNext =
			item.info->iSide < item.info->numSidesContour - 1
			? iCurr + 1 : iPrevZero;
		++iCurr;
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
		item.info = SupportFunctionDataItemInfoPtr(new
				SupportFunctionDataItemInfo());
		item.info->iContour = contour->id;
		item.info->numSidesContour = contour->ns;
		item.info->iSide = iSide;
		item.info->segment = Segment_3(Point_3(side->A1),
				Point_3(side->A2));
		item.info->normalShadow = contour->plane.norm;
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

SupportFunctionDataPtr SupportFunctionDataConstructor::run(
	std::vector<Point_3> directions,
	Polyhedron_3 polyhedron)
{
	DEBUG_START;
	std::vector<Vector_3> vertices = polyhedron.getVertices();
	ASSERT(vertices.size() > 0);

	std::vector<SupportFunctionDataItem> items;

	int iDirection = 0;
	int numNegative = 0;
	for (auto &direction: directions)
	{
		Vector_3 vDirection = direction - CGAL::ORIGIN;
		double scalarProductMax = -1e100;
		Vector_3 supportPoint(0., 0., 0.);
		int iVertexTangient = 0;
		int iVertex = 0;
		for (auto &vertex: vertices)
		{
			double scalarProduct = vDirection * vertex;
			if (scalarProduct > scalarProductMax)
			{
				scalarProductMax = scalarProduct;
				supportPoint = vertex;
				iVertexTangient = iVertex;
			}
			++iVertex;
		}
		tangientIDs_.push_back(iVertexTangient);
		if (scalarProductMax <= 0.)
		{
			ERROR_PRINT("scalar product max: %lf", scalarProductMax);
			std::cerr << "direction count: " << iDirection << std::endl;
			std::cerr << "direction: " << direction << std::endl;
			std::cerr << "support point: " << supportPoint << std::endl;
			++numNegative;
		}
		// ASSERT(scalarProductMax > 0.);
		SupportFunctionDataItem item(direction, scalarProductMax);
		item.info = SupportFunctionDataItemInfoPtr(
			new SupportFunctionDataItemInfo());
		item.info->point = supportPoint;
		items.push_back(item);
		++iDirection;
	}
	if (numNegative > 0)
		ERROR_PRINT("Number of negative: %d, total: %lu", numNegative, directions.size());

	DEBUG_END;
	return SupportFunctionDataPtr(new SupportFunctionData(items));
}

std::vector<int> SupportFunctionDataConstructor::getTangientIDs()
{
	DEBUG_START;
	DEBUG_END;
	return tangientIDs_;
}
