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
 * @file NaiveFacetJoiner.cpp
 * @brief Naive facet joiner that joins facets based on the angle between their
 * planes (implementation).
 */

#include <list>
#include <set>
#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Recoverer/NaiveFacetJoiner.h"
#include "Recoverer/Colouring.h"

static double threshold = 0.;

NaiveFacetJoiner::NaiveFacetJoiner(double thresholdInput)
{
	DEBUG_START;
	threshold = thresholdInput;
	DEBUG_END;
}

NaiveFacetJoiner::~NaiveFacetJoiner()
{
	DEBUG_START;
	DEBUG_END;
}

double calculateEdgeAngle(Polyhedron_3::Facet_handle facet,
		Polyhedron_3::Facet_handle facetOpposite)
{
	DEBUG_START;
	auto plane = facet->plane();
	auto norm = plane.orthogonal_vector();
	norm = norm / norm.squared_length();
	auto planeOpposite = facetOpposite->plane();
	auto normOpposite = planeOpposite.orthogonal_vector();
	normOpposite = normOpposite / normOpposite.squared_length();
	double product = norm * normOpposite;
	double angle = acos(product);
	DEBUG_END;
	return angle;
}

std::vector<std::set<int>> buildClusters(Polyhedron_3 polyhedron)
{
	DEBUG_START;
	int iHalfedge = 0;
	std::vector<std::set<int>> clusters;
	for (auto halfedge = polyhedron.halfedges_begin();
			halfedge != polyhedron.halfedges_end();
			++halfedge)
	{
		auto facet = halfedge->facet();
		auto facetOpposite = halfedge->opposite()->facet();
		double angle = calculateEdgeAngle(facet, facetOpposite);

		if (angle < threshold)
		{
			bool ifOneRegistered = false;
			for (auto &cluster: clusters)
			{
				if (cluster.find(facet->id) != cluster.end())
				{
					cluster.insert((int) facetOpposite->id);
					ifOneRegistered = true;
					break;
				}
				if (cluster.find(facetOpposite->id)
						!= cluster.end())
				{
					cluster.insert((int) facet->id);
					ifOneRegistered = true;
				}
			}
			if (!ifOneRegistered)
			{
				std::set<int> clusterNew;
				clusterNew.insert(facet->id);
				clusterNew.insert(facetOpposite->id);
				clusters.push_back(clusterNew);
			}
		}
		++iHalfedge;
	}
	DEBUG_END;
	return clusters;
}

std::list<Plane_3> collectClusterPlanes(Polyhedron_3 polyhedron,
		std::set<int> cluster)
{
	DEBUG_START;
	std::list<Plane_3> planes;
	for (auto facet = polyhedron.facets_begin();
			facet != polyhedron.facets_end(); ++facet)
	{
		if (cluster.find(facet->id) != cluster.end())
		{
			planes.push_back(facet->plane());
		}
	}
	DEBUG_END;
	return planes;
}

std::list<Point_3> collectClusterPoints(Polyhedron_3 polyhedron,
		std::set<int> cluster)
{
	DEBUG_START;
	std::set<int> indices;
	for (auto facet = polyhedron.facets_begin();
			facet != polyhedron.facets_end(); ++facet)
	{
		if (cluster.find(facet->id) != cluster.end())
		{
			auto halfedge = facet->facet_begin();
			do
			{
				indices.insert(halfedge->vertex()->id);
				++halfedge;
			}
			while (halfedge != facet->facet_begin());

		}
	}
	std::list<Point_3> points;
	for (auto vertex = polyhedron.vertices_begin();
			vertex != polyhedron.vertices_end(); ++vertex)
	{
		if (indices.find(vertex->id) != indices.end())
		{
			points.push_back(vertex->point());
		}
	}
	DEBUG_END;
	return points;
}

