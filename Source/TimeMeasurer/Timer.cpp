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

#include <sys/time.h>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Timer.h"

Timer::Timer()
{
	DEBUG_START;
	seconds_ = 0;
	useconds_ = 0;
	DEBUG_END;
}

Timer::Timer(const Timer& other)
{
	DEBUG_START;
	seconds_ = other.seconds_;
	useconds_ = other.useconds_;
	DEBUG_END;

}

Timer::~Timer()
{
	DEBUG_START;
	DEBUG_END;
}

Timer& Timer::operator=(const Timer& other)
{
	DEBUG_START;
	seconds_ = other.seconds_;
	useconds_ = other.useconds_;
	DEBUG_END;
	return *this;
}

bool Timer::operator==(const Timer& other) const
{
	return seconds_ == other.seconds_ && useconds_ == other.useconds_;
}

bool Timer::operator!=(const Timer& other) const
{
	return seconds_ != other.seconds_ || useconds_ != other.useconds_;
}

long int Timer::getSeconds()
{
	return seconds_;
}

long int Timer::getUSeconds()
{
	return useconds_;
}

void Timer::setSeconds(long int seconds)
{
	DEBUG_START;
	seconds_ = seconds;
	DEBUG_END;	
}

void Timer::setUSeconds(long int useconds)
{
	DEBUG_START;
	useconds_ = useconds;
	DEBUG_END;	
}

void Timer::start()
{
	DEBUG_START;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	seconds_ = tv.tv_sec;
	useconds_ = tv.tv_usec;
	DEBUG_END;
}
