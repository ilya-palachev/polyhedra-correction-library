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
 * @file SupportFunctionEstimationDataConditionConstructor.h
 * @brief Constructor of support function estimation data condition
 * - declaration.
 */

#ifndef SUPPORT_FUNCTION_ESTIMATION_DATA_CONDITION_CONSTRUCTOR
#define SUPPORT_FUNCTION_ESTIMATION_DATA_CONDITION_CONSTRUCTOR

#include "PolyhedronCGAL.h"

/** Array of handles of vertices of tetrahedron.  */
class SupportFunctionEstimationDataConditionConstructor
{
private:
	/** Number of vertices in tetrahedron. */
	static const unsigned int NUM_VERTICES = 4;

	/** Vertices of tetrahedron. */
	Polyhedron_3::Vertex_handle handles[NUM_VERTICES];

public:
	/**
	 * Data constructor.
	 *
	 * @param haldedge	The halfedge that defines a polyhedron.
	 */
	SupportFunctionEstimationDataConditionConstructor(
			Polyhedron_3::Halfedge_iterator halfedge);

	/**
	 * Constructs support function estimation conditions as pairs of
	 * item ID and value.
	 *
	 * @param ifScale	Whether the elements should be scaled
	 * @return 		Vector that represents the consition (usually of
	 * 			size 4)
	 */
	std::vector<std::pair<int, double>>
	constructCondition(bool ifScale) const;
	
	/**
	 * Less operator for the array of handles of vertices of tetrahedron.
	 *
	 * @param left	Left side of comparison
	 * @param right	Right side of comparison
	 * @
	 */
	friend bool
	operator<(const SupportFunctionEstimationDataConditionConstructor &left,
		const SupportFunctionEstimationDataConditionConstructor &right)
	{
		return std::tie(left.handles[0]->id,
				left.handles[1]->id,
				left.handles[2]->id,
				left.handles[3]->id)
			< std::tie(right.handles[0]->id,
				right.handles[1]->id,
				right.handles[2]->id,
				right.handles[3]->id);
	}

	Polyhedron_3::Vertex_handle &operator[](const int i)
	{
		return handles[i];
	}
};



#endif /* SUPPORT_FUNCTION_ESTIMATION_DATA_CONDITION_CONSTRUCTOR */
