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
 * @file SupportFunctionData.cpp
 * @brief General class for support function data container
 * - implementation.
 */

#include <sys/time.h>
#include <Eigen/Dense>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "PCLDumper.h"
#include "DataContainers/SupportFunctionData/SupportFunctionData.h"
#include "Analyzers/SizeCalculator/SizeCalculator.h"
#include "Polyhedron_3/Polyhedron_3.h"
#include "halfspaces_intersection.h"


SupportFunctionData::SupportFunctionData() :
	items()
{
	DEBUG_START;
	DEBUG_END;
}

SupportFunctionData::SupportFunctionData(const SupportFunctionData &data) :
	items()
{
	DEBUG_START;
	items = data.items;
	DEBUG_END;
}

SupportFunctionData::SupportFunctionData(const SupportFunctionData *data) :
	items()
{
	DEBUG_START;
	items = data->items;
	DEBUG_END;
}

SupportFunctionData::SupportFunctionData(const SupportFunctionDataPtr data) :
	items()
{
	DEBUG_START;
	items = data->items;
	DEBUG_END;
}

SupportFunctionData::SupportFunctionData(
		const std::vector<SupportFunctionDataItem> itemsGiven)
{
	DEBUG_START;
	items = itemsGiven;
	DEBUG_END;
}

SupportFunctionData::~SupportFunctionData()
{
	DEBUG_START;
	items.clear();
	DEBUG_END;
}

SupportFunctionData &SupportFunctionData::operator=(const SupportFunctionData
		&data)
{
	DEBUG_START;
	items = data.items;
	DEBUG_END;
	return *this;
}

SupportFunctionDataItem &SupportFunctionData::operator[](const int iPosition)
{
	DEBUG_START;
	ASSERT(iPosition >= 0);
	ASSERT(items.size() > (unsigned) iPosition);
	DEBUG_END;
	return items[iPosition];
}

long int SupportFunctionData::size()
{
	DEBUG_START;
	long int size = items.size();
	DEBUG_END;
	return size;
}

SupportFunctionDataPtr SupportFunctionData::removeEqual()
{
	DEBUG_START;
	std::vector<SupportFunctionDataItem> itemsUnequal;

	for (auto item = items.begin(); item != items.end(); ++item)
	{
		SupportFunctionDataItem itemOrig = *item;

		/*
		 * If the item's direction was not normalized before, it is
		 * normalized here.
		 */
		itemOrig.direction.norm(1.);
		
		bool ifEqual = false;
		for (auto itemPrev = itemsUnequal.begin();
			itemPrev != itemsUnequal.end(); ++itemPrev)
		{
			if (equal(itemOrig.direction, itemPrev->direction,
				EPS_SUPPORT_DIRECTION_EQUALITY))
			{
				ifEqual = true;
				break;
			}
		}
		if (!ifEqual)
		{
			itemsUnequal.push_back(*item);
		}
	}
	SupportFunctionDataPtr dataUnequal(new
		SupportFunctionData(itemsUnequal));
	DEBUG_END;
	return dataUnequal;
}

std::vector<Vector3d> SupportFunctionData::supportDirections()
{
	DEBUG_START;
	std::vector<Vector3d> directions;
	for (auto item = items.begin(); item != items.end(); ++item)
	{
		directions.push_back(item->direction);
	}
	DEBUG_END;
	return directions;
}

std::vector<Point_3> SupportFunctionData::supportDirectionsCGAL()
{
	DEBUG_START;
	std::vector<Point_3> directions;
	for (auto item = items.begin(); item != items.end(); ++item)
	{
		Vector3d v = item->direction;
		directions.push_back(Point_3(v.x, v.y, v.z));
	}
	DEBUG_END;
	return directions;
}

VectorXd SupportFunctionData::supportValues()
{
	DEBUG_START;
	VectorXd values(items.size());
	int i = 0;
	for (auto item = items.begin(); item != items.end(); ++item)
	{
		values(i++) = item->value;
	}
	DEBUG_END;
	return values;
}

std::vector<Plane_3> SupportFunctionData::supportPlanes()
{
	DEBUG_START;
	std::vector<Plane_3> planes;
	for (auto item = items.begin(); item != items.end(); ++item)
	{
		ASSERT(item->value > 0);
		Plane_3 plane(item->direction.x, item->direction.y,
				item->direction.z, -item->value);
		ASSERT(equal(plane.a(), item->direction.x));
		ASSERT(equal(plane.b(), item->direction.y));
		ASSERT(equal(plane.c(), item->direction.z));
		ASSERT(equal(plane.d(), -item->value));
		planes.push_back(plane);
	}
	DEBUG_END;
	return planes;
}

std::vector<Vector3d> SupportFunctionData::supportPoints()
{
	DEBUG_START;
	std::vector<Vector3d> points;
	for (auto &item: items)
	{
		ASSERT(item.info);
		if (!item.info)
		{
			ERROR_PRINT("Support points were not kept.");
			exit(EXIT_FAILURE);
		}
		points.push_back(item.info->point);
	}
	DEBUG_END;
	return points;
}

