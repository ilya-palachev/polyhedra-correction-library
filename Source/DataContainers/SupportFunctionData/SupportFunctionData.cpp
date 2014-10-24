/*
 * Copyright (c) 2009-2014 Ilya Palachev <iliyapalachev@gmail.com>
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
	std::copy(data.items.begin(), data.items.end(), items.begin());
	DEBUG_END;
}

SupportFunctionData::SupportFunctionData(const SupportFunctionData *data) :
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

SupportFunctionData &SupportFunctionData::operator= (const SupportFunctionData
		&data)
{
	DEBUG_START;
	if (!items.empty())
	{
		items.clear();
	}
	std::copy(data.items.begin(), data.items.end(), items.begin());
	DEBUG_END;
	return *this;
}

SupportFunctionDataItem &SupportFunctionData::operator[] (const int iPosition)
{
	DEBUG_START;
	ASSERT(iPosition >= 0);
	ASSERT(items.size() > (unsigned) iPosition);
	DEBUG_END;
	return items[iPosition];
}

SupportFunctionDataPtr SupportFunctionData::removeEqual()
{
	DEBUG_START;
	std::vector<SupportFunctionDataItem> itemsUnequal;

	for (auto item = items.begin(); item != items.end(); ++item)
	{
		bool ifEqual = false;
		for (auto itemPrev = itemsUnequal.begin();
			itemPrev != itemsUnequal.end(); ++itemPrev)
		{
			if (equal(item->direction, itemPrev->direction,
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
