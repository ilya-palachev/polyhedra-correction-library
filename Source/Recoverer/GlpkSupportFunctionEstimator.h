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

/**
 * @file GlpkSupportFunctionEstimator.h
 * @brief Declaration of Glpk-based linear programming solver used for support
 * function estimation.
 */

#ifdef USE_GLPK

#include <glpk.h>
#include "Recoverer/SupportFunctionEstimator.h"

#ifndef GLPKSUPPORTFUNCTIONESTIMATOR_H_
#define GLPKSUPPORTFUNCTIONESTIMATOR_H_

/**
 * Glpk-based linear programming solver used for support function estimation.
 */
class GlpkSupportFunctionEstimator : public SupportFunctionEstimator
{
public:
	/**
	 * Default constructor
	 *
	 * @param data	Support function estimation data (input)
	 */
	GlpkSupportFunctionEstimator(SupportFunctionEstimationDataPtr data);

	/** Default destructor. */
	~GlpkSupportFunctionEstimator();

	/**
	 * Runs the Glpk algorithm.
	 *
	 * @return	The solution of LP problem.
	 */
	virtual VectorXd run(void);
};

#endif /* GLPKSUPPORTFUNCTIONESTIMATOR_H_ */
#endif /* USE_GLPK*/
