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

/**
 * @file SupportMatrix.cpp
 * @brief General support matrix that contains sparse matrix
 * - implementation.
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "DataContainers/SupportFunctionEstimationData/SupportMatrix.h"

SupportMatrix::SupportMatrix() :
	SparseMatrix(1, 1)
{
	DEBUG_START;
	DEBUG_END;
}

SupportMatrix::SupportMatrix(long int numRows, long int numColumns) :
	SparseMatrix(numRows, numColumns)
{
	DEBUG_START;
	DEBUG_END;
}

SupportMatrix::~SupportMatrix()
{
	DEBUG_START;
	DEBUG_END;
}

void SupportMatrix::eliminateRedundantRows()
{
	DEBUG_START;
	ASSERT(0 && "unreachable!");
	DEBUG_END;
}
