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
 * @file join2DFacets.cpp
 * @brief Prototype for best fitting polygon in support funciton metrics.
 */
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <cstdio>
#include <set>
#include <vector>
#include <Eigen/LU>

#include <CGAL/Cartesian.h>
typedef CGAL::Cartesian<double> K;
typedef CGAL::Point_2<K> Point_2;
typedef CGAL::Line_2<K> Line_2;
typedef CGAL::Vector_2<K> Vector_2;
#include <CGAL/convex_hull_2.h>
#include <CGAL/intersections.h>

#include "Constants.h"
#include "DebugPrint.h"
#include "DebugAssert.h"
#include "PCLDumper.h"

/**
 * Generates random number d such that |d| <= maxDelta
 *
 * @param maxDelta	maximum absolute limit of generated number
 */
static double genRandomDouble(double maxDelta)
{
	DEBUG_START;
	// srand((unsigned) time(0));
	struct timeval t1;
	gettimeofday(&t1, NULL);
	srand(t1.tv_usec * t1.tv_sec);

	int randomInteger = rand();
	double randomDouble = randomInteger;
	const double halfRandMax = RAND_MAX * 0.5;
	randomDouble -= halfRandMax;
	randomDouble /= halfRandMax;

	randomDouble *= maxDelta;

	DEBUG_END;
	return randomDouble;
}

#define MAX_DELTA 10.

std::vector<Point_2> generateExtremePoints(int numPoints)
{
	std::vector<Point_2> points;
	for (int i = 0; i < numPoints; ++i)
	{
		do
		{
			Point_2 point(genRandomDouble(MAX_DELTA), genRandomDouble(MAX_DELTA));
			points.push_back(point);
			std::vector<Point_2> hull;
			convex_hull_2(points.begin(), points.end(), std::back_inserter(hull));
			if (hull.size() == points.size())
				break;
			points.pop_back();
		} while (1);
	}
#if 0
	double maxDelta = 2. / ((double) numPoints);
	for (int i = 0; i < numPoints; ++i)
	{
		double angle = 2. * M_PI * ((double) i) / ((double) numPoints);
		double xShift = genRandomDouble(maxDelta);
		double yShift = genRandomDouble(maxDelta);
		Point_2 point(cos(angle) + xShift, sin(angle) + yShift);
		points.push_back(point);
	}
	std::vector<Point_2> hull;
	convex_hull_2(points.begin(), points.end(), std::back_inserter(hull));
	assert(hull.size() == numPoints);
	std::cout << "Number of points in the hull: " << hull.size() << std::endl;
#endif
	return points;
}

void centerizeExtremePoints(std::vector<Point_2> &points)
{
	double centerX = 0., centerY = 0.;
	for (auto point : points)
	{
		centerX += point.x();
		centerY += point.y();
	}
	centerX /= points.size();
	centerY /= points.size();
	Vector_2 center(centerX, centerY);
	for (auto &point : points)
	{
		point = point - center;
	}
}

std::pair<std::vector<Vector_2>, std::vector<double>> prepareSupportData(std::vector<Point_2> points)
{
	std::vector<Vector_2> directions;
	std::vector<double> values;
	int numPoints = points.size();
	for (int i = 0; i < numPoints; ++i)
	{
		int iNext = (i + 1) % numPoints;
		Line_2 line(points[i], points[iNext]);
		double a = line.a();
		double b = line.b();
		double c = line.c();
		if (a * points[i].x() + b * points[i].y() + c < 0)
		{
			a = -a;
			b = -b;
			c = -c;
		}
		Vector_2 normal(line.a(), line.b());
		double length = sqrt(normal.squared_length());
		Vector_2 direction(normal.x() / length, normal.y() / length);
		directions.push_back(direction);
		double value = -line.c() / length;
		values.push_back(value);
	}
	return std::make_pair(directions, values);
}

