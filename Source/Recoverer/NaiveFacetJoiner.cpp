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
#include "LeastSquaresMethod.h"
#include "DebugPrint.h"
#include "DebugAssert.h"
#include "PCLDumper.h"
#include "Common.h"
#include "Recoverer/NaiveFacetJoiner.h"
#include "Recoverer/Colouring.h"

std::ostream &operator<<(std::ostream &stream, std::set<int> cluster)
{
	if (cluster.size() == 0)
	{
		stream << "<empty>";
	}
	else
	{
		for (int i: cluster)
		{
			stream << " " << i;
		}
	}
	return stream;
}

static std::vector<int> facetOriginalIndices;

NaiveFacetJoiner::NaiveFacetJoiner(Polyhedron_3 polyhedron, double threshold) :
	polyhedron_(polyhedron),
	clusters_(),
	index_(polyhedron.size_of_facets()),
	facets_(polyhedron.size_of_facets()),
	vertices_(polyhedron.size_of_vertices()),
	thresholdBigFacet_(THRESHOLD_BIG_FACET_DEFAULT),
	thresholdClusterError_(threshold)
{
	DEBUG_START;
	for (int i = 0; i < (int) index_.size(); ++i)
		index_[i] = INDEX_NOT_PROCESSED;

	std::cerr << "indexPlanes_:";
	for (int i: polyhedron_.indexPlanes_)
		std::cerr << " " << i;
	std::cerr << std::endl;

	int iFacet = 0;
	for (auto facet = polyhedron_.facets_begin();
			facet != polyhedron_.facets_end(); ++facet)
	{
		facets_[iFacet] = facet;
		int iFacetOriginal = polyhedron_.indexPlanes_[iFacet];
		facetOriginalIndices.push_back(iFacetOriginal);
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
	DEBUG_END;
	return indicesBigFacets;
}

std::pair<Plane_3, double> NaiveFacetJoiner::analyzeCluster(
		std::set<int> indicesCluster)
{
	DEBUG_START;
	std::set<int> indicesVerticesAll;
	std::cerr << "Analyzing cluster: " << indicesCluster << std::endl;
	for (int iFacet: indicesCluster)
	{
		auto indicesVertices = getIncidentVerticesIndices(iFacet);
		indicesVerticesAll.insert(indicesVertices.begin(),
				indicesVertices.end());
	}
	std::vector<Point_3> points;
	for (int iVertex: indicesVerticesAll)
	{
		points.push_back(vertices_[iVertex]->point());
	}
	double *x = new double[points.size()];
	double *y = new double[points.size()];
	double *z = new double[points.size()];
	for (int i = 0; i < (int) points.size(); ++i)
	{
		x[i] = points[i].x();
		y[i] = points[i].y();
		z[i] = points[i].z();
	}
	double a, b, c, d;
	runListSquaresMethod(points.size(), x, y, z, a, b, c, d);
	delete[] x;
	delete[] y;
	delete[] z;
	Plane_3 planeBest(a, b, c, d);

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

	std::cerr << "error for this cluster: " << distanceMaximal
		<< std::endl;
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

std::vector<std::pair<std::set<int>, double>>
NaiveFacetJoiner::findFirstClusterCandidates(std::set<int> indicesBigFacets)
{
	DEBUG_START;
	/* Find first cluster candidates among them: */
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

	/*  Sort found first cluster candidates by error values: */
	std::vector<std::pair<std::set<int>, double>> clusterCandidatesSorted;
	for (auto clusterCandidate: clusterCandidates)
	{
		clusterCandidatesSorted.push_back(clusterCandidate);
		auto cluster = clusterCandidate.first;
	}
	std::sort(clusterCandidatesSorted.begin(),
			clusterCandidatesSorted.end(),
			[](std::pair<std::set<int>, double> a,
				std::pair<std::set<int>, double> b)
	{
		return a.second < b.second;
	});
	if (getenv("DO_PARAVIEW_VISUALIZATION"))
		doParaviewVisualization(polyhedron_, clusterCandidatesSorted);
	DEBUG_END;
	return clusterCandidatesSorted;
}

bool NaiveFacetJoiner::mergeClusters(int iClusterFirst, int iClusterSecond)
{
	DEBUG_START;
	std::cerr << "Trying to merge clusters:" << std::endl;
	std::cerr << "  cluster #" << iClusterFirst << ": "
		<< clusters_[iClusterFirst] << std::endl;
	std::cerr << "  cluster #" << iClusterSecond << ": "
		<< clusters_[iClusterSecond] << std::endl;
	if (iClusterFirst == iClusterSecond)
	{
		std::cerr << "The same clusters!" << std::endl;
		return false;
	}
	std::set<int> clusterCompetitor;
	int iMin = std::min(iClusterFirst, iClusterSecond);
	int iMax = std::max(iClusterFirst, iClusterSecond);
	clusterCompetitor.insert(clusters_[iMin].begin(),
			clusters_[iMin].end());
	clusterCompetitor.insert(clusters_[iMax].begin(),
			clusters_[iMax].end());
	auto pair = analyzeCluster(clusterCompetitor);
	double errorCompetitor = pair.second;
	if (errorCompetitor <= thresholdClusterError_)
	{
		std::cerr << "Mering clusters!" << std::endl;
		clusters_[iMin] = clusterCompetitor;
		std::set<int> empty;
		clusters_[iMax] = empty;
		for (int iFacet: clusterCompetitor)
		{
			index_[iFacet] = iMin;
		}
		DEBUG_END;
		return true;
	}
	DEBUG_END;
	return false;
}

bool NaiveFacetJoiner::extendCluster(int indexCompetitor,
		std::set<int> cluster)
{
	DEBUG_START;
	std::set<int> clusterCompetitor;
	clusterCompetitor.insert(
			clusters_[indexCompetitor].begin(),
			clusters_[indexCompetitor].end());
	clusterCompetitor.insert(cluster.begin(),
			cluster.end());
	auto pair = analyzeCluster(clusterCompetitor);
	double errorCompetitor = pair.second;
	if (errorCompetitor <= thresholdClusterError_)
	{
		std::cerr << "Merging pair to the cluster!"
			<< std::endl;
		clusters_[indexCompetitor] = clusterCompetitor;
		for (int iFacet: clusterCompetitor)
		{
			index_[iFacet] = indexCompetitor;
		}
		return true;
	}
	DEBUG_END;
	return false;
}

bool checkClusters(std::vector<std::set<int>> clusters, std::vector<int> index)
{
	DEBUG_START;
	bool ifValid = true;
	for (int iCluster = 0; iCluster < (int) clusters.size(); ++iCluster)
	{
		for (auto iFacet: clusters[iCluster])
		{
			if (index[iFacet] != iCluster)
			{
				int iClusterMistaken = index[iFacet];
				std::cerr << "Error: facet #" << iFacet
					<< " reported in cluster #"
					<< iClusterMistaken
					<< ": " << clusters[iClusterMistaken]
					<< std::endl;
				std::cerr << " while it is in cluster #"
					<< iCluster << ": "
					<< clusters[iCluster] << std::endl;
				ifValid = false;
			}
		}
	}
	DEBUG_END;
	return ifValid;
}

void NaiveFacetJoiner::buildFirstClusters(
		std::vector<std::pair<std::set<int>, double>>
		clusterCandidates)
{
	DEBUG_START;
	std::vector<int> index(polyhedron_.size_of_facets());
	for (int i = 0; i < (int) index.size(); ++i)
		index[i] = INDEX_NOT_PROCESSED;
	int iCluster = 0;
	for (auto clusterCandidate: clusterCandidates)
	{
		if (!checkClusters(clusters_, index_))
			exit(EXIT_FAILURE);
		std::set<int> cluster = clusterCandidate.first;
		std::cerr << "==========================================="
			<< std::endl;
		std::cerr << "[" << iCluster++ << "/"
			<< clusterCandidates.size()
			<< "] Analyzing pair: " << cluster << std::endl;
		double error = clusterCandidate.second;
		std::cerr << "         error: " << error << std::endl;
		if (error > thresholdClusterError_)
		{
			std::cerr << "Error too big, breaking..." << std::endl;
			break;
		}
		for (int iFacet: cluster)
		{
			int iCluster = index_[iFacet];
			if (iCluster != INDEX_NOT_PROCESSED)
			{
				if (clusters_[iCluster].find(iFacet)
						== clusters_[iCluster].end())
				{
					std::cerr << "Reported " << iFacet
						<< " in cluster #" << iCluster
						<< ": " << clusters_[iCluster]
						<< std::endl;
					exit(EXIT_FAILURE);
				}
			}
		}
		std::vector<int> indices;
		indices.insert(indices.end(), cluster.begin(), cluster.end());
		ASSERT(cluster.size() == 2);
		int iFacetFirst = indices[0];
		int iFacetSecond = indices[1];
		std::cerr << "first = " << iFacetFirst << ", second = "
			<< iFacetSecond << std::endl;
		if (index_[iFacetFirst] != INDEX_NOT_PROCESSED
				&& index_[iFacetSecond] != INDEX_NOT_PROCESSED)
		{
			if (index_[iFacetFirst] == index_[iFacetSecond])
			{
				std::cerr << "Already in the same cluster!" << std::endl;
			}
			std::cerr << "Number of found competitors: 2 "
				<< std::endl;
			bool DEBUG_VARIABLE ifMerged = mergeClusters(
					index_[iFacetFirst],
					index_[iFacetSecond]);
			continue;
			/* FIXME: Maybe here will be cluster breaking. */
		}

		int indexCompetitor = INDEX_NEW_CLUSTER;
		int iFacetNew = INDEX_NEW_CLUSTER;
		int iFacetCommon = INDEX_NEW_CLUSTER;
		if (index_[iFacetFirst] != INDEX_NOT_PROCESSED)
		{
			indexCompetitor = index_[iFacetFirst];
			iFacetNew = iFacetSecond;
			iFacetCommon = iFacetFirst;
		}
		if (index_[iFacetSecond] != INDEX_NOT_PROCESSED)
		{
			indexCompetitor = index_[iFacetSecond];
			iFacetNew = iFacetFirst;
			iFacetCommon = iFacetSecond;
		}
		if (indexCompetitor != INDEX_NEW_CLUSTER)
		{
			std::cerr << "Number of found competitors: 1 "
				<< std::endl;
			std::cerr << "Competitor for facet #" << iFacetCommon
				<< ": cluster #" << indexCompetitor << ": "
				<< clusters_[indexCompetitor] << std::endl;
			bool ifExtended = extendCluster(indexCompetitor,
					cluster);

			/* FIXME: Maybe here will be cluster breaking. */
			if (!ifExtended)
			{
				std::cerr << "Adding new cluster with one "
					<< "facet " << iFacetNew << " !"
					<< std::endl;
				std::set<int> clusterNew;
				clusterNew.insert(iFacetNew);
				index_[iFacetNew] = clusters_.size();
				clusters_.push_back(clusterNew);
			}
		}
		else
		{
			std::cerr << "Adding new cluster!" << std::endl;
			for (int iFacet: cluster)
			{
				index_[iFacet] = clusters_.size();
			}
			clusters_.push_back(cluster);
		}
	}
	
	DEBUG_END;
}

bool NaiveFacetJoiner::tryMergeClusterPairs()
{
	DEBUG_START;
	bool ifMergedAtLeastOnce = false;
	for (int i = 0; i < (int) clusters_.size(); ++i)
	{
		if (clusters_[i].empty())
			continue;
		auto indicesFacets = getNeighborsIndicesCluster(i);
		std::set<int> indicesClusters;
		for (int iFacet: indicesFacets)
		{
			int iCluster = index_[iFacet];
			if (iCluster != INDEX_NOT_PROCESSED)
				indicesClusters.insert(iCluster);
		}
		for (int j: indicesClusters)
		{
			if (j == i)
				continue;
			if (clusters_[j].empty())
				continue;
			bool ifMerged = mergeClusters(i, j);
			if (ifMerged)
				ifMergedAtLeastOnce = true;
		}
	}
	DEBUG_END;
	return ifMergedAtLeastOnce;
}

void NaiveFacetJoiner::buildAdditionalClusters(std::set<int> indicesFreeFacets)
{
	DEBUG_START;
	for (int iFacet: indicesFreeFacets)
	{
		std::cerr << "===================" << std::endl;
		std::cerr << "Analyzing facet #" << iFacet << std::endl;
		if (index_[iFacet] == INDEX_NOT_PROCESSED)
		{
			std::set<int> cluster;
			cluster.insert(iFacet);
			auto indicesNeighbors = getNeighborsIndices(iFacet);
			int ifJoined = false;
			for (int iFacetNeighbor: indicesNeighbors)
			{
				if (index_[iFacetNeighbor]
						!= INDEX_NOT_PROCESSED)
				{
					ifJoined = extendCluster(
							index_[iFacetNeighbor],
							cluster);
					if (ifJoined)
						break;
				}
			}
			if (!ifJoined)
			{
				index_[iFacet] =
					clusters_.size();
				clusters_.push_back(cluster);
			}
		}
		else
		{
			std::cerr << "ALready in cluster #" << index_[iFacet]
				<< " :" << clusters_[index_[iFacet]]
				<< std::endl;
		}
	}
	DEBUG_END;
}

std::set<int> NaiveFacetJoiner::getNeighborsIndicesCluster(int iCluster)
{
	DEBUG_START;
	std::set<int> cluster = clusters_[iCluster];
	std::set<int> indicesNeighbors;
	for (int iFacet: cluster)
	{
		auto indices = getNeighborsIndices(iFacet);
		for (int i: indices)
		{
			if (cluster.find(i) == cluster.end())
			{
				indicesNeighbors.insert(i);
			}
		}
	}
	DEBUG_END;
	return indicesNeighbors;
}

bool NaiveFacetJoiner::finalizeClusters()
{
	DEBUG_START;
	if (!checkClusters(clusters_, index_))
		exit(EXIT_FAILURE);
	std::vector<int> indexBest(index_.size());
	for (int &i: indexBest)
		i = INDEX_NEW_CLUSTER;
	std::vector<double> errorMinimal(index_.size());
	for (double &error: errorMinimal)
		error = ALPHA_CLUSTER_INFINITY;
	
	for (int iCluster = 0; iCluster < (int) clusters_.size(); ++iCluster)
	{
		auto indicesNeighbors = getNeighborsIndicesCluster(iCluster);
		for (int iFacet: indicesNeighbors)
		{
			if (index_[iFacet] != INDEX_NOT_PROCESSED)
				continue;
			std::set<int> cluster;
			cluster.insert(iFacet);
			cluster.insert(clusters_[iCluster].begin(),
					clusters_[iCluster].end());
			auto pair = analyzeCluster(cluster);
			double error = pair.second;
			if (error <= thresholdClusterError_)
			{
				if (error < errorMinimal[iFacet])
				{
					errorMinimal[iFacet] = error;
					indexBest[iFacet] = iCluster;
				}
			}
		}
	}
	
	std::vector<std::vector<std::pair<int, double>>>
		extensions(clusters_.size());
	for (int iFacet = 0; iFacet < (int) index_.size(); ++iFacet)
	{
		int iCluster = indexBest[iFacet];
		if (iCluster != INDEX_NOT_PROCESSED)
		{
			auto pair = std::make_pair(iFacet,
					errorMinimal[iFacet]);
			extensions[iCluster].push_back(pair);
		}
	}

	int numExtensionsHappened = 0;
	for (int iCluster = 0; iCluster < (int) clusters_.size(); ++iCluster)
	{
		if (extensions[iCluster].size() == 0)
			continue;
		std::sort(extensions[iCluster].begin(),
			extensions[iCluster].end(),
			[](std::pair<int, double> a, std::pair<int, double> b)
		{
			return a.second < b.second;
		});
		for (auto extension: extensions[iCluster])
		{
			int iFacet = extension.first;
			std::set<int> cluster;
			cluster.insert(iFacet);
			bool ifExtended = extendCluster(iCluster, cluster);
			numExtensionsHappened += ifExtended;
		}
	}

	std::set<int> indicesFreeFacets;
	for (int iFacet = 0; iFacet < (int) index_.size(); ++iFacet)
	{
		if (index_[iFacet] == INDEX_NOT_PROCESSED
				&& indexBest[iFacet] == INDEX_NEW_CLUSTER)
		{
			indicesFreeFacets.insert(iFacet);
		}
	}
	buildAdditionalClusters(indicesFreeFacets);
	if (!checkClusters(clusters_, index_))
		exit(EXIT_FAILURE);
	
	DEBUG_END;
	return (numExtensionsHappened + indicesFreeFacets.size() > 0);
}

std::pair<Polyhedron_3, std::vector<std::vector<int>>> NaiveFacetJoiner::run()
{
	DEBUG_START;
	std::streambuf *cerrBuffer = std::cerr.rdbuf();
	std::stringstream stringBuffer;
	std::cerr.rdbuf(stringBuffer.rdbuf());

	thresholdClusterError_ *= 0.5;
	/* 1. Find big facets: */
	auto indicesBigFacets = findBigFacets(polyhedron_, thresholdBigFacet_);
	printColouredPolyhedron(polyhedron_, indicesBigFacets,
			"big-facets-cluster.ply");

	/* 2. Find first cluster candidates: */
	auto clusterCandidates = findFirstClusterCandidates(indicesBigFacets);
	std::set<int> clusterCandidatesUnion;
	for (auto clusterCandidate: clusterCandidates)
	{
		auto cluster = clusterCandidate.first;
		clusterCandidatesUnion.insert(cluster.begin(), cluster.end());
	}
	printColouredPolyhedron(polyhedron_, clusterCandidatesUnion,
			"first-clusters-union.ply");

	/* 3. Build first clusters: */
	buildFirstClusters(clusterCandidates);
	printColouredPolyhedron(polyhedron_, clusters_, "first-clusters.ply");
	std::cerr << "Number of first clusters: " << clusters_.size()
		<< std::endl;

	/* 4. Try to merge first clusters if possible: */
	tryMergeClusterPairs();
	printColouredPolyhedron(polyhedron_, clusters_,
			"first-clusters-merged.ply");
	std::cerr << "Number of first clusters merged: " << clusters_.size()
		<< std::endl;

	/* 
	 * 5. Try to form new clusters with other (non-clustered) big
	 * facets.
	 */
	buildAdditionalClusters(indicesBigFacets);
	printColouredPolyhedron(polyhedron_, clusters_,
			"second-clusters.ply");
	std::cerr << "Number of second clusters: " << clusters_.size()
		<< std::endl;
	int iCluster = 0;
	for (auto cluster: clusters_)
	{
		std::cerr << "Cluster #" << iCluster++ << ": " << cluster
			<< std::endl;
	}

	/*
	 * 6. Finalize clusters by extending them with neighboring facets:
	 */
	int iIteration = 0;
	bool ifExtended = false;
	do
	{
		std::cerr << "======================================"
			<< std::endl;
		std::cerr << "Running final iteration " << iIteration++
			<< std::endl;
		ifExtended = finalizeClusters();
	}
	while (ifExtended);
	printColouredPolyhedron(polyhedron_, clusters_,
			"clusters-finalized.ply");
	std::cerr << "Number of final clusters: " << clusters_.size()
		<< std::endl;

	/* 7. Try to merge first clusters if possible: */
	thresholdClusterError_ *= 2.;
	bool ifMerged = false;
	do
	{
		ifMerged = tryMergeClusterPairs();
	}
	while (ifMerged);
	printColouredPolyhedron(polyhedron_, clusters_,
			"clusters-finalized-merged.ply");
	std::cerr << "Number of final clusters merged: " << clusters_.size()
		<< std::endl;

	std::vector<Plane_3> planes;
	for (auto cluster: clusters_)
	{
		if (cluster.empty())
			continue;
		auto pair = analyzeCluster(cluster);
		Plane_3 plane = pair.first;
		planes.push_back(plane);
	}
	Polyhedron_3 intersection(planes);

	std::string naiveJoinerLog = stringBuffer.str();
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "naive-facet-joiner.log")
		<< naiveJoinerLog;
	std::cerr.rdbuf(cerrBuffer);
	
	if (getenv("DUMP_CLUSTERS"))
	{
		std::cerr << "Start printing in " << __FILE__ << std::endl;
		int iCluster = 0;
		for (auto cluster: clusters_)
		{
			if (cluster.empty())
				continue;
			std::cerr << "Cluster #" << iCluster << ":"
				<< std::endl;
			for (int iFacet: cluster)
			{
				std::cerr << " " << iFacet << " ("
					<< polyhedron_.indexPlanes_[iFacet]
					<< ") -> ";
				Plane_3 plane = facets_[iFacet]->plane();
				std::cerr << plane << std::endl;

			}
			std::cerr << std::endl;
			++iCluster;
		}
	}

	std::cerr << "facetOriginalIndices:";
	for (int i: facetOriginalIndices)
		std::cerr << " " << i;
	std::cerr << std::endl;

	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
			"just-facet-joined-polyhedron.ply") << intersection;
	std::vector<std::vector<int>> clustersOriginal;
	for (auto facet = intersection.facets_begin();
			facet != intersection.facets_end(); ++facet)
	{
		int iCluster = facet->id;
		std::vector<int> clusterOriginal;
		auto cluster = clusters_[iCluster];
		if (cluster.empty())
			continue;
		for (int iFacet: cluster)
		{
			int iFacetOriginal = facetOriginalIndices[iFacet];
			clusterOriginal.push_back(iFacetOriginal);
		}
		clustersOriginal.push_back(clusterOriginal);
	}

	for (auto cluster: clustersOriginal)
	{
		std::cerr << "Prepared cluster:";
		for (int i: cluster)
		{
			std::cerr << " " << i;
		}
		std::cerr << std::endl;
	}


	DEBUG_END;
	return std::make_pair(intersection, clustersOriginal);
}
