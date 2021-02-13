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
 * @file PCLKernel.h
 * @brief The implementation of PCl 2d kernel
 */

#ifndef PCLKERNEL_H
#define PCLKERNEL_H

#include "PCLKernel/PCLConstruct_bbox_2.h"
#include "PCLKernel/PCLConstruct_coord_iterator.h"
#include "PCLKernel/PCLConstruct_point_2.h"
#include "PCLKernel/PCLPoint_2.h"
#include "PCLKernel/PCLSegment_2.h"
#include <CGAL/Cartesian.h>

/* K_ is the new kernel, and K_base is the old kernel. */
template <typename K_, typename K_Base>
class PCLCartesian_base : public K_Base::template Base<K_>::Type
{
	typedef typename K_Base::template Base<K_>::Type OldK;

public:
	typedef K_ Kernel;
	typedef PCLPoint_2 Point_2;
	typedef PCLSegment_2<Kernel> Segment_2;
	typedef PCLConstruct_point_2<Kernel, OldK> Construct_point_2;
	typedef const double *Cartesian_const_iterator_2;
	typedef PCLConstruct_coord_iterator Construct_cartesian_const_iterator_2;
	typedef PCLConstruct_bbox_2<typename OldK::Construct_bbox_2>
		Construct_bbox_2;

	Construct_point_2 construct_point_2_object() const
	{
		return Construct_point_2();
	}

	Construct_bbox_2 construct_bbox_2_object() const
	{
		return Construct_bbox_2();
	}

	Construct_cartesian_const_iterator_2
	construct_cartesian_const_iterator_object() const
	{
		return Construct_cartesian_const_iterator_2();
	}

	template <typename Kernel2> struct Base
	{
		typedef PCLCartesian_base<Kernel2, K_Base> Type;
	};
};

template <typename FT_>
struct PCLKernel : public CGAL::Type_equality_wrapper<
					   PCLCartesian_base<PCLKernel<FT_>, CGAL::Cartesian<FT_>>,
					   PCLKernel<FT_>>
{
};
#endif /* PCLKERNEL_H */
