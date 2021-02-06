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
 * @file HalfedgeIndexed.h
 * @brief Extension of CGAL halfedfe with added index.
 */

#ifndef HALFEDGEINDEXED_H_
#define HALFEDGEINDEXED_H_

#include <CGAL/HalfedgeDS_halfedge_base.h>

/** A halfedge type with an ID member variable. */
template <class Refs>
class HalfedgeIndexed : public CGAL::HalfedgeDS_halfedge_base<Refs>
{
public:
	long int id;
};

#endif /* HALFEDGEINDEXED_H_ */
