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
 * @file SupportFunction.h
 * @brief Basic functions for operating with the support function of the range
 * of points
 */

#ifndef SUPPORT_FUNCTION_H
#define SUPPORT_FUNCTION_H

#include <algorithm>

template <class RangeT, class VectorT>
VectorT selectExtremePointByDirection(const RangeT &vertices, const VectorT &direction)
{
	VectorT extremeVertex =
		*std::max_element(std::begin(vertices), std::end(vertices),
						  [&direction = std::as_const(direction)](const auto &lhs, const auto &rhs) {
							  return lhs.dot(direction) < rhs.dot(direction);
						  });
	return extremeVertex;
}

template <class RangeT, class VectorT> double calculateSupportFunction(const RangeT &vertices, const VectorT &direction)
{
	ASSERT(std::begin(vertices) != std::end(vertices));
	return selectExtremePointByDirection(vertices, direction).dot(direction);
}

#endif /* SUPPORT_FUNCTION_H */
