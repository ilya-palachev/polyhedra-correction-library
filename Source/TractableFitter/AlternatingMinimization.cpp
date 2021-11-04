/*
 * Copyright (c) 2021 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file AlternatingMinimization.cpp
 * @brief Alternating minimization algorithm for recovering convex polyhedron
 * from the set of its support function masurements, as it was described in the
 * following paper:
 *
 * - Soh Y.S., Chandrasekaran V. Fitting tractable convex sets to support
 *   function evaluations. Discrete & Computational Geometry. 2021 Jan 3:1-42.
 *   URL: https://link.springer.com/article/10.1007/s00454-020-00258-0
 */

#include <random>

#include "Common.h"
#include "DebugAssert.h"
#include "SupportFunction.h"

#include "TractableFitter/AlternatingMinimization.h"

using Eigen::MatrixXd;

static Eigen::Vector3d toEigenVector(Point_3 v)
{
	Eigen::Vector3d u;
	u(0) = v.x();
	u(1) = v.y();
	u(2) = v.z();
	return u;
}

static Eigen::VectorXd matrixToVector(const Eigen::MatrixXd &m)
{
	MatrixXd copy = m;
	VectorXd v(
		Eigen::Map<Eigen::VectorXd>(copy.data(), copy.cols() * copy.rows()));
	return v;
}

static Point_3 toCGALPoint(const Eigen::VectorXd &v)
{
	Point_3 p(v(0), v(1), v(2));
	return p;
}

double evaluateFit(Eigen::MatrixXd &A, SupportFunctionDataPtr data,
				   const std::vector<VectorXd> &simplexVertices)
{
	double error = 0.;
	Eigen::MatrixXd AT = A.transpose();
	for (unsigned i = 0; i < data->size(); ++i)
	{
		auto item = (*data)[i];
		MatrixXd direction = AT * toEigenVector(item.direction);
		auto vector = matrixToVector(direction);
		double diff =
			calculateSupportFunction(simplexVertices, vector) - item.value;
		error += diff * diff;
	}
	return error;
}

bool isFinite(const MatrixXd &M)
{
	for (unsigned i = 0; i < M.rows(); ++i)
		for (unsigned j = 0; j < M.cols(); ++j)
			if (!std::isfinite(M(i, j)))
				return false;
	return true;
}

unsigned numImplemented = 0;
unsigned numNonImplemented = 0;

static std::vector<VectorXd> generateSimplex(unsigned n)
{
	std::vector<VectorXd> vertices;
	for (unsigned i = 0; i < n; ++i)
	{
		VectorXd v = VectorXd::Zero(n);
		v(i) = 1.;
		vertices.push_back(v);
	}
	return vertices;
}