/**
 * Generates random number d such that |d| <= maxDelta
 *
 * @param maxDelta	maximum absolute limit of generated number
 */
static double genRandomDouble(double maxDelta)
{
	DEBUG_START;
	//srand((unsigned) time(0));
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


void SupportFunctionData::shiftValues(double maxDelta)
{
	DEBUG_START;
#ifndef NDEBUG
	for (auto &item: items)
	{
		DEBUG_PRINT("old value: %lf", item.value);
	}
#endif /* NDEBUG */
	for (auto &item: items)
	{
		double randomDouble = genRandomDouble(maxDelta);
		double DEBUG_VARIABLE oldValue = item.value;
		item.value += randomDouble;
		DEBUG_PRINT("Changed value from %lf to %lf", oldValue,
				item.value);
	}
#ifndef NDEBUG
	for (auto &item: items)
	{
		DEBUG_PRINT("new value: %lf", item.value);
	}
#endif /* NDEBUG */
	DEBUG_END;
}

std::vector<Point> SupportFunctionData::getShiftedDualPoints(double epsilon)
{
	DEBUG_START;
	std::vector<Point> points;
	long int numDirections = size();
	for (int i = 0; i < numDirections; ++i)
	{
		auto item = items[i];
		Plane_3 plane(item.direction.x, item.direction.y,
				item.direction.z, -item.value - epsilon);
		Point_3 point = dual(plane);
		points.push_back(Point(point.x(), point.y(), point.z()));
	}
	DEBUG_END;
	return points;
}

std::vector<Point_3> SupportFunctionData::getShiftedDualPoints_3(double epsilon)
{
	DEBUG_START;
	std::vector<Point_3> points;
	long int numDirections = size();
	for (int i = 0; i < numDirections; ++i)
	{
		auto item = items[i];
		Plane_3 plane(item.direction.x, item.direction.y,
				item.direction.z, -item.value - epsilon);
		Point_3 point = dual(plane);
		points.push_back(point);
	}
	DEBUG_END;
	return points;
}

std::vector<Point_3> SupportFunctionData::getShiftedDualPoints_3(
		std::vector<double> epsilons)
{
	DEBUG_START;
	ASSERT((int) epsilons.size() == size());
	std::vector<Point_3> points;
	long int numDirections = size();
	for (int i = 0; i < numDirections; ++i)
	{
		auto item = items[i];
		Plane_3 plane(item.direction.x, item.direction.y,
				item.direction.z, -item.value - epsilons[i]);
		Point_3 point = dual(plane);
		points.push_back(point);
	}
	DEBUG_END;
	return points;
}

std::vector<std::vector<int>> getContoursIndices(
		std::vector<SupportFunctionDataItem> items)
{
	DEBUG_START;
	/* Find the number of contours. */
	int iContourMax = 0;
	for (auto item: items)
	{
		int iContour = item.info->iContour;
		if (iContour > iContourMax)
			iContourMax = iContour;
	}
	int numContours = iContourMax + 1;
	std::cerr << "Found " << numContours << " contours in support "
		<< "function data. " << std::endl;

	/* Construct arrays of contours' item IDs. */
	std::vector<std::vector<int>> contoursIndices(numContours);
	int iItem = 0;
	for (auto item: items)
	{
		contoursIndices[item.info->iContour].push_back(iItem);
		++iItem;
	}
	int iContour = 0;
	for (auto contourIndices: contoursIndices)
	{
		std::cerr << "Contour #" << iContour << ": ";
		for (auto iItem: contourIndices)
		{
			std::cerr << iItem << " ";
		}
		std::cerr << std::endl;
		++iContour;
	}
	DEBUG_END;
	return contoursIndices;
}

int calculateRank(double threshold, Plane_3 alpha, Plane_3 beta, Plane_3 gamma)
{
	DEBUG_START;
	typedef Eigen::Matrix<double, 3, 4> PlaneTripletMatrix;
	PlaneTripletMatrix matrix;
	matrix << alpha.a(), alpha.b(), alpha.c(), alpha.d(),
	       beta.a(), beta.b(), beta.c(), beta.d(),
	       gamma.a(), gamma.b(), gamma.c(), gamma.d();

	Eigen::FullPivLU<PlaneTripletMatrix> decomposition(matrix);
	decomposition.setThreshold(threshold);
	int rank = decomposition.rank();
	DEBUG_END;
	return rank;
}

int findBestPlaneID(Plane_3 plane, std::vector<Plane_3> planes)
{
	DEBUG_START;
	double minimal = 1e100;
	int iPlaneMinimal = 0;
	for (int iPlane = 0; iPlane < (int) planes.size(); ++iPlane)
	{
		double a = plane.a() - planes[iPlane].a();
		double b = plane.b() - planes[iPlane].b();
		double c = plane.c() - planes[iPlane].c();
		double d = plane.d() - planes[iPlane].d();
		double difference = a * a + b * b + c * c + d * d;
		if (difference < minimal)
		{
			minimal = difference;
			iPlaneMinimal = iPlane;
		}
	}
	DEBUG_END;
	return iPlaneMinimal;
}

std::vector<std::vector<int>> getContoursFacetsIndices(PolyhedronPtr polyhedron,
		std::vector<Plane_3> planes,
		std::vector<std::vector<int>> contoursIndices)
{
	DEBUG_START;
	std::vector<std::vector<int>> contoursFacetsIDs(contoursIndices.size());
	for (int iFacet = 0; iFacet < polyhedron->numFacets; ++iFacet)
	{
		Facet *facet = &polyhedron->facets[iFacet];
		Plane_3 plane(facet->plane);
		int id = findBestPlaneID(plane, planes);
		int iContour = 0;
		for (auto contourIndices: contoursIndices)
		{
			bool ifFound = false;
			for (int iItem = 0; iItem < (int) contourIndices.size();
					++iItem)
			{	if (contourIndices[iItem] == id)
				{
					ifFound = true;
					break;
				}
			}
			if (ifFound)
			{
				contoursFacetsIDs[iContour].push_back(iFacet);
				break;
			}
			++iContour;
		}
	}
	DEBUG_END;
	return contoursFacetsIDs;
}

void printColouredIntersection(std::vector<Plane_3> planes,
		std::vector<std::vector<int>> contoursIndices)
{
	DEBUG_START;
	Polyhedron_3 intersection;
	CGAL::internal::halfspaces_intersection(planes.begin(), planes.end(),
			intersection, Kernel());
	PolyhedronPtr polyhedron(new Polyhedron(intersection));
	auto contoursFacetsIDs = getContoursFacetsIndices(polyhedron, planes,
			contoursIndices);
	srand(time(NULL));
	for (auto contourFacetsIDs: contoursFacetsIDs)
	{
		unsigned char red = rand() % 256;
		unsigned char green = rand() % 256;
		unsigned char blue = rand() % 256;
		for (int iFacet: contourFacetsIDs)
		{
			polyhedron->facets[iFacet].set_rgb(red, green, blue);
		}
	}
	Polyhedron *polyhedronCopy = new Polyhedron(polyhedron);
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
			"naively-recovered-coloured.ply") << *polyhedronCopy;
	DEBUG_END;
}

