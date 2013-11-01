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

#ifndef TIMER_H
#define TIMER_H

class Timer
{
private:
	long int seconds_;
	long int useconds_;
public:
	Timer();
	Timer(const Timer& other);
	virtual ~Timer();
	Timer& operator=(const Timer& other);
	bool operator==(const Timer& other) const;
	bool operator!=(const Timer& other) const;
	
	long int getSeconds();
	long int getUSeconds();
	
	void setSeconds(long int seconds);
	void setUSeconds(long int useconds);
	
	void start();
};

#endif // TIMER_H
