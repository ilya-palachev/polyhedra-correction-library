/*
 * PAnalyzer.cpp
 *
 *  Created on: 21.06.2013
 *      Author: ilya
 */

#include "PolyhedraCorrectionLibrary.h"

PAnalyzer::PAnalyzer() :
		polyhedron()
{
	DEBUG_START;
	DEBUG_END;
}

PAnalyzer::PAnalyzer(Polyhedron* p) :
		polyhedron(p)
{
	DEBUG_START;
	DEBUG_END;
}

PAnalyzer::~PAnalyzer()
{
	DEBUG_START;
	DEBUG_END;
}

