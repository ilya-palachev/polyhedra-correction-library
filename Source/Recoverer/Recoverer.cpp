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

#include <cmath>

#include <Eigen/Dense>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "PCLDumper.h"
#include "TimeMeasurer/TimeMeasurer.h"
#include "DataContainers/ShadowContourData/ShadowContourData.h"
#include "Recoverer/Recoverer.h"
#include "Recoverer/CGALSupportFunctionEstimator.h"
#include "Recoverer/IpoptSupportFunctionEstimator.h"
#include "Recoverer/TsnnlsSupportFunctionEstimator.h"
#include "Recoverer/GlpkSupportFunctionEstimator.h"
#include "Recoverer/ClpSupportFunctionEstimator.h"
#include "Recoverer/CPLEXSupportFunctionEstimator.h"
#include "Recoverer/NativeSupportFunctionEstimator.h"
#include "DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataConstructor.h"
#include "Recoverer/NaiveFacetJoiner.h"
#include "Recoverer/TrustedEdgesDetector.h"

TimeMeasurer timer;
std::set<int> indicesDirectionsIgnored;

Recoverer::Recoverer() :
	estimatorType(CGAL_ESTIMATOR),
	ifBalancing(false),
	balancingVector_(0., 0., 0.),
	ifConvexifyContours(false),
	ifScaleMatrix(false),
	problemType_(DEFAULT_ESTIMATION_PROBLEM_NORM),
	numMaxContours(IF_ANALYZE_ALL_CONTOURS),
	fileNamePolyhedron_(NULL),
	threshold_(0.),
	zMinimalNorm_(0.)
{
	DEBUG_START;
	DEBUG_END;
}

Recoverer::~Recoverer()
{
	DEBUG_START;
	DEBUG_END;
}

void Recoverer::enableBalancing(void)
{
	DEBUG_START;
	ifBalancing = true;
	DEBUG_END;
}

void Recoverer::enableContoursConvexification(void)
{
	DEBUG_START;
	ifConvexifyContours = true;
	DEBUG_END;
}

void Recoverer::disableContoursConvexification(void)
{
	DEBUG_START;
	ifConvexifyContours = false;
	DEBUG_END;
}

void Recoverer::enableMatrixScaling(void)
{
	DEBUG_START;
	ifScaleMatrix = true;
	DEBUG_END;
}

void Recoverer::setEstimatorType(RecovererEstimatorType e)
{
	DEBUG_START;
	estimatorType = e;
	DEBUG_END;
}

void Recoverer::setSupportMatrixType(SupportMatrixType type)
{
	DEBUG_START;
	supportMatrixType_ = type;
	DEBUG_END;
}
void Recoverer::setStartingBodyType(
		SupportFunctionEstimationStartingBodyType type)
{
	DEBUG_START;
	startingBodyType_ = type;
	DEBUG_END;
}

void Recoverer::setProblemType(EstimationProblemNorm type)
{
	DEBUG_START;
	problemType_ = type;
	DEBUG_END;
}

void Recoverer::setFileNamePolyhedron(char *fileNamePolyhedron)
{
	DEBUG_START;
	fileNamePolyhedron_ = fileNamePolyhedron;
	DEBUG_END;
}

#define ACCEPTED_TOL 1e-6

static void printEstimationReport(SparseMatrix Q, VectorXd h0, VectorXd h,
	RecovererEstimatorType estimatorType)
{
	DEBUG_START;
	if (h.size() != h0.size())
	{
		MAIN_PRINT("no or incorrect 3rd-party data");
	}
	MAIN_PRINT("Estimation report:");
	double DEBUG_VARIABLE L1 = 0.;
	double DEBUG_VARIABLE L2 = 0.;
	double DEBUG_VARIABLE Linf = 0.;
	ASSERT(h0.size() == h.size());
	int numDirectionsAccounted = 0;
	for (int i = 0; i < h.size(); ++i)
	{
		if (indicesDirectionsIgnored.find(i)
				!= indicesDirectionsIgnored.end())
			continue;
		++numDirectionsAccounted;
		double delta = h0(i) - h(i);
		L1 += fabs(delta);
		L2 += delta * delta;
		Linf = delta > Linf ? delta : Linf;
	}
	MAIN_PRINT("L1 = %lf = %le", L1, L1);
	double L1average = L1 / numDirectionsAccounted;
	MAIN_PRINT("L1 / %d = %lf = %le", numDirectionsAccounted, L1average,
			L1average);
	MAIN_PRINT("Sum of squares = %lf = %le", L2, L2);
	L2 = sqrt(L2);
	MAIN_PRINT("L2 = %lf = %le", L2, L2);
	double L2average = L2 / sqrt(numDirectionsAccounted);
	MAIN_PRINT("L2 / sqrt(%d) = %lf = %le", numDirectionsAccounted, L2average,
			L2average);
	MAIN_PRINT("Linf = %lf = %le", Linf, Linf);
	if (getenv("PCL_PRINT_VARIABLE_CHANGE"))
	{
		for (int i = 0; i < h.size(); ++i)
		{
			double delta = h0(i) - h(i);
			MAIN_PRINT("h[%d] :  %lf - %lf = %lf", i, h0(i), h(i),
					delta);
		}
	}

	DEBUG_PRINT("-------------------------------");
	DEBUG_END;
}

