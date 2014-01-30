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

/**
 * @file Recoverer.h
 * @brief Declaration of the main recovering engine, based on support function
 * estimation tuned for our specific case.
 */

#ifndef RECOVERER_H
#define RECOVERER_H

#include <vector>

#include "Polyhedron/Polyhedron.h"
#include "DataContainers/ShadeContourData/ShadeContourData.h"

/**
 * Main recovering engine, based on support function estimation tuned for our
 * specific needs.
 */
class Recoverer
{
private:

	/**
	 * Extracts support planes from shadow contours.
	 * 
	 * @param SCData	Shadow contours
	 */
	vector<Plane> extractSupportPlanes(ShadeContourDataPtr SCData);

	/**
	 * Performs the transformation of polar duality for the given set of planes,
	 * i. e. maps each plane as follows:
	 *
	 * @param planes	The vector of planes
	 */
	vector<Vector3d> mapPlanesToDualSpace(vector<plane> planes);
public:

	/**
	 * Empty constructor.
	 */
	Recoverer();

	/**
	 * Empty destructor.
	 */
	~Recoverer();

	/**
	 * Builds naive polyhedron using naive approach that intersect half-spaces
	 * corresponding to support planes.
	 *
	 * @param SCData	Shadow contour data based on noisy experimental
	 * measurements.
	 */
	Polyhedron* buildNaivePolyhedron(ShadeContourDataPtr SCData);
};

typedef shared_ptr<Recoverer> RecovererPtr;

#endif /* RECOVERER_H */