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
 * @file SupportFunctionEstimationDataConditionConstructor.cpp
 * @brief Constructor of support function estimation data condition
 * - implementation.
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "DataConstructors/SupportFunctionEstimationDataConstructor/SupportFunctionEstimationDataConditionConstructor.h"

SupportFunctionEstimationDataConditionConstructor::SupportFunctionEstimationDataConditionConstructor(
		Polyhedron_3::Halfedge_iterator halfedge) :
	handles()
{
	DEBUG_START;
	/* Order on vertex handles. */
	auto constexpr vertexComparer = [](Polyhedron_3::Vertex_handle a,
			Polyhedron_3::Vertex_handle b)
	{
		return a->id < b->id;
	};

	/* Sort given vertex handles by IDs. */
	std::set<Polyhedron_3::Vertex_handle, decltype(vertexComparer)>
		handlesSorted(vertexComparer);
	handlesSorted.insert(halfedge->vertex());
	handlesSorted.insert(halfedge->prev()->vertex());
	handlesSorted.insert(halfedge->next()->vertex());
	handlesSorted.insert(halfedge->opposite()->next()->vertex());
	ASSERT(handlesSorted.size() == NUM_VERTICES);
	
	auto handle = handlesSorted.begin();
	/* Save sorted vertex handles. */
	for (unsigned int i = 0; i < NUM_VERTICES; ++i)
	{
		ASSERT(handle != handlesSorted.end());
		handles[i] = *handle;
		DEBUG_PRINT("Adding handle with ID = %ld", handles[i]->id);
		++handle;
	}
	DEBUG_END;
}

std::vector<std::pair<int, double>>
SupportFunctionEstimationDataConditionConstructor::constructCondition(
		bool ifScale) const
{
	DEBUG_START;
	/* Compute elements of the matrix row. */
	double determinants[NUM_VERTICES];
	Vector_3 vertices[NUM_VERTICES];
	for (unsigned int i = 0; i < NUM_VERTICES; ++i)
		vertices[i] = Vector_3(CGAL::ORIGIN, handles[i]->point());
	determinants[0] =   CGAL::determinant(vertices[1], vertices[2],
			vertices[3]);
	determinants[1] = - CGAL::determinant(vertices[0], vertices[2],
			vertices[3]);
	determinants[2] =   CGAL::determinant(vertices[0], vertices[1],
			vertices[3]);
	determinants[3] = - CGAL::determinant(vertices[0], vertices[1],
			vertices[2]);

	/* Change signs of elements if their some is negative. */
	double determinant = 0.;
	for (unsigned int i = 0; i < NUM_VERTICES; ++i)
		determinant += determinants[i];
	if (determinant < 0.)
	{
		for (unsigned int i = 0; i < NUM_VERTICES; ++i)
			determinants[i] = -determinants[i];
	}

	/* Scale elements if needed. */
	if (ifScale)
	{
		DEBUG_PRINT("condition is scaled");
		/* Compute L2 norm of the matrix row. */
		double norm = 0.;
		for (unsigned int i = 0; i < NUM_VERTICES; ++i)
			norm += determinants[i] * determinants[i];
		norm = sqrt(norm);

		/* Normalize each element of the row. */
		double scale = 1. / norm;
		for (unsigned int i = 0; i < NUM_VERTICES; ++i)
			determinants[i] *= scale;
	}

	/* Construct the vector of pairs. */
	std::vector<std::pair<int, double>> pairs;
	for (unsigned int i = 0; i < NUM_VERTICES; ++i)
		pairs.push_back(std::pair<int, double>(handles[i]->id,
					determinants[i]));
		
	DEBUG_END;
	return pairs;
}

