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
 * @file NativeEstimatorCommonFunctions.cpp
 * @brief Common functions of native estimators (declaration).
 */

#ifndef NATIVEESTIMATORCOMMONFUNCTIONS_H
#define NATIVEESTIMATORCOMMONFUNCTIONS_H

#include "Recoverer/SupportFunctionEstimator.h"

std::vector<int> collectInnerPointsIDs(std::vector<Point_3> points);

std::set<int> findTangientPointPlanesIDs(Polyhedron_3 *polyhedron, Polyhedron_3::Vertex_iterator vertex,
										 std::vector<int> index);

VectorXd calculateSolution(SupportFunctionDataPtr data, VectorXd values);
#endif /* NATIVEESTIMATORCOMMONFUNCTIONS_H */
