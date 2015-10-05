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
 * @file TrustedEdgesDetector
 * @brief The detector of edges that have iamges on multiple (at least three)
 * support directions, i. e. groups of planes that are intersecting (with some
 * precision) by some spatial line (declaration).
 */

#ifndef TRUSTEDEDGESDETECTOR_H_
#define TRUSTEDEDGESDETECTOR_H_

/**
 * The detector of trusted edges.
 */
class TrustedEdgesDetector
{
private:
	/** The initial support planes. */
	std::vector<Plane_3> planes_;

	/** The initial support directions. */
	std::vector<Point_3> points_;

	/** The initial support values. */
	VectorXd values_;

	/** The current clusters. */
	std::vector<std::set<int>> clusters_;

	/** Markers that show which cluster the plane belongs to. */
	std::vector<int> index_;

	/**
	 * The Delaunay triangulation of sphere which vertices are the initial
	 * support directions.
	 */
	Polyhedron_3 shere_;

	/** The vector that implements random access to to vertices. */
	std::vector<Polyhedron_3::Vertex_iterator> vertices_;

	/** The worst possible cluster error. */
	double thresholdClusterError_;

public:
	/**
	 * Teh defaulr constructor.
	 *
	 * @param planes	The list of initial support planes.
	 * @param polyhedron	The polyhedron obtained after naive facet
	 * 			clusterization.
	 * @param threshold	The threshold for cluster error.
	 */
	TrustedEdgesDetector(std::vector<Plane_3> planes,
			Polyhedron_3 polyhedron, double threshold);

	/**
	 * The empty destructor.
	 */
	~TrustedEdgesDetector();

	/**
	 * Runs the trusted edges detection.
	 *
	 * @return		The information about detected planes clusters.
	 */
	std::vector<std::pair<std::set<int>, std::pair<Point_3, Vector_3>>>
	run();
};

#endif /* TRUSTEDEDGESDETECTOR_H_ */
