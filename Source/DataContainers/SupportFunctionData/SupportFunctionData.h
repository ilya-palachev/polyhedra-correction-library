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
 * @file SupportFunctionData.h
 * @brief General class for support function data container
 * - declaration.
 */

#ifndef SUPPORT_FUNCTION_DATA_H_FORWARD
#define SUPPORT_FUNCTION_DATA_H_FORWARD

#include <memory>
#define CGAL_LINKED_WITH_TBB 1
#include <CGAL/Simple_cartesian.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/AABB_face_graph_triangle_primitive.h>
typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_3 Point;
typedef K::Vector_3 Vector;
typedef K::Segment_3 Segment;
typedef CGAL::Polyhedron_3<K> CGALPolyhedron;
typedef CGAL::AABB_face_graph_triangle_primitive<CGALPolyhedron> Primitive;
typedef CGAL::AABB_traits<K, Primitive> Traits;
typedef CGAL::AABB_tree<Traits> Tree;
typedef Tree::Point_and_primitive_id Point_and_primitive_id;

/* Forward declararion. */
class SupportFunctionData;

/** Shared pointer to support function data. */
typedef std::shared_ptr<SupportFunctionData> SupportFunctionDataPtr;

#endif /* SUPPORT_FUNCTION_DATA_H_FORWARD */

#ifndef SUPPORT_FUNCTION_DATA_H
#define SUPPORT_FUNCTION_DATA_H

#include <vector>

#include "DataContainers/SupportFunctionData/SupportFunctionDataItem.h"
#include "KernelCGAL/KernelCGAL.h"
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
	SupportFunctionData(const std::vector<SupportFunctionDataItem> itemsGiven);

	/**
	 * Reads support support function data from input stream.
	 *
	 * @param stream 	The stream for data to be read from.
	 */
	SupportFunctionData(std::istream &stream);

	/**
	 * Assignment operator.
	 *
	 * @param data	The original data.
	 * @return The assigned data.
	 */
	SupportFunctionData &operator=(const SupportFunctionData &data);

	/** Destructor. */
	virtual ~SupportFunctionData();

	/**
	 * Subscription operator.
	 *
	 * @param iPosition	The ID of item that is demanded.
	 * @return The reference to the demanded item.
	 */
	SupportFunctionDataItem &operator[](const int iPosition);
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

	std::vector<SupportFunctionDataItem> getItems()
	{
		return items;
	}

	/**
	 * Gets the vector of support directions.
	 *
	 * @return Vector of support directions.
	 */
	template <class TemplPoint>
	std::vector<TemplPoint> supportDirections() const
	{
		DEBUG_START;
		std::vector<TemplPoint> directions;
		for (auto item = items.begin(); item != items.end(); ++item)
		{
			Vector3d v = item->direction;
			directions.push_back(TemplPoint(v.x, v.y, v.z));
		}
		DEBUG_END;
		return directions;
	}

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

	/**
	 * Shifts all support values to random number that are not bigger than
	 * given limit.
	 *
	 * @param maxDelta	The maximum of absolute value of shift.
	 */
	void shiftValues(double maxDelta);

	/**
	 * Gets duals of support planes which free coefficients were incremented
	 * on a given number.
	 *
	 * @param data		Support dunction data.
	 * @param epsilon	The shifting number.
	 *
	 * @return		Duals of shifted support planes.
	 */
	std::vector<Point> getShiftedDualPoints(double epsilon);

	/**
	 * Gets duals of support planes which free coefficients were incremented
	 * on a given number.
	 *
	 * @param data		Support dunction data.
	 * @param epsilon	The shifting number.
	 *
	 * @return		Duals of shifted support planes.
	 */
	std::vector<Point_3> getShiftedDualPoints_3(double epsilon);

	/**
	 * Gets duals of support planes which free coefficients were incremented
	 * on given numbers.
	 *
	 * @param data		Support dunction data.
	 * @param epsilons	Shifting numbers.
	 *
	 * @return		Duals of shifted support planes.
	 */
	std::vector<Point_3> getShiftedDualPoints_3(std::vector<double> epsilons);

	/**
	 * Searches trusted edges that are depicted by support data.
	 *
	 * @param threshold	The value controlling the closeness of lines.
	 */
	void searchTrustedEdges(double threshold);
};

/**
 * Prints support function data in PLY format.
 *
 * @param stream	The output stream.
 * @param data		The data.
 */
std::ostream &operator<<(std::ostream &stream, SupportFunctionDataPtr data);

#endif /* SUPPORT_FUNCTION_DATA_H */
