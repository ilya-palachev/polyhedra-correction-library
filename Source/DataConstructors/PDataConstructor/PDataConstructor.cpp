/*
 * PDataConstructor.cpp
 *
 *  Created on: 17.05.2013
 *      Author: iliya
 */

#include "PolyhedraCorrectionLibrary.h"

PDataConstructor::PDataConstructor(shared_ptr<Polyhedron> p) :
				polyhedron(p)
{
	DEBUG_START;
	DEBUG_END;
}

PDataConstructor::~PDataConstructor()
{
	DEBUG_START;
	DEBUG_END;
}

