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

// TODO: Make unified version using templates
// TODO: Extract below two functions into a proper header
std::pair<double, Vector3d>
calculateSupportFunction(const std::vector<Vector3d> &vertices,
						 const Vector3d &direction);
std::pair<double, VectorXd>
calculateSupportFunction(const std::vector<VectorXd> &vertices,
						 const VectorXd &direction);

class AlternatingMinimization {
public:
	std::pair<Polyhedron_3, double> run(SupportFunctionDataPtr data,
										std::vector<Vector3d> startingBody,
										unsigned numLiftingDimensions);
};

#endif /* ALTERNATING_MINIMIZATION_H */
