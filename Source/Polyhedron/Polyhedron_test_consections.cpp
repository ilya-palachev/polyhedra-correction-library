#include "PolyhedraCorrectionLibrary.h"

int Polyhedron::test_consections(bool ifPrint)
{
	Verifier* verifier = new Verifier(this, ifPrint);
	int numConsections = verifier->test_consections();
	delete verifier;
	return numConsections;
}
