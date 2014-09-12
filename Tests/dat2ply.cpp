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
 * @file dat2ply.cpp
 * @brief Convert dat polyhedron format to PLY format (with the same name)
 */

#include "DebugPrint.h"
#include "Polyhedron/Polyhedron.h"
#include "Recoverer/Recoverer.h"

#define NUM_ARGS 2
int main (int argc, char **argv)
{
	if (argc != NUM_ARGS)
	{
		ERROR_PRINT("Usage: %s <file name>", argv[0]);
	}

	char *name = makeNameWithSuffix(argv[1], ".ply");
	PolyhedronPtr p;
	p->fscan_default_1_1(argv[1]);
	p->fprint_ply_autoscale(DEFAULT_MAX_COORDINATE,
                name, "dat2ply");

	free(name);
}
