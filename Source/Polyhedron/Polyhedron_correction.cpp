/*
 * Polyhedron_correction.cpp
 *
 *  Created on: 07.06.2013
 *      Author: iliya
 */

#include "PolyhedraCorrectionLibrary.h"

void Polyhedron::correctGlobal(ShadeContourData* contourData,
		GSCorrectorParameters* parameters)
{
	DEBUG_START;
	preprocessAdjacency();
	GlobalShadeCorrector* gsCorrector = new GlobalShadeCorrector(this,
			contourData, parameters);
	gsCorrector->runCorrection();
	delete gsCorrector;
	DEBUG_END;
}

