/*
 * Copyright (c) 2009-2017 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file ContourModeRecoverer.cpp
 * @brief Recovering in so-called "contour mode" (implementation)
 */

#include <iostream>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <CGAL/Polyhedron_incremental_builder_3.h>
#include "Common.h"
#include "DebugAssert.h"
#include "DebugPrint.h"
#include "PCLDumper.h"
#include "Polyhedron_3/Polyhedron_3.h"
#include "KernelCGAL/KernelCGAL.h"
#include "Recoverer/ContourModeRecoverer.h"

static unsigned getContoursNumber(SupportFunctionDataPtr data)
{
	DEBUG_START;
	std::set<int> contourIDs;
	for (int i = 0; i < data->size(); ++i)
	{
		auto item = (*data)[i];
		int iContour = item.info->iContour;
		contourIDs.insert(iContour);
	}
	std::cout << "Number of shadow contours: " << contourIDs.size()
		<< std::endl;
	for (int iContour : contourIDs)
		ASSERT(iContour < int(contourIDs.size()) && "Wrong numeration");
	DEBUG_END;
	return contourIDs.size();
}

typedef std::vector<std::vector<SupportFunctionDataItem>> ContourVectorTy;
typedef std::vector<std::vector<unsigned>> SideIDsTy;

SideIDsTy getLongSidesIDs(const ContourVectorTy &contours,
		double edgeLengthLimit)
{
	DEBUG_START;
	SideIDsTy longSideIDs(contours.size());
	unsigned maxSidesNumberLocal = 0;
	for (auto &contour : contours)
	{
		maxSidesNumberLocal = std::max(maxSidesNumberLocal,
			unsigned(contour.size()));
		for (unsigned i = 0; i < contour.size(); ++i)
		{
			ASSERT(contour[i].info->iSide == int(i)
					&& "Wrong numeration");
		}
	}
	std::cout << "Maximal sides number per contour: "
		<< maxSidesNumberLocal << std::endl;

	std::vector<unsigned> numLongSidesLocal(maxSidesNumberLocal);
	for (unsigned i = 0; i < maxSidesNumberLocal; ++i)
		numLongSidesLocal[i] = 0.;

	unsigned numLongSides = 0;
	for (unsigned i = 0; i < contours.size(); ++i)
	{
		auto &contour = contours[i];
		int numLongSidesCurrent = 0;
		for (unsigned j = 0; j < contour.size(); ++j)
		{
			auto item = contour[j];
			double length = sqrt(
					item.info->segment.squared_length());
			if (length >= edgeLengthLimit)
			{
				++numLongSides;
				++numLongSidesCurrent;
				longSideIDs[i].push_back(j);
			}
		}
#if 0
		std::cout << "Contour " << i << " has " << numLongSidesCurrent
			<< " long sides, and total " << contour.size()
			<< std::endl;
#endif
		++numLongSidesLocal[numLongSidesCurrent];
	}

	std::cout << "There are " << numLongSides << " contour sides, which "
		"length is greater than " << edgeLengthLimit << std::endl;
	for (unsigned i = 0; i < maxSidesNumberLocal; ++i)
	{
		if (numLongSidesLocal[i] > 0)
			std::cout << "    Number of contours with " << i
				<< " long sides: " << numLongSidesLocal[i]
				<< std::endl;
	}
	DEBUG_END;
	return longSideIDs;
}

typedef std::vector<std::vector<double>> AnglesTy;

static AnglesTy calculateAngles(const ContourVectorTy &contours,
		const SideIDsTy &longSideIDs)
{
	DEBUG_START;
	Vector_3 ez(0., 0., 1.);
	AnglesTy angles(contours.size());
	bool fullLog = getenv("FULL_LOG") != nullptr;
	for (unsigned iContour = 0; iContour < contours.size(); ++iContour)
	{
		if (fullLog)
			std::cout << "Angles for contour #" << iContour
				<< ", which has "
				<< longSideIDs[iContour].size()
				<< " long sides from total "
				<< contours[iContour].size()
				<< " sides:" << std::endl;
		unsigned iLong = 0;
		for (unsigned iSide = 0; iSide < contours[iContour].size();
				++iSide)
		{
			SupportFunctionDataItem item
				= contours[iContour][iSide];
			Vector_3 norm = item.direction;
			double angle = acos(norm * ez);
			Vector_3 product = CGAL::cross_product(ez, norm);
			if (product * item.info->normalShadow < 0.)
				angle *= -1.;
			angles[iContour].push_back(angle);
			double length = sqrt(
				item.info->segment.squared_length());
			if (longSideIDs[iContour][iLong] == iSide)
			{
				std::cout << COLOUR_YELLOW;
				if (length > 1.)
					std::cout << COLOUR_RED;
				++iLong;
			}
			if (fullLog)
			{
				std::cout << "    Angle for side #" << iSide
					<< ":\t";
				if (angle >= 0.)
					std::cout << " ";
				std::cout << angle << ",\tlength: "
					<< length << std::endl;
			}
			std::cout << COLOUR_NORM;
		}
	}
	DEBUG_END;
	return angles;
}

