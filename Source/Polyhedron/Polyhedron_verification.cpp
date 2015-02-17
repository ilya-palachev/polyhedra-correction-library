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

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Polyhedron/Polyhedron.h"
#include "Polyhedron/Verifier/Verifier.h"
#include "Polyhedron/Facet/Facet.h"

int Polyhedron::test_structure()
{
	DEBUG_START;
	int i, res;
	res = 0;
	for (i = 0; i < numFacets; ++i)
	{
		if (!facets[i].verifyIncidenceStructure())
		{
			++res;
		}
	}
	DEBUG_END;
	return res;
}

int Polyhedron::countConsections(bool ifPrint)
{
	DEBUG_START;
	Verifier* verifier = new Verifier(get_ptr(), ifPrint);
	int numConsections = verifier->countConsections();
	delete verifier;
	DEBUG_END;
	return numConsections;
}

int Polyhedron::checkEdges(EdgeDataPtr edgeData)
{
	DEBUG_START;
	Verifier* verifier = new Verifier(get_ptr());
	int numEdgesDestructed = verifier->checkEdges(edgeData);
	delete verifier;
	DEBUG_END;
	return numEdgesDestructed;
}

bool Polyhedron::nonZeroPlanes()
{
	DEBUG_START;
	bool zeroExists = false;
	for (int i = 0; i < numFacets; ++i)
	{
		if (!facets[i].correctPlane())
		{
			DEBUG_PRINT("Plane in facet #%d is zero.", i);
			zeroExists = true;
		}
	}
	DEBUG_END;
	return !zeroExists;
}
