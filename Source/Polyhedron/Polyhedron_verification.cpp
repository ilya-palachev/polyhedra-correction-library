/*
 * Polyhedron_verification.cpp
 *
 *  Created on: 19.06.2013
 *      Author: ilya
 */

#include "PolyhedraCorrectionLibrary.h"

int Polyhedron::test_structure()
{
	int i, res;
	res = 0;
	for (i = 0; i < numFacets; ++i)
	{
		res += facets[i].test_structure();
	}
	return res;
}

int Polyhedron::countConsections(bool ifPrint)
{
	Verifier* verifier = new Verifier(this, ifPrint);
	int numConsections = verifier->countConsections();
	delete verifier;
	return numConsections;
}
