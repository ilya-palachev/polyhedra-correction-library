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

#ifndef TIMEMEASURER_H
#define TIMEMEASURER_H

#include <stack>
#include <sys/time.h>

using namespace std;

class TimeMeasurer
{
private:
	stack<struct timeval> timers;
public:
	TimeMeasurer();
	TimeMeasurer(const TimeMeasurer& other);
	virtual ~TimeMeasurer();
	virtual TimeMeasurer& operator=(const TimeMeasurer& other);
	virtual bool operator==(const TimeMeasurer& other) const;
	void push_timer();
	void pop_timer();
};

#endif // TIMEMEASURER_H
