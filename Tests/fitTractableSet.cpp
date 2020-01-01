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

std::vector<Vector3d> generateLInfinityBall()
{
	std::vector<Vector3d> cube;
	cube.push_back(Vector3d(1., 1., 1.));
	cube.push_back(Vector3d(1., -1., 1.));
	cube.push_back(Vector3d(-1., -1., 1.));
	cube.push_back(Vector3d(-1., 1., 1.));
	cube.push_back(Vector3d(1., 1., -1.));
	cube.push_back(Vector3d(1., -1., -1.));
	cube.push_back(Vector3d(-1., -1., -1.));
	cube.push_back(Vector3d(-1., 1., -1.));
	return cube;
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
bool negativeMode = false;

class ConvexAffinePooling
{
	// Input structures:
	std::vector<VectorXd> simplexVertices;
	MatrixXd A;
	bool dualMode;

	// Auxiliary structures:
	MatrixXd AT;
	std::vector<Point_3> points;
	Polyhedron_3 P;
public:
	ConvexAffinePooling(const std::vector<VectorXd> &simplexVertices,
			const MatrixXd &A, bool dualMode) :
		simplexVertices(simplexVertices), A(A), dualMode(dualMode) {}

	void init()
	{
		ASSERT(simplexVertices.size() == unsigned(A.cols()));
		AT = A.transpose();

		if (!dualMode)
			return;

		for (unsigned i = 0; i < simplexVertices.size(); ++i)
		{
			auto point = toCGALPoint(A.col(i));
			points.push_back(point);
		}
		std::vector<Plane_3> planes;
		for (const auto &point : points)
		{
			auto plane = dual(point);
			planes.push_back(plane);
		}
		Polyhedron_3 intersection(planes.begin(), planes.end());
		P = intersection;
	}

	std::pair<double, VectorXd> calculateEandDistance(const VectorXd &u)
	{
		if (!dualMode)
		{
			return calculateSupportFunction(simplexVertices,
					matrixToVector(AT * u));
		}

		std::vector<int> goodFacets;
		std::vector<int> commonVertices;
		bool initialized = false;
		Eigen::Vector3d singleAlpha;
		auto vBest = P.vertices_begin();
		for (auto v = P.vertices_begin(); v != P.vertices_end(); ++v)
		{
			std::vector<int> indices;
			std::vector<VectorXd> a;
			auto circulator = v->vertex_begin();
			do
			{
				int id = circulator->facet()->id;
				ASSERT(id >= 0);
				ASSERT(unsigned(id) < points.size());

				indices.push_back(id);
				a.push_back(toEigenVector(points[id]));

				++circulator;
			} 
			while (circulator != v->vertex_begin());

			MatrixXd M(3, 3);
			for (unsigned i = 0; i < 3; ++i)
			{
				VectorXd column = a[i];
				for (unsigned j = 0; j < 3; ++j)
				{
					M(j, i) = column(j);
				}
			}
			ASSERT(isFinite(M));
			Eigen::Vector3d alpha = M.inverse() * u;
			ASSERT(isFinite(alpha));

			bool positive = true;
			for (unsigned i = 0; i < 3; ++i)
				if (alpha[i] <= -1e-8)
					positive = false;

			if (positive)
			{
				goodFacets.push_back(v->id);
				if (!initialized)
				{
					commonVertices = indices;
					initialized = true;
				}
				else
				{
					std::vector<int> tmp = commonVertices;
					auto it = std::set_intersection(commonVertices.begin(), commonVertices.end(), indices.begin(), indices.end(), tmp.begin());
					tmp.resize(it - tmp.begin());
					commonVertices = tmp;
				}
				singleAlpha = alpha;
				vBest = v;
			}
		}

#if 0
		std::cout << "Number of good facets: " << goodFacets.size()
			<< ", common vertices: " << commonVertices.size() << std::endl;
#endif

		VectorXd result = VectorXd::Zero(simplexVertices.size());
		double distance = 0.;
		if ((commonVertices.size() == 3 && goodFacets.size() == 1)
			|| (commonVertices.size() == 2 && goodFacets.size() == 2)
			|| (commonVertices.size() == 1 && goodFacets.size() > 1))
		{
			Eigen::Vector3d alpha = singleAlpha;
			ASSERT(isFinite(alpha));
			Plane_3 p = dual(vBest->point());
			Point_3 norm(p.a(), p.b(), p.c());
			ASSERT(isFinite(toEigenVector(norm)));
			double product = toEigenVector(norm).dot(u);
			distance = -p.d() / product;
			alpha *= distance;
			ASSERT(isFinite(alpha));

			for (unsigned i = 0; i < 3; ++i)
			{
				result(commonVertices[i]) = alpha(i);
				//ASSERT(alpha(i) >= -1e-8 * factor);
			}
			++numImplemented;

		}
		else
		{
			// Not implemented yet!
			++numNonImplemented;
		}

		return std::make_pair(distance, result);

	}
};

Polyhedron_3 fitSimplexAffineImage(const std::vector<VectorXd> &simplexVertices,
		SupportFunctionDataPtr data,
		unsigned numLiftingDimensions, bool dualMode)
{
	std::cout << "Starting to fit in " << (dualMode ? "dual" : "primal")
		<< " mode." << std::endl;

	unsigned numOuterIterations = 100;
	unsigned numInnerIterations = 100;
	double regularizer = 0.5;
	double errorBest = -1.;
	MatrixXd Abest;

	std::default_random_engine generator;
	std::normal_distribution<double> distribution(0., 1.);
	auto normal = [&] (int) {return distribution(generator);};

	for (unsigned iOuter = 0; iOuter < numOuterIterations; ++iOuter)
	{
		MatrixXd A = MatrixXd::NullaryExpr(3, numLiftingDimensions, normal);
		ASSERT(A.rows() == 3);
		ASSERT(A.cols() == numLiftingDimensions);
		double errorInitial = evaluateFit(A, data, simplexVertices);
		double errorLast = 0.;

		for (unsigned iInner = 0; iInner < numInnerIterations; ++iInner)
		{
			unsigned size = 3 * numLiftingDimensions;
			MatrixXd matrix = regularizer * MatrixXd::Identity(size, size);
			ASSERT(isFinite(matrix));
			VectorXd Alinearized = matrixToVector(A);
			VectorXd vector = regularizer * Alinearized;

			ConvexAffinePooling pool(simplexVertices, A, dualMode);
			pool.init();
			double error = 0.;

			for (unsigned k = 0; k < data->size(); ++k)
			{
				double y = (*data)[k].value;

				if (dualMode)
				{
					y = 1. / y;
				}
				VectorXd u = toEigenVector((*data)[k].direction);
				std::pair<double, VectorXd> result = pool.calculateEandDistance(u);
				double diff = result.first - y;
				error += diff * diff;
				MatrixXd e = result.second;
				ASSERT(isFinite(e));
				VectorXd V = matrixToVector(u * e.transpose());
				MatrixXd VT = V.transpose();
				ASSERT(isFinite(VT));
				vector += V * y;
				matrix += V * VT;
			}

			MatrixXd Anew = matrix.inverse() * vector;
			ASSERT(isFinite(Anew));

			A = Eigen::Map<MatrixXd>(Anew.data(), 3, numLiftingDimensions);
			ASSERT(A.rows() == 3);
			ASSERT(A.cols() == numLiftingDimensions);

#if 0
			std::cout << "  Outer " << iOuter << " inner " << iInner
				<< ", error: " << error << std::endl;
#endif

			if (error > 1000. * errorInitial)
			{
				std::cout << "Early stop, algorithm doesn't coverge" << std::endl;
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
		std::cout << "Error on iteration #" << iOuter << ": "
			<< errorBest << " (current is " << error << ")"
			<< std::endl;
	}

	std::vector<Point_3> points;
	for (unsigned i = 0; i < numLiftingDimensions; ++i)
	{
		points.push_back(toCGALPoint(Abest.col(i)));
	}

	Polyhedron_3 result;

	if (dualMode)
	{
		std::vector<Plane_3> planes;
		for (const auto &point : points)
		{
			planes.push_back(dual(point));
		}
		Polyhedron_3 intersection(planes.begin(), planes.end());
		result = intersection;
	}
	else
	{
		Polyhedron_3 hull;
		CGAL::convex_hull_3(points.begin(), points.end(), hull);
		result = hull;
	}

	return result;
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

void fit(unsigned n, std::vector<Vector3d> &directions,
		unsigned numLiftingDimensions,
		unsigned numLiftingDimensionsDual,
		std::vector<Vector3d> &targetPoints, const char *title)
{
	std::default_random_engine generator;
	std::normal_distribution<double> noise(0., 0.001);
	std::vector<SupportFunctionDataItem> exactItems;
	std::vector<SupportFunctionDataItem> noisyItems;
	std::vector<SupportFunctionDataItem> dualItems;

	for (const auto &direction : directions)
	{
		double value = calculateSupportFunction(targetPoints, direction).first;
		ASSERT(value > 0.);
		exactItems.push_back(SupportFunctionDataItem(direction, value));
		double noisyValue =  value + noise(generator);
		ASSERT(noisyValue > 0.);
		noisyItems.push_back(SupportFunctionDataItem(direction,
					noisyValue));
	}

	for (const auto &direction : directions)
	{
		double maxValue = 0.;
		for (const auto &item : noisyItems)
		{
			auto point = (1. / item.value) * item.direction;
			double value = point * direction;
			if (value > maxValue)
				maxValue = value;
		}
		dualItems.push_back(SupportFunctionDataItem(direction,
					maxValue));
	}

	SupportFunctionDataPtr exactData(new SupportFunctionData(exactItems));
	SupportFunctionDataPtr noisyData(new SupportFunctionData(noisyItems));
	SupportFunctionDataPtr dualData(new SupportFunctionData(dualItems));


	std::cout << "Preparing recoverer..." << std::endl;
	RecovererPtr recoverer(new Recoverer());
	recoverer->setEstimatorType(IPOPT_ESTIMATOR);
	recoverer->setProblemType(ESTIMATION_PROBLEM_NORM_L_2);
	recoverer->enableContoursConvexification();
	recoverer->enableMatrixScaling();
	recoverer->enableBalancing();
	globalPCLDumper.setNameBase(title);
	globalPCLDumper.enableVerboseMode();

	std::cout << "Running Ipopt estimator..." << std::endl;
	auto polyhedron = recoverer->run(noisyData);
	globalPCLDumper(PCL_DUMPER_LEVEL_OUTPUT, "recovered.ply") << polyhedron;

#if 0
	auto polyhedronAM = fitSimplexAffineImage(
			generateSimplex(numLiftingDimensions), noisyData,
			numLiftingDimensions, false);
	globalPCLDumper(PCL_DUMPER_LEVEL_OUTPUT, "am-recovered.ply") << polyhedronAM;
#endif

	auto polyhedronAMdualStar = fitSimplexAffineImage(
			generateSimplex(numLiftingDimensionsDual), dualData,
			numLiftingDimensionsDual, false);
	globalPCLDumper(PCL_DUMPER_LEVEL_OUTPUT, "am-dual-star-recovered.ply")
		<< polyhedronAMdualStar;

	std::vector<Plane_3> planes;
	for (auto it = polyhedronAMdualStar.vertices_begin();
			it != polyhedronAMdualStar.vertices_end(); ++it)
	{
		auto point = it->point();
		planes.push_back(dual(point));
	}
	Polyhedron_3 intersection(planes.begin(), planes.end());
	globalPCLDumper(PCL_DUMPER_LEVEL_OUTPUT, "am-star-recovered.ply")
		<< intersection;
#if 0
	auto polyhedronAMdual = fitSimplexAffineImage(
			generateSimplex(numLiftingDimensionsDual), noisyData,
			numLiftingDimensionsDual, true);
	globalPCLDumper(PCL_DUMPER_LEVEL_OUTPUT, "am-dual-recovered.ply") << polyhedronAMdual;
#endif

	std::cout << "Number of implemented cases: " << numImplemented << std::endl;
	std::cout << "Number of non-implemented cases: " << numNonImplemented << std::endl;
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

	auto lInfinityBall = generateLInfinityBall();
	fit(n, directions, 8, 6, lInfinityBall, "cube");

	//auto l1ball = generateL1Ball();
	//fit(n, directions, 6, 8, l1ball, "octahedron");

	return EXIT_SUCCESS;
}
