/*
 * PData.cpp
 *
 *  Created on: 17.05.2013
 *      Author: iliya
 */

#include "PolyhedraCorrectionLibrary.h"

PData::PData(shared_ptr<Polyhedron> p) :
		polyhedron(p)
{
	DEBUG_START;
	DEBUG_END;
}

PData::~PData()
{
	DEBUG_START;
	DEBUG_END;
}

