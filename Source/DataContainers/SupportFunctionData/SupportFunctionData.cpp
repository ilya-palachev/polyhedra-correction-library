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
#include "Recoverer/Colouring.h"


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

SupportFunctionData::SupportFunctionData(std::istream &stream)
{
	DEBUG_START;
	double x, y, z, h;
	while (stream >> x >> y >> z >> h)
	{
		SupportFunctionDataItem item(Vector3d(x, y, z), h);
		items.push_back(item);
	}
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
				std::cout << "Found equal items!" << std::endl;
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

int calculateRank(double threshold, std::vector<Plane_3> planes,
		std::set<int> cluster)
{
	DEBUG_START;
	Eigen::MatrixXd matrix;
	matrix.resize(cluster.size(), 4);
	int iPlane = 0;
	for (int id: cluster)
	{
		matrix(iPlane, 0) = planes[id].a();
		matrix(iPlane, 1) = planes[id].b();
		matrix(iPlane, 2) = planes[id].c();
		matrix(iPlane, 3) = planes[id].d();
		++iPlane;
	}
	Eigen::FullPivLU<Eigen::MatrixXd> decomposition(matrix);
	decomposition.setThreshold(threshold);
	int rank = decomposition.rank();
	DEBUG_END;
	return rank;
}

void addTripletToClusters(int threshold, std::vector<std::set<int>> &clusters,
		std::set<int> triplet, std::vector<Plane_3> planes)
{
	DEBUG_START;
	bool ifFound = false;
	for (auto &cluster: clusters)
	{
		std::set<int> clustersUnion;
		for (int id: cluster)
			clustersUnion.insert(id);
		for (int id: triplet)
			clustersUnion.insert(id);
		if (clustersUnion.size() < cluster.size() + triplet.size())
		{
			int rank = calculateRank(threshold, planes,
					clustersUnion);
			if (rank == 2)
			{
				cluster = clustersUnion;
				ifFound = true;
			}
		}
	}
	if (!ifFound)
	{
		clusters.push_back(triplet);
	}
	DEBUG_END;
}

std::vector<std::set<int>> getTriplets(
		std::vector<Point_3> directions,
		double height,
		std::vector<int> a,
		std::vector<int> b,
		std::vector<int> c)
{
	std::vector<std::set<int>> triplets;
	for (int id: a)
	{
		for (int idPrev: b)
		{
			if (fabs(directions[id].z()
					- directions[idPrev].z())
					> height)
				continue;
			for (int idNext: c)
			{
				if (fabs(directions[id].z()
						- directions[idNext].z())
						> height)
					continue;
				std::set<int> triplet;
				triplet.insert(id);
				triplet.insert(idPrev);
				triplet.insert(idNext);
				triplets.push_back(triplet);
			}
		}
	}
	return triplets;
}

void SupportFunctionData::searchTrustedEdges(double threshold)
{
	DEBUG_START;
	std::cerr << "SupportFunctionData::searchTrustedEdges is called!"
		<< std::endl;
	std::cerr << "threshold = " << threshold << std::endl;
	auto planes = supportPlanes();
	auto contoursIndices = getContoursIndices(items);
	printColouredIntersection(planes, contoursIndices,
			"naive-coloured-by-contours.ply");
	int numTriplets = 0;
	std::vector<std::set<int>> clusters;
	std::vector<std::set<int>> triplets;
	char *depthString = getenv("DEPTH");
	int depth = 1;
	if (depthString)
		depth = atoi(depthString);

	int numContours = contoursIndices.size();
	char *heightString = getenv("HEIGHT");
	double height = 1e-1;
	if (heightString)
		height = strtod(heightString, NULL);
	auto directions = supportDirections<Point_3>();
	for (int iContour = 0; iContour < numContours; ++iContour)
	{
		for (int i = 0; i < depth; ++i)
		{
			int iContourPrev = (numContours + iContour - i - 1)
				% numContours;
			for (int j = 0; j < depth; ++j)
			{
				int iContourNext = (numContours + iContour + j
						+ 1)
					% numContours;
				auto tripletsPortion = getTriplets(
						directions, height,
						contoursIndices[iContourPrev],
						contoursIndices[iContour],
						contoursIndices[iContourNext]);
				for (auto triplet: tripletsPortion)
					triplets.push_back(triplet);
			}
		}
	}
	int iTriplet = 0;
	for (auto triplet: triplets)
	{
		std::cerr << "Processing triplet " << iTriplet++
			<< " of " << triplets.size() << "        " << '\r';
		int rank = calculateRank(threshold, planes, triplet);
		if (rank == 2)
		{
			++numTriplets;
			addTripletToClusters(threshold,	clusters, triplet,
						planes);
		}
	}
	std::cerr << std::endl;
	std::cerr << "Found " << numTriplets << " degenerate triplets."
		<< std::endl;
	std::cerr << "Found " << clusters.size() << " clusters" << std::endl;
	printColouredIntersection(planes, clusters,
			"naive-coloured-by-edge-clusters.ply");
	DEBUG_END;
}

std::ostream &operator<<(std::ostream &stream, SupportFunctionDataPtr data)
{
	DEBUG_START;
	stream << "ply" << std::endl;
	stream << "format ascii 1.0" << std::endl;
	stream << "comment generated by Polyhedra Correction Library"
		<< std::endl;
	stream << "element vertex " << 4 * data->size() << std::endl;
	stream << "property float x" << std::endl;
	stream << "property float y" << std::endl;
	stream << "property float z" << std::endl;
	stream << "element face " << data->size() << std::endl;
	stream << "property list uchar uint vertex_indices" << std::endl;
	stream << "property uchar red" << std::endl;
	stream << "property uchar green" << std::endl;
	stream << "property uchar blue" << std::endl;
	stream << "end_header" << std::endl;
	std::vector<Point_3> points;
	std::vector<Colour> colours(data->size());
	for (int i = 0; i < (int) data->size(); ++i)
	{
		auto item = (*data)[i];
		auto info = item.info;
		auto normal = info->normalShadow;
		normal = normal * (0.01 / sqrt(normal.squared_length()));
		auto segment = info->segment.target() - info->segment.source();
		auto vector = CGAL::cross_product(normal, segment);
		vector = vector * (128. / sqrt(vector.squared_length()));
		colours[i].red = std::floor(vector.x()) + 128;
		colours[i].green = std::floor(vector.y()) + 128;
		colours[i].blue = std::floor(vector.z()) + 128;
		points.push_back(info->segment.source() - normal);
		points.push_back(info->segment.source() + normal);
		points.push_back(info->segment.target() + normal);
		points.push_back(info->segment.target() - normal);
	}
	double scale = 1e+6;
	for (Point_3 point: points)
	{
		stream << (int) std::floor(scale * point.x()) << " "
			<< (int) std::floor(scale * point.y()) << " "
			<< (int) std::floor(scale * point.z()) << std::endl;
	}
	for (int i = 0; i < (int) data->size(); ++i)
	{
		stream << "4";
		Colour colour = colours[i];
		for (int j = 0; j < 4; ++j)
			stream << " " << 4 * i + j;
		stream << " " << static_cast<int>(colour.red)
			<< " " << static_cast<int>(colour.green)
			<< " " << static_cast<int>(colour.blue) << std::endl;
	}
	DEBUG_END;
	return stream;
}
