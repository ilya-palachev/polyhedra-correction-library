/*
 * Copyright (c) 2009-2015 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file SupportFunctionDataConstructor_test.cpp
 * @brief Constructor class for support function data - unit tests.
 */

#include "DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataConstructor.h"

#include <gtest/gtest.h>

/** Class for testing class SupportFunctionDataConstructor */
class SupportFunctionDataConstructorTest : public ::testing::Test
{
protected:
	/** The constructor that should be created and tested. */
	SupportFunctionDataConstructor *constructor;

	/** Shadow contour data that will be passed to it. */
	ShadowContourDataPtr data;

	virtual void SetUp()
	{
		constructor = new SupportFunctionDataConstructor();
		data.reset(new ShadowContourData());
	}

	virtual void TearDown()
	{
		if (constructor)
			delete constructor;
	}

};

TEST_F(SupportFunctionDataConstructorTest, FailsForNullData)
{
	EXPECT_DEATH({constructor->run(NULL);}, ".*");
}

TEST_F(SupportFunctionDataConstructorTest, FailsForEmptyData)
{
	EXPECT_DEATH(
	{
		data.reset(new ShadowContourData());
		EXPECT_TRUE(data->numContours == 0);
		constructor->run(data);
	}, ".*");	
}