static SupportFunctionEstimator *constructEstimator(
		SupportFunctionEstimationDataPtr data,
		RecovererEstimatorType estimatorType,
		EstimationProblemNorm problemType,
		double threshold)
{
	DEBUG_START;

	SupportFunctionEstimator *estimator = NULL;
	switch (estimatorType)
	{
	case ZERO_ESTIMATOR:
		DEBUG_PRINT("Zero estimatorType is on!");
		break;
#define ESTIMATOR_CASE(ID, TYPE) \
	case ID: \
		estimator = new TYPE(data); \
		break
	ESTIMATOR_CASE(NATIVE_ESTIMATOR, NativeSupportFunctionEstimator);
#ifdef USE_TSNNLS
	ESTIMATOR_CASE(TSNNLS_ESTIMATOR, TsnnlsSupportFunctionEstimator);
#endif /* USE_TSNNLS */
#ifdef USE_IPOPT
	ESTIMATOR_CASE(IPOPT_ESTIMATOR, IpoptSupportFunctionEstimator);
#endif /* USE_IPOPT */
#ifdef USE_GLPK
	ESTIMATOR_CASE(GLPK_ESTIMATOR, GlpkSupportFunctionEstimator);
#endif /* USE_GLPK */
#ifdef USE_CLP
	ESTIMATOR_CASE(CLP_ESTIMATOR, ClpSupportFunctionEstimator);
	ESTIMATOR_CASE(CLP_COMMAND_ESTIMATOR,
			ClpCommandLineSupportFunctionEstimator);
#endif /* USE_CLP */
#ifdef USE_CPLEX
	ESTIMATOR_CASE(CPLEX_ESTIMATOR, CPLEXSupportFunctionEstimator);
#endif /* USE_CPLEX */
	ESTIMATOR_CASE(CGAL_ESTIMATOR, CGALSupportFunctionEstimator);
#undef ESTIMATOR_CASE
	}
	estimator->setProblemType(problemType);
	DEBUG_END;
	return estimator;
}

static std::vector<Plane_3> produceCorrectedPlanes(
		SupportFunctionEstimationDataPtr dataEstimation,
		VectorXd estimate)
{
	DEBUG_START;
	/* Construct vector of corrected support planes. */
	std::vector<Vector3d> directions = dataEstimation->supportDirections();
	ASSERT(directions.size() == (unsigned int) estimate.rows());
	std::vector<Plane_3> planes;
	for (unsigned int i = 0; i < directions.size(); ++i)
	{
		planes.push_back(Plane_3(directions[i].x, directions[i].y,
					directions[i].z, -estimate(i)));
	}
	DEBUG_END;
	return planes;
}

static Polyhedron_3 producePolyhedron(
		SupportFunctionEstimationDataPtr dataEstimation,
		VectorXd estimate)
{
	DEBUG_START;
	/* Intersect halfspaces corresponding to corrected planes. */
	auto planes = produceCorrectedPlanes(dataEstimation, estimate);
	Polyhedron_3 intersection(planes);
	DEBUG_END;
	return intersection;
}

static VectorXd supportValuesFromPoints(std::vector<Point_3> directions,
	VectorXd v)
{
	DEBUG_START;
	VectorXd values(directions.size());
	ASSERT(3 * directions.size() == (unsigned) v.rows());
	int num = directions.size();
#ifndef NDEBUG
	for (int i = 0; i < v.rows(); ++i)
	{
		DEBUG_PRINT("v(%d) = %lf", i, v(i));
	}
#endif /* NDEBUG */
	for (int i = 0; i < num; ++i)
	{
		Point_3 direction = directions[i];
		values(i) = direction.x() * v(3 * i)
			+ direction.y() * v(3 * i + 1)
			+ direction.z() * v(3 * i + 2);
	}
	DEBUG_END;
	return values;
}


void Recoverer::setNumMaxContours(int number)
{
	DEBUG_START;
	numMaxContours = number;
	DEBUG_END;
}

