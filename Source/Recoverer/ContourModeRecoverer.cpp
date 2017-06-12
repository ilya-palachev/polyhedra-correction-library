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
#include "Common.h"
#include "DebugAssert.h"
#include "DebugPrint.h"
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
		std::cout << "Contour " << i << " has " << numLongSidesCurrent
			<< " long sides, and total " << contour.size()
			<< std::endl;
		++numLongSidesLocal[numLongSidesCurrent];
	}

	std::cout << "There are " << numLongSides << " contour sides, which "
		"length is greater than " << edgeLengthLimit << std::endl;
	for (unsigned i = 0; i < maxSidesNumberLocal; ++i)
	{
		std::cout << "    Number of contour with " << i <<
			" long sides: " << numLongSidesLocal[i] << std::endl;
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
		std::cout << "Searching neighbors between contours " << iContour
			<< " and " << iNext << std::endl;
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
					std::cout << "Found neighbor (" << i
						<< ", " << j << ")"
						<< std::endl;
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
		<< ")" << std::endl;
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

#if 0
	ASSERT(fabs(std::imag(compVector(0))) < 1e-16);
	ASSERT(fabs(std::imag(compVector(1))) < 1e-16);
	ASSERT(fabs(std::imag(compVector(2))) < 1e-16);
#endif
	Vector_3 vector(std::real(compVector(0)), std::real(compVector(1)),
			std::real(compVector(2)));

	double error = 0.;
	for (const auto &point : points)
		error += point * point - (point * vector) / (vector * vector);

#if 1
	std::cout << "  Error for " << points.size() << " points: " << error
		<< std::endl;
#endif
	DEBUG_END;
	return error;
}

ClusterTy clusterizeOne(const ContourVectorTy &contours,
		const NeighborsTy &neighbors, unsigned iContour,
		unsigned iSide, double maxClusterError)
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
	for (const auto &pair : possibleCluster)
	{
		ClusterTy clusterNew = cluster;
		clusterNew.push_back(pair);
		if (clusterNew.size() == 1
			|| (calculateError(contours, clusterNew)
				<= maxClusterError))
			cluster = clusterNew;
		else
			break;
	}
	DEBUG_END;
	return cluster;
}

ClusterVectorTy clusterize(const ContourVectorTy &contours,
		const NeighborsTy &neighbors, double maxClusterError)
{
	DEBUG_START;
	ClusterVectorTy allClusters;
	for (unsigned iContour = 0; iContour < contours.size(); ++iContour)
	{
		auto &contour = contours[iContour];
		for (unsigned iSide = 0; iSide < contour.size(); ++iSide)
		{
			//std::cout << "Processing side " << iSide << std::endl;
			std::cout << "";
			ClusterTy cluster = clusterizeOne(contours, neighbors,
					iContour, iSide, maxClusterError);
			if (!cluster.empty() && cluster.size() > 1)
			{
				std::cout << "Found cluster of "
					<< cluster.size()
					<< " items for contour " << iContour
					<< ", side " << iSide << std::endl;
				allClusters.push_back(cluster);
			}

		}
	}
	/* FIXME: Choose best clusters for each side here. */
	auto clusters = allClusters;
	DEBUG_END;
	return clusters;
}

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

	ContourVectorTy contours(getContoursNumber(data));
	for (int i = 0; i < data->size(); ++i)
	{
		SupportFunctionDataItem item = (*data)[i];
		int iContour = item.info->iContour;
		contours[iContour].push_back(item);
	}

	SideIDsTy longSideIDs = getLongSidesIDs(contours, edgeLengthLimit);
	AnglesTy angles = calculateAngles(contours, longSideIDs);
	NeighborsTy neighbors = detectNeighbors(contours, longSideIDs, angles);

	double maxClusterError = 0.;
	if (!tryGetenvDouble("MAX_CLUSTER_ERROR", maxClusterError))
	{
		ERROR_PRINT("Failed to get MAX_CLUSTER_ERROR");
		DEBUG_END;
		return;
	}
	ClusterVectorTy clusters = clusterize(contours, neighbors,
			maxClusterError);
	DEBUG_END;
}
