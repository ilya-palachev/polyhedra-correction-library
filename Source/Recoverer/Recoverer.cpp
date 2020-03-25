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
#include "Recoverer/SupportPolyhedronCorrector.h"
#include "Recoverer/EdgeCorrector.h"
#include "Recoverer/ContourModeRecoverer.h"

TimeMeasurer timer;
std::stack<const char *> taskNames;

void pushTimer(const char *taskName)
{
	taskNames.push(taskName);
	std::cout << COLOUR_GREEN << "========== Starting task: \"" << taskName
		<< "\" ==========" << COLOUR_NORM << std::endl;
	timer.pushTimer();
}

void popTimer()
{
	const char *taskName = taskNames.top();
	std::cout << COLOUR_GREEN << "========== Completed task \"" << taskName
		<< "\" in time: " << timer.popTimer() << " =========="
		<< COLOUR_NORM << std::endl;
	taskNames.pop();
}

std::set<int> indicesDirectionsIgnored;

Recoverer::Recoverer() :
	estimatorType(CGAL_ESTIMATOR),
	ifBalancing(false),
	balancingVector_(0., 0., 0.),
	ifConvexifyContours(false),
	ifScaleMatrix(false),
	supportMatrixType_(DEFAULT_SUPPORT_MATRIX_TYPE),
	startingBodyType_(SUPPORT_FUNCTION_ESTIMATION_STARTING_BODY_TYPE_CYLINDERS_INTERSECTION),
	problemType_(DEFAULT_ESTIMATION_PROBLEM_NORM),
	numMaxContours(IF_ANALYZE_ALL_CONTOURS),
	fileNamePolyhedron_(NULL),
	threshold_(0.),
	zMinimalNorm_(0.),
	ifShadowHeuristics_(false),
	ifContourMode_(false)
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

void printEstimationReport(VectorXd h0, VectorXd h)
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
		EstimationProblemNorm problemType)
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

static Polyhedron_3 producePolyhedron(
		SupportFunctionEstimationDataPtr data,
		VectorXd values, const char *title, bool ifPrintReport = true)
{
	DEBUG_START;
	/* Intersect halfspaces corresponding to corrected planes. */
	auto planes = produceCorrectedPlanes(data, values);
	Polyhedron_3 intersection(planes);
	
	/* Dump the resulting polyhedron to the debug file. */
	std::string name = title;
       	name += ".DONT_USE_THIS_FILE.ply";
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, name.c_str()) << intersection;

	if (ifPrintReport)
	{
		/* Write the estimation report about this polyhedron. */
		std::cout << "Report about \"" << title << "\":" << std::endl;
		printEstimationReport(data->supportVector(), values);
	}

	DEBUG_END;
	return intersection;
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

/**
 * Produces directions IDs that should be ignored.
 *
 * @param directions		The initial directions.
 * @param normMinimal		The minimal norm of Oxy projection that
 * 				is required for the direction for not to be 
 * 				ignored.
 *
 * FIXME: Ignore only down-side directions, upper-side should be stayed "as is".
 */
std::set<int> prepareIgnoredIndices(std::vector<Point_3> directions,
		double normMinimal)
{
	DEBUG_START;
	std::set<int> ignored;
	for (int i = 0; i < (int) directions.size(); ++i)
	{
		Point_3 direction = directions[i];
		double normOXYsquared = direction.x() * direction.x()
				+ direction.y() * direction.y();
		if (normOXYsquared < normMinimal && direction.z() < 0.)
		{
			ignored.insert(i);
		}
	}
	std::cerr << "Number of ignored directions: " << ignored.size()
		<< std::endl;
	DEBUG_END;
	return ignored;
}

std::pair<VectorXd, SupportFunctionEstimationDataPtr>
Recoverer::runEstimation(SupportFunctionDataPtr SData)
{
	DEBUG_START;
	std::cout << "Number of support function items: " << SData->size()
		<< std::endl;
	if (ifShadowHeuristics_)
		globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "support-planes.ply")
			<< SData;

	/* 1. Build support function estimation SData. */
	pushTimer("SEData preparation");
	SupportFunctionEstimationDataConstructor constructor;
	if (ifScaleMatrix)
		constructor.enableMatrixScaling();
	if (estimatorType == NATIVE_ESTIMATOR)
		supportMatrixType_ = SUPPORT_MATRIX_TYPE_EMPTY;
	if (ifShadowHeuristics_)
		constructor.enableShadowHeuristics();
	SupportFunctionEstimationDataPtr SEData
		= constructor.run(SData, supportMatrixType_,
				startingBodyType_);
	popTimer();

	/* 2. Build support function estimator. */
	pushTimer("Estimation");
	SupportFunctionEstimator *estimator = constructEstimator(SEData,
			estimatorType, problemType_);
	if (ifShadowHeuristics_)
		estimator->enableShadowHeuristics();

	/* 3. Run support function estimation. */
	VectorXd estimate(SEData->numValues());
	if (estimator)
		estimate = estimator->run();
	else
		estimate = SEData->supportVector();
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "support-vector-estimate.mat")
		<< estimate;
	popTimer();

	/* 4. Validate the result of estimation. */
	if(!constructor.checkResult(SEData, supportMatrixType_, estimate))
	{
		exit(EXIT_FAILURE);
	}
	VectorXd consistentValues = supportValuesFromPoints(
			SData->supportDirections<Point_3>(), estimate);
	DEBUG_END;
	return std::make_pair(consistentValues, SEData);
}

