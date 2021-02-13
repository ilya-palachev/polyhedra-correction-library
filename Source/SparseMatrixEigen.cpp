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
 * @file SparseMatrixEigen.cpp
 * @brief Inclusions from Eigen library
 * - implementation of additional functions.
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Vector3d.h"
#include "SparseMatrixEigen.h"

bool operator==(const Eigen::SparseMatrix<double> matrixLeft,
				const Eigen::SparseMatrix<double> matrixRight)
{
	DEBUG_START;
	if (matrixLeft.outerSize() != matrixRight.outerSize())
	{
		DEBUG_END;
		return false;
	}
	ASSERT(matrixLeft.outerSize() == matrixRight.outerSize());
	for (int k = 0; k < matrixLeft.outerSize(); ++k)
	{
		SparseMatrix::InnerIterator itLeft(matrixLeft, k);
		SparseMatrix::InnerIterator itRight(matrixRight, k);
		while (itLeft && itRight)
		{
			if (!equal(itLeft.value(), itRight.value()) ||
				itLeft.row() != itRight.row() || itLeft.col() != itRight.col())
			{
				DEBUG_END;
				return false;
			}
			++itLeft;
			++itRight;
		}
	}
	DEBUG_END;
	return true;
}
