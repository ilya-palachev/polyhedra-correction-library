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

NaiveFacetJoiner::NaiveFacetJoiner(Polyhedron_3 polyhedron) :
	polyhedron_(polyhedron),
	facets_(polyhedron.size_of_facets()),
	vertices_(polyhedron.size_of_vertices()),
	thresholdBigFacet_(),
	thresholdLeastSquaresQuality_()
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

	char *thresholdBigFacetString = getenv("THRESHOLD_BIG_FACET");
	std::set<int> indicesBigFacets;
	if (thresholdBigFacetString)
	{
		thresholdBigFacet_ = strtod(thresholdBigFacetString, NULL);
	}
	else
	{
		thresholdBigFacet_ = THRESHOLD_BIG_FACET_DEFAULT;
	}

	char *thresholdLeastSquaresQualityString = getenv(
			"THRESHOLD_LEAST_SQUARES_QUALITY");
	std::set<int> indicesLeastSquaresQualitys;
	if (thresholdLeastSquaresQualityString)
	{
		thresholdLeastSquaresQuality_ = strtod(
				thresholdLeastSquaresQualityString, NULL);
	}
	else
	{
		thresholdLeastSquaresQuality_
			= THRESHOLD_LEAST_SQUARES_QUALITY_DEFAULT;
	}
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

Polyhedron_3 NaiveFacetJoiner::run()
{
	DEBUG_START;
	auto indicesBigFacets = findBigFacets(polyhedron_, thresholdBigFacet_);
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
		decltype(comparer)> clustersInformations(comparer);
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
				clustersInformations.insert(
						std::make_pair(cluster,
							pair.second));
			}
		}
	}
	std::vector<std::pair<std::set<int>, double>> informations;
	for (auto information: clustersInformations)
	{
		informations.push_back(information);
	}
	std::sort(informations.begin(), informations.end(),
			[](std::pair<std::set<int>, double> a,
				std::pair<std::set<int>, double> b)
	{
		return a.second < b.second;
	});
	for (auto clusterInformation: informations)
	{
		std::cerr << "Error " << clusterInformation.second
			<< " for cluster ";
		for (int iFacet: clusterInformation.first)
		{
			std::cerr << iFacet << " ";
		}
		std::cerr << std::endl;
		if (getenv("DO_PARAVIEW_VISUALIZATION"))
			printColouredPolyhedronAndLoadParaview(polyhedron_,
				clusterInformation.first);
	}
	DEBUG_END;
	return polyhedron_; /* FIXME */
}
