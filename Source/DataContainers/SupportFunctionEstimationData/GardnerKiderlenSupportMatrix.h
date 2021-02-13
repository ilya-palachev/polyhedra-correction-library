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
 * @file GardnerKiderlenSupportMatrix.h
 * @brief Support matrix based on the approach described by Gardner and
 * Kiderlen
 * - declaration.
 *
 * Literature:
 *   Gardner, Kiderlen - A new algorithm for 3D reconstruction from support
 *   functions
 *   - see: http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.86.9984
 */

#ifndef GARDNER_KIDERLEN_SUPPORT_MATRIX_H
#define GARDNER_KIDERLEN_SUPPORT_MATRIX_H

#include "DataContainers/SupportFunctionEstimationData/SupportMatrix.h"
#include "DataContainers/SupportFunctionData/SupportFunctionData.h"

/**
 * Support matrix based on the approach described by Gardner and
 * Kiderlen
 */
class GardnerKiderlenSupportMatrix : public SupportMatrix
{
public:
	/**
	 * The factor number that is used as the multiplier for epsilon
	 */
	static double epsilonFactor;

	/**
	 * Constructor by the size of matrix.
	 *
	 * @param numRows	The number of rows
	 * @param numColumns	The number of columns
	 */
	GardnerKiderlenSupportMatrix(long int numRows, long int numColumns);

	/** Destructor. */
	virtual ~GardnerKiderlenSupportMatrix();
};

/**
 * Constructs Gardner-Kiderlen support matrix.
 *
 * @param data	Support function data.
 *
 * @return	Gardner-Kiderlen support matrix.
 */
GardnerKiderlenSupportMatrix *
constructGardnerKiderlenSupportMatrix(SupportFunctionDataPtr data);

/**
 * Constructs reduced Gardner-Kiderlen support matrix (i. e. in which redundant
 * conditions are eliminated).
 *
 * @param data		Support function data.
 * @param epsilon	The maximal absolute delta for starting point.
 *
 * @return	Reduced Gardner-Kiderlen support matrix.
 */
GardnerKiderlenSupportMatrix *constructReducedGardnerKiderlenSupportMatrix(
	SupportFunctionDataPtr data, double epsilon, bool ifShadowHeuristics);

#endif /* GARDNER_KIDERLEN_SUPPORT_MATRIX_H */
