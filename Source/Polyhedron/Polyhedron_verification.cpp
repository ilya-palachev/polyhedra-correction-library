/*
 * Polyhedron_verification.cpp
 *
 *  Created on: 19.06.2013
 *      Author: ilya
 */

#include "PolyhedraCorrectionLibrary.h"

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
