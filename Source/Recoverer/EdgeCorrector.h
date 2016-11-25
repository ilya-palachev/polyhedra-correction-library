/*
 * Copyright (c) 2009-2016 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file EdgeCorrector.h
 * @brief Correction of polyhedron based on support function measurements and
 * edge heuristics.
 */

#ifndef EDGECORRECTOR_H
#define EDGECORRECTOR_H

#include "Polyhedron_3/Polyhedron_3.h"
#include "DataContainers/SupportFunctionData/SupportFunctionData.h"

/**
 * Correction of polyhedron based on support funciton measurements and edge
 * heuristics.
 */
class EdgeCorrector
{
	/** The initial polyhedron to be corrected. */
	Polyhedron_3 initialP;

	/** The support function data that should be considered. */
	SupportFunctionDataPtr SData;
public:
	/** Creates the corector for given polyhedron and data. */
	EdgeCorrector(Polyhedron_3 initialP,
			SupportFunctionDataPtr SData);

	/** Runs the correction process and builds the polyhedron. */
	Polyhedron_3 run();

};

struct SimpleEdge_3
{
	Point_3 A;
	Point_3 B;
	Plane_3 piA;
	Plane_3 piB;
	std::vector<Plane_3> tangients;
	int iForward;
	int iBackward;
};

#endif /* EDGECORRECTOR_H */
