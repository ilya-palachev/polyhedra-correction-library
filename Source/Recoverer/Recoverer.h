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

/**
 * @file Recoverer.h
 * @brief Declaration of the main recovering engine, based on support function
 * estimation tuned for our specific case.
 */

#ifndef RECOVERER_H
#define RECOVERER_H

#include <vector>

#include <CGAL/basic.h>
#include <CGAL/Filtered_kernel.h>

#include "PCLKernel/PCLKernel.h"
#include "PCLKernel/PCLPoint_2_iostream.h"

#include "Polyhedron/Polyhedron.h"
#include "DataContainers/ShadowContourData/ShadowContourData.h"
#include "DataContainers/SupportFunctionEstimationData/SupportFunctionEstimationData.h"
#include "DataConstructors/SupportFunctionEstimationDataConstructor/SupportFunctionEstimationDataConstructor.h"

/**
 * Sets the estimator to be used for support function estimation.
 */
typedef enum
{
	/** Estimator that does nothing */
	ZERO_ESTIMATOR,

	/** Native (builtin) estimator. */
	NATIVE_ESTIMATOR,

#ifdef USE_TSNNLS
	/** TSNNLS estimator (not working). */
	TSNNLS_ESTIMATOR,
#endif /* USE_TSNNLS */

#ifdef USE_IPOPT
	/** Ipopt estimator. */
	IPOPT_ESTIMATOR,
#endif /* USE_IPOPT */

#ifdef USE_GLPK
	/** Linear Glpk estimator. */
	GLPK_ESTIMATOR,
#endif /* USE_GLPK */

#ifdef USE_CLP
	/** Linear Clp estimator that uses CLP as shared library. */
	CLP_ESTIMATOR,

	/** Linear Clp estimator that uses CLP as a command-line program. */
	CLP_COMMAND_ESTIMATOR,
#endif /* USE_CLP */

#ifdef USE_CPLEX
	/** Lnear CPLEX estimator. */
	CPLEX_ESTIMATOR,
#endif /* USE_CPLEX */

	/** Built-in CGAL estimator.  */
	CGAL_ESTIMATOR
} RecovererEstimatorType;

/**
 * Main recovering engine, based on support function estimation tuned for our
 * specific needs.
 */
class Recoverer
{
private:

	/** Type of estimator to be used. */
	RecovererEstimatorType estimatorType;

	/** Whether to balance the vertical center of contours. */
	bool ifBalancing;

	/** The vector on which all shadow data was shifted. */
	Vector3d balancingVector_;

	/** Whether to convexify shadow contours. */
	bool ifConvexifyContours;

	/** Whether to scale the matrix of problem. */
	bool ifScaleMatrix;

	/** Support matrix type. */
	SupportMatrixType supportMatrixType_;

	/** Starting body type. */
	SupportFunctionEstimationStartingBodyType startingBodyType_;

	/** Problem type. */
	EstimationProblemNorm problemType_;

	/** Number of analyzed contours. */
	int numMaxContours;

	/** File name of 3rd-party constructed polyhedron. */
	char *fileNamePolyhedron_;

	/** The threshold of angle betweeb adjacent facets. */
	double threshold_;

	/**
	 * The minimal Z norm for which down-side support directions are not
	 * ignored during the production of the estimation report.
	 */
	double zMinimalNorm_;
public:

	/**
	 * Empty constructor.
	 */
	Recoverer();

	/**
	 * Empty destructor.
	 */
	~Recoverer();

	/**
	 * Sets the type of estimator.
	 *
	 * @param e	The type of estimator.
	 */
	void setEstimatorType(RecovererEstimatorType e);

	/**
	 * Enables balancing in all functions.
	 */
	void enableBalancing(void);

	/**
	 * Enables the convexification of shadow contours.
	 */
	void enableContoursConvexification(void);

	/**
	 * Disables the convexification of shadow contours.
	 */
	void disableContoursConvexification(void);

	/**
	 * Enables the scaling of the matrix of the problem.
	 */
	void enableMatrixScaling(void);

	/**
	 * Sets support matrix type.
	 *
	 * @param type	The type of support matrix that is to be used.
	 */
	void setSupportMatrixType(SupportMatrixType type);

	/**
	 * Sets starting body type.
	 *
	 * @param type	The type of starting body that is to be used.
	 */
	void setStartingBodyType(
			SupportFunctionEstimationStartingBodyType type);

	/**
	 * Sets the type of problem.
	 *
	 * @param type	The type of problem.
	 */
	void setProblemType(EstimationProblemNorm type);

	/**
	 * Sets the maximum number of analyzed contours.
	 *
	 * @param number	The maximum number of analyzed contours.
	 */
	void setNumMaxContours(int number);

	/**
	 * Sets the file name of 3rd-party constructed polyhedron.
	 *
	 * @param fileNamePolyheedron	The file name of 3rd-party constructed
	 * 				polyhedron.
	 */
	void setFileNamePolyhedron(char *fileNamePolyhedron);

	/**
	 * Sets the threshold of angle between adjacent facets.
	 *
	 * @param thresold	The value of threshold.
	 */
	void setThreshold(double threshold)
	{
		threshold_ = threshold;
	}

	/**
	 * Sets the Z minimal norm.
	 *
	 * @param zMinimalNorm	The Z minimal norm to be set.
	 */
	void setZMinimalNorm(double zMinimalNorm)
	{
		zMinimalNorm_ = zMinimalNorm;
	}

	/**
	 * Runs the recovering procedure for support function data.
	 *
	 * @param data		Support function data
	 */
	Polyhedron_3 run(SupportFunctionDataPtr data);

	/**
	 * Runs the recovering procedure for shadow contours.
	 *
	 * @param SCData	Shadow contour data
	 */
	Polyhedron_3 run(ShadowContourDataPtr SCData);
};

typedef std::shared_ptr<Recoverer> RecovererPtr;

#endif /* RECOVERER_H */