Polyhedron_3 Recoverer::buildConsistentBody(VectorXd consistentValues,
		SupportFunctionEstimationDataPtr SEData)
{
	DEBUG_START;
	pushTimer("reporting and final post-processing");
	/* 1. Prepare directions' IDs that should be ignored. */
	auto directions = SEData->supportData()->supportDirections<Point_3>();
	indicesDirectionsIgnored = prepareIgnoredIndices(directions,
			zMinimalNorm_);

	/* 2. Prepare 3rd-party SData to be compared with. */
	if (fileNamePolyhedron_)
	{
		VectorXd h3rdParty = prepare3rdPartyValues(fileNamePolyhedron_,
				directions, balancingVector_);
		producePolyhedron(SEData, h3rdParty, "3rd-party-body");
	}

	/* 3. Prepare starting body to be reported. */
	auto hStarting = supportValuesFromPoints(directions,
			SEData->startingVector());
	producePolyhedron(SEData, hStarting, "starting-body");

	/* 4. Prepare naive body to be reported. */
	producePolyhedron(SEData, SEData->supportVector(), "naive-body", false);

	/* 5. Prepare consistent body to be reported. */
	Polyhedron_3 P = producePolyhedron(SEData, consistentValues,
			"consistent-body");

	popTimer();
	DEBUG_END;
	return P;
}

/**
 * Simplifies the given body according to some heuristics.
 *
 * @param P			The body to be simplified.
 * @param consistentValues	The result of support function
 * 				estimation.
 * @param SEData		The support function estimation initial
 * 				data.
 * @paran threshold		The threshold for simplification.
 */
static Polyhedron_3 simplifyBody(Polyhedron_3 P, VectorXd consistentValues,
		SupportFunctionEstimationDataPtr SEData, double threshold)
{
	DEBUG_START;
	pushTimer("facet joining");
	/*
	 * 2. Reset polyhedron facets' IDs according to initial items
	 * order.
	 */
	auto planes = produceCorrectedPlanes(SEData, consistentValues);
	P.initialize_indices(planes);

	/* 2. Produce joined polyhedron: */
	NaiveFacetJoiner joiner(P, threshold);
	pushTimer("naive facet joining itself");
	P = joiner.run().first;
	popTimer();

	/* 3. Produce vector of tangient points: */
	auto directions = SEData->supportData()->supportDirections<Point_3>();
	VectorXd estimateJoined =
		P.findTangientPointsConcatenated(directions);

	/* 4. Now produce the joined polyheron and report about it: */
	producePolyhedron(SEData, supportValuesFromPoints(directions,
		estimateJoined), "naively-joined-body");
	popTimer();

	/* 5. Correct the simplified body. */
	if (getenv("CORRECT_SIMPLIFIED_BODY"))
	{
		pushTimer("polyhedron correction");
		globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
				"before-FT-correction.ply") << P;
		SupportPolyhedronCorrector corrector(P, SEData->supportData());
		Polyhedron_3 PC = corrector.run();
		globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
				"after-FT-correction.ply") << PC;
		popTimer();
		producePolyhedron(SEData, supportValuesFromPoints(directions,
			estimateJoined), "naively-joined-body");
		VectorXd estimateCorrected =
			PC.findTangientPointsConcatenated(directions);
		producePolyhedron(SEData, supportValuesFromPoints(
					directions, estimateCorrected),
				"corrected-joined-body");
		P = PC;
	}

	if (getenv("EDGE_CORRECTION"))
	{
		pushTimer("edge correction");
		globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
				"before-edge-correction.ply") << P;
		EdgeCorrector corrector(P, SEData->supportData());
		Polyhedron_3 PC = corrector.run();
		popTimer();
		globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
				"after-edge-correction.ply") << PC;
		VectorXd E = PC.findTangientPointsConcatenated(directions);
		producePolyhedron(SEData, supportValuesFromPoints(directions,
					E), "edge-corrected-joined-body");
		P = PC;
	}

	DEBUG_END;
	return P;
}

Polyhedron_3 Recoverer::run(SupportFunctionDataPtr SData)
{
	DEBUG_START;
	if (ifContourMode_)
	{
		ContourModeRecoverer recoverer(SData);
		recoverer.run();
		DEBUG_END;
		return Polyhedron_3();
	}

	/* 1. Run classical support function estimation process. */
	VectorXd consistentValues;
	SupportFunctionEstimationDataPtr SEData;
	std::tie(consistentValues, SEData) = runEstimation(SData);

	/* 2. Build the consistent body from the obtained estimate. */
	Polyhedron_3 P = buildConsistentBody(consistentValues, SEData);

	/* 3. Simplify this (very complicated!) consistent body. */
	if (threshold_ > 0.)
		P = simplifyBody(P, consistentValues, SEData, threshold_);

	DEBUG_END;
	return P;
}

Polyhedron_3 Recoverer::run(ShadowContourDataPtr dataShadow)
{
	DEBUG_START;
	/* 0. Build support function data. */
	pushTimer("support data extraction");
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
	popTimer();

	/* 1. Run the recoverer for the constructed data. */
	Polyhedron_3 polyhedron = run(data);

	DEBUG_END;
	return polyhedron;
}
