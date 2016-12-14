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
 * @file FixedTopology.cpp
 * @brief Fixed topology used for the correction of polyhedrons.
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataConstructor.h"
#include "FixedTopology.h"


FixedTopology::FixedTopology(Polyhedron_3 initialP,
		SupportFunctionDataPtr SData) :
	tangient(initialP.size_of_vertices()),
	incident(initialP.size_of_facets()),
	influent(initialP.size_of_facets()),
	neighbors(initialP.size_of_vertices())
{
	DEBUG_START;
	SupportFunctionDataConstructor constructor;
	constructor.run(SData->supportDirections<Point_3>(), initialP);
	auto IDs = constructor.getTangientIDs();
	for (unsigned i = 0; i < IDs.size(); ++i)
	{
		tangient[IDs[i]].insert(i);
	}

	initialP.initialize_indices();

	unsigned iVertex = 0;
	for (auto I = initialP.vertices_begin(),
			E = initialP.vertices_end(); I != E; ++I)
	{
		std::cout << "Constructing vertrex #" << iVertex
			<< ": ";
		auto C = I->vertex_begin();
		do
		{
			int iNeighbor = C->opposite()->vertex()->id;
			std::cout << iNeighbor << " ";
			neighbors[iVertex].insert(iNeighbor);
		} while (++C != I->vertex_begin());
		++iVertex;
		std::cout << std::endl;
	}
	
	unsigned iFacet = 0;
	for (auto I = initialP.facets_begin(),
			E = initialP.facets_end(); I != E; ++I)
	{
		auto C = I->facet_begin();
		do
		{
			int iVertex = C->vertex()->id;
			incident[iFacet].insert(iVertex);
		} while (++C != I->facet_begin());

		ASSERT(C == I->facet_begin());
		do
		{
			int iVertex = C->vertex()->id;
			for (int i : neighbors[iVertex])
			{
				influent[iFacet].insert(i);
			}
		} while (++C != I->facet_begin());

		std::cout << "Facet #" << iFacet << ":" << std::endl;
		std::cout << "  incident: ";
		for (int i : incident[iFacet])
		{
			ASSERT(influent[iFacet].count(i));
			std::cout << i << " ";
		}
		std::cout << std::endl;
		std::cout << "  influent: ";
		for (int i : influent[iFacet])
			std::cout << i << " ";
		std::cout << std::endl;

		++iFacet;
	}
	DEBUG_END;
}
