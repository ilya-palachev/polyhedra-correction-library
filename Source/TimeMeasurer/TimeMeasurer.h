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
 * @file TimeMeasurer.h
 * @brief time measurer is a class used for time measurement inside tests
 * (implementation).
 */


#ifndef TIMEMEASURER_H
#define TIMEMEASURER_H

#include <stack>
#include <sys/time.h>

#include "Timer.h"

using namespace std;

/**
 * time measurer is a data structure of stack of timeval structures
 * When new timer is pushed, it is placed to the stack, and its value will be
 * printed when it will be poped out from stack.
 */
class TimeMeasurer
{
private:

	/**
	 * Stack of started timers
	 */
	stack<Timer> timers;

public:

	/**
	* Creates empty time measurer
	*/
	TimeMeasurer();

	/**
	* Copies exisiting time measurer to existing one
	* 
	* @param other time measurer to be copied.
	*/	
	TimeMeasurer(const TimeMeasurer& other);

	/**
	* Frees time measurer.
	*/
	~TimeMeasurer();

	/**
	* Assigns one time measurer to another.
	* 
	* @param other the measurer to be assigned to
	*/	
	TimeMeasurer& operator=(const TimeMeasurer& other);

	/**
	* Compares two TimeMeasurers.
	* 
	* @param other timer measurer to be compared with
	* 
	* @retval true if equal
	* @retval false if unequal
	*/	
	bool operator==(const TimeMeasurer& other) const;


	void push_timer();


	void pop_timer();
};

#endif // TIMEMEASURER_H