const double DEFAULT_QUALITY_BOUND = 0.99;
std::list<Plane_3> joinClusterLeastSquaresCGAL(Polyhedron_3 polyhedron,
		std::set<int> cluster)
{
	DEBUG_START;
	std::list<Plane_3> planes;
	auto planesOld = collectClusterPlanes(polyhedron, cluster);
	auto points = collectClusterPoints(polyhedron, cluster);
	Plane_3 planeBest;
	double quality = CGAL::linear_least_squares_fitting_3(
			points.begin(), points.end(), planeBest,
			CGAL::Dimension_tag<0>());
	for (auto plane: planesOld)
	{
		if (plane.orthogonal_vector()
				* planeBest.orthogonal_vector() < 0.)
		{
			planeBest = Plane_3(-planeBest.a(),
					-planeBest.b(), -planeBest.c(),
					-planeBest.d());
		}
	}
	if (getenv("PCL_DEEP_DEBUG"))
		std::cerr << "... quality " << quality << " ";
	double qualityBound = DEFAULT_QUALITY_BOUND;
	char *qualityBoundString = getenv("PCL_QUALITY_BOUND");
	if (qualityBoundString)
	{
		char *error = NULL;
		qualityBound = strtod(qualityBoundString, &error);
		if (error && *error)
		{
			ERROR_PRINT("Failed to read PCL_QUALITY_BOUND: %s",
					error);
			exit(EXIT_FAILURE);
		}
	}
	if (quality > qualityBound && std::isfinite(quality))
	{
		if (getenv("PCL_DEEP_DEBUG"))
			std::cerr << "ADDED!!!" << std::endl;
		planes.push_back(planeBest);
	}
	else
	{
		if (getenv("PCL_DEEP_DEBUG"))
			std::cerr << std::endl;
		for (auto plane: planesOld)
			planes.push_back(plane);
	}
	DEBUG_END;
	return planes;
}

std::list<Plane_3> collectNonJoinedPlanes(Polyhedron_3 polyhedron,
		std::vector<std::set<int>> clusters)
{
	DEBUG_START;
	std::list<Plane_3> planes;
	std::set<int> clustersUnion;
	for (auto cluster: clusters)
	{
		clustersUnion.insert(cluster.begin(), cluster.end());
	}
	int iFacet = 0;
	std::cerr << "Total size of clusters: " << clustersUnion.size()
		<< std::endl;
	for (auto facet = polyhedron.facets_begin();
			facet != polyhedron.facets_end(); ++facet)
	{
		if (clustersUnion.find(facet->id) == clustersUnion.end())
		{
			planes.push_back(facet->plane());
		}
		++iFacet;
	}
	DEBUG_END;
	return planes;
}

Polyhedron_3 NaiveFacetJoiner::run(Polyhedron_3 polyhedron)
{
	DEBUG_START;
	if (threshold <= 0.)
		return polyhedron;
	auto clusters = buildClusters(polyhedron);
	printColouredPolyhedron(polyhedron, clusters,
			"recovered-coloured-by-plane-clusters.ply");
	std::cerr << "Found " << clusters.size()
		<< " clusters for threshold: "
		<< threshold << std::endl;
	int iCluster = 0;
	std::list<Plane_3> planes;
	int numSuccessfullJoins = 0;
	for (auto cluster: clusters)
	{
		if (getenv("PCL_DEEP_DEBUG"))
		{
			std::cerr << "Cluster #" << iCluster << ": ";
			for (int iFacet: cluster)
			{
				std::cerr << iFacet << " ";
			}
		}
		auto planesNew = joinClusterLeastSquaresCGAL(polyhedron,
				cluster);
		if (planesNew.size() == 1)
			++numSuccessfullJoins;
		planes.insert(planes.end(), planesNew.begin(),
				planesNew.end());
		++iCluster;
	}
	std::cerr << "Successfully joined " << numSuccessfullJoins << " of "
		<< clusters.size() << " clusters" << std::endl;
	auto planesOld = collectNonJoinedPlanes(polyhedron, clusters);
	planes.insert(planes.end(), planesOld.begin(), planesOld.end());

	Polyhedron_3 intersection;
	CGAL::internal::halfspaces_intersection(planes.begin(), planes.end(),
			intersection, Kernel()); 
	intersection.initialize_indices();
	int numFacetsBefore = polyhedron.size_of_facets();
	int numFacetsAfter = intersection.size_of_facets();
	int numFacetsReduced = numFacetsBefore - numFacetsAfter;
	std::cerr << "Number of facets reduced from " << numFacetsBefore
		<< " to " << numFacetsAfter << " ( difference is "
		<< numFacetsReduced << ")" << std::endl;

	DEBUG_END;
	return intersection;
}
