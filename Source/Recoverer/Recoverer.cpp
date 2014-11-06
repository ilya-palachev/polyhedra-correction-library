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
#include "DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataConstructor.h"
#include "DataConstructors/SupportFunctionEstimationDataConstructor/SupportFunctionEstimationDataConstructor.h"
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
	VectorXd Qh0 = Q * h0;
	VectorXd Qh  = Q * h;
	ASSERT(Qh0.size() == Qh.size());
	for (int i = 0; i < Qh.size(); ++i)
	{
		if (fabs (Qh0(i)) > ACCEPTED_TOL
			|| fabs (Qh(i)) > ACCEPTED_TOL)
		{
			if (Qh0(i) < 0. && Qh(i) >= 0)
				DEBUG_PRINT("Q * h[%d] : " COLOUR_RED "%le"
					COLOUR_NORM " -> %le", i,
					Qh0(i), Qh(i));
			else if (Qh0(i) >= 0. && Qh(i) < 0)
				DEBUG_PRINT("Q * h[%d] : %le" COLOUR_RED
					" -> %le" COLOUR_NORM, i,
					Qh0(i), Qh(i));
			else if (Qh0(i) < 0. && Qh(i) < 0)
				DEBUG_PRINT("Q * h[%d] : " COLOUR_RED "%le"
					" -> %le" COLOUR_NORM, i,
					Qh0(i), Qh(i));
			else
				DEBUG_PRINT("Q * h[%d] : %le -> %le", i,
					Qh0(i), Qh(i));
		}
		if (Qh(i) >= -ACCEPTED_TOL || estimatorType == ZERO_ESTIMATOR)
		{
			continue;
		}
		double sum = 0.;
		for (int k = 0; k < Q.outerSize(); ++k)
		{
			for (Eigen::SparseMatrix<double>::InnerIterator
				it(Q, k); it; ++it)
			{
				if (it.row() == i)
				{
					double local = it.value()
						* h(it.col());
					DEBUG_PRINT("Q[%d][%d] = %le",
						it.row(), it.col(),
						it.value());
					DEBUG_PRINT(" *   h[%d] = %le",
						it.col(), h(it.col()));
					DEBUG_PRINT(" = %le", local);
					DEBUG_PRINT("---------------");
					sum += local;
				}
			}
		}
		ASSERT(equal(Qh(i), sum));
		ASSERT(Qh(i) >= -ACCEPTED_TOL);
	}
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
		= constructorEstimation.run(data, supportMatrixType_);

	/* Build support function estimator. */
	SupportFunctionEstimator *estimator = constructEstimator(dataEstimation,
			estimatorType);

	/* Run support function estimation. */
	VectorXd estimate(dataEstimation->numValues());
	if (estimator)
		estimate = estimator->run();
	else
		estimate = dataEstimation->supportVector();
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, ".support-vector-estimate.mat")
		<< estimate;
	printEstimationReport(dataEstimation->supportMatrix(),
			dataEstimation->supportVector(), estimate,
			estimatorType);

	/* Now produce final polyhedron from the estimate. */
	PolyhedronPtr polyhedron = produceFinalPolyhedron(dataEstimation,
			estimate);

	/* Also produce naive polyhedron (to compare recovered one with it). */
	PolyhedronPtr polyhedronNaive = produceFinalPolyhedron(dataEstimation,
			dataEstimation->supportVector());
	Polyhedron *pCopy = new Polyhedron(polyhedron);
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, ".naively-recovered.ply")
		<< *pCopy;

	DEBUG_END;
	return polyhedron;
}
