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

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Polyhedron/Polyhedron.h"
#include "Correctors/Coalescer/Coalescer.h"

#define EPS_PARALL 1e-16
#define MAX_MIN_DIST 1e+1

void Polyhedron::coalesceFacets(int fid0, int fid1)
{
	DEBUG_START;
	Coalescer* coalescer = new Coalescer(get_ptr());
	coalescer->run(fid0, fid1);
	delete coalescer;
	DEBUG_END;
}

void Polyhedron::coalesceFacets(int n, int* fid)
{
	DEBUG_START;
	Coalescer* coalescer = new Coalescer(get_ptr());
	coalescer->run(n, fid);
	delete coalescer;
	DEBUG_END;
}
