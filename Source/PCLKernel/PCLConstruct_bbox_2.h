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
 * @file PCLConstruct_bbox_2.h
 * @brief Implementation of functionr for bounding box computation -
 * to make basic algorithms work with PCLPoint_2
 */

#ifndef PCLCONSTRUCT_BBOX_2
#define PCLCONSTRUCT_BBOX_2

template <class ConstructBbox_2> class PCLConstruct_bbox_2 : public ConstructBbox_2
{
public:
	using ConstructBbox_2::operator();

	CGAL::Bbox_2 operator()(const PCLPoint_2 &p) const
	{
		return CGAL::Bbox_2(p.x(), p.y(), p.x(), p.y());
	}
};

#endif /* PCLCONSTRUCT_BBOX_2 */
