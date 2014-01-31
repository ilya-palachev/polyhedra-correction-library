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

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/algorithm.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/convex_hull_3.h>

#include "Polyhedron/Polyhedron.h"
#include "DataContainers/ShadeContourData/ShadeContourData.h"

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Polyhedron_3<K> Polyhedron_3;
typedef K::Segment_3 Segment_3;

/** Define point creator */
typedef K::Point_3 Point_3;
typedef CGAL::Creator_uniform_3<double, Point_3> PointCreator;

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
	 * (ax + by + cz + d = 0) |--> (-a/d, -b/d, -c/d)
	 *
	 * @param planes	The vector of planes
	 */
	vector<Vector3d> mapPlanesToDualSpace(vector<Plane> planes);
	
	/**
	 * Constructs convex hull of the point set using CGAL API.
	 *
	 * @param points	Vector of points
	 */
	Polyhedron_3 constructConvexHull(vector<Vector3d>);
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