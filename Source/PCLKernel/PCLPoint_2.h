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
	double m_vec[2];
	int m_id;

public:
	PCLPoint_2()
		: m_id(0)
	{
		m_vec[0] = 0.;
		m_vec[1] = 0.;
	}

	PCLPoint_2(const double x, const double y, int id = 0)
		: m_id(id)
	{
		m_vec[0] = x;
		m_vec[1] = y;
	}

	const double& x() const {
		return m_vec[0];
	}

	const double& y() const {
		return m_vec[1];
	}

	double& x() {
		return m_vec[0];
	}

	double& y() {
		return m_vec[1];
	}

	int id() const {
		return m_id;
	}

	int& id() {
		return m_id;
	}

	bool operator==(const PCLPoint_2 &p) const
	{
		return (m_vec[0] == p.m_vec[0]) &&
		       (m_vec[1] == p.m_vec[1]) &&
		       (m_id == p.m_id);
	}

	bool operator!=(const PCLPoint_2 &p) const
	{
		return !(*this == p);
	}
};

#endif /* PCLPOINT_2_H */