std::pair<Polyhedron_3, double>
AlternatingMinimization::run(SupportFunctionDataPtr data,
							 std::vector<Vector3d> startingBody,
							 unsigned numLiftingDimensions)
{
	std::cout << "Starting to fit in primal mode." << std::endl;

	unsigned numOuterIterations = 100;
	double numOuterIterationsEnv = -1.;
	tryGetenvDouble("N_OUTER", numOuterIterationsEnv);
	if (numOuterIterationsEnv > 0.)
		numOuterIterations = static_cast<int>(numOuterIterationsEnv);
	if (getenv("USE_STARTING_BODY"))
	{
		numOuterIterations = 1;
	}

	unsigned numInnerIterations = 100;
	double numInnerIterationsEnv = -1;
	tryGetenvDouble("N_INNER", numInnerIterationsEnv);
	if (numInnerIterationsEnv > 0.)
		numInnerIterations = static_cast<int>(numInnerIterationsEnv);

	double regularizer = 0.5;
	tryGetenvDouble("REGULARIZER", regularizer);

	std::cout << "The following hyperparameters are used:" << std::endl;
	std::cout << "  Number of outer iterations: " << numOuterIterations
			  << std::endl;
	std::cout << "  Number of inner iterations: " << numInnerIterations
			  << std::endl;
	std::cout << "                 Regularizer: " << regularizer << std::endl;

	double errorBest = -1.;
	MatrixXd Abest;

	std::default_random_engine generator;
	std::normal_distribution<double> distribution(0., 1.);
	auto normal = [&](int) { return distribution(generator); };

	auto simplexVertices = generateSimplex(numLiftingDimensions);

	for (unsigned iOuter = 0; iOuter < numOuterIterations; ++iOuter)
	{
		MatrixXd A(3, numLiftingDimensions);
		if (getenv("USE_STARTING_BODY"))
		{
			ASSERT(!startingBody.empty());
			int i = 0;
			std::cout << startingBody.size() << " " << A.cols() << std::endl;
			ASSERT(startingBody.size() == static_cast<unsigned long>(A.cols()));
			for (auto point : startingBody)
			{
				auto p = toEigenVector(point);
				for (int j = 0; j < 3; ++j)
					A(j, i) = p(j);
				++i;
			}
			ASSERT(A.coeff(0, 0) == startingBody[0].x);
			std::cout << "Initial matrix A: " << A << std::endl;
		}
		else
		{
			A = MatrixXd::NullaryExpr(3, numLiftingDimensions, normal);
		}
		MatrixXd Anew = MatrixXd::NullaryExpr(3, numLiftingDimensions, normal);
		ASSERT(A.rows() == 3);
		ASSERT(A.cols() == numLiftingDimensions);
		double errorInitial = evaluateFit(A, data, simplexVertices);
		std::cout << "Initial error on outer " << iOuter << ": " << errorInitial
				  << std::endl;
		double errorLast = 0.;

		for (unsigned iInner = 0; iInner < numInnerIterations; ++iInner)
		{
			unsigned size = 3 * numLiftingDimensions;
			MatrixXd matrix = regularizer * MatrixXd::Identity(size, size);
			ASSERT(isFinite(matrix));
			VectorXd Alinearized = matrixToVector(A);
			VectorXd vector = regularizer * Alinearized;

			MatrixXd AT = A.transpose();
			double error = 0.;

			for (unsigned k = 0; k < data->size(); ++k)
			{
				double y = (*data)[k].value;

				VectorXd u = toEigenVector((*data)[k].direction);

				VectorXd direction = matrixToVector(AT * u);
				VectorXd e =
					selectExtremePointByDirection(simplexVertices, direction);

				double diff = e.dot(direction) - y;
				error += diff * diff;
				ASSERT(isFinite(e));
				VectorXd V = matrixToVector(u * e.transpose());
				MatrixXd VT = V.transpose();
				ASSERT(isFinite(VT));
				vector += V * y;
				matrix += V * VT;
			}

			Anew = matrix.inverse() * vector;
			ASSERT(isFinite(Anew));

			A = Eigen::Map<MatrixXd>(Anew.data(), 3, numLiftingDimensions);
			ASSERT(A.rows() == 3);
			ASSERT(A.cols() == numLiftingDimensions);

			if (getenv("DEBUG_AM"))
				std::cout << "  Outer " << iOuter << " inner " << iInner
						  << ", error: " << error << std::endl;

			if (error > 1000. * errorInitial)
			{
				std::cout << "Early stop, algorithm doesn't coverge"
						  << std::endl;
				break;
			}
			errorLast = error;
		}
		double error = evaluateFit(A, data, simplexVertices);
		if (errorLast * errorBest < errorBest * errorBest)
		{
			Abest = A;
			errorBest = errorLast;
		}
		std::cout << "Error on iteration #" << iOuter << ": " << errorBest
				  << " (current is " << error << ")" << std::endl;
	}
	std::vector<Point_3> points;
	for (unsigned i = 0; i < numLiftingDimensions; ++i)
	{
		points.push_back(toCGALPoint(Abest.col(i)));
	}

	Polyhedron_3 result;

	std::cout << "Making a hull from " << points.size()
			  << " points:" << std::endl;
	for (auto point : points)
	{
		std::cout << "  " << point << std::endl;
	}
	Polyhedron_3 hull;
	// FIXME: CGAL has a bug: all planes in the hull are the same.
	CGAL::convex_hull_3(points.begin(), points.end(), hull);
	result = hull;
	std::cout << "Hull vertices: " << std::endl;
	for (auto I = hull.vertices_begin(), E = hull.vertices_end(); I != E; ++I)
	{
		std::cout << "  " << I->point() << std::endl;
	}

	std::cout << "Number of implemented cases: " << numImplemented << std::endl;
	std::cout << "Number of non-implemented cases: " << numNonImplemented
			  << std::endl;
	return std::make_pair(result, errorBest);
}
