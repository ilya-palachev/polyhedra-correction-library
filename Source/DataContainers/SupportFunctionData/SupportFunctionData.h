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
 * @file SupportFunctionData.h
 * @brief General class for support function data container
 * - declaration.
 */

#ifndef SUPPORT_FUNCTION_DATA_H_FORWARD
#define SUPPORT_FUNCTION_DATA_H_FORWARD

#include <memory>
/* Forward declararion. */
class SupportFunctionData;

/** Shared pointer to support function data. */
typedef std::shared_ptr<SupportFunctionData> SupportFunctionDataPtr;

#endif /* SUPPORT_FUNCTION_DATA_H_FORWARD */

#ifndef SUPPORT_FUNCTION_DATA_H
#define SUPPORT_FUNCTION_DATA_H

#include <vector>

#include "DataContainers/SupportFunctionData/SupportFunctionDataItem.h"
#include "PolyhedronCGAL.h"
#include "SparseMatrixEigen.h"


/** Minimal limit under which support firections are considered to be equal. */
const double EPS_SUPPORT_DIRECTION_EQUALITY = 1e-15;

/** General class for support function data container. */
class SupportFunctionData
{
private:
	/** Vector of support data items. */
	std::vector<SupportFunctionDataItem> items;

public:
	/** Empty constructor. */
	SupportFunctionData();

	/**
	 * Copy constructor.
	 *
	 * @param data	Reference to existing data.
	 */
	SupportFunctionData(const SupportFunctionData &data);

	/**
	 * Copy constructor from pointer.
	 *
	 * @param data	Pointer to existing data.
	 */
	SupportFunctionData(const SupportFunctionData *data);

	/**
	 * Copy constructor from shared pointer.
	 *
	 * @param data	Shared pointer to existing data.
	 */
	SupportFunctionData(const SupportFunctionDataPtr data);

	/**
	 * Constructor from the vector of items.
	 *
	 * @param itemsGiven	Support function data items.
	 */
	SupportFunctionData(
			const std::vector<SupportFunctionDataItem> itemsGiven);

	/**
	 * Assignment operator.
	 *
	 * @param data	The original data.
	 * @return The assigned data.
	 */
	SupportFunctionData &operator= (const SupportFunctionData &data);

	/** Destructor. */
	virtual ~SupportFunctionData();

	/**
	 * Subscription operator.
	 *
	 * @param iPosition	The ID of item that is demanded.
	 * @return The reference to the demanded item.
	 */
	SupportFunctionDataItem &operator[] (const int iPosition);
	/**
	 * Gets the size of vector fo items.
	 *
	 * @return The size of vector items.
	 */
	long int size();

	/**
	 * Constructs support function data that consists only pairwise-inequal
	 * data items. The constructed items are normalized before checking the
	 * equality of items.
	 *
	 * @return	The data in which all items are unique.
	 */
	SupportFunctionDataPtr removeEqual();

	/**
	 * Gets the vector of support directions.
	 *
	 * @return Vector of support directions.
	 */
	std::vector<Vector3d> supportDirections();

	/**
	 * Gets the vector of support directions represented as CGAL points.
	 *
	 * @return Vector of support directions as CGAL points.
	 */
	std::vector<Point_3> supportDirectionsCGAL();

	/**
	 * Gets the vector of support values.
	 *
	 * @return Vector of support values.
	 */
	VectorXd supportValues();

	/**
	 * Gets the vector of support planes.
	 *
	 * @return Vector of support planes.
	 */
	std::vector<Plane_3> supportPlanes();

	/**
	 * Gets the vector of support points.
	 *
	 * @return Vector of support points.
	 */
	std::vector<Vector3d> supportPoints();
};

#endif /* SUPPORT_FUNCTION_DATA_H */