typedef std::vector<std::vector<std::pair<unsigned, unsigned>>> NeighborsTy;

NeighborsTy detectNeighbors(ContourVectorTy contours, SideIDsTy longSideIDs,
		AnglesTy angles)
{
	DEBUG_START;
	double angleDiffLimit = 0.;
	if (!tryGetenvDouble("ANGLE_DIFF_LIMIT", angleDiffLimit))
	{
		ERROR_PRINT("Failed to get ANGLE_DIFF_LIMIT");
		DEBUG_END;
		exit(EXIT_FAILURE);
	}

	NeighborsTy neighbors(contours.size());
	for (unsigned iContour = 0; iContour < contours.size(); ++iContour)
	{
		unsigned iNext = (contours.size() + iContour + 1)
			% contours.size();
#if 0
		std::cout << "Searching neighbors between contours " << iContour
			<< " and " << iNext << std::endl;
#endif
		for (unsigned i : longSideIDs[iContour])
		{
			double angle0 = angles[iContour][i];

			// FIXME: Hardcoded constant!
			if (fabs(angle0 + M_PI) < 0.01)
				continue;

			for (unsigned j : longSideIDs[iNext])
			{
				double angle1 = angles[iNext][j];
				if (fabs(angle0 - angle1) < angleDiffLimit)
				{
#if 0
					std::cout << "Found neighbor (" << i
						<< ", " << j << ")"
						<< std::endl;
#endif
					neighbors[iContour].push_back(
						std::make_pair(i, j));
				}
			}
		}
	}
	DEBUG_END;
	return neighbors;
}

typedef std::vector<std::pair<unsigned, unsigned>> ClusterTy;
typedef std::vector<ClusterTy> ClusterVectorTy;

void printPair(const std::pair<unsigned, unsigned> pair)
{
	std::cout << "(" << pair.first << ", " << pair.second
		<< ")";
}

ClusterTy getPossibleCluster(const ContourVectorTy &contours,
		const NeighborsTy &neighbors, unsigned iContour,
		unsigned iSide)
{
	DEBUG_START;
	ClusterTy cluster;
	unsigned iContourCurr = iContour;
	unsigned iSideCurr = iSide;
	do
	{
		cluster.push_back(std::make_pair(iContourCurr, iSideCurr));
		unsigned iContourNext = (contours.size() + iContourCurr + 1)
			% contours.size();
		unsigned iSideNext = 0;
		unsigned numPairs = 0;
		for (const auto &pair : neighbors[iContourCurr])
			if (pair.first == iSideCurr)
			{
				++numPairs;
				iSideNext = pair.second;
			}
		ASSERT(numPairs <= 1 && "Not implemented yet");
		if (numPairs == 0)
			break;

		iContourCurr = iContourNext;
		iSideCurr = iSideNext;
	} while (iContourCurr != iContour);
	DEBUG_END;
	return cluster;
}

