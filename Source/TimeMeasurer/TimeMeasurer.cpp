/*
 * Copyright (c) 2009-2013 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file TimeMeasurer.cpp
 * @brief time measurer is a class used for time measurement inside tests
 * (implementation).
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "TimeMeasurer.h"

/**
 * Creates empty time measurer
 */
TimeMeasurer::TimeMeasurer()
{
	DEBUG_START;
	DEBUG_END;
}


/**
 * Copies exisiting time measurer to existing one
 * 
 * @param other time measurer to be copied.
 */
TimeMeasurer::TimeMeasurer(const TimeMeasurer& other)
{
	DEBUG_START;
	timers = other.timers;
	DEBUG_END;
}


/**
 * Frees time measurer.
 */
TimeMeasurer::~TimeMeasurer()
{
	DEBUG_START;
	timers.clear();
	DEBUG_END;
}

/**
 * Assigns one time measurer to another.
 * 
 * @param other the measurer to be assigned to
 */
TimeMeasurer& TimeMeasurer::operator=(const TimeMeasurer& other)
{	
	DEBUG_START;
	timers = other.timers;
	DEBUG_END;
	return *this;
}

/**
 * Compares two TimeMeasurers (equality).
 * 
 * @param other timer measurer to be compared with
 * 
 * @retval true if equal
 * @retval false if unequal
 */
bool TimeMeasurer::operator==(const TimeMeasurer& other) const
{
	DEBUG_START;
	
	auto it_timer1 = timers.begin();
	auto it_timer2 = other.timers.begin();
	
	while (it_timer1 != timers.end() && it_timer2 != other.timers.end())
	{
		if (*it_timer1 != *it_timer2)
		{
			DEBUG_END;
			return false;
		}
	}
	
	DEBUG_END;
	return true;
}

/**
 * Compares two TimeMeasurers (unequality).
 *
 * @param other timer measurer to be compared with
 *
 * @retval true if equal
 * @retval false if unequal
 */
bool TimeMeasurer::operator!=(const TimeMeasurer& other) const
{
	DEBUG_START;
	DEBUG_END;
	return !(*this == other);
}
