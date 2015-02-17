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
 * @file SupportFunctionDataItemExtractor_test.cpp
 * @brief Extractor of support function data item from given contour side
 * unit tests.
 */

#include "DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataItemExtractorByPoints.h"
#include "gtest/gtest.h"

/** Tests class SupportFunctionDataItemExtractorByPoints */
class SupportFunctionDataItemExtractorByPointsTest : public ::testing::Test
{
protected:
	/** The tested extractor. */
	SupportFunctionDataItemExtractorByPoints *extractor;

	/** The side for which the extractor is run. */
	SideOfContour *side;

	/** Initializes the testsuite. */
	virtual void SetUp()
	{
		extractor = new SupportFunctionDataItemExtractorByPoints();
		side = new SideOfContour();
	}

	/** Finalizes the testsuite. */
	virtual void TearDown()
	{
		delete extractor;
		delete side;
	}
};

/** Checks for crash in case when the points of given side are equal. */
TEST_F(SupportFunctionDataItemExtractorByPointsTest, CrashesIfPointsEqual)
{
	EXPECT_DEATH(
	{
		side->A1 = Vector3d(1., 2., 3.);
		side->A2 = side->A1;
		extractor->run(side);
	}, ".*");
}

/**
 * Checks that extractor is adequate, i. e. checks its result with precomputed
 * vaalue.
 */
TEST_F(SupportFunctionDataItemExtractorByPointsTest, Adequate)
{
	side->A1 = Vector3d(0., 1., 0.);
	side->A2 = Vector3d(0., 0., 1.);
	SupportFunctionDataItem item = extractor->run(side);

	Vector3d directionExpected(0., sqrt(0.5), sqrt(0.5));
	EXPECT_TRUE(equal(directionExpected, item.direction,
		EXTRACTOR_BY_POINTS_GUARANTEED_PRECISION))
		<< "Result of extracting is support direction "
		<< item.direction << ", while expected is "
		<< directionExpected;

	double valueExpected = sqrt(0.5);
	EXPECT_TRUE(equal(valueExpected, item.value,
		EXTRACTOR_BY_POINTS_GUARANTEED_PRECISION))
		<< "Result of extracting is support value " << item.value
		<< ", while expected is " << valueExpected;
}

