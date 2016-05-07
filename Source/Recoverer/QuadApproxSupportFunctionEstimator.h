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
 * @file QuadApproxSupportFunctionEstimator.h
 * @brief Estimation engine based on iterative solution of quadratic
 * approximations of the initial problem (declaration).
 */

#include "Recoverer/SupportFunctionEstimator.h"

#ifndef QUADAPPROXSUPPORTFUNCTIONESTIMATOR_H_
#define QUADAPPROXSUPPORTFUNCTIONESTIMATOR_H_

/**
 * Quadratic approximation-based estimator.
 */
class QuadApproxSupportFunctionEstimator : public SupportFunctionEstimator
{
public:
	/**
	 * Default constructor
	 *
	 * @param data	Support function data
	 */
	QuadApproxSupportFunctionEstimator(
			SupportFunctionEstimationDataPtr supportFunctionData);

	/** Default destructor. */
	~QuadApproxSupportFunctionEstimator();
	
	/** Runs the estimation process. */
	virtual VectorXd run(void);
};

#endif /* QUADAPPROXSUPPORTFUNCTIONESTIMATOR_H_ */
