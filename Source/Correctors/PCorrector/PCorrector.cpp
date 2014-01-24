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

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Correctors/PCorrector/PCorrector.h"

PCorrector::PCorrector() :
				polyhedron(NULL)
{
	DEBUG_START;
	DEBUG_END;
}

PCorrector::PCorrector(shared_ptr<Polyhedron> p) :
				polyhedron(p)
{
	DEBUG_START;
	DEBUG_END;
}

PCorrector::PCorrector(Polyhedron* p) :
				polyhedron()
{
	DEBUG_START;
	polyhedron.reset(p);
	DEBUG_END;
}

PCorrector::~PCorrector()
{
	DEBUG_START;
	DEBUG_END;
}

