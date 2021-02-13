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
#include <Eigen/LU>

#include "Common.h"
#include "PolyhedraCorrectionLibrary.h"
#include "Recoverer/SupportPolyhedronCorrector.h"
#include "DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataConstructor.h"

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

std::vector<Vector3d> generateCubeCut()
{
	std::vector<Vector3d> cube;
	cube.push_back(Vector3d(1., 1., 0.5));
	cube.push_back(Vector3d(1., 0.5, 1.));
	cube.push_back(Vector3d(0.5, 1., 1.));
	cube.push_back(Vector3d(1., -1., 1.));
	cube.push_back(Vector3d(-1., -1., 1.));
	cube.push_back(Vector3d(-1., 1., 1.));
	cube.push_back(Vector3d(1., 1., -1.));
	cube.push_back(Vector3d(1., -1., -1.));
	cube.push_back(Vector3d(-1., -1., -1.));
	cube.push_back(Vector3d(-1., 1., -1.));
	return cube;
}

std::vector<Vector3d> generateDodecahedron()
{
	std::vector<Vector3d> body;
	body.push_back(Vector3d(1., 1., 1.));
	body.push_back(Vector3d(1., 1., -1.));
	body.push_back(Vector3d(1., -1., 1.));
	body.push_back(Vector3d(1., -1., -1.));
	body.push_back(Vector3d(-1., 1., 1.));
	body.push_back(Vector3d(-1., 1., -1.));
	body.push_back(Vector3d(-1., -1., 1.));
	body.push_back(Vector3d(-1., -1., -1.));
	double phi = 0.5 + sqrt(1.2);
	body.push_back(Vector3d(0., 1. / phi, phi));
	body.push_back(Vector3d(0., 1. / phi, -phi));
	body.push_back(Vector3d(0., -1. / phi, phi));
	body.push_back(Vector3d(0., -1. / phi, -phi));
	body.push_back(Vector3d(1. / phi, phi, 0));
	body.push_back(Vector3d(1. / phi, -phi, 0));
	body.push_back(Vector3d(-1. / phi, phi, 0));
	body.push_back(Vector3d(-1. / phi, -phi, 0));
	body.push_back(Vector3d(phi, 0, 1. / phi));
	body.push_back(Vector3d(phi, 0, -1. / phi));
	body.push_back(Vector3d(-phi, 0, 1. / phi));
	body.push_back(Vector3d(-phi, 0, -1. / phi));
	return body;
}

