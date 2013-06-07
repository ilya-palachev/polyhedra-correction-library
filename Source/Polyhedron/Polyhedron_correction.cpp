/*
 * Polyhedron_correction.cpp
 *
 *  Created on: 07.06.2013
 *      Author: iliya
 */

void Polyhedron::correctGlobal(ShadeContourData* contourData)
{
	GlobalShadeCorrector* gsCorrector = new GlobalShadeCorrector(this, contourData);
	gsCorrector->runCorrection();
}

