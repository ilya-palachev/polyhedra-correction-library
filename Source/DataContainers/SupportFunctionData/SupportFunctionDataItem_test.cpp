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
 * @file SupportFunctionDataItem_test.cpp
 * @brief Support function data items contain support value and support
 * direction assiciated with it. Also it can contain a pointer to auxiliary
 * info structure that stores the information about the origin of this value
 * (i. e. number of contour, number of side or anything else)
 * - unit tests.
 */

#include "DataContainers/SupportFunctionData/SupportFunctionDataItem.h"

#include <gtest/gtest.h>

/** Class for unit testing of support function data item class. */
class SupportFunctionDataItemTest : public ::testing::Test
{
protected:
	/** The item to be tested. */
	SupportFunctionDataItemPtr item;

	/** Initializes the test. */
	void SetUp()
	{
		item.reset(new SupportFunctionDataItem(Vector3d(1., 2., 3.),
			4.));
	}

	/** Finalizes the test. */
	void TearDown()
	{
	}
};

/** Checks that empty constructor works. */
TEST_F(SupportFunctionDataItemTest, EmptyConstructorWorks)
{
	auto itemNew = new SupportFunctionDataItem();
	EXPECT_TRUE(itemNew);
	EXPECT_TRUE(!itemNew->direction); /* The direction is zero vector. */
	EXPECT_TRUE(equal(itemNew->value, 0.));
	EXPECT_FALSE(itemNew->info);
	delete itemNew;
}

/** Checks that copy-by-reference constructor works. */
TEST_F(SupportFunctionDataItemTest, CopyByReferenceConstructorWorks)
{
	auto itemNew = new SupportFunctionDataItem(*item);
	EXPECT_TRUE(itemNew);
	EXPECT_EQ(itemNew->direction, item->direction);
	EXPECT_TRUE(equal(itemNew->value, item->value));
	EXPECT_EQ(itemNew->info, item->info);
	delete itemNew;
}

/** Checks that data constructor works. */
TEST_F(SupportFunctionDataItemTest, DataConstructorWorks)
{
	Vector3d direction(1., 2., 3.);
	double value = 4.;
	auto itemNew = new SupportFunctionDataItem(direction, value);
	EXPECT_TRUE(itemNew);
	EXPECT_EQ(itemNew->direction, direction);
	EXPECT_TRUE(equal(itemNew->value, value));
	EXPECT_FALSE(itemNew->info);
	delete itemNew;
}

/** Checks that assignment operator works. */
TEST_F(SupportFunctionDataItemTest, AssignmentOperatorWorks)
{
	SupportFunctionDataItem itemNew;
	itemNew = *item;
	EXPECT_EQ(itemNew.direction, item->direction);
	EXPECT_TRUE(equal(itemNew.value, item->value));
	EXPECT_EQ(itemNew.info, item->info);
}

/** Checks that equality operator works. */
TEST_F(SupportFunctionDataItemTest, EqualityOperatorWorks)
{
	auto itemNull = new SupportFunctionDataItem();
	auto itemNew = new SupportFunctionDataItem(*item);
	EXPECT_EQ(*itemNew, *item);
	EXPECT_FALSE(*itemNull == *item);
	delete itemNull;
	delete itemNew;
}

/** Checks that inequality operator works. */
TEST_F(SupportFunctionDataItemTest, InequalityOperatorWorks)
{
	auto itemNull = new SupportFunctionDataItem();
	auto itemNew = new SupportFunctionDataItem(*item);
	EXPECT_NE(*itemNull, *item);
	EXPECT_FALSE(*itemNew != *item);
	delete itemNull;
	delete itemNew;
}
