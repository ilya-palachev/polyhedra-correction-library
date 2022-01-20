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
 * @file SparseMatrixEigen.h
 * @brief Inclusions from Eigen library.
 */

#ifndef SPARSEMATRIXEIGEN_H_
#define SPARSEMATRIXEIGEN_H_

#include <Eigen/Sparse>

/**
 * Equality operator for comparison of sparse matrix.
 *
 * @param matrixLeft	matrix at the left side of comparison
 * @param matrixRight	matrix at the right side of comparison
 *
 * @return True, if matrix are totally equal at topological and FP sence
 */
bool operator==(const Eigen::SparseMatrix<double> matrixLeft, const Eigen::SparseMatrix<double> matrixRight);

typedef Eigen::SparseMatrix<double> SparseMatrix;

typedef Eigen::Triplet<double> Triplet;

typedef Eigen::VectorXd VectorXd;

typedef Eigen::RowVectorXd RowVectorXd;

typedef Eigen::VectorXi VectorXi;

#endif
