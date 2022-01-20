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
 * @file SupportFunctionData_test.cpp
 * @brief General class for support function data container
 * - unit tests.
 */

#include "DataContainers/SupportFunctionData/SupportFunctionData.h"

#include <gtest/gtest.h>

/** Class for unit testing of support function data class. */
class SupportFunctionDataTest : public ::testing::Test
{
protected:
	/** The data to be tested. */
	SupportFunctionDataPtr data;

	/** Support function data items that are constructed manually. */
	std::vector<SupportFunctionDataItem> items;

	/** Initializes the test. */
	virtual void SetUp()
	{
		/*
		 * Prepare the vector of items - it will by used later multiple
		 * times.
		 */
		items.push_back(SupportFunctionDataItem(Vector3d(1., 0., 0.), 1.));
		items.push_back(SupportFunctionDataItem(Vector3d(0., 1., 0.), 1.));
		items.push_back(SupportFunctionDataItem(Vector3d(0., 0., 1.), 1.));

		data.reset(new SupportFunctionData(items));
	}

	/** Finilizes the test. */
	virtual void TearDown()
	{
	}
};

/** Checks that empty constructor works. */
TEST_F(SupportFunctionDataTest, EmptyConstructorsWorks)
{
	SupportFunctionData *dataNew = new SupportFunctionData();
	EXPECT_TRUE(dataNew);
	EXPECT_EQ(dataNew->size(), 0);
	delete dataNew;
}

/** Checks that copy-by-reference constructor works. */
TEST_F(SupportFunctionDataTest, CopyByReferenceConstructorWorks)
{
	SupportFunctionData *dataNew = new SupportFunctionData(*data);
	EXPECT_TRUE(dataNew);
	delete dataNew;
}

/** Checks that copy-by-pointer constructor works. */
TEST_F(SupportFunctionDataTest, CopyByPointerConstructorWorks)
{
	SupportFunctionData *dataNew = new SupportFunctionData(&*data);
	EXPECT_TRUE(dataNew);
	EXPECT_EQ(dataNew->size(), data->size());
	for (long int i = 0; i < dataNew->size(); ++i)
	{
		EXPECT_EQ((*dataNew)[i], (*data)[i]);
	}
	delete dataNew;
}

/** Checks that copy-by-shared-pointer constructor works. */
TEST_F(SupportFunctionDataTest, CopyBySharedPointerConstructorWorks)
{
	SupportFunctionData *dataNew = new SupportFunctionData(data);
	EXPECT_TRUE(dataNew);
	EXPECT_EQ(dataNew->size(), data->size());
	for (long int i = 0; i < dataNew->size(); ++i)
	{
		EXPECT_EQ((*dataNew)[i], (*data)[i]);
	}
	delete dataNew;
}

/** Checks that data constructor works. */
TEST_F(SupportFunctionDataTest, DataConstructorWorks)
{
	SupportFunctionData *dataNew = new SupportFunctionData(items);
	EXPECT_TRUE(dataNew);
	EXPECT_EQ(dataNew->size(), data->size());
	for (long int i = 0; i < dataNew->size(); ++i)
	{
		EXPECT_EQ((*dataNew)[i], items[i]);
	}
	delete dataNew;
}

/** Checks that assignment operator works. */
TEST_F(SupportFunctionDataTest, AssignmentOperatorWorks)
{
	SupportFunctionData dataNew;
	dataNew = *data;
	EXPECT_EQ(dataNew.size(), data->size());
	for (long int i = 0; i < dataNew.size(); ++i)
	{
		EXPECT_EQ(dataNew[i], items[i]);
	}
}

/** Checks that subscription operator works. */
TEST_F(SupportFunctionDataTest, SubscriptionOperatorWorks)
{
	EXPECT_DEATH({ auto item = (*data)[-1]; }, ".*");
	EXPECT_DEATH({ auto item = (*data)[data->size() + 1]; }, ".*");
	for (long int i = 0; i < data->size(); ++i)
	{
		EXPECT_EQ((*data)[i], items[i]);
	}
}

/** Checks that member function "size" works. */
TEST_F(SupportFunctionDataTest, FunctionSizeWorks)
{
	EXPECT_EQ(data->size(), items.size());
}

/** Checks that member function "removeEqual"works. */
TEST_F(SupportFunctionDataTest, FunctionRemoveEqualWorks)
{
	/* Create vector of items with only 1st and last items inequal. */
	std::vector<SupportFunctionDataItem> itemsWithEqualities;
	Vector3d v(1., 0., 0.);
	itemsWithEqualities.push_back(SupportFunctionDataItem(v, 1.));
	itemsWithEqualities.push_back(SupportFunctionDataItem(v, 2.));
	itemsWithEqualities.push_back(SupportFunctionDataItem(v + Vector3d(EPS_SUPPORT_DIRECTION_EQUALITY, 0., 0.), 1.));
	itemsWithEqualities.push_back(SupportFunctionDataItem(v + Vector3d(0., EPS_SUPPORT_DIRECTION_EQUALITY, 0.), 1.));
	itemsWithEqualities.push_back(SupportFunctionDataItem(v + Vector3d(0., 0., EPS_SUPPORT_DIRECTION_EQUALITY), 1.));
	itemsWithEqualities.push_back(SupportFunctionDataItem(2. * v, 1.));
	itemsWithEqualities.push_back(
		SupportFunctionDataItem(v + Vector3d(0., 2. * EPS_SUPPORT_DIRECTION_EQUALITY, 0.), 1.));

	/* Create vector of inequal items of previous vector. */
	std::vector<SupportFunctionDataItem> itemsWithoutEqualities;
	itemsWithoutEqualities.push_back(itemsWithEqualities[0]);
	itemsWithoutEqualities.push_back(itemsWithEqualities[6]);

	/* Assign our data to 1st vector of items. */
	data.reset(new SupportFunctionData(itemsWithEqualities));

	/* Run the removal of equal items. */
	auto dataUnequal = data->removeEqual();

	/* Check that constructed data is equal to expected one. */
	EXPECT_EQ(dataUnequal->size(), itemsWithoutEqualities.size());
	for (long int i = 0; i < dataUnequal->size(); ++i)
	{
		EXPECT_EQ((*dataUnequal)[i], itemsWithoutEqualities[i]);
	}
}
