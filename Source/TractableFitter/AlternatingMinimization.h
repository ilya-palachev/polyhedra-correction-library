/*
 * Copyright (c) 2021 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file AlternatingMinimization.h
 * @brief Alternating minimization algorithm for recovering convex polyhedron
 * from the set of its support function masurements, as it was described in the
 * following paper:
 *
 * - Soh Y.S., Chandrasekaran V. Fitting tractable convex sets to support
 *   function evaluations. Discrete & Computational Geometry. 2021 Jan 3:1-42.
 *   URL: https://link.springer.com/article/10.1007/s00454-020-00258-0
 */

#ifndef ALTERNATING_MINIMIZATION_H
#define ALTERNATING_MINIMIZATION_H

#include <utility>

#include <Eigen/LU>

#include "Polyhedron_3/Polyhedron_3.h"

using Eigen::MatrixXd;

class AlternatingMinimization
{
public:
	AlternatingMinimization() : useStartingBody_(false), startingBody_(emptyFakeStartingBody)
	{
	}

	explicit AlternatingMinimization(const std::vector<Vector3d> &startingBody) :
		useStartingBody_(true), startingBody_(startingBody)
	{
	}

	Polyhedron_3 run(SupportFunctionDataPtr data, unsigned numLiftingDimensions) const;

private:
	inline static std::vector<Vector3d> emptyFakeStartingBody = {};

	bool useStartingBody_ = false;
	const std::vector<Vector3d> &startingBody_;

	// Default values from the initial implementation by Y.S. Soh
	// TODO: Make getters and setters for these values
	unsigned numOuterIterations_ = 100;
	unsigned numInnerIterations_ = 100;
	double regularizer_ = 0.5;

	unsigned verboseLevel_ = 1;

	MatrixXd makeOuterInitialization(unsigned numLiftingDimensions) const;
};

#endif /* ALTERNATING_MINIMIZATION_H */