std::vector<std::vector<int>> reindexClusters(
		SupportFunctionEstimationDataPtr data,
		VectorXd estimate,
		Polyhedron_3 polyhedronConsistent,
		std::vector<std::vector<int>> clusters)
{
	DEBUG_START;
	/* Build map from polyhedron facets' IDs to support items' IDs. */
	std::vector<int> map(polyhedronConsistent.size_of_facets());
	for (auto facet = polyhedronConsistent.facets_begin();
			facet != polyhedronConsistent.facets_end();
			++facet)
	{
		int id = polyhedronConsistent.indexPlanes_[facet->id];
		map[facet->id] = id;
	}

	/* Map clusters' IDs to support items IDs. */
	std::vector<std::vector<int>> clustersNew;
	for (auto cluster: clusters)
	{
		std::vector<int> clusterNew;
		for (int iFacet: cluster)
		{
			int id = map[iFacet];
			clusterNew.push_back(id);
		}
		std::cerr << "new cluster:";
		for (int iFacet: clusterNew)
		{
			std::cerr << " " << iFacet;
		}
		std::cerr << std::endl;
		clustersNew.push_back(clusterNew);
	}
	DEBUG_END;
	return clustersNew;
}

static Polyhedron_3 produceFinalPolyhedron(
	SupportFunctionEstimationDataPtr data,
	VectorXd estimate,
	VectorXd h3rdParty,
	RecovererEstimatorType estimatorType,
	double threshold)
{
	DEBUG_START;
	timer.pushTimer();
	auto directions = data->supportData()->supportDirectionsCGAL();
	char *normMinimalString = getenv("NORM_MINIMAL");
	double normMinimal = 0.;
	if (normMinimalString)
	{
		char *mistake = NULL;
		normMinimal = strtod(normMinimalString, &mistake);
		if (mistake && *mistake)
		{
			std::cerr << "mistake: " << mistake << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	for (int i = 0; i < (int) directions.size(); ++i)
	{
		Point_3 direction = directions[i];
		double normOXYsquared = direction.x() * direction.x()
				+ direction.y() * direction.y();
		if (normOXYsquared < normMinimal && direction.z() < 0.)
		{
			indicesDirectionsIgnored.insert(i);
		}
	}
	std::cerr << "Number of ignored directions: "
		<< indicesDirectionsIgnored.size() << std::endl;

	auto h = supportValuesFromPoints(directions, estimate);
	/* Now produce polyhedron from the estimate. */
	Polyhedron_3 polyhedron = producePolyhedron(data, h);

	/* Also produce naive polyhedron (to compare recovered one with it). */
	auto h0 = data->supportVector();
	Polyhedron_3 polyhedronNaive = producePolyhedron(data,	h0);
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "naively-recovered.ply")
		<< polyhedronNaive;

	/* Also produce polyhedron from starting point of the algorithm. */
	auto hStarting = supportValuesFromPoints(directions,
			data->startingVector());
	Polyhedron_3 polyhedronStart = producePolyhedron(data,	hStarting);
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "starting-polyhedron.ply")
		<< polyhedronStart;

	MAIN_PRINT("===== 3rd-party polyhedron =====");
	printEstimationReport(data->supportMatrix(), h0, h3rdParty,
			estimatorType);
	MAIN_PRINT("===== Starting point: =====");
	printEstimationReport(data->supportMatrix(), h0, hStarting,
			estimatorType);
	MAIN_PRINT("======== First estimate: ========");
	printEstimationReport(data->supportMatrix(), h0, h, estimatorType);

	std::cout << "Final polyhedron construction: " << timer.popTimer()
		<< std::endl;

	/* Reset polyhedron facets' IDs according to initial items order. */
	auto planes = produceCorrectedPlanes(data, h);
	polyhedron.initialize_indices(planes);

	Polyhedron_3 polyhedronConsistent = polyhedron;

	std::vector<std::vector<int>> clusters;
	if (threshold > 0.)
	{
		/* Produce joined polyhedron: */
		NaiveFacetJoiner joiner(polyhedron, threshold);
		auto pairJoined = joiner.run();
		Polyhedron_3 polyhedronJoined = pairJoined.first;
		VectorXd estimateJoined =
			polyhedronJoined.findTangientPointsConcatenated(
					directions);
		auto hJoined = supportValuesFromPoints(directions,
				estimateJoined);
		globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "estimated-joined.ply");
		MAIN_PRINT("======== Joined estimate: ========");
		printEstimationReport(data->supportMatrix(), h0, hJoined,
				estimatorType);
		polyhedron = polyhedronJoined;
		clusters = pairJoined.second;
	}

	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "support-planes.ply")
		<< data->supportData();

	DEBUG_END;
	return polyhedron;
}

