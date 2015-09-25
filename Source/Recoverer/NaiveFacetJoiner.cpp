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

void tryGetenvDouble(const char *envName, double &value)
{
	DEBUG_START;
	char *mistake = NULL;
	char *string = getenv(envName);
	if (string)
	{
		double valueNew = strtod(string, &mistake);
		if (mistake && *mistake)
			return;
		else
			value = valueNew;
	}
	DEBUG_END;
}

NaiveFacetJoiner::NaiveFacetJoiner(Polyhedron_3 polyhedron) :
	polyhedron_(polyhedron),
	facets_(polyhedron.size_of_facets()),
	vertices_(polyhedron.size_of_vertices()),
	thresholdBigFacet_(THRESHOLD_BIG_FACET_DEFAULT),
	thresholdLeastSquaresQuality_(THRESHOLD_LEAST_SQUARES_QUALITY_DEFAULT),
	thresholdClusterError_(THRESHOLD_CLUSTER_ERROR_DEFAULT)
{
	DEBUG_START;
	int iFacet = 0;
	for (auto facet = polyhedron_.facets_begin();
			facet != polyhedron_.facets_end(); ++facet)
	{
		facets_[iFacet] = facet;
		facet->id = iFacet;
		++iFacet;
	}

	int iVertex = 0;
	for (auto vertex = polyhedron_.vertices_begin();
			vertex != polyhedron_.vertices_end(); ++vertex)
	{
		vertices_[iVertex] = vertex;
		vertex->id = iVertex;
		++iVertex;
	}
	tryGetenvDouble("THRESHOLD_BIG_FACET", thresholdBigFacet_);
	tryGetenvDouble("THRESHOLD_LEAST_SQUARES_QUALITY",
			thresholdLeastSquaresQuality_);
	tryGetenvDouble("THRESHOLD_CLUSTER_ERROR", thresholdClusterError_);
	DEBUG_END;
}

NaiveFacetJoiner::~NaiveFacetJoiner()
{
	DEBUG_START;
	DEBUG_END;
}

double calculateFacetArea(Polyhedron_3::Facet facet)
{
	DEBUG_START;
	auto halfedge = facet.facet_begin();
	auto halfedgeNext = halfedge;
	++halfedgeNext;
	Point_3 pointBegin = facet.facet_begin()->vertex()->point();
	double areaFacet = 0.;
	do
	{
		Point_3 point = halfedge->vertex()->point();
		Point_3 pointNext = halfedgeNext->vertex()->point();
		CGAL::Triangle_3<Kernel> triangle(pointBegin, point,
				pointNext);
		double areaTriangle = sqrt(triangle.squared_area());
		areaFacet += areaTriangle;
		++halfedge;
		++halfedgeNext;
	}
	while (halfedgeNext != facet.facet_begin());
	areaFacet = fabs(areaFacet);
	DEBUG_END;
	return areaFacet;
}

std::set<int> findBigFacets(Polyhedron_3 polyhedron, double thresholdBigFacet)
{
	DEBUG_START;
	std::set<int> indicesBigFacets;
	int iFacet = 0;
	for (auto facet = polyhedron.facets_begin();
			facet != polyhedron.facets_end(); ++facet)
	{
		double area = calculateFacetArea(*facet);
		if (area > thresholdBigFacet)
		{
			indicesBigFacets.insert(iFacet);
		}
		++iFacet;
	}
	std::cerr << indicesBigFacets.size() << " of "
		<< polyhedron.size_of_facets() << " facets are big"
		<< std::endl;
	printColouredPolyhedron(polyhedron, indicesBigFacets,
			"big-facets-cluster.ply");
	DEBUG_END;
	return indicesBigFacets;
}


