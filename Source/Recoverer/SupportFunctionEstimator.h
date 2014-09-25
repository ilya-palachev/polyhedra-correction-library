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
 * @file SupportFunctionEstimator.h
 * @brief Generic support function estimator (declaration).
 */

#ifndef SUPPORTFUNCTIONESTIMATOR_H_
#define SUPPORTFUNCTIONESTIMATOR_H_

#include "Recoverer/SupportFunctionEstimationData.h"

/** The generic support function estimator. */
class SupportFunctionEstimator : public SupportFunctionEstimationData
{
public:
	/**
	 * Constructs the estimator and sets its data.
	 *
	 * @param data	Support function estimation data
	 */
	SupportFunctionEstimator(SupportFunctionEstimationData *data);

	/** Destructs the estimator. */
	virtual ~SupportFunctionEstimator();

	/** Runs the estimator (should be defined in child classes). */
	virtual VectorXd run(void) = 0;
};

#endif /* SUPPORTFUNCTIONESTIMATOR_H_ */
