/*
 * Copyright (c) 2009-2014 Ilya Palachev <iliyapalachev@gmail.com>
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
#include "DataContainers/ShadowContourData/ShadowContourData.h"
#include "Recoverer/Recoverer.h"
#include "Recoverer/CGALSupportFunctionEstimator.h"
#include "Recoverer/IpoptSupportFunctionEstimator.h"
#include "Recoverer/TsnnlsSupportFunctionEstimator.h"
#include "Recoverer/GlpkSupportFunctionEstimator.h"
#include "DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataConstructor.h"
#include "halfspaces_intersection.h"

Recoverer::Recoverer() :
	estimatorType(CGAL_ESTIMATOR),
	ifBalancing(false),
	ifConvexifyContours(false),
	ifScaleMatrix(false)
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

#define ACCEPTED_TOL 1e-6

static void printEstimationReport(SparseMatrix Q, VectorXd h0, VectorXd h,
	RecovererEstimatorType estimatorType)
{
	DEBUG_START;
	MAIN_PRINT("Estimation report:");
	double DEBUG_VARIABLE L1 = 0.;
	double DEBUG_VARIABLE L2 = 0.;
	double DEBUG_VARIABLE Linf = 0.;
	ASSERT(h0.size() == h.size());
	for (int i = 0; i < h.size(); ++i)
	{
		double DEBUG_VARIABLE delta = h0(i) - h(i);
		DEBUG_PRINT("h[%d] :  %lf - %lf = %lf", i, h0(i), h(i), delta);
		L1 += fabs(delta);
		L2 += delta * delta;
		Linf = delta > Linf ? delta : Linf;
	}
	MAIN_PRINT("L1 = %lf = %le", L1, L1);
	MAIN_PRINT("L2 = %lf = %le", L2, L2);
	MAIN_PRINT("Linf = %lf = %le", Linf, Linf);

	DEBUG_PRINT("-------------------------------");
	DEBUG_END;
}

static SupportFunctionEstimator *constructEstimator(
		SupportFunctionEstimationDataPtr data,
		RecovererEstimatorType estimatorType)
{
	DEBUG_START;

	SupportFunctionEstimator *estimator = NULL;
	switch (estimatorType)
	{
	case ZERO_ESTIMATOR:
		DEBUG_PRINT("Zero estimatorType is on!");
		break;
#ifdef USE_TSNNLS
	case TSNNLS_ESTIMATOR:
		estimator = new TsnnlsSupportFunctionEstimator(data);
		break;
#endif /* USE_TSNNLS */
#ifdef USE_IPOPT
	case IPOPT_ESTIMATOR:
		estimator = new IpoptSupportFunctionEstimator(data,
				IPOPT_ESTIMATION_QUADRATIC);
		break;
	case IPOPT_ESTIMATOR_LINEAR:
		estimator = new IpoptSupportFunctionEstimator(data,
				IPOPT_ESTIMATION_LINEAR);
		break;
#endif /* USE_IPOPT */
#ifdef USE_GLPK
	case GLPK_ESTIMATOR:
		estimator = new GlpkSupportFunctionEstimator(data);
		break;
#endif /* USE_GLPK */
	case CGAL_ESTIMATOR:
		estimator = new CGALSupportFunctionEstimator(data,
				CGAL_ESTIMATION_QUADRATIC);
		break;
	case CGAL_ESTIMATOR_LINEAR:
		estimator = new CGALSupportFunctionEstimator(data,
				CGAL_ESTIMATION_LINEAR);
		break;
	}
	DEBUG_END;
	return estimator;
}

static PolyhedronPtr produceFinalPolyhedron(
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

	/* Intersect halfspaces corresponding to corrected planes. */
	Polyhedron_3 intersection;
	CGAL::internal::halfspaces_intersection(planes.begin(), planes.end(),
			intersection, Kernel());

	/* Construct final polyhedron. */
	PolyhedronPtr polyhedron(new Polyhedron(intersection));
	DEBUG_END;
	return polyhedron;
}

static VectorXd supportValuesFromPoints(std::vector<Vector3d> directions,
	VectorXd v)
{
	DEBUG_START;
	VectorXd values(directions.size());
	ASSERT(3 * directions.size() == (unsigned) v.rows());
	int num = directions.size();
	for (int i = 0; i < num; ++i)
	{
		values(i) = directions[i].x * v(3 * i)
			+ directions[i].y * v(3 * i + 1)
			+ directions[i].z * v(3 * i + 2);
	}
	DEBUG_END;
	return values;
}

PolyhedronPtr Recoverer::run(ShadowContourDataPtr dataShadow)
{
	DEBUG_START;

	/* Build support function data. */
	SupportFunctionDataConstructor constructor;
	if (ifBalancing)
		constructor.enableBalanceShadowContours();
	if (ifConvexifyContours)
		constructor.enableConvexifyShadowContour();
	SupportFunctionDataPtr data = constructor.run(dataShadow);

	/* Build support function estimation data. */
	SupportFunctionEstimationDataConstructor constructorEstimation;
	if (ifScaleMatrix)
		constructorEstimation.enableMatrixScaling();
	SupportFunctionEstimationDataPtr dataEstimation
		= constructorEstimation.run(data, supportMatrixType_,
				startingBodyType_);

	/* Build support function estimator. */
	SupportFunctionEstimator *estimator = constructEstimator(dataEstimation,
			estimatorType);

	/* Run support function estimation. */
	VectorXd estimate(dataEstimation->numValues());
	if (estimator)
		estimate = estimator->run();
	else
		estimate = dataEstimation->supportVector();
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "support-vector-estimate.mat")
		<< estimate;

	auto h0 = dataEstimation->supportVector();
	auto h = supportValuesFromPoints(dataEstimation->supportDirections(),
		estimate);
	auto hStarting = supportValuesFromPoints(
		dataEstimation->supportDirections(),
		dataEstimation->startingVector());	

	printEstimationReport(dataEstimation->supportMatrix(), h0, h,
		estimatorType);

	/* Now produce final polyhedron from the estimate. */
	PolyhedronPtr polyhedron = produceFinalPolyhedron(dataEstimation, h);

	/* Also produce naive polyhedron (to compare recovered one with it). */
	PolyhedronPtr polyhedronNaive = produceFinalPolyhedron(dataEstimation,
			h0);
	Polyhedron *pCopy = new Polyhedron(polyhedronNaive);
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "naively-recovered.ply")
		<< *pCopy;

	/* Also produce polyhedron from starting point of the algorithm. */
	PolyhedronPtr polyhedronStart = produceFinalPolyhedron(dataEstimation,
			hStarting);
	Polyhedron *pCopy2 = new Polyhedron(polyhedronStart);
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "starting-polyhedron.ply")
		<< *pCopy2;

	DEBUG_END;
	return polyhedron;
}
