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

PCorrector::PCorrector(shared_ptr<Polyhedron> p) :
				polyhedron(p)
{
	DEBUG_START;
	DEBUG_END;
}

PCorrector::PCorrector(Polyhedron* p) :
				polyhedron()
{
	DEBUG_START;
	polyhedron.reset(p);
	DEBUG_END;
}

PCorrector::~PCorrector()
{
	DEBUG_START;
	DEBUG_END;
}