std::pair<double, Vector3d>
calculateSupportFunction(const std::vector<Vector3d> &vertices,
			const Vector3d &direction)
{
	ASSERT(vertices.size() > 0);
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

std::pair<Polyhedron_3, double>
fitSimplexAffineImage(const std::vector<VectorXd> &simplexVertices,
		SupportFunctionDataPtr data, std::vector<Vector3d> startingBody,
		unsigned numLiftingDimensions, bool dualMode)
{
	std::cout << "Starting to fit in " << (dualMode ? "dual" : "primal")
		<< " mode." << std::endl;

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

	double eta = 1.;
	tryGetenvDouble("ETA", eta);

	double beta = 0.8;
	tryGetenvDouble("BETA", beta);

	std::cout << "The following hyperparameters are used:" << std::endl;
	std::cout << "  Number of outer iterations: " << numOuterIterations << std::endl;
	std::cout << "  Number of inner iterations: " << numInnerIterations << std::endl;
	if (getenv("GRADIENT_DESCENT"))
	{
		std::cout << "                         Eta: " << eta << std::endl;
		std::cout << "                        Beta: " << beta << std::endl;
	}
	else
	{
		std::cout << "                 Regularizer: " << regularizer << std::endl;
	}

	double errorBest = -1.;
	MatrixXd Abest;

	std::default_random_engine generator;
	std::normal_distribution<double> distribution(0., 1.);
	auto normal = [&] (int) {return distribution(generator);};

	for (unsigned iOuter = 0; iOuter < numOuterIterations; ++iOuter)
	{
		MatrixXd A(3, numLiftingDimensions);
        if (getenv("USE_STARTING_BODY"))
        {
            std::cout << A << std::endl;
            ASSERT(!startingBody.empty());
            int i = 0;
            for (auto point : startingBody)
                A.col(i++) = toEigenVector(point);
            std::cout << A << std::endl;
            std::cout << A.coeff(0, 0) << " " << startingBody[0].x << std::endl;
            ASSERT(A.coeff(0, 0) == startingBody[0].x);
        }
        else
        {
            A = MatrixXd::NullaryExpr(3, numLiftingDimensions, normal);
        }
        std::cout << "Allocating Anew" << std::endl;
		MatrixXd Anew = MatrixXd::NullaryExpr(3, numLiftingDimensions, normal);
		ASSERT(A.rows() == 3);
		ASSERT(A.cols() == numLiftingDimensions);
		double errorInitial = evaluateFit(A, data, simplexVertices);
		double errorLast = 0.;

		for (unsigned iInner = 0; iInner < numInnerIterations; ++iInner)
		{
			unsigned size = 3 * numLiftingDimensions;
            std::cout << "Allocating " << size << "x" << size << " matrix" << std::endl;
			MatrixXd matrix = regularizer * MatrixXd::Identity(size, size);
            std::cout << "Allocated";
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
				if (getenv("GRADIENT_DESCENT"))
				{
					vector += V * diff;
				}
				else
				{
					MatrixXd VT = V.transpose();
					ASSERT(isFinite(VT));
					vector += V * y;
					matrix += V * VT;
				}
			}

			if (getenv("GRADIENT_DESCENT"))
			{
				double eta_current = eta;
				double square = vector.norm();
				square *= square;
				double error_first = evaluateFit(A, data, simplexVertices);
				double error_new = error_first;
				int n_iterations = 0;

				if (getenv("DEBUG_AM"))
				{
					std::cout << "  error initial " << error << std::endl;
					std::cout << "  error first " << error_first << std::endl;
					std::cout << "  square " << square << std::endl;
				}

				do
				{
					n_iterations += 1;

					Anew = Eigen::Map<MatrixXd>(A.data(), 3, numLiftingDimensions);
					ASSERT(Anew.rows() == 3);
					ASSERT(Anew.cols() == numLiftingDimensions);
					Anew -= eta_current * vector;

					error_new = evaluateFit(Anew, data, simplexVertices);
					if (getenv("DEBUG_AM"))
					{
						double distance = (A - Anew).norm();
						std::cout << "    distance " << distance << std::endl;
						std::cout << "    error " << error_new << std::endl;
						std::cout << "    error first "
							<< evaluateFit(A, data, simplexVertices) << std::endl; 
						std::cout << std::endl;
					}
					if (error_new <= error_first - 0.5 * eta_current * square)
						break;
					eta_current *= beta;
				}
				while (eta_current > 1e-16);
				error = error_new;
				if (getenv("DEBUG_AM"))
					std::cout << "  iterations:" << n_iterations << std::endl;
			}
			else
			{
				Anew = matrix.inverse() * vector;
			}
			ASSERT(isFinite(Anew));

			A = Eigen::Map<MatrixXd>(Anew.data(), 3, numLiftingDimensions);
			ASSERT(A.rows() == 3);
			ASSERT(A.cols() == numLiftingDimensions);

			if (getenv("DEBUG_AM"))
				std::cout << "  Outer " << iOuter << " inner " << iInner
					<< ", error: " << error << std::endl;

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
		// FIXME: CGAL has a bug: all planes in the hull are the same.
		CGAL::convex_hull_3(points.begin(), points.end(), hull);
		result = hull;
	}

	return std::make_pair(result, errorBest);
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

static Polyhedron_3 dual(Polyhedron_3 p)
{
	std::vector<Plane_3> planes;
	for (auto it = p.vertices_begin(); it != p.vertices_end(); ++it)
	{
		auto point = it->point();
		planes.push_back(dual(point));
	}
	Polyhedron_3 intersection(planes.begin(), planes.end());
	return intersection;
}

void printEstimationReport(VectorXd h0, VectorXd h);

void printEstimationReport(Polyhedron_3 p, SupportFunctionDataPtr data)
{
	VectorXd h0 = data->supportValues();
	auto directions = data->supportDirections<PCLPoint_3>();
	std::cout << "Calculating support values..." << std::endl;
	VectorXd h = p.calculateSupportData(directions)->supportValues();
	std::cout << "Calculating support values... Done" << std::endl;
	std::cout << "Printing estimation report..." << std::endl;
	printEstimationReport(h0, h);
	std::cout << "Printing estimation report... Done" << std::endl;
}

SupportFunctionDataPtr
calculateProvisionalEstimate(std::vector<Vector3d> &directions,
		SupportFunctionDataPtr noisyData,
		const char *linearSolver)
{
	// Gardner & Kiderlen LSE algorithm for noisy primal data

	std::cout << "Preparing recoverer..." << std::endl;
	RecovererPtr recoverer(new Recoverer());
	recoverer->setEstimatorType(IPOPT_ESTIMATOR);
	recoverer->setProblemType(ESTIMATION_PROBLEM_NORM_L_2);
	recoverer->enableContoursConvexification();
	recoverer->enableMatrixScaling();
	recoverer->enableBalancing();
	std::cout << "Using linear solver " << linearSolver << std::endl;
	recoverer->setLinearSolver(linearSolver);

	std::cout << "Running Ipopt estimator..." << std::endl;
	auto polyhedron = recoverer->run(noisyData);
	globalPCLDumper(PCL_DUMPER_LEVEL_OUTPUT, "recovered.ply") << polyhedron;

	// Approximate support function evaluations in dual space from the dual image
	// of the recovered body
	std::vector<PCLPoint_3> directionsPCL(directions.begin(),
			directions.end());
	return dual(polyhedron).calculateSupportData(directionsPCL);
}

SupportFunctionDataPtr
generateSupportData(std::vector<Vector3d> &directions,
		std::vector<Vector3d> &targetPoints,
		double variance)
{

	std::default_random_engine generator;
	std::normal_distribution<double> noise(0., variance);
	std::vector<SupportFunctionDataItem> items;

	for (const auto &direction : directions)
	{
		double value = calculateSupportFunction(targetPoints, direction).first;
		ASSERT(value > 0.);
		double noisyValue =  value + noise(generator);
		ASSERT(noisyValue > 0.);
		items.push_back(SupportFunctionDataItem(direction, noisyValue));
	}
	SupportFunctionDataPtr data(new SupportFunctionData(items));
	return data;
}

std::vector<Vector3d> calculateDualTargetPoints(std::vector<Vector3d> &targetPoints)
{
	std::vector<Plane_3> planes;
	for (const auto &point : targetPoints)
	{
		planes.push_back(dual(point));
	}
	Polyhedron_3 dualBody(planes.begin(), planes.end());

	std::vector<Vector3d> dualTargetPoints;
	for (auto v = dualBody.vertices_begin(); v != dualBody.vertices_end(); ++v)
	{
		dualTargetPoints.push_back(Vector3d::fromCGAL(v->point()));
	}

	return dualTargetPoints;
}

double calculateError(SupportFunctionDataPtr a, SupportFunctionDataPtr b)
{
	auto valuesA = a->supportValues();
	auto valuesB = b->supportValues();
	auto diff = valuesB - valuesA;
	return diff.squaredNorm();
}

double fit(unsigned n, std::vector<Vector3d> &directions,
		unsigned numLiftingDimensions,
		unsigned numLiftingDimensionsDual,
		std::vector<Vector3d> &targetPoints, const char *title,
		const char *linearSolver, bool synthetic,
		SupportFunctionDataPtr noisyData)
{
	SupportFunctionDataPtr dualData;
	SupportFunctionDataPtr dualDataNotNoisy;
	double variance = 0.01;
	tryGetenvDouble("NOISE_VARIANCE", variance);

	globalPCLDumper.setNameBase(title);
	globalPCLDumper.enableVerboseMode();

	if (getenv("PRIMAL_MODE"))
	{
		ASSERT(synthetic);
		SupportFunctionDataPtr data = generateSupportData(directions, targetPoints, variance);

		auto pair = fitSimplexAffineImage(
				generateSimplex(numLiftingDimensions),
				data, targetPoints, numLiftingDimensions, false);
		auto polyhedronAM = pair.first;
		double error = pair.second;
		std::cout << "Algorithm error (sum of squares): " << error << std::endl;
		globalPCLDumper(PCL_DUMPER_LEVEL_OUTPUT, "am-recovered.ply") << polyhedronAM;
		return error;
	}
	else if (getenv("GAUGE_MODE"))
	{
		ASSERT(synthetic);
		// 1A. Calculate gauge function of primal body through support function of dual body
		auto dualTargetPoints = calculateDualTargetPoints(targetPoints);
		dualData = generateSupportData(directions, dualTargetPoints, variance);
		dualDataNotNoisy = generateSupportData(directions, dualTargetPoints, 0.);
	}
	else if (!synthetic)
	{
		dualData = calculateProvisionalEstimate(directions, noisyData,
				linearSolver);
	}
	else
	{
		// 1B. Provisional estimate for dual support function evaluations
		noisyData = generateSupportData(directions, targetPoints, variance);
		dualData = calculateProvisionalEstimate(directions, noisyData,
				linearSolver);
	}

	// 2. Soh & Chandrasekaran algorithm is used for estimating the body's shape

	auto pair = fitSimplexAffineImage(
			generateSimplex(numLiftingDimensionsDual),
			dualData, targetPoints, numLiftingDimensionsDual, false);
	auto polyhedronAMdual2 = pair.first;
	double error = pair.second;
	std::cout << "Algorithm error (sum of squares): " << error << std::endl;
	globalPCLDumper(PCL_DUMPER_LEVEL_OUTPUT, "am-dual2-recovered.ply")
		<< polyhedronAMdual2;
	auto polyhedronAM2 = dual(polyhedronAMdual2);
	globalPCLDumper(PCL_DUMPER_LEVEL_OUTPUT, "am2-recovered.ply")
		<< polyhedronAM2;

	if (getenv("GAUGE_MODE"))
	{
		printEstimationReport(polyhedronAMdual2, dualData);
		std::vector<PCLPoint_3> directionsPCL(directions.begin(),
				                        directions.end());
		return calculateError(dualDataNotNoisy, polyhedronAMdual2.calculateSupportData(directionsPCL));
	}
	else
		printEstimationReport(polyhedronAM2, noisyData);

	if (getenv("EARLY_STOP"))
		return error;

	// 3. Alternating minimization to recover the body with the shape estimated
	//    at the previous step

	auto Pcurrent = polyhedronAM2;
	for (int i = 0; i < 2; ++i)
	{
		for (auto I = Pcurrent.facets_begin(), E = Pcurrent.facets_end();
				                        I != E; ++I)
		{
			Plane_3 plane = I->plane();
			std::cout << "plane: " << plane << std::endl;
			// CHECK: planes should be not identical
		}
		SupportPolyhedronCorrector corrector(Pcurrent, noisyData);
		Pcurrent = corrector.run();
		printEstimationReport(Pcurrent, noisyData);
		auto name = std::string("corrected") + std::to_string(i) + ".ply";
		globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, name) << Pcurrent;
	}

	std::cout << "Number of implemented cases: " << numImplemented << std::endl;
	std::cout << "Number of non-implemented cases: " << numNonImplemented << std::endl;
	return error;
}

int runSyntheticCase(char **argv)
{
	int n = atoi(argv[1]);

	if (n <= 0)
	{
		std::cerr << "Expected positive number of measurements"
			<< std::endl;
		return EXIT_FAILURE;
	}

	std::vector<Vector3d> body;
	unsigned numLiftingDimensions = 0, numLiftingDimensionsDual = 0;
	const char *title = argv[2];
	if (!strcmp("cube", title))
	{
		body = generateLInfinityBall();
		numLiftingDimensions = 8;
		numLiftingDimensionsDual = 6;
	}
	else if (!strcmp("cubecut", title))
	{
		body = generateCubeCut();
		numLiftingDimensions = 10;
		numLiftingDimensionsDual = 7;
	}
	else if (!strcmp("octahedron", title))
	{
		body = generateL1Ball();
		numLiftingDimensions = 6;
		numLiftingDimensionsDual = 8;
	}
	else if (!strcmp("dodecahedron", title))
	{
		body = generateDodecahedron();
		numLiftingDimensions = 20;
		numLiftingDimensionsDual = 12;
	}
	else
	{
		std::cerr << "Unknown body" << std::endl;
		return EXIT_FAILURE;
	}

	const char *linearSolver = argv[3];

	if (!getenv("RANGE_MODE"))
	{
		std::cout << "Preparing data..." << std::endl;
		auto directions = generateDirections(n);

		double error = fit(n, directions, numLiftingDimensions,
				numLiftingDimensionsDual, body, title,
				linearSolver, true, nullptr);
		std::cout << "RESULT " << n << " " << error << std::endl;
		return EXIT_SUCCESS;
	}

	for (int n_current = 10; n_current <= n; n_current += 10)
	{
		std::cout << "Preparing data..." << std::endl;
		auto directions = generateDirections(n_current);

		double error = fit(n_current, directions, numLiftingDimensions,
				numLiftingDimensionsDual, body, title,
				linearSolver, true, nullptr);
		std::cout << "RESULT " << n_current << " " << error << std::endl;
	}

	return EXIT_SUCCESS;
}

int runRealCase(char **argv)
{
	const char *path = argv[1];
	int numVertices = atoi(argv[2]);
	if (numVertices <= 0)
	{
		std::cerr << "Expected positive number of vertices"
			<< std::endl;
		return EXIT_FAILURE;
	}
	int numFacets = atoi(argv[3]);
	if (numFacets <= 0)
	{
		std::cerr << "Expected positive number of facets"
			<< std::endl;
		return EXIT_FAILURE;
	}


	const char *linearSolver = argv[4];

	const char *title = "diamond";

	/* Create fake empty polyhedron. */
        PolyhedronPtr p(new Polyhedron());

        /* Create shadow contour data and associate p with it. */
        ShadowContourDataPtr SCData(new ShadowContourData(p));

        /* Read shadow contours data from file. */
        SCData->fscanDefault(path);

	SupportFunctionDataConstructor constructor;

	constructor.enableBalanceShadowContours();
	constructor.enableConvexifyShadowContour();
	SupportFunctionDataPtr data = constructor.run(SCData, SCData->numContours);
	Vector3d balancingVector = constructor.balancingVector();

	auto directions = data->supportDirections<Vector3d>();
	std::vector<Vector3d> fake;
	double error = fit(directions.size(), directions, numVertices,
			numFacets, fake, title, linearSolver,
			false, data);
	// FIXME: Shift the body back by this vector:
	std::cout << "Balancing vector: " << balancingVector << std::endl;
	std::cout << "RESULT " << directions.size() << " " << error << std::endl;

	return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
	if (argc == 4)
		return runSyntheticCase(argv);
	if (argc == 5)
		return runRealCase(argv);

	std::cerr << "Expected 4 or 5 arguments:" << std::endl;
	std::cerr << "\t" << argv[0] << " measurements_number body_name linear_solver" << std::endl;
	std::cerr << "\t" << argv[0] << " shadow_contours_path n_vertices n_facets liner_solver" << std::endl;
	return EXIT_FAILURE;
}