/**
 * Prepares the data about the 3rd-party polyhedron costructed not in our
 * library. It will be used to compare out result with it in different support
 * metrics.
 *
 * @param fileNamePolyhedron	The path to 3rd-party polyhedron.
 * @param directions		The support directions for measurements.
 * @param shift			The shift vector on which we alreadt shifted our
 * 				data to fit the constraint that (0, 0, 0) is
 * 				inside the negative side of all support values.
 */
VectorXd prepare3rdPartyValues(char *fileNamePolyhedron,
		std::vector<Point_3> directions, Vector_3 shift)
{
	DEBUG_START;
	PolyhedronPtr p(new Polyhedron());
	/* 
	 * FIXME: Current implementation assumes one fixed type of 3rd-party
	 * data.
	 */
	p->fscan_default_1_2(fileNamePolyhedron);

	Polyhedron_3 polyhedron(p);
	polyhedron.shift(shift);
	SupportFunctionDataConstructor constructor;
	auto data3rdParty = constructor.run(directions, polyhedron);
	auto h3rdParty = data3rdParty->supportValues();

	Polyhedron *pCopy = new Polyhedron(polyhedron);

	/*
	 * We dump 3-rd party polyhedron here, since it's shifted and can be
	 * comfortably compared with our polyhedron using some vizualization
	 * program.
	 */
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "3rd-party-recovered.ply")
		<< *pCopy;
	std::cerr << "3rd party polyhedron has " << p->numFacets << " facets"
		<< std::endl;

	DEBUG_END;
	return h3rdParty;
}

Polyhedron_3 Recoverer::run(SupportFunctionDataPtr data)
{
	DEBUG_START;
	std::cout << "Number of support function items: " << data->size()
		<< std::endl;

	/* 1. Build support function estimation data. */
	timer.pushTimer();
	SupportFunctionEstimationDataConstructor constructorEstimation;
	if (ifScaleMatrix)
		constructorEstimation.enableMatrixScaling();
	if (estimatorType == NATIVE_ESTIMATOR)
		supportMatrixType_ = SUPPORT_MATRIX_TYPE_EMPTY;
	SupportFunctionEstimationDataPtr dataEstimation
		= constructorEstimation.run(data, supportMatrixType_,
				startingBodyType_);
	std::cout << "Time for estimation data preparation: "
		<< timer.popTimer() << std::endl;

	/* 2. Build support function estimator. */
	timer.pushTimer();
	SupportFunctionEstimator *estimator = constructEstimator(dataEstimation,
			estimatorType, problemType_, threshold_);

	/* 3. Run support function estimation. */
	VectorXd estimate(dataEstimation->numValues());
	if (estimator)
		estimate = estimator->run();
	else
		estimate = dataEstimation->supportVector();
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "support-vector-estimate.mat")
		<< estimate;
	std::cout << "Tine for estimation: " << timer.popTimer() << std::endl;

	/* 4. Validate the result of estimation. */
	if(!constructorEstimation.checkResult(dataEstimation,
				supportMatrixType_, estimate))
	{
		exit(EXIT_FAILURE);
	}

	/* 5. Prepare 3rd-party data to be compared with. */
	VectorXd h3rdParty(1);
	if (fileNamePolyhedron_)
	{
		auto directions = data->supportDirectionsCGAL();
		h3rdParty = prepare3rdPartyValues(fileNamePolyhedron_,
				directions, balancingVector_);
	}

	/* 6. Produce final polyhedron and reports about it. */
	Polyhedron_3 polyhedron = produceFinalPolyhedron(dataEstimation,
			estimate, h3rdParty, estimatorType, threshold_);

	DEBUG_END;
	return polyhedron;
}

Polyhedron_3 Recoverer::run(ShadowContourDataPtr dataShadow)
{
	DEBUG_START;
	/* 0. Build support function data. */
	timer.pushTimer();
	SupportFunctionDataConstructor constructor;
	if (ifBalancing)
	{
		constructor.enableBalanceShadowContours();
	}
	if (ifConvexifyContours)
		constructor.enableConvexifyShadowContour();
	SupportFunctionDataPtr data = constructor.run(dataShadow,
			numMaxContours);
	if (ifBalancing)
	{
		balancingVector_ = constructor.balancingVector();
	}
	std::cout << "Support data extraction: " << timer.popTimer()
		<< std::endl;

	/* 1. Run the recoverer for the constructed data. */
	Polyhedron_3 polyhedron = run(data);

	DEBUG_END;
	return polyhedron;
}