void printClusters(std::vector<std::set<int>> clusters)
{
	for (const auto &cluster : clusters)
	{
		std::cout << "cluster: ";
		for (const int i : cluster)
			std::cout << i << " ";
		std::cout << std::endl;
		if (cluster.empty())
		{
			std::cerr << "Cluster is empty!" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
}

std::vector<std::set<int>> prepareInitialClusters(int numPoints, int numClusters)
{
	std::vector<std::set<int>> clusters(numClusters);
	double step = ((double)numPoints) / ((double)numClusters);
	double bound = 0.;
	int iPoint = 0;
	int iCluster = 0;
	for (auto &cluster : clusters)
	{
		bound += step;
		while (iPoint < bound && iPoint < numPoints)
			cluster.insert(iPoint++);
		++iCluster;
	}
	std::cout << "Initial clusters:" << std::endl;
	printClusters(clusters);
	return clusters;
}

std::pair<Point_2, double> calculateClusterErrorAndCenter(const std::vector<Vector_2> &directions,
														  const std::vector<double> &values,
														  const std::set<int> &cluster)
{
	Eigen::Matrix2d A;
	Eigen::Vector2d b;
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
			A(i, j) = 0.;
		b(i) = 0.;
	}

	for (const int &i : cluster)
	{
		double u[2];
		u[0] = directions[i].x();
		u[1] = directions[i].y();
		double h = values[i];

		for (int i = 0; i < 2; ++i)
		{
			for (int j = 0; j < 2; ++j)
				A(i, j) += u[i] * u[j];
			b(i) += u[i] * h;
		}
	}

	Eigen::Matrix2d inverse = A.inverse();
	Eigen::Vector2d x = inverse * b;

	double error = 0.;
	for (const int &i : cluster)
	{
		double u[2];
		u[0] = directions[i].x();
		u[1] = directions[i].y();
		double h = values[i];
		double diff = u[0] * x(0) + u[1] * x[1] - h;
		error += diff * diff;
	}
	Point_2 center(x(0), x(1));
	return std::make_pair(center, error);
}

double calculateClusterError(const std::vector<Vector_2> &directions, const std::vector<double> &values,
							 const std::set<int> &cluster)
{
	return calculateClusterErrorAndCenter(directions, values, cluster).second;
}

void exchangeItemToRight(std::set<int> &cluster, std::set<int> &clusterNext)
{
	auto itLast = cluster.end();
	--itLast;
	clusterNext.insert(*itLast);
	cluster.erase(itLast);
}

void exchangeItemToRight(std::vector<std::set<int>> &clusters, int iCluster)
{
	int numClusters = clusters.size();
	int iClusterNext = (numClusters + iCluster + 1) % numClusters;
	exchangeItemToRight(clusters[iCluster], clusters[iClusterNext]);
}

void exchangeItemToLeft(std::set<int> &cluster, std::set<int> &clusterNext)
{
	auto itFirst = clusterNext.begin();
	cluster.insert(*itFirst);
	clusterNext.erase(itFirst);
}

void exchangeItemToLeft(std::vector<std::set<int>> &clusters, int iCluster)
{
	int numClusters = clusters.size();
	int iClusterNext = (numClusters + iCluster + 1) % numClusters;
	exchangeItemToLeft(clusters[iCluster], clusters[iClusterNext]);
}

double calculateClusterErrorsRight(const std::vector<Vector_2> &directions, const std::vector<double> &values,
								   const std::set<int> &cluster, const std::set<int> &clusterNext)
{
	std::set<int> clusterChanged = cluster;
	std::set<int> clusterNextChanged = clusterNext;
	exchangeItemToRight(clusterChanged, clusterNextChanged);
	return calculateClusterError(directions, values, clusterChanged) +
		   calculateClusterError(directions, values, clusterNextChanged);
}

double calculateClusterErrorsLeft(const std::vector<Vector_2> &directions, const std::vector<double> &values,
								  const std::set<int> &cluster, const std::set<int> &clusterNext)
{
	std::set<int> clusterChanged = cluster;
	std::set<int> clusterNextChanged = clusterNext;
	exchangeItemToLeft(clusterChanged, clusterNextChanged);
	return calculateClusterError(directions, values, clusterChanged) +
		   calculateClusterError(directions, values, clusterNextChanged);
}

struct EnhancementDescription
{
	int iClusterBestChange;
	double bestChange;
	bool ifRightDirection;

	EnhancementDescription() : iClusterBestChange(0), bestChange(0.), ifRightDirection(false)
	{
	}
};

EnhancementDescription findBestEnhancement(const std::vector<std::set<int>> &clusters,
										   const std::vector<Vector_2> &directions, const std::vector<double> &values,
										   const std::vector<double> &errors)
{
	EnhancementDescription ED;
	int iCluster = 0;
	int numClusters = clusters.size();
	double bestChange = 0.;
	for (const auto &cluster : clusters)
	{
		int iClusterNext = (numClusters + iCluster + 1) % numClusters;
		const auto &clusterNext = clusters[iClusterNext];
		double errorInitial = errors[iCluster] + errors[iClusterNext];
		double change = errorInitial - calculateClusterErrorsRight(directions, values, cluster, clusterNext);
		if (change > ED.bestChange)
		{
			ED.ifRightDirection = true;
			ED.iClusterBestChange = iCluster;
			ED.bestChange = change;
		}

		change = errorInitial - calculateClusterErrorsLeft(directions, values, cluster, clusterNext);
		if (change > bestChange)
		{
			ED.ifRightDirection = false;
			ED.iClusterBestChange = iCluster;
			ED.bestChange = change;
		}

		++iCluster;
	}
	return ED;
}

void constructBestClusters(std::vector<std::set<int>> &clusters, const std::vector<Vector_2> &directions,
						   const std::vector<double> &values)
{
	int numClusters = clusters.size();
	while (true)
	{
		std::vector<double> errors(numClusters);
		int iCluster = 0;
		for (const auto &cluster : clusters)
		{
			double error = calculateClusterError(directions, values, cluster);
			errors[iCluster] = error;
			++iCluster;
		}

		EnhancementDescription ED = findBestEnhancement(clusters, directions, values, errors);

		if (ED.bestChange > 0.)
		{
			if (ED.ifRightDirection)
				exchangeItemToRight(clusters, ED.iClusterBestChange);
			else
				exchangeItemToLeft(clusters, ED.iClusterBestChange);
		}
		else
			break;
	}
}

int main(int argc, char **argv)
{
	if (argc != 3)
		return EXIT_FAILURE;
	int numPoints = atoi(argv[1]);
	int numClusters = atoi(argv[2]);

	std::cout << "Convex hull of " << numPoints << " will be generated" << std::endl;
	if (numPoints <= 0)
		return EXIT_FAILURE;

	auto points = generateExtremePoints(numPoints);
	centerizeExtremePoints(points);

	std::sort(points.begin(), points.end(),
			  [](const Point_2 &a, const Point_2 &b) { return atan2(a.y(), a.x()) > atan2(b.y(), b.x()); });
	std::vector<Vector_2> directions;
	std::vector<double> values;
	std::tie(directions, values) = prepareSupportData(points);
	std::cout << "Data has been prepared." << std::endl;

	auto clusters = prepareInitialClusters(numPoints, numClusters);
	constructBestClusters(clusters, directions, values);
	std::cout << "Best clusters:" << std::endl;
	printClusters(clusters);
	return EXIT_SUCCESS;
}
