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
 * @file SupportFunctionEstimationData_test.cpp
 * @brief Structure that represents the input data for support
 * function estimation engines
 * - unit tests.
 */

#include "DataContainers/SupportFunctionEstimationData/SupportFunctionEstimationData.h"

#include <gtest/gtest.h>

/** Class for unit testing of support function estimation data class. */
class SupportFunctionEstimationDataTest : public ::testing::Test
{
protected:
	/** The data to be tested. */
	SupportFunctionEstimationDataPtr data;

	/** Initializes the test. */
	void SetUp()
	{
		SparseMatrix matrix(5, 5);
		typedef Eigen::Triplet<double> Triplet;
		std::vector<Triplet> triple;
		triple.reserve(12);
		triple.push_back(Triplet(0, 0, 3.));
		triple.push_back(Triplet(1, 1, 6.));
		triple.push_back(Triplet(2, 2, 9.));
		matrix.setFromTriplets(triple.begin(), triple.end());

		VectorXd supportVector(5);
		supportVector << 10., 11., 12., 13., 14.;
		VectorXd startingVector(5);
		startingVector << 13., 14, 15., 16., 17.;
		std::vector<Vector3d> supportDirections;
		supportDirections.push_back(Vector3d(1., 0., 0.));
		supportDirections.push_back(Vector3d(0., 1., 0.));
		supportDirections.push_back(Vector3d(0., 0., 1.));
		supportDirections.push_back(Vector3d(1., 1., 0.));
		supportDirections.push_back(Vector3d(1., 1., 1.));

		data.reset(new SupportFunctionEstimationData(matrix, supportVector, startingVector, supportDirections));
	}

	/** Finalizes the test. */
	void TearDown()
	{
	}
};

/** Checks that empty constructor crashes. */
TEST_F(SupportFunctionEstimationDataTest, EmptyConstructorCrashes)
{
	EXPECT_DEATH({ auto dataNew = SupportFunctionEstimationData(); }, ".*");
}

/** Checks that copy-by-pointer constructor works. */
TEST_F(SupportFunctionEstimationDataTest, CopyByPointerConstructorWorks)
{
	auto dataNew = new SupportFunctionEstimationData(&*data);
	EXPECT_EQ(dataNew->numValues(), data->numValues());
	EXPECT_EQ(dataNew->numConditions(), data->numConditions());
	/*
	 * TODO: also check equality of support matrix here. Currently g++ does
	 * not see implementation of operator== in SparseMatrixEigen.cpp
	 *
	 * The check should look as follows:
	 *
	 * EXPECT_EQ(dataNew->suportMatrix(), data->supportMatrix());
	 */
	EXPECT_EQ(dataNew->supportVector(), data->supportVector());
	EXPECT_EQ(dataNew->startingVector(), data->startingVector());
	EXPECT_EQ(dataNew->supportDirections(), data->supportDirections());
	delete dataNew;
}
