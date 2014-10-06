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
 * @file SupportItemSet.h
 * @brief The declaration of support item set structure.
 */

#ifndef SUPPORT_ITEM_SET_H
#define SUPPORT_ITEM_SET_H

#include "Vector3d.h"

/** Item of support function measurement. */
struct SupportItem
{
	Vector3d u; /**< the direction. */
	double h;   /**< the support value. */
	SupportItem() : u(Vector3d(0., 0., 0.)), h(0) {}
	SupportItem(Vector3d u_, double h_) : u(u_), h(h_) {}
};

auto supportItem_comparer = [](SupportItem a, SupportItem b)
{
	return a.u < b.u;
};

typedef std::set<SupportItem, decltype(supportItem_comparer)> SupportItemSet;

#endif /* SUPPORT_ITEM_SET_H */