double calculateError(const ContourVectorTy &contours, const ClusterTy &cluster)
{
	DEBUG_START;
	std::vector<Vector_3> points;
	Vector_3 center;

	for (const auto &pair : cluster)
	{
		auto item = contours[pair.first][pair.second];
		Vector_3 point = item.direction;
		point = point * (1. / item.value);
		points.push_back(point);
		center = center + point;
	}
	center = center * (1. / cluster.size());
	for (auto &point : points)
		point = point - center;

	Eigen::MatrixXd A;
	A.resize(points.size(), 3);
	for (unsigned i = 0; i < points.size(); ++i)
	{
		Vector_3 point = points[i];
		A(i, 0) = point.x();
		A(i, 1) = point.y();
		A(i, 2) = point.z();
	}

	Eigen::MatrixXd P;
	P.resize(points.size(), points.size());
	for (unsigned i = 0; i < points.size(); ++i)
		for (unsigned j = 0; j < points.size(); ++j)
			if (i == j)
				P(i, j) = 1. - 1. / points.size();
			else
				P(i, j) = -1. / points.size();

	Eigen::MatrixXd B = A.transpose() * P * A;
	Eigen::EigenSolver<Eigen::MatrixXd> solver(B);
	unsigned iMax = 0;
	double maxValue = 0.;
	for (unsigned i = 0; i < 3; ++i)
	{
		std::complex<double> lambda = solver.eigenvalues()[i];
		double value = std::real(lambda);
		if (fabs(std::imag(lambda)) >= 1e-16)
			std::cout << "Imag: " << fabs(std::imag(lambda))
				<< std::endl;
		ASSERT(fabs(std::imag(lambda)) < 1e-16);
		ASSERT(value >= -1e-16);
		if (value > maxValue)
		{
			maxValue = value;
			iMax = i;
		}
		++i;
	}

	Eigen::VectorXcd compVector = solver.eigenvectors().col(iMax);

	ASSERT(fabs(std::imag(compVector(0))) < 1e-16);
	ASSERT(fabs(std::imag(compVector(1))) < 1e-16);
	ASSERT(fabs(std::imag(compVector(2))) < 1e-16);
	Vector_3 vector(std::real(compVector(0)), std::real(compVector(1)),
			std::real(compVector(2)));

	double error = 0.;
	for (const auto &point : points)
		error += point * point - (point * vector) / (vector * vector);

#if 0
	std::cout << "  Error for " << points.size() << " points: " << error
		<< std::endl;
#endif
	DEBUG_END;
	return error;
}

double calculateLengthError(const ContourVectorTy &contours,
		const ClusterTy &cluster)
{
	DEBUG_START;
	std::vector<double> lengths;
	for (const auto &pair : cluster)
	{
		auto item = contours[pair.first][pair.second];
		double length = sqrt(item.info->segment.squared_length());
		lengths.push_back(length);
	}

	double mean = 0.;
	for (double length : lengths)
		mean += length;
	mean /= double(lengths.size());

	double error = 0.;
	for (double length : lengths)
	{
		double diff = length - error;
		error += diff * diff;
	}
	error /= double(lengths.size());
	error = sqrt(error);
	DEBUG_END;
	return error;
}

ClusterTy clusterizeOne(const ContourVectorTy &contours,
		const NeighborsTy &neighbors, unsigned iContour,
		unsigned iSide, double maxClusterError, double maxLengthError)
{
	DEBUG_START;
	ClusterTy possibleCluster = getPossibleCluster(contours, neighbors,
			iContour, iSide);
	if (possibleCluster.empty())
	{
		std::cout << "  Stopping, possible cluster is empty..."
			<< std::endl;
		DEBUG_END;
		return possibleCluster;
	}

	ClusterTy cluster;
	unsigned iAttempt = 0;
	for (const auto &pair : possibleCluster)
	{
		ClusterTy clusterNew = cluster;
		clusterNew.push_back(pair);
		std::cout << "  Attempt #" << iAttempt << std::endl;
		double clusterError = calculateError(contours, clusterNew);
		std::cout << "    Cluster error: " << clusterError << std::endl;
		double lengthError = calculateLengthError(contours, clusterNew);
		std::cout << "    Length  error: " << lengthError << std::endl;
		if (clusterNew.size() == 1
			|| (clusterError <= maxClusterError
				&& lengthError <= maxLengthError))
			cluster = clusterNew;
		else
		{
			std::cout << "  Stopping attempts..." << std::endl;
			break;
		}
	}
	DEBUG_END;
	return cluster;
}

unsigned countContained(const ClusterTy &a, const ClusterTy &b)
{
	DEBUG_START;
	unsigned numContained = 0;
	for (const auto &pair : b)
		if (std::find(a.begin(), a.end(), pair) != a.end())
			++numContained;
	DEBUG_END;
	return numContained;
}

