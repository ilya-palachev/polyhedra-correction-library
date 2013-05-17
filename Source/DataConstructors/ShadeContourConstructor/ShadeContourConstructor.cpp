/*
 * ShadeContourConstructor.cpp
 *
 *  Created on: 17.05.2013
 *      Author: iliya
 */

#include "ShadeContourConstructor.h"

ShadeContourConstructor::ShadeContourConstructor(const Polyhedron* p,
		const ShadeContourData* d) :
				polyhedron(p),
				data(d)
{
}

ShadeContourConstructor::~ShadeContourConstructor()
{
}

void ShadeContourConstructor::buildShadeContours(int numContoursNeeded,
		double firstAngle)
{

}

