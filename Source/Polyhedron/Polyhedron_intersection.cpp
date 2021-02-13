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

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Polyhedron/Polyhedron.h"
#include "Correctors/Intersector/Intersector.h"

void Polyhedron::intersect(Plane iplane)
{
	DEBUG_START;
	Intersector *intersector = new Intersector(get_ptr());
	intersector->run(iplane);
	delete intersector;
	DEBUG_END;
}

void Polyhedron::intersectCoalesceMode(Plane iplane, int jfid)
{
	DEBUG_START;
	Intersector *intersector = new Intersector(get_ptr());
	intersector->runCoalesceMode(iplane, jfid);
	delete intersector;
	DEBUG_END;
}
