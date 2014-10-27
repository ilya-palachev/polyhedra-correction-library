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
 * @file PCLPoint_2.h
 * @brief Hand-made class for the replacement of usual CGAL Point_2 class
 */

#ifndef PCLPOINT_2_H
#define PCLPOINT_2_H

#include <CGAL/Origin.h>
#include <CGAL/Bbox_2.h>

class PCLPoint_2
{
private:
	double vec_[2];
	int id_;

public:
	PCLPoint_2()
		: id_(0)
	{
		vec_[0] = 0.;
		vec_[1] = 0.;
	}

	PCLPoint_2(const double x, const double y, int id = 0)
		: id_(id)
	{
		vec_[0] = x;
		vec_[1] = y;
	}

	const double& x() const {
		return vec_[0];
	}

	const double& y() const {
		return vec_[1];
	}

	double& x() {
		return vec_[0];
	}

	double& y() {
		return vec_[1];
	}

	int id() const {
		return id_;
	}

	int& id() {
		return id_;
	}

	bool operator==(const PCLPoint_2 &p) const
	{
		return (vec_[0] == p.vec_[0]) &&
		       (vec_[1] == p.vec_[1]) &&
		       (id_ == p.id_);
	}

	bool operator!=(const PCLPoint_2 &p) const
	{
		return !(*this == p);
	}
};

#endif /* PCLPOINT_2_H */
