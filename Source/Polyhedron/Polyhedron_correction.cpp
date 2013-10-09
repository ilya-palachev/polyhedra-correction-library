/*
 * Polyhedron_correction.cpp
 *
 *  Created on: 07.06.2013
 *      Author: iliya
 */

#include "PolyhedraCorrectionLibrary.h"

void Polyhedron::correctGlobal(shared_ptr<ShadeContourData> contourData,
		GSCorrectorParameters* parameters)
{
	DEBUG_START;
	preprocessAdjacency();
	GlobalShadeCorrector* gsCorrector = new GlobalShadeCorrector(get_ptr(),
			contourData, parameters);
	gsCorrector->runCorrection();
	delete gsCorrector;
	DEBUG_END;
}

