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
 * @file NaiveFacetJoiner.h
 * @brief Naive facet joiner that joins facets based on the angle between their
 * planes (declaration).
 */

#ifndef NAIVEFACETJOINER_H_
#define NAIVEFACETJOINER_H_
#include "halfspaces_intersection.h"
#include "Polyhedron_3/Polyhedron_3.h"
#include <CGAL/linear_least_squares_fitting_3.h>

const double THRESHOLD_BIG_FACET_DEFAULT = 0.05;
const double THRESHOLD_LEAST_SQUARES_QUALITY_DEFAULT = 0.9;
const double THRESHOLD_CLUSTER_ERROR_DEFAULT = 0.002;
const double ALPHA_CLUSTER_INFINITY = 1e16;

const int INDEX_NOT_PROCESSED = -1;

class NaiveFacetJoiner
{
private:
	/** The initial polyhedron. */
	Polyhedron_3 polyhedron_;

	/** The current cluters. */
	std::vector<std::set<int>> clusters_;

	/** Markers that show which cluster facets belong to. */
	std::vector<int> index_;

	/** The vector that implements random access to to facets. */
	std::vector<Polyhedron_3::Facet_iterator> facets_;

	/** The vector that implements random access to to vertices. */
	std::vector<Polyhedron_3::Vertex_iterator> vertices_;

	/**
	 * Threshold of facet area that corresponds to the minimal area of big
	 * facet.
	 */
	double thresholdBigFacet_;

	/**
	 * Threshold of linear least squares qulity (lower bound).
	 */
	double thresholdLeastSquaresQuality_;

	/** The worst possible cluster error. */
	double thresholdClusterError_;

	/**
	 * Analyzes the given cluster of facets (characterized by the set of
	 * indices).
	 *
	 * @param indicesCluster	The set of cluster facets' indices.
	 * @return			The pair of best plane and error.
	 */
	std::pair<Plane_3, double> analyzeCluster(std::set<int> indicesCluster);

	/**
	 * Gets the set of indices that correspond to the facets that are
	 * neighbors of the given one.
	 *
	 * @param iFacet		The ID of facet.
	 * @return			The set of neighbor facets IDs.
	 */
	std::set<int> getNeighborsIndices(int iFacet);

	/**
	 * Gets the set of indices that correspond to the vertices that are
	 * incident to the given facet.
	 *
	 * @param iFacet		The ID of facet.
	 * @return			The set of incident vertices IDs.
	 */
	std::set<int> getIncidentVerticesIndices(int iFacet);

	/**
	 * Finds first cluster candidates.
	 *
	 * @param indicesBigFacets	Big facets' IDs.
	 * @return			Cluster candidates.
	 */
	std::vector<std::pair<std::set<int>, double>>
		findFirstClusterCandidates(std::set<int> indicesBigFacets);

	void buildFirstClusters(std::vector<std::pair<std::set<int>, double>>
			clusterCandidates);

	bool mergeClusters(int iClusterFirst, int iClusterSecond);

	bool extendCluster(int iCluster, std::set<int> cluster);

	void tryMergeClusterPairs();

	void buildSecondClusters(std::set<int> indicesBigFacets);
public:
	/**
	 * The default constructor.
	 *
	 * @param polyhedron	The input polyhedron.
	 */
	NaiveFacetJoiner(Polyhedron_3 polyhedron);

	/**
	 * The empty destructor.
	 */
	~NaiveFacetJoiner();

	/**
	 * Runs the join procedure.
	 *
	 * @return		The processed polyhedron.
	 */
	Polyhedron_3 run();
};

#endif /* NAIVEFACETJOINER_H_ */
