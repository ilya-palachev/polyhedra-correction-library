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
}

PAnalyzer::PAnalyzer(Polyhedron* p) :
		polyhedron(p)
{
}

PAnalyzer::~PAnalyzer()
{
}

