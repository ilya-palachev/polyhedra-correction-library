/* 
 * Copyright (c) 2009-2013 Ilya Palachev <iliyapalachev@gmail.com>
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

#ifndef PCORRECTOR_H_
#define PCORRECTOR_H_

#include <memory>

#include "Polyhedron/Polyhedron.h"

class PCorrector
{
public:
	shared_ptr<Polyhedron> polyhedron;

	PCorrector();
	PCorrector(shared_ptr<Polyhedron> p);
	PCorrector(Polyhedron* p);
	virtual ~PCorrector();
};

#endif /* PCORRECTOR_H_ */