std::pair<Plane_3, double> NaiveFacetJoiner::analyzeCluster(
		std::set<int> indicesCluster)
{
	DEBUG_START;
	std::set<int> indicesVerticesAll;
	for (int iFacet: indicesCluster)
	{
		auto indicesVertices = getIncidentVerticesIndices(iFacet);
		indicesVerticesAll.insert(indicesVertices.begin(),
				indicesVertices.end());
	}
	std::set<Point_3> points;
	for (int iVertex: indicesVerticesAll)
	{
		points.insert(vertices_[iVertex]->point());
	}

	Plane_3 planeBest;
	double quality = CGAL::linear_least_squares_fitting_3(
	                points.begin(), points.end(), planeBest,
	                CGAL::Dimension_tag<0>());
	if (quality < thresholdLeastSquaresQuality_ || !std::isfinite(quality))
	{
		DEBUG_END;
		return std::make_pair(Plane_3(), ALPHA_CLUSTER_INFINITY);
	}
	double distanceMaximal = 0.;
	for (auto point: points)
	{
		double distance = sqrt(CGAL::squared_distance(point,
					planeBest));
		if (distance > distanceMaximal)
		{
			distanceMaximal = distance;
		}
	}

	DEBUG_END;
	return std::make_pair(planeBest, distanceMaximal);
}

std::set<int> NaiveFacetJoiner::getNeighborsIndices(int iFacet)
{
	DEBUG_START;
	auto facet = facets_[iFacet];
	auto halfedge = facet->facet_begin();
	std::set<int> indicesNeighbors;
	do
	{
		indicesNeighbors.insert(halfedge->opposite()->facet()->id);
		++halfedge;
	}
	while (halfedge != facet->facet_begin());
	DEBUG_END;
	return indicesNeighbors;
}

std::set<int> NaiveFacetJoiner::getIncidentVerticesIndices(int iFacet)
{
	DEBUG_START;
	auto facet = facets_[iFacet];
	auto halfedge = facet->facet_begin();
	std::set<int> indicesVertices;
	do
	{
		indicesVertices.insert(halfedge->vertex()->id);
		++halfedge;
	}
	while (halfedge != facet->facet_begin());
	DEBUG_END;
	return indicesVertices;
}

void doParaviewVisualization(Polyhedron_3 polyhedron,
		std::vector<std::pair<std::set<int>, double>> clusterCandidates)
{
	DEBUG_START;
	for (auto clusterCandidate: clusterCandidates)
	{
		std::cerr << "Error " << clusterCandidate.second
			<< " for cluster ";
		for (int iFacet: clusterCandidate.first)
		{
			std::cerr << iFacet << " ";
		}
		std::cerr << std::endl;
			printColouredPolyhedronAndLoadParaview(polyhedron,
				clusterCandidate.first);
	}
	DEBUG_END;
}

