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
#include "Recoverer/SupportItemSet.h"


/**
 * Sets the estimator to be used for support function estimation.
 */
typedef enum
{
	/** Estimator that does nothing */
	ZERO_ESTIMATOR,
#ifdef USE_TSNNLS
	/** TSNNLS estimator (not working). */
	TSNNLS_ESTIMATOR,
#endif /* USE_TSNNLS */

#ifdef USE_IPOPT
	/** Ipopt estimator. */
	IPOPT_ESTIMATOR,

	/** Linear Ipopt estimator. */
	IPOPT_ESTIMATOR_LINEAR,
#endif /* USE_IPOPT */

	/** Quadratic primal CGAL estimator.  */
	CGAL_ESTIMATOR,

	/** Linear CGAL estimator. */
	CGAL_ESTIMATOR_LINEAR
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

	/** Whether to convexify shadow contours. */
	bool ifConvexifyContours;

	/** Whether to scale the matrix of problem. */
	bool ifScaleMatrix;

	/** Support matrix type. */
	SupportMatrixType supportMatrixType_;

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
	 * Runs the recovering procedure.
	 *
	 * @param SCData	Shadow contour data
	 */
	PolyhedronPtr run(ShadowContourDataPtr SCData);
};

typedef std::shared_ptr<Recoverer> RecovererPtr;

#endif /* RECOVERER_H */
