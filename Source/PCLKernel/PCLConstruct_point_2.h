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
 * @file PCLConstruct_point_2.h
 * @brief Implementation of functor which construct points.
 */

#ifndef PCLCPNSTRUCT_POINT_2_H
#define PCLCPNSTRUCT_POINT_2_H

template <typename K, typename OldK> class PCLConstruct_point_2
{
	typedef typename K::RT RT;
	typedef typename K::Point_2 Point_2;
	typedef typename K::Line_2 Line_2;
	typedef typename Point_2::Rep Rep;

public:
	typedef Point_2 result_type;

	/*
	 * (from the example)
	 * Note: the CGAL:Return_base_tag si really internal CGAL stuff.
	 * Unfortunately it is needed for optimizing away copy-constructions,
	 * due to current lack of delegating constructors in the C++ standard.
	 */

	Rep /* Point_2 */
	operator()(CGAL::Return_base_tag, CGAL::Origin o) const
	{
		return Rep(o);
	}

	Rep /* Point_2 */
	operator()(CGAL::Return_base_tag, const RT &x, const RT &y) const
	{
		return Rep(x, y);
	}

	Rep /* Point_2 */
	operator()(CGAL::Return_base_tag, const RT &x, const RT &y,
			   const RT &w) const
	{
		return Rep(x, y, w);
	}

	Point_2 operator()(const CGAL::Origin &) const
	{
		return PCLPoint_2(0, 0, 0);
	}

	Point_2 operator()(const RT &x, const RT &y) const
	{
		return PCLPoint_2(x, y, 0);
	}

	Point_2 operator()(const Line_2 &l) const
	{
		typename OldK::Construct_point_2 base_operator;
		Point_2 p = base_operator(l);
		return p;
	}

	Point_2 operator()(const Line_2 &l, int i) const
	{
		typename OldK::Construct_point_2 base_operator;
		Point_2 p = base_operator(l, i);
		return p;
	}

	Point_2 operator()(const RT &x, const RT &y, const RT &w) const
	{
		if (w != 1)
		{
			return PCLPoint_2(x / w, y / w, 0);
		}
		else
		{
			return PCLPoint_2(x, y, 0);
		}
	}
};

#endif /* PCLCPNSTRUCT_POINT_2_H */
