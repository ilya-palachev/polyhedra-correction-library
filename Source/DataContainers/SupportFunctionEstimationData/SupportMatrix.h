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
 * @file SupportMatrix.h
 * @brief General support matrix that contains sparse matrix.
 * - declaration.
 */

#ifndef SUPPORT_MATRIX_H
#define SUPPORT_MATRIX_H

#include "SparseMatrixEigen.h"
#include "Polyhedron_3/Polyhedron_3.h"

/** General support matrix that contains sparse matrix. */
class SupportMatrix : public SparseMatrix
{
public:
	/** Empty constructor. */
	SupportMatrix();

	/**
	 * Constructor by the size of matrix.
	 *
	 * @param numRows	The number of rows
	 * @param numColumns	The number of columns
	 */
	SupportMatrix(long int numRows, long int numColumns);

	/** Destructor. */
	virtual ~SupportMatrix();
};

#define UNUSED __attribute__((unused))

/**
 * Builds hull of directions by given support directions.
 *
 * @param directions	Given support directions.
 * @return 		Hull of directions.
 */
Polyhedron_3 buildDirectionsHull(std::vector<Point_3> directions);

/**
 * Checks the hull of directions for correctness.
 *
 * @param hull	The hull of directions.
 * @return	True, if the hull is correct.
 */
bool checkDirectionsHull(Polyhedron_3 hull) UNUSED;

#endif /* SUPPORT_MATRIX_H */
