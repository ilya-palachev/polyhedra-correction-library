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

#include "Common.h"
#include "DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataConstructor.h"
#include "PolyhedraCorrectionLibrary.h"
#include "Recoverer/SupportPolyhedronCorrector.h"
#include "SupportFunction.h"
#include "TractableFitter/AlternatingMinimization.h"

template <typename VectorT> std::vector<VectorT> generateDirections(int n)
{
	std::default_random_engine generator;
	std::normal_distribution<double> distribution(0., 1.);

	std::vector<VectorT> directions;
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

		VectorT v(x, y, z);
		directions.push_back(v);
	}

	return directions;
}

std::vector<Vector3d> generateCollinearDirections(int n, Plane_3 plane)
{
	std::default_random_engine generator;
	std::normal_distribution<double> distribution(0., 1.);

	std::vector<Vector3d> directions;
	for (int i = 0; i < n; ++i)
	{
		double x = 0., y = 0., lambda = 0.;
		do
		{
			x = distribution(generator);
			y = distribution(generator);
			lambda = sqrt(x * x + y * y);
		} while (lambda < 1e-16);

		x /= lambda;
		y /= lambda;
		// Now (x, y) is uniformly distributed on the 2D circle

		Vector_3 v = plane.base1() * x + plane.base2() * y;
		v /= sqrt(v.squared_length());
		directions.push_back(Vector3d::fromCGAL(v));
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

static std::vector<Vector3d> makeInitialBody(std::vector<Vector3d> &directions,
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

	double threshold = 0.1;
	tryGetenvDouble("JOIN_THRESHOLD", threshold);
	recoverer->setThreshold(threshold);

	std::cout << "Running Ipopt estimator..." << std::endl;
	auto polyhedron = recoverer->run(noisyData);
	globalPCLDumper(PCL_DUMPER_LEVEL_OUTPUT, "initial-body.ply") << polyhedron;

	auto dualP = dual(polyhedron);
	std::set<Vector3d> points;
	CGAL::Origin O;
	for (auto I = dualP.vertices_begin(), E = dualP.vertices_end(); I != E; ++I)
	{
		auto truePoint = Vector3d::fromCGAL(I->point() - O);
		points.insert(truePoint);
	}
	auto pointsWithoutRepeats =
		std::vector<Vector3d>(points.begin(), points.end());
	auto last = std::unique(
		pointsWithoutRepeats.begin(), pointsWithoutRepeats.end(),
		[](Vector3d a, Vector3d b) { return length(a - b) < 1e-6; });
	pointsWithoutRepeats.erase(last, pointsWithoutRepeats.end());

	for (auto p : pointsWithoutRepeats)
		std::cout << "Initial point: " << p << std::endl;

	return pointsWithoutRepeats;
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
	globalPCLDumper(PCL_DUMPER_LEVEL_OUTPUT, "provisional-estimate.ply")
		<< polyhedron;

	// Approximate support function evaluations in dual space from the dual
	// image of the recovered body
	std::vector<PCLPoint_3> directionsPCL(directions.begin(), directions.end());
	return dual(polyhedron).calculateSupportData(directionsPCL);
}

SupportFunctionDataPtr generateSupportData(std::vector<Vector3d> &directions,
										   std::vector<Vector3d> &targetPoints,
										   double variance)
{

	std::default_random_engine generator;
	std::normal_distribution<double> noise(0., variance);
	std::vector<SupportFunctionDataItem> items;

	for (const auto &direction : directions)
	{
		double value = calculateSupportFunction(targetPoints, direction);
		ASSERT(value > 0.);
		double noisyValue = value + noise(generator);
		ASSERT(noisyValue > 0.);
		items.push_back(SupportFunctionDataItem(direction, noisyValue));
	}
	SupportFunctionDataPtr data(new SupportFunctionData(items));
	return data;
}

double calculateError(SupportFunctionDataPtr a, SupportFunctionDataPtr b)
{
	auto valuesA = a->supportValues();
	auto valuesB = b->supportValues();
	auto diff = valuesB - valuesA;
	return diff.squaredNorm();
}

static std::vector<Vector3d>
makeTrueStartingBody(std::vector<Vector3d> &targetPoints)
{
	CGAL::Origin O;
	Polyhedron_3 hull;
	std::vector<Point_3> targetPointsCGAL;
	std::cout << "True points:" << std::endl;
	for (auto point : targetPoints)
	{
		targetPointsCGAL.push_back(O + Vector_3(point));
		std::cout << point << std::endl;
	}

	CGAL::convex_hull_3(targetPointsCGAL.begin(), targetPointsCGAL.end(), hull);
	auto dualHull = dual(hull);
	globalPCLDumper(PCL_DUMPER_LEVEL_OUTPUT, "true-dual.ply") << dualHull;
	std::set<Vector3d> trueDualPlanes;
	for (auto I = dualHull.vertices_begin(), E = dualHull.vertices_end();
		 I != E; ++I)
	{
		auto truePoint = Vector3d::fromCGAL(I->point() - O);
		trueDualPlanes.insert(truePoint);
	}

	std::cout << "True dual planes:" << std::endl;
	for (auto truePoint : trueDualPlanes)
	{
		std::cout << truePoint << std::endl;
	}

	return std::vector<Vector3d>(trueDualPlanes.begin(), trueDualPlanes.end());
}

void fit(unsigned n, std::vector<Vector3d> &directions,
		 unsigned numLiftingDimensions, unsigned numLiftingDimensionsDual,
		 std::vector<Vector3d> &targetPoints, const char *title,
		 const char *linearSolver, SupportFunctionDataPtr noisyData)
{
	SupportFunctionDataPtr dualData;
	SupportFunctionDataPtr dualDataNotNoisy;

	globalPCLDumper.setNameBase(title);
	globalPCLDumper.enableVerboseMode();

	// 1. Provisional estimate for dual support function evaluations
	dualData =
		calculateProvisionalEstimate(directions, noisyData, linearSolver);

	// 2. Soh & Chandrasekaran algorithm is used for estimating the body's shape

	auto minimization =
		getenv("USE_TRUE_STARTING_BODY")
			? AlternatingMinimization(makeTrueStartingBody(targetPoints))
			: getenv("USE_STARTING_BODY")
				  ? AlternatingMinimization(
						makeInitialBody(directions, noisyData, linearSolver))
				  : AlternatingMinimization();
	auto polyhedronAMdual2 =
		minimization.run(dualData, numLiftingDimensionsDual);
	std::cout << "Dual polyhedron has " << polyhedronAMdual2.size_of_vertices()
			  << " vertice vertices" << std::endl;
	globalPCLDumper(PCL_DUMPER_LEVEL_OUTPUT, "am-dual2-recovered.ply")
		<< polyhedronAMdual2;

	auto polyhedronAM2 = dual(polyhedronAMdual2);
	globalPCLDumper(PCL_DUMPER_LEVEL_OUTPUT, "am2-recovered.ply")
		<< polyhedronAM2;

	printEstimationReport(polyhedronAM2, noisyData);

	if (getenv("EARLY_STOP"))
		return;

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

	return;
}

struct BodyDescription
{
	std::vector<Vector3d> vertices;
	unsigned numVertices;
	unsigned numFacets;
};

BodyDescription makeBody(const char *title)
{
	if (!strcmp("cube", title))
		return {generateLInfinityBall(), 8, 6};
	else if (!strcmp("cubecut", title))
		return {generateCubeCut(), 10, 7};
	else if (!strcmp("octahedron", title))
		return {generateL1Ball(), 6, 8};
	else if (!strcmp("dodecahedron", title))
		return {generateDodecahedron(), 20, 12};
	else
	{
		std::cerr << "Unknown body name: " << title << std::endl;
		exit(EXIT_FAILURE);
	}
}

int runSyntheticCase(char **argv)
{
	int n = atoi(argv[1]);

	if (n <= 0)
	{
		std::cerr << "Expected positive number of measurements" << std::endl;
		return EXIT_FAILURE;
	}

	const char *title = argv[2];
	BodyDescription body = makeBody(title);

	const char *linearSolver = argv[3];

	double variance = 0.01;
	tryGetenvDouble("NOISE_VARIANCE", variance);

	if (!getenv("RANGE_MODE"))
	{
		std::cout << "Preparing data..." << std::endl;
		auto directions = generateDirections<Vector3d>(n);

		auto noisyData =
			generateSupportData(directions, body.vertices, variance);
		fit(n, directions, body.numVertices, body.numFacets, body.vertices,
			title, linearSolver, noisyData);
		return EXIT_SUCCESS;
	}

	for (int n_current = 10; n_current <= n; n_current += 10)
	{
		std::cout << "Preparing data..." << std::endl;
		auto directions = generateDirections<Vector3d>(n_current);

		auto noisyData =
			generateSupportData(directions, body.vertices, variance);
		fit(n_current, directions, body.numVertices, body.numFacets,
			body.vertices, title, linearSolver, noisyData);
	}

	return EXIT_SUCCESS;
}

SupportFunctionDataPtr generateEvenDataFromContours(ShadowContourDataPtr SCData,
													int n)
{
	std::vector<SupportFunctionDataItem> items;
	for (int i = 0; i < SCData->numContours; ++i)
	{
		SContour contour = SCData->contours[i];
		Plane_3 plane(contour.plane);
		auto directions = generateCollinearDirections(n, plane);
		auto points = contour.getPoints();
		for (Vector3d direction : directions)
		{
			double value = calculateSupportFunction(points, direction);
			items.emplace_back(direction, value);
		}
	}

	SupportFunctionDataPtr data(new SupportFunctionData(items));
	return data;
}

int runSyntheticContourCase(char **argv)
{
	int n = atoi(argv[1]);

	if (n <= 0)
	{
		std::cerr << "Expected positive number of measurements" << std::endl;
		return EXIT_FAILURE;
	}

	// 1. Read the model of 3D polyhedron from the PLY file

	const char *title = argv[2];
	PolyhedronPtr p(new Polyhedron());
	std::string path = "poly-data-in/";
	path += title;
	path += ".ply";
	std::cout << "Scanning PLY file format..." << std::endl;
	p->fscan_ply(path.c_str());
	std::cout << "Scanning PLY file format... done" << std::endl;
	if (p->numVertices == 0)
	{
		exit(EXIT_FAILURE);
	}

	globalPCLDumper.setNameBase(title);
	globalPCLDumper.enableVerboseMode();

	// 2. Generate shadow contours from the polyhedron with error `variance`

	ASSERT(p->nonZeroPlanes());
	ShadowContourDataPtr SCData(new ShadowContourData(p));
	ASSERT(p->nonZeroPlanes());
	ShadowContourConstructorPtr shadowConstructor(
		new ShadowContourConstructor(p, SCData));
	std::cout << "Constructing contours..." << std::endl;
	shadowConstructor->run(n, 0.01); // this is angle, not error
	std::cout << "Constructing contours... done" << std::endl;
	ASSERT(!SCData->empty());

	double variance = 0.01;
	tryGetenvDouble("NOISE_VARIANCE", variance);

	SCData->shiftRandomly(variance);
	ASSERT(!SCData->empty());

	// 3. Generate EVEN support function measurements from the shadow contours

	unsigned numItemsPerContour = 10;
	double numItemsPerContourEnv = -1;
	tryGetenvDouble("N_ITEMS_PER_CONTOUR", numItemsPerContourEnv);
	if (numItemsPerContourEnv > 0.)
		numItemsPerContour = static_cast<int>(numItemsPerContourEnv);
	auto data = generateEvenDataFromContours(SCData, numItemsPerContour);

	// 4. Run the AM algorithm in the primal space, considering the number of
	// vertices to be known

	auto directions = data->supportDirections<Vector3d>();
	auto polyhedronAM = AlternatingMinimization().run(data, p->numVertices);
	globalPCLDumper(PCL_DUMPER_LEVEL_OUTPUT, "am-intermediate-recovered.ply")
		<< polyhedronAM;

	// 5. Produce the gauge function data from the primal-AM-recovered body

	// NOTA BENE: These are new directions!!! It is very crucial for the quality
	// of results produced by the AM algorithm
	auto otherDirections = generateDirections<Point_3>(data->size());
	auto dualData = dual(polyhedronAM).calculateSupportData(otherDirections);

	// 6. Run the AM algorithm in the dual space, using the produced gauge
	// function measurements

	auto polyhedronDualAM =
		AlternatingMinimization().run(dualData, p->numFacets);
	globalPCLDumper(PCL_DUMPER_LEVEL_OUTPUT, "am-dual-recovered.ply")
		<< polyhedronDualAM;

	// 7. Convert the body form the dual space back to the primal space to
	// obtain the body with the proper topology of spaces and vertices

	Polyhedron_3 polyhedronTopologyAM = dual(polyhedronDualAM);
	globalPCLDumper(PCL_DUMPER_LEVEL_OUTPUT, "am-topology-recovered.ply")
		<< polyhedronTopologyAM;

	return EXIT_SUCCESS;
}

int runRealCase(char **argv)
{
	const char *path = argv[1];
	int numVertices = atoi(argv[2]);
	if (numVertices <= 0)
	{
		std::cerr << "Expected positive number of vertices" << std::endl;
		return EXIT_FAILURE;
	}
	int numFacets = atoi(argv[3]);
	if (numFacets <= 0)
	{
		std::cerr << "Expected positive number of facets" << std::endl;
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
	fit(directions.size(), directions, numVertices, numFacets, fake, title,
		linearSolver, data);
	// FIXME: Shift the body back by this vector:
	std::cout << "Balancing vector: " << balancingVector << std::endl;

	return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
	if (argc == 3)
		return runSyntheticContourCase(argv);
	if (argc == 4)
		return runSyntheticCase(argv);
	if (argc == 5)
		return runRealCase(argv);

	std::cerr << "Expected 4 or 5 arguments:" << std::endl;
	std::cerr << "\t" << argv[0]
			  << " measurements_number body_name linear_solver" << std::endl;
	std::cerr << "\t" << argv[0]
			  << " shadow_contours_path n_vertices n_facets liner_solver"
			  << std::endl;
	return EXIT_FAILURE;
}
