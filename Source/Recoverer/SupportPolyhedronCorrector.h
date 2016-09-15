/*
 * Copyright (c) 2009-2015 Ilya Palachev <iliyapalachev@gmail.com>
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

/**
 * @file SupportPolyhedronCorrector.h
 * @brief Correction of polyhedron based on support function measurements.
 */

#ifndef SUPPORTPOLYHEDRONCORRECTOR_H
#define SUPPORTPOLYHEDRONCORRECTOR_H

#include "Polyhedron_3/Polyhedron_3.h"
#include "DataContainers/SupportFunctionData/SupportFunctionData.h"

/** Correction of polyhedron based on support funciton measurements. */
class SupportPolyhedronCorrector
{
	/** The initial polyhedron to be corrected. */
	Polyhedron_3 initialP;

	/** The support function data that should be considered. */
	SupportFunctionDataPtr SData;
public:
	/** Creates the corector for given polyhedron and data. */
	SupportPolyhedronCorrector(Polyhedron_3 initialP,
			SupportFunctionDataPtr SData);

	/** Runs the correction process and builds the polyhedron. */
	Polyhedron_3 run();

};

#endif /* SUPPORTPOLYHEDRONCORRECTOR_H */
