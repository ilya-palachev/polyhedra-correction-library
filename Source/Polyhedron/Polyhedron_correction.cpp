/*
 * Copyright (c) 2009-2014 Ilya Palachev <iliyapalachev@gmail.com>
 *
 * This file is part of Polyhedra Correction Library.
 *
 * Polyhedra Correction Library is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Polyhedra Correction Library is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Polyhedra Correction Library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Polyhedron/Polyhedron.h"
#include "Correctors/GlobalShadeCorrector/GlobalShadeCorrector.h"

void Polyhedron::correctGlobal(shared_ptr<ShadeContourData> contourData,
		GSCorrectorParameters* parameters, list<int>* facetsCorrected)
{
	DEBUG_START;
	preprocessAdjacency();
	GlobalShadeCorrector* gsCorrector = new GlobalShadeCorrector(get_ptr(),
			contourData, parameters);
	if (facetsCorrected)
	{
		gsCorrector->setFacetsCorrected(*facetsCorrected);
	}
	gsCorrector->runCorrection();
	delete gsCorrector;
	DEBUG_END;
}

