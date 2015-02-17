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

#ifndef CONSTANTS_H
#define	 CONSTANTS_H

#include <cstdlib>

/* Some common constants used by different methods. */

/* During the array initialization, not initialized fields are assigned to this
 * value (for debugging purposes). */
const int INT_NOT_INITIALIZED = -RAND_MAX;

/* We assume that 2 planes or 2 lines are colinear if the angle between them is
 * less than this value. */
const double EPS_COLLINEARITY = 1e-14;

/* This value is returned by functions which return double variable in case of
 * failure. */
const double DEFAULT_ERROR_FOR_DOUBLE_FUNCTIONS = -RAND_MAX;

/* Minimal double number. */
const double EPS_MIN_DOUBLE = 1e-16;

/** Whether to analyze all contours. */
const int IF_ANALYZE_ALL_CONTOURS = -1;

#endif /* CONSTANTS_H */