void addTripletToClusters(std::vector<std::set<int>> &clusters,
		std::set<int> triplet)
{
	DEBUG_START;
	bool ifFound = false;
	for (auto &cluster: clusters)
	{
		for (int id: triplet)
		{
			if (cluster.find(id) != cluster.end())
			{
				ifFound = true;
				break;
			}
		}
		if (ifFound)
		{
			for (int id: triplet)
			{
				cluster.insert(id);
			}
			break;
		}
	}
	if (!ifFound)
	{
		clusters.push_back(triplet);
	}
	DEBUG_END;
}

void SupportFunctionData::searchTrustedEdges(double threshold)
{
	DEBUG_START;
	std::cerr << "SupportFunctionData::searchTrustedEdges is called!"
		<< std::endl;
	std::cerr << "threshold = " << threshold << std::endl;
	auto planes = supportPlanes();
	auto contoursIndices = getContoursIndices(items);
	printColouredIntersection(planes, contoursIndices);
	int numTriplets = 0;
	std::vector<std::set<int>> clusters;
	for (int iContour = 0; iContour < (int) contoursIndices.size();
			++iContour)
	{
		int iContourPrev = iContour - 1;
		int iContourNext = iContour + 1;
		if (iContourPrev < 0)
			iContourPrev = contoursIndices.size() - 1;
		if (iContourNext >= (int) contoursIndices.size())
			iContourNext = 0;
		for (auto id: contoursIndices[iContour])
			for (auto idPrev: contoursIndices[iContourPrev])
				for (auto idNext: contoursIndices[iContourNext])
				{
					std::set<int> triplet;
					triplet.insert(id);
					triplet.insert(idPrev);
					triplet.insert(idNext);
					int rank = calculateRank(threshold,
							planes[id],
							planes[idPrev],
							planes[idNext]);
					if (rank == 2)
					{
						std::cerr << "Found triplet ";
						for (int id: triplet)
							std::cerr << id << " ";
						std::cerr << std::endl;
						++numTriplets;
						addTripletToClusters(clusters,
								triplet);
					}
				}

	}
	std::cerr << "Found " << numTriplets << " degenerate triplets."
		<< std::endl;
	std::cerr << "Found " << clusters.size() << " clusters" << std::endl;
	std::vector<std::vector<int>> semiContours;
	int iCluster = 0;
	for (auto cluster: clusters)
	{
		std::vector<int> semiContour;
		std::cerr << "Cluster #" << iCluster << ": ";
		for (int id: cluster)
		{
			std::cerr << id << " ";
			semiContour.push_back(id);
		}
		std::cerr << std::endl;
		semiContours.push_back(semiContour);
		++iCluster;
	}
	printColouredIntersection(planes, semiContours);
	DEBUG_END;
}