Polyhedron_3 NaiveFacetJoiner::run()
{
	DEBUG_START;

	/* 1. Find big facets: */
	auto indicesBigFacets = findBigFacets(polyhedron_, thresholdBigFacet_);

	/* 2. Find first cluster candidates among them: */
	auto comparer = [](std::pair<std::set<int>, double> a,
			std::pair<std::set<int>, double> b)
	{
		auto iA = a.first.begin();
		auto iB = b.first.begin();
		do
		{
			if (*iA < *iB)
			{
				return true;
			}
			if (*iA > *iB)
			{
				return false;
			}
			++iA;
			++iB;
		}
		while (iA != a.first.end() && iB != b.first.end());
		return false;
	};
	std::set<std::pair<std::set<int>, double>,
		decltype(comparer)> clusterCandidates(comparer);
	for (int iFacetBig: indicesBigFacets)
	{
		auto indicesNeighbors = getNeighborsIndices(iFacetBig);
		for (int iFacetNeighbor: indicesNeighbors)
		{
			if (indicesBigFacets.find(iFacetNeighbor)
					!= indicesBigFacets.end())
			{
				std::set<int> cluster;
				cluster.insert(iFacetBig);
				cluster.insert(iFacetNeighbor);
				auto pair = analyzeCluster(cluster);
				clusterCandidates.insert(
						std::make_pair(cluster,
							pair.second));
			}
		}
	}

	/* 3. Sort found first cluster candidates by error values: */
	std::vector<std::pair<std::set<int>, double>> clusterCandidatesSorted;
	std::set<int> clusterCandidatesUnion;
	for (auto clusterCandidate: clusterCandidates)
	{
		clusterCandidatesSorted.push_back(clusterCandidate);
		auto cluster = clusterCandidate.first;
		clusterCandidatesUnion.insert(cluster.begin(), cluster.end());
	}
	printColouredPolyhedron(polyhedron_, clusterCandidatesUnion,
			"first-clusters-union.ply");
	std::sort(clusterCandidatesSorted.begin(),
			clusterCandidatesSorted.end(),
			[](std::pair<std::set<int>, double> a,
				std::pair<std::set<int>, double> b)
	{
		return a.second < b.second;
	});
	if (getenv("DO_PARAVIEW_VISUALIZATION"))
		doParaviewVisualization(polyhedron_, clusterCandidatesSorted);

	/* 4. Start build clusters from first clusters candidates: */
	const int INDEX_NOT_PROCESSED = -1;
	const int INDEX_NEW_CLUSTER = -1;
	std::vector<std::set<int>> clusters;
	std::vector<int> index(polyhedron_.size_of_facets());
	for (int i = 0; i < (int) index.size(); ++i)
		index[i] = INDEX_NOT_PROCESSED;
	for (auto clusterCandidate: clusterCandidatesSorted)
	{
		std::set<int> cluster = clusterCandidate.first;
		double error = clusterCandidate.second;
		if (error > thresholdClusterError_)
			break;
		std::vector<int> indices;
		indices.insert(indices.end(), cluster.begin(), cluster.end());
		DEBUG_ASSERT(cluster.size() == 2);
		int iFacetFirst = indices[0];
		int iFacetSecond = indices[1];
		if (index[iFacetFirst] != INDEX_NOT_PROCESSED
				&& index[iFacetFirst] != INDEX_NOT_PROCESSED)
		{
			std::set<int> clusterCompetitor;
			int iMin = std::min(index[iFacetFirst],
					index[iFacetSecond]);
			int iMax = std::max(index[iFacetFirst],
					index[iFacetSecond]);
			clusterCompetitor.insert(clusters[iMin].begin(),
					clusters[iMin].end());
			clusterCompetitor.insert(clusters[iMax].begin(),
					clusters[iMax].end());
			auto pair = analyzeCluster(clusterCompetitor);
			double errorCompetitor = pair.second;
			if (errorCompetitor <= thresholdClusterError_)
			{
				clusters[iMin] = clusterCompetitor;
				clusters.erase(cluster.begin() + iMax);
				for (int iFacet: clusterCompetitor)
				{
					index[iFacet] = iMin;
				}
				continue;
			}

		}

		/* FIXME: code below is not correct. */
		double errorMinimal = error;
		int indexBest = INDEX_NEW_CLUSTER;
		std::set<int> clusterBest = cluster;
		for (int iFacet: cluster)
		{
			if (index[iFacet] == INDEX_NOT_PROCESSED)
				continue;
			std::set<int> clusterCompetitor;
			clusterCompetitor.insert(
					clusters[index[iFacet]].begin(),
					clusters[index[iFacet]].end());
			clusterCompetitor.insert(
					cluster.begin(),
					cluster.end());
			auto pair = analyzeCluster(clusterCompetitor);
			double errorCompetitor = pair.second;
			if (errorCompetitor < errorMinimal)
			{
				errorMinimal = errorCompetitor;
				indexBest = index[iFacet];
				clusterBest = clusterCompetitor;
			}
		}
		if (indexBest != INDEX_NEW_CLUSTER)
		{
			if (errorMinimal > thresholdClusterError_)
				continue;
			clusters[indexBest] = clusterBest;
		}
		else
		{
			indexBest = clusters.size();
			clusters.push_back(clusterBest);
		}
		for (int iFacet: clusterBest)
		{
			index[iFacet] = indexBest;
		}
	}
	
	printColouredPolyhedron(polyhedron_, clusters, "")
	DEBUG_END;
	return polyhedron_; /* FIXME */
}
