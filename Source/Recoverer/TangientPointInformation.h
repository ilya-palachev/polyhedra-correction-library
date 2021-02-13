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
 * @file TangientPointInformation.h
 * @brief Information about tangient points (declaration).
 */

#ifndef TANGIENTPOINTINFORMATION_H
#define TANGIENTPOINTINFORMATION_H

#include "DataContainers/SupportFunctionData/SupportFunctionData.h"
#include "DataContainers/SupportFunctionEstimationData/SupportFunctionEstimationData.h"
#include "SparseMatrixEigen.h"
#include <Eigen/LU>

/**
 * Contains preliminary caluclated slice of gradient.
 */
struct GradientSlice
{
	/** The ID of the quadruple. */
	int id_;

	/** The values of the quadruple. */
	double values_[4];
};

/**
 * Contains preliminary calculated slice of the hessian.
 */
struct HessianSlice
{
	/** The ID of the block row. */
	int idRow_;

	/** The Id of the block column. */
	int idColumn_;

	/** The matrix block. */
	Eigen::Matrix4d values_;
};

/**
 * The information about tangient points required during the re-calculating of
 * functional and conditions of the problem.
 */
struct TangientPointInformation
{
	/** The ID of the point. */
	int iPoint_;

	/** The tangient point. */
	Vector_3 point_;

	/** The ID of the direction. */
	int iDirection_;

	/** The support direction. */
	Vector_3 direction_;

	/** The array of planes IDs. */
	int indices_[3];

	/** The array of planes. */
	Plane_3 planes_[3];

	/** The support value. */
	double supportValue_;

	/** The inverse of the planes normals matrix. */
	Eigen::Matrix3d inverse_;

	/**
	 * Calculates the inverse of the planes normals matrix.
	 *
	 * @return		The inverse of the planes normals matrix.
	 */
	Eigen::Matrix3d calculateInverse(void);

	/**
	 * Creates an empty information structure.
	 */
	TangientPointInformation();

	/**
	 * Destroys the information structure.
	 */
	~TangientPointInformation();

	/**
	 * Creates the information about tangient points.
	 *
	 * @param iDirection	The ID of the direction.
	 * @param direction	The direction.
	 * @param vertex	The vertex iterator pointing to the vertex.
	 */
	TangientPointInformation(int iDirection, Vector_3 direction,
							 Polyhedron_3::Vertex_iterator vertex);

	/**
	 * Prints the information to the stream.
	 *
	 * @param stream	The stream.
	 * @param info		The information.
	 *
	 * @return		The srteam ready for further printing.
	 */
	friend std::ostream &operator<<(std::ostream &stream,
									TangientPointInformation &info);

	/**
	 * Calculates 3 slices (consecutive qeuadruples of the gradient
	 * components) of the gradient.
	 *
	 * @return 		The pointer to slices array.
	 */
	GradientSlice *calculateFirstDerivative(void);

	/**
	 * Calculates 9 slices (4x4 matrix blocks) of the hessian.
	 *
	 * @return		The pointer to the slices array.
	 */
	HessianSlice *calculateSecondDerivative(void);
};

#endif /* TANGIENTPOINTINFORMATION_H */
