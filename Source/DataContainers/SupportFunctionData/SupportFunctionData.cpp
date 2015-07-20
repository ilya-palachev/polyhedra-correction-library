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

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "DataContainers/SupportFunctionData/SupportFunctionData.h"
#include "Analyzers/SizeCalculator/SizeCalculator.h"

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
	ASSERT(epsilons.size() == size());
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
