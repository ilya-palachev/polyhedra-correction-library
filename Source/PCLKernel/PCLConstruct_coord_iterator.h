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
 * @file PCLConstruct_coord_iterator.h
 * @brief Implementation of iterator needed for Point_2 extension.
 */

#ifndef PCLCONSTRUCT_COORD_ITERATOR_H
#define PCLCONSTRUCT_COORD_ITERATOR_H

class PCLConstruct_coord_iterator
{
public:
	const double *operator()(const PCLPoint_2 &p)
	{
		return &p.x();
	}

	const double *operator()(const PCLPoint_2 &p, int)
	{
		const double *pyptr = &p.y();
		pyptr++;
		return pyptr;
	}
};

#endif /* PCLCONSTRUCT_COORD_ITERATOR_H */
