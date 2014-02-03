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

#include <unistd.h>

#include "PolyhedraCorrectionLibrary.h"

#define NUM_ARGS_EXPECTED 2

/**
 * Mode of recoverer testing.
 */
enum ModeOfRecovererTesting
{
	/** Test on real shadow contour data written to file. */
	RECOVERER_REAL_TESTING,

	/** Test on synthetic model based shadow contour data. */
	RECOVERER_SYNTHETIC_TESTING
};

/**
 * The result of command line option parsing.
 */
typedef struct
{
	/** Mode of recoverer testing. */
	ModeOfRecovererTesting mode;

	/** The string with intput data location. */
	union input
	{
		/** File name with existent shadow contour data. */
		char *fileName;

		/** The name of synthetic geometric figure. */
		char *figureName;
	};
} CommandLineOptions;

/**
 * Parses command line string to obtain options from it.
 * 
 * @param argc	Standard Linux argc
 * @param argv	Standard Linux argv
 */
CommandLineOptions* parseCommandLine(int argc, char** argv)
{
	DEBUG_START;
	CommandLineOptions* options = new CommandLineOptions();
	DEBUG_END;
}

/**
 * Prints the usage of the program.
 */
void printUsage(void)
{
	DEBUG_START;
	printf("Usage:\n");
	printf("./Recoverer_test <input file>\n");
	DEBUG_END;
}

/**
 * The main function of the test.
 * 
 * @param argc	Standard Linux argc
 * @param argv	Standard Linux argv
 */
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