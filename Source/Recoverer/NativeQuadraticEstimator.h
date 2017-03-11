/*
 * Copyright (c) 2009-2017 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file NativeQuadraticEstimator.h
 * @brief Native quadratic estimation engine (declaration).
 */

#include "Recoverer/SupportFunctionEstimator.h"

#ifndef NATIVEQUADRATICESTIMATOR_H
#define NATIVEQUADRATICESTIMATOR_H

/**
 * Native quadratic estimation engine.
 */
class NativeQuadraticEstimator : public SupportFunctionEstimator
{
public:
	/**
	 * Default constructor
	 *
	 * @param data	Support function data
	 */
	NativeQuadraticEstimator(
			SupportFunctionEstimationDataPtr supportFunctionData);

	/** Default destructor. */
	~NativeQuadraticEstimator();
	
	/** Runs the estimation process. */
	virtual VectorXd run(void);
};

#endif /* NATIVEQUADRATICESTIMATOR_H */
