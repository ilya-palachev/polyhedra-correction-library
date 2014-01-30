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

#include "PolyhedraCorrectionLibrary.h"

#define NUM_ARGS_EXPECTED 2

void printUsage(void)
{
	DEBUG_START;
	printf("Usage:\n");
	printf("./Recoverer_test <input file>\n");
	DEBUG_END;
}

int main(int argc, char** argv)
{
	DEBUG_START;
	if (argc != NUM_ARGS_EXPECTED)
	{
		ERROR_PRINT("Wrong arguments provided to program.");
		printUsage();
		DEBUG_END;
		return EXIT_FAILURE;
	}
	
	/* TODO: Here should be a good command-line parser function. */

	PolyhedronPtr p(new Polyhedron());
	ShadeContourDataPtr SCData(new ShadeContourData(p));
	SCData->fscanDefault(argv[1]);
	RecovererPtr recoverer(new Recoverer());
	recoverer->buildNaivePolyhedron(SCData);
	
	DEBUG_END;
	return EXIT_SUCCESS;
}