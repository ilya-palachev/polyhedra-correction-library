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
 * @file NaiveFacetJoiner.h
 * @brief Naive facet joiner that joins facets based on the angle between their
 * planes (declaration).
 */

#ifndef NAIVEFACETJOINER_H_
#define NAIVEFACETJOINER_H_
#include "Polyhedron_3/Polyhedron_3.h"
#include <CGAL/linear_least_squares_fitting_3.h>

const double THRESHOLD_BIG_FACET_DEFAULT = 0.05;
const double ALPHA_CLUSTER_INFINITY = 1e16;

const int INDEX_NOT_PROCESSED = -1;
const int INDEX_NEW_CLUSTER = -1;

class NaiveFacetJoiner
{
private:
	/** The initial polyhedron. */
	Polyhedron_3 polyhedron_;

	/** The current clusters. */
	std::vector<std::set<int>> clusters_;

	/** Markers that show which cluster facets belong to. */
	std::vector<int> index_;

	/** The vector that implements random access to facets. */
	std::vector<Polyhedron_3::Facet_iterator> facets_;

	/** The vector that implements random access to vertices. */
	std::vector<Polyhedron_3::Vertex_iterator> vertices_;

	/**
	 * Threshold of facet area that corresponds to the minimal area of big
	 * facet.
	 */
	double thresholdBigFacet_;

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
	 * Gets the set of indices that correspond to the facets that are
	 * neighbors of the given cluster of facets.
	 *
	 * @param iCluster		The ID of cluster.
	 * @return			The set of neighbor facets IDs.
	 */
	std::set<int> getNeighborsIndicesCluster(int iCluster);

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

	/**
	 * Builds first clusters from cluster candidates depending on whether
	 * the cluster candidate has lower error than the threshold.
	 *
	 * @param clusterCandidates	The information about caluster
	 * 				candidates.
	 */
	void buildFirstClusters(std::vector<std::pair<std::set<int>, double>>
			clusterCandidates);

	/**
	 * Merges two clusters if it is possible (i. e. when the error of sum
	 * is not bigger than the threshold).
	 *
	 * @param iClusterFirst		The ID of the 1st cluster.
	 * @param iClusterSecond	The ID of the 2nd cluster.
	 * @return			True, if the merge has been performed
	 * 				successfully, false otherwise.
	 */
	bool mergeClusters(int iClusterFirst, int iClusterSecond);

	/**
	 * Extends given cluster with new facets that are represented as a set 
	 * of facet indices. It is done if it is possible (i. e. when the error
	 * of the sum is not bigger than the threshold).
	 *
	 * @param iCluster		The ID of cluster that shoudl be
	 * 				extended.
	 * @param cluster		The set of indices (that are currently
	 * 				not forming any registered cluster)
	 * 				that should be added to the existing
	 * 				cluster.
	 * @return			True, if the extension has been
	 * 				performed successfully, false otherwise.
	 */
	bool extendCluster(int iCluster, std::set<int> cluster);

	/**
	 * Tries to merge all possible pairs of clusters that are the
	 * neighbours of each other if it is possible (i. e. when the error of
	 * the sum is not bigger than the threshold).
	 *
	 * @return			True, if at least one pair of clusters
	 * 				has been merged successfully.
	 */
	bool tryMergeClusterPairs();

	/**
	 * Builds new additional clusters from the given set of facets indices.
	 *
	 * @param indicesBigFacets 	The set of indices from which new
	 * 				clusters should be formed.
	 */
	void buildAdditionalClusters(std::set<int> indicesBigFacets);

	/**
	 * Tries to extend all clusters with their neighbor facets if it is
	 * possible (i. e. when the error of the extended cluster is not bigger
	 * than the threshold) and in a best manner (i. e. if some facet can be
	 * appended to multiple clusters then the best of them in the sense of
	 * best error is chosen)
	 *
	 * @return			True, if at least one cluster has been
	 * 				extended.
	 */
	bool finalizeClusters();


public:
	/**
	 * The default constructor.
	 *
	 * @param polyhedron	The input polyhedron.
	 * @param threshold	The threshold of cluster error.
	 */
	NaiveFacetJoiner(Polyhedron_3 polyhedron, double threshold);

	/**
	 * The empty destructor.
	 */
	~NaiveFacetJoiner();

	/**
	 * Runs the join procedure.
	 *
	 * @return		The processed polyhedron and the IDs of facets
	 * 			that were added to clusters.
	 */
	std::pair<Polyhedron_3, std::vector<std::vector<int>>>run();
};

#endif /* NAIVEFACETJOINER_H_ */
