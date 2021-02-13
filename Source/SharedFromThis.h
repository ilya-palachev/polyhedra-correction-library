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
 * @file SharedFromThis.h
 * @brief Class for enabling "shared from this" feature.
 */

#ifndef SHAREDFROMTHIS_H_
#define SHAREDFROMTHIS_H_

/**
 * Implementation of class needed for working of "shared from this" feature.
 *
 * Workaround to enable opportunity of use of shared_from_this in derived
 * classes of Polyhedron, such as Cube.
 *
 * For details, see question number
 * 9374610/bad-weak-ptr-when-calling-shared-from-this-in-base-class
 * on http://stackoverflow.com/questions/
 */
template <class Derived> class enable_shared_from_This
{
public:
	/**
	 * Member that stores a pointer to object
	 */
	typedef shared_ptr<Derived> Ptr;

	/**
	 * Wrapper around shared_from_this
	 */
	Ptr shared_from_This()
	{
		return static_pointer_cast<Derived>(
			static_cast<Derived *>(this)->shared_from_this());
	}

	/**
	 * Wrapper around shared_from_this
	 */
	Ptr shared_from_This() const
	{
		return static_pointer_cast<Derived>(
			static_cast<Derived *>(this)->shared_from_this());
	}
};

#endif /* SHAREDFROMTHIS_H_ */
