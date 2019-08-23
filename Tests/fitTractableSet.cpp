/*
 * Copyright (c) 2009-2019 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file fitTractableSet.cpp
 * @brief Fitting tractable convex sets to support funciton evaluations.
 */

#include <iostream>
#include <random>
#include "PolyhedraCorrectionLibrary.h"
#include <Eigen/LU>

using Eigen::MatrixXd;

std::vector<Vector3d> generateDirections(int n)
{
	std::default_random_engine generator;
	std::normal_distribution<double> distribution(0., 1.);

	std::vector<Vector3d> directions;
	for (int i = 0; i < n; ++i)
	{
		double x = 0., y = 0., z = 0., lambda = 0.;
		do
		{
			x = distribution(generator);
			y = distribution(generator);
			z = distribution(generator);
			lambda = sqrt(x * x + y * y + z * z);
		} while (lambda < 1e-16);

		x /= lambda;
		y /= lambda;
		z /= lambda;
		// Now (x, y, z) is uniformly distributed on the sphere

		Vector3d v(x, y, z);
		directions.push_back(v);
	}

	return directions;
}

std::vector<Vector3d> generateL1Ball()
{
	std::vector<Vector3d> l1ball;
	l1ball.push_back(Vector3d(1., 0., 0.));
	l1ball.push_back(Vector3d(0., 1., 0.));
	l1ball.push_back(Vector3d(0., 0., 1.));
	l1ball.push_back(Vector3d(-1., 0., 0.));
	l1ball.push_back(Vector3d(0., -1., 0.));
	l1ball.push_back(Vector3d(0., 0., -1.));
	return l1ball;
}

std::pair<double, Vector3d>
calculateSupportFunction(const std::vector<Vector3d> &vertices,
			const Vector3d &direction)
{
	double maxProduct = vertices[0] * direction;
	Vector3d tangient = vertices[0];
	for (const Vector3d &vertex : vertices)
	{
		double product = vertex * direction;
		if (product > maxProduct)
		{
			maxProduct = product;
			tangient = vertex;
		}
	}
	return std::make_pair(maxProduct, tangient);
}

std::pair<double, VectorXd>
calculateSupportFunction(const std::vector<VectorXd> &vertices,
			const VectorXd &direction)
{
	double maxProduct = vertices[0].dot(direction);
	VectorXd tangient = vertices[0];
	for (const VectorXd &vertex : vertices)
	{
		double product = vertex.dot(direction);
		if (product > maxProduct)
		{
			maxProduct = product;
			tangient = vertex;
		}
	}
	return std::make_pair(maxProduct, tangient);
}

static Eigen::Vector3d toEigenVector(const Vector3d &v)
{
	Eigen::Vector3d u;
	u(0) = v.x;
	u(1) = v.y;
	u(2) = v.z;
	return u;
}

static Eigen::VectorXd matrixToVector(const Eigen::MatrixXd &m)
{
	MatrixXd copy = m;
	VectorXd v(Eigen::Map<Eigen::VectorXd>(copy.data(), copy.cols() * copy.rows()));
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
		double diff = calculateSupportFunction(simplexVertices,
				matrixToVector(direction)).first - item.value;
		error += diff * diff;
	}
	return error;
}

Polyhedron_3 fitSimplexAffineImage(const std::vector<VectorXd> &simplexVertices,
		SupportFunctionDataPtr data,
		unsigned numLiftingDimensions)
{
	unsigned numOuterIterations = 20;
	unsigned numInnerIterations = 50;
	double regularizer = 0.5;
	double errorBest = -1.;
	MatrixXd Abest;

	for (unsigned iOuter = 0; iOuter < numOuterIterations; ++iOuter)
	{
		MatrixXd A = MatrixXd::Random(3, numLiftingDimensions);

		for (unsigned iInner = 0; iInner < numInnerIterations; ++iInner)
		{
			unsigned size = 3 * numLiftingDimensions;
			MatrixXd ope = regularizer * MatrixXd::Identity(size, size);
			MatrixXd vec = regularizer * A;

			for (unsigned k = 0; k < data->size(); ++k)
			{
				VectorXd u = toEigenVector((*data)[k].direction);
				MatrixXd e = calculateSupportFunction(simplexVertices,
						matrixToVector(A.transpose() * u)).second;
				MatrixXd outerProduct = u * e.transpose();
				VectorXd linearized = matrixToVector(outerProduct);
				vec += outerProduct * (*data)[k].value;
				MatrixXd linearizedT = linearized.transpose();
				ope += linearized * linearizedT;
			}
			A = ope.inverse() * vec;
		}
		double error = evaluateFit(A, data, simplexVertices);
		if (error * errorBest < errorBest * errorBest)
		{
			Abest = A;
			errorBest = error;
		}
	}

	std::vector<Point_3> points;
	for (unsigned i = 0; i < data->size(); ++i)
	{
		points.push_back(toCGALPoint(Abest.col(i)));
	}
	Polyhedron_3 hull;
	CGAL::convex_hull_3(points.begin(), points.end(), hull);
	return hull;
}

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

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Expected 1 argument" << std::endl;
		return EXIT_FAILURE;
	}

	int n = atoi(argv[1]);

	if (n <= 0)
	{
		std::cerr << "Expected positive number of measurements"
			<< std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "Preparing data..." << std::endl;
	auto directions = generateDirections(n);
	auto l1ball = generateL1Ball();

	std::default_random_engine generator;
	std::normal_distribution<double> noise(0., 0.1);
	std::vector<SupportFunctionDataItem> exactItems;
	std::vector<SupportFunctionDataItem> noisyItems;

	for (const auto &direction : directions)
	{
		double value = calculateSupportFunction(l1ball, direction).first;
		ASSERT(value > 0.);
		exactItems.push_back(SupportFunctionDataItem(direction, value));
		double noisyValue =  value + noise(generator);
		ASSERT(noisyValue > 0.);
		noisyItems.push_back(SupportFunctionDataItem(direction,
					noisyValue));
	}
	SupportFunctionDataPtr exactData(new SupportFunctionData(exactItems));
	SupportFunctionDataPtr noisyData(new SupportFunctionData(noisyItems));

	std::cout << "Preparing recoverer..." << std::endl;
	RecovererPtr recoverer(new Recoverer());
	recoverer->setEstimatorType(IPOPT_ESTIMATOR);
	recoverer->setProblemType(ESTIMATION_PROBLEM_NORM_L_2);
	recoverer->enableContoursConvexification();
	recoverer->enableMatrixScaling();
	recoverer->enableBalancing();
	globalPCLDumper.setNameBase("octahedron");
	globalPCLDumper.enableVerboseMode();

	std::cout << "Running Ipopt estimator..." << std::endl;
	auto polyhedron = recoverer->run(noisyData);
	globalPCLDumper(PCL_DUMPER_LEVEL_OUTPUT, "recovered.ply") << polyhedron;

	auto simplex = generateSimplex(6);
	auto polyhedronAM = fitSimplexAffineImage(simplex, noisyData, 6);
	globalPCLDumper(PCL_DUMPER_LEVEL_OUTPUT, "am-recovered.ply") << polyhedronAM;

	return EXIT_SUCCESS;
}
