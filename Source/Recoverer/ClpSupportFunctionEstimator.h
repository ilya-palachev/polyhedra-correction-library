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
 * @file ClpSupportFunctionEstimator.h
 * @brief Declaration of Clp-based linear programming solver used for support
 * function estimation.
 */

#ifdef USE_CLP

#include <glpk.h>
#include <coin/ClpSimplex.hpp>
#include "Recoverer/SupportFunctionEstimator.h"

#ifndef CLPSUPPORTFUNCTIONESTIMATOR_H_
#define CLPSUPPORTFUNCTIONESTIMATOR_H_

/**
 * Clp-based linear programming solver used for support function estimation.
 */
class ClpSupportFunctionEstimator : public SupportFunctionEstimator
{
protected:
	/** Whether to run the CLP as a sommand-line program. */
	bool ifCommandlineMode;

public:
	/**
	 * Default constructor
	 *
	 * @param data			Support function estimation data (input)
	 * @param ifCommandlineMode	Whether to run through the command line
	 */
	ClpSupportFunctionEstimator(SupportFunctionEstimationDataPtr data, bool ifCommandlineMode = false);

	/** Default destructor. */
	~ClpSupportFunctionEstimator();

	/**
	 * Runs the Clp algorithm.
	 *
	 * @return	The solution of LP problem.
	 */
	virtual VectorXd run(void);
};

/** The same as above, but CLP is run through the command line. */
class ClpCommandLineSupportFunctionEstimator : public ClpSupportFunctionEstimator
{
public:
	/**
	 * Default constructor.
	 *
	 * @param data			Support function estimation data (input)
	 */
	ClpCommandLineSupportFunctionEstimator(SupportFunctionEstimationDataPtr data);
};

#endif /* CLPSUPPORTFUNCTIONESTIMATOR_H_ */
#endif /* USE_CLP */
