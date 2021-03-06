
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
 * @file SupportFunctionDataItemInfo_test.cpp
 * @brief General information about support data item (its origin)
 * - unit tests.
 */

#include "DataContainers/SupportFunctionData/SupportFunctionDataItemInfo.h"

#include <gtest/gtest.h>

/** Checks that empty constructor works. */
TEST(SupportFunctionDataItemInfoTest, EmptyConstructorWorks)
{
	SupportFunctionDataItemInfo *info = new SupportFunctionDataItemInfo();
	EXPECT_TRUE(info);
	delete info;
}