ClusterVectorTy chooseBestClusters(const ContourVectorTy &contours,
		ClusterVectorTy allClusters)
{
	DEBUG_START;
	std::sort(allClusters.begin(), allClusters.end(),
		[](const ClusterTy &a, const ClusterTy &b) -> bool
		{
			return a.size() > b.size();
		});

	ClusterVectorTy clusters;
	for (unsigned iCluster = 0; iCluster < allClusters.size(); ++iCluster)
	{
		ClusterTy cluster = allClusters[iCluster];
		if (cluster.size() < 3)
			continue;

		if (clusters.empty())
		{
			clusters.push_back(cluster);
			continue;
		}

		unsigned numContainedMax = 0;
		unsigned numContainerItems = 0;
		unsigned iBestCluster = 0;
		for (unsigned iCluster = 0; iCluster < clusters.size();
				++iCluster)
		{
			unsigned numContained = countContained(
					clusters[iCluster], cluster);
			if (numContained > numContainedMax)
			{
				numContainedMax = numContained;
				numContainerItems = clusters[iCluster].size();
				iBestCluster = iCluster;
			}
		}

		if (numContainedMax > cluster.size() / 2
				&& cluster.size() < numContainerItems)
			continue;
		else if (cluster.size() == numContainerItems
				&& calculateError(contours, cluster) >=
				calculateError(contours, clusters[iBestCluster]))
		{
			clusters[iBestCluster] = cluster;
			continue;
		}

		clusters.push_back(cluster);
	}

	DEBUG_END;
	return clusters;
}

ClusterVectorTy clusterize(const ContourVectorTy &contours,
		const NeighborsTy &neighbors, double maxClusterError,
		double maxLengthError)
{
	DEBUG_START;
	ClusterVectorTy allClusters;
	for (unsigned iContour = 0; iContour < contours.size(); ++iContour)
	{
		auto &contour = contours[iContour];
		for (unsigned iSide = 0; iSide < contour.size(); ++iSide)
		{
			/* 
			 * FIXME: !!! Without this line the program doesn't
			 * work!
			 */
			std::cout << "Clusterizing contour #" << iContour
				<< ", side #" << iSide << std::endl;;
			ClusterTy cluster = clusterizeOne(contours, neighbors,
					iContour, iSide, maxClusterError,
					maxLengthError);
			if (!cluster.empty() && cluster.size() > 1)
			{
#if 0
				std::cout << "Found cluster of "
					<< cluster.size()
					<< " items for contour " << iContour
					<< ", side " << iSide << std::endl;
#endif
				allClusters.push_back(cluster);
			}

		}
	}

	std::cout << "The number of clusters before post-processing: "
		<< allClusters.size() << std::endl;
	auto clusters = chooseBestClusters(contours, allClusters);
#if 0
	std::cout << "Found clusters:" << std::endl;
	unsigned iCluster = 0;
	for (const auto &cluster : clusters)
	{
		std::cout << "  Cluster #" << iCluster++ << ": ";
		for (const auto &pair : cluster)
		{
			printPair(pair);
			std::cout << " ";
		}
		std::cout << std::endl;
	}
#endif
	DEBUG_END;
	return clusters;
}

void printClustersStatistics(const ClusterVectorTy &clusters)
{
	DEBUG_START;
	std::map<std::pair<unsigned, unsigned>, unsigned> map;
	for (const auto &cluster : clusters)
	{
		for (auto pair : cluster)
		{
			if (map.find(pair) == map.end())
				map.insert(std::make_pair(pair, 1));
			else
				map[pair] = map[pair] + 1;
		}
	}

	unsigned numMaxClusters = 0;
	for (const auto &pair : map)
		numMaxClusters = std::max(numMaxClusters, pair.second);
	std::cout << "Maximal number of clusters per item: " << numMaxClusters
		<< std::endl;

	std::vector<std::vector<std::pair<unsigned, unsigned>>> mapSorted(
			numMaxClusters + 1);
	for (const auto &pair : map)
	{
		ASSERT(pair.second <= numMaxClusters);
		mapSorted[pair.second].push_back(pair.first);
	}

	std::cout << "Statistics about items:" << std::endl;
	for (unsigned n = 0; n < mapSorted.size(); ++n)
		if (!mapSorted[n].empty())
		{
			std::cout << "There are " << mapSorted[n].size()
				<< " items, which have " << n << " clusters: ";
#if 0
			for (const auto &pair : mapSorted[n])
			{
				printPair(pair);
				std::cout << " ";
			}
#endif
			std::cout << std::endl;
		}
	DEBUG_END;
}

template <class HDS>
class ClusterPolyhedronBuilder : public CGAL::Modifier_base<HDS>
{
	Vector_3 center;
	ContourVectorTy contours;
	ClusterTy cluster;
public:
	ClusterPolyhedronBuilder(const Vector_3 &center,
			const ContourVectorTy &contours,
			const ClusterTy &cluster) :
		center(center),
		contours(contours),
		cluster(cluster)
	{
		DEBUG_START;
		DEBUG_END;
	}

