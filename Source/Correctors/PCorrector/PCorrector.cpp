/*
 * PCorrector.cpp
 *
 *  Created on: 12.05.2013
 *      Author: iliya
 */

#include "PolyhedraCorrectionLibrary.h"

PCorrector::PCorrector() :
				polyhedron(NULL)
{
	DEBUG_START;
	DEBUG_END;
}

PCorrector::PCorrector(Polyhedron* input) :
				polyhedron(input)
{
	DEBUG_START;
	DEBUG_END;
}

PCorrector::~PCorrector()
{
	DEBUG_START;
	DEBUG_END;
}

