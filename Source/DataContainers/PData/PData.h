/*
 * Copyright (c) 2009-2015 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file PData.h
 * @brief Abstract polyhedral data.
 */

#ifndef PDATA_H_
#define PDATA_H_

#include <memory>

#include "Polyhedron/Polyhedron.h"

/** Abstract class for general polyhedral data. */
class PData
{
public:
	/**
	 * The polyhedron pointer the data is associated with.
	 *
	 * TODO: Polyhedral data in practice can be associated with no polyhedron.
	 * So we need not have a pointer to parent polyhedron here.
	 */
	PolyhedronPtr polyhedron;

	/** Empty constructor. */
	PData() : polyhedron(NULL) {}

	/**
	 * Constructor by pointer to polyhedron.
	 *
	 * @param p	The pointer to polyhedron.
	 */
	PData(PolyhedronPtr p) {}

	/** The destructor. */
	virtual ~PData() {}
};

#endif /* PDATA_H_ */