	void operator()(HDS &hds)
	{
		DEBUG_START;
		CGAL::Polyhedron_incremental_builder_3<HDS> builder(hds, true);
		ASSERT(contours.size() > 0);
		ASSERT(cluster.size() > 0);
		unsigned numVertices = 2 * cluster.size() + 1;
		unsigned numFacets = cluster.size();
		unsigned numHalfedges = 6 * cluster.size();
		std::cout << "For cluster with " << cluster.size()
			<< " items we create polyhedron with " << numVertices
			<< " vertices, " << numFacets << " facets, "
			<< numHalfedges << " halfedges" << std::endl;
		builder.begin_surface(numVertices, numFacets, numHalfedges);

		builder.add_vertex(CGAL::Origin() + center);
		for (const auto &pair : cluster)
		{
			unsigned iContour = pair.first;
			unsigned iSide = pair.second;
			Segment_3 segment = contours[iContour][iSide].info->segment;
			builder.add_vertex(segment.source());
			builder.add_vertex(segment.target());
		}

		for (unsigned i = 0; i < cluster.size(); ++i)
		{
			builder.begin_facet();
			builder.add_vertex_to_facet(0);
			builder.add_vertex_to_facet(2 * i + 1);
			builder.add_vertex_to_facet(2 * i + 2);
			builder.end_facet();
		}

		builder.end_surface();
		DEBUG_END;
	}

};

void ContourModeRecoverer::run()
{
	DEBUG_START;
	std::cout << "Starting contour mode recovering..." << std::endl;
	std::cout << "There are " << data->size() << " support items "
		<< std::endl;
	ASSERT(data->size() > 0 && "The data must be non-empty");

	double edgeLengthLimit = 0.;
	if (!tryGetenvDouble("EDGE_LENGTH_LIMIT", edgeLengthLimit))
	{
		ERROR_PRINT("Failed to get EDGE_LENGTH_LIMIT");
		DEBUG_END;
		return;
	}

	Vector_3 center;
	ContourVectorTy contours(getContoursNumber(data));
	double maxLength = 0.;
	for (int i = 0; i < data->size(); ++i)
	{
		SupportFunctionDataItem item = (*data)[i];
		int iContour = item.info->iContour;
		contours[iContour].push_back(item);
		Segment_3 segment = item.info->segment;
		center = center + (segment.source() - CGAL::Origin());
		center = center + (segment.target() - CGAL::Origin());
		maxLength = std::max(maxLength, sqrt(segment.squared_length()));
	}
	center = center * (0.5 / data->size());
	std::cout << "Center of contours: " << center << std::endl;

	SideIDsTy longSideIDs = getLongSidesIDs(contours, edgeLengthLimit);
	AnglesTy angles = calculateAngles(contours, longSideIDs);
	NeighborsTy neighbors = detectNeighbors(contours, longSideIDs, angles);

	double maxClusterError = maxLength; // Disable the check by default
	if (!tryGetenvDouble("MAX_CLUSTER_ERROR", maxClusterError))
	{
		ERROR_PRINT("Failed to get MAX_CLUSTER_ERROR");
		DEBUG_END;
		return;
	}

	double maxLengthError = maxLength;
	if (tryGetenvDouble("MAX_LENGTH_ERROR", maxLengthError))
		std::cout << "Checking length error is enabled..." << std::endl;

	ClusterVectorTy clusters = clusterize(contours, neighbors,
			maxClusterError, maxLengthError);
	std::cout << "The number of found clusters: " << clusters.size()
		<< std::endl;
	printClustersStatistics(clusters);

	for (unsigned iCluster = 0; iCluster < clusters.size(); ++iCluster)
	{
		std::cout << "******* Cluster #" << iCluster << " *******"
			<< std::endl;
		Polyhedron_3 clusterP;
		ClusterTy cluster = clusters[iCluster];
		double error = calculateError(contours, cluster);
		std::cout << "        error: " << error << std::endl;
		ClusterPolyhedronBuilder<Polyhedron_3::HalfedgeDS> builder(
				center, contours, cluster);
		clusterP.delegate(builder);
		std::string suffix("cluster-");
		suffix += std::to_string(iCluster);
		suffix += ".ply";
		globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, suffix.c_str())
			<< clusterP;
	}

	DEBUG_END;
}
