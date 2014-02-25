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
 * @file Recoverer_test.cpp
 * @brief Unit tests for Recoverer of the polyhedron.
 */

#include <unistd.h>
#include <errno.h>
#include <cstring>

#include "Constants.h"
#include "PolyhedraCorrectionLibrary.h"

/** The name of the test. */
#define TEST_NAME			"./Recoverer_test"

/** The number of command line arguments expected. */
#define NUM_ARGS_EXPECTED	2

/** Option "-f" takes the argument with file name. */
#define OPTION_FILE_NAME 'f'

/** Option "-m" takes the argument with synthetic model name */
#define OPTION_MODEL_NAME 'm'

/*
 * Option "-n" takes the argument with the number of generated contours (for
 * synthetic testing mode).
 */
#define OPTION_CONTOURS_NUMBER 'n'

/**
 * Option "-a" takes the angle from which the 1st shadow contour is generated.
 */
#define OPTION_FIRST_ANGLE 'a'

/**
 * Option "-d" forces recoverer to avoid recovering and just visualize
 * dual non-convex polyhedron.
 */
#define OPTION_DUAL_NONCONVEX_POLYHEDRON 'd'

/**
 * Option "-c" forces recoverer to build only a polyhedron consisting of
 * facets constructed from shadow contours.
 */
#define OPTION_CONTOURS 'c'

/**
 * Option "-b" enables balancing of contour data before its processing.
 */
#define OPTION_BALANCE_DATA 'b'

/**
 * Option "-p" enable the mode of printing the problem, i.e. the support vector
 * and the matrix of conditions.
 */
#define OPTION_PRINT_PROBLEM 'p'

/**
 * Option "-r" runs recovering.
 */
#define OPTION_RECOVER 'r'

/**
 * Option "-s" enables matrix scaling.
 */
#define OPTION_SCALE_MATRIX 's'

/** Getopt returns '?' in case of parsing error (missing argument). */
#define GETOPT_QUESTION '?'

/**
 * Definition of the option set for recoverer test.
 */
#define RECOVERER_OPTIONS_GETOPT_DESCRIPTION "f:m:n:a:bcdprs"


/** Error return value of getopt function. */
#define GETOPT_FAILURE -1

/**
 * Unumeration that describes possible synthetic models that can be used in
 * recoverer testing.
 */
enum RecovererTestModelID
{
	MODEL_CUBE = 0,			/**< The cube */
	MODEL_PYRAMID = 1,		/**< The pyramid */
	MODEL_PRISM = 2,		/**< The prism */
	MODEL_CUBE_CUTTED = 3	/**< The cube with a whole cutted out from it */
};

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
	union
	{
		/** File name with existent shadow contour data. */
		char *fileName;

		/** Parameters of synthetic geometric figure. */
		struct
		{
			RecovererTestModelID id;	/**< The ID of model */
			int numContours;			/**< The number of generated contours */
			double shiftAngleFirst;		/**< The value of first angle */
		} model;
	} input;

	/** Whether to build dual non-convex polyhedron. */
	bool ifBuildDualNonConvexPolyhedron;

	/** Whether to build polyhedron consisting of contours. */
	bool ifBuildContours;

	/** Whether to balance contour data before processing. */
	bool ifBalancing;

	/** Whether the mode of problem printing is enabled. */
	bool ifPrintProblem;

	/** Whether the recovering mode is enabled. */
	bool ifRecover;

	/** Whether to scale the matrix of problem. */
	bool ifScaleMatrix;
} CommandLineOptions;

/** The number of possible test models. */
#define RECOVERER_TEST_MODELS_NUMBER	4

/** Structure describing given test model. */
typedef struct
{
	RecovererTestModelID id;	/**< The ID of model */
	const char *name;			/**< Name */
	const char *description;	/**< Description */
} RecovererTestModel;

RecovererTestModel recovererTestModels[] =
{
	{
		MODEL_CUBE,
		"cube",
		"Simple cube with coordinates of points +1 / -1."
	},
	{
		MODEL_PYRAMID,
		"pyramid",
		"Triangled pyramid with ideal triangle in bottom."
	},
	{
		MODEL_PRISM,
		"prism",
		"Triangled prism with ideal triangles in top and bottom."
	},
	{
		MODEL_CUBE_CUTTED,
		"cube_cutted",
		"The cube with a whole cutted out from it"
	}
};

/**
 * Prints the usage of the program.
 *
 * @param argc	Standard Linux argc
 * @param argv	Standard Linux argv
 */
void printUsage(int argc, char** argv)
{
	DEBUG_START;
	STDERR_PRINT("%s - Unit tests for Recoverer of the polyhedron.\n\n",
		TEST_NAME);
	STDERR_PRINT("Usage:\n");
	STDERR_PRINT("%s [-%c <input file name>] [-%c <model name> -%c <number of "
		"contours>]\n", TEST_NAME, OPTION_FILE_NAME, OPTION_MODEL_NAME,
		OPTION_CONTOURS_NUMBER);
	STDERR_PRINT("Options:\n");
	STDERR_PRINT("\t-%c\tThe name of file with shadow contours to be "
		"processed\n", OPTION_FILE_NAME);
	STDERR_PRINT("\t-%c\tThe name of synthetic model to be tested on.\n",
		OPTION_MODEL_NAME);
	STDERR_PRINT("\t-%c\tThe number of contours to be generated from the "
		"synthetic model\n", OPTION_CONTOURS_NUMBER);
	STDERR_PRINT("\t-%c\tThe fist angle from which the first shadow contour is "
		"obtained\n", OPTION_FIRST_ANGLE);
	STDERR_PRINT("\t-%c\tBuild only dual non-convex polyhedron.\n",
		OPTION_DUAL_NONCONVEX_POLYHEDRON);
	STDERR_PRINT("\t-%c\tBuild polyhedron consisting of shadow contours.\n",
		OPTION_CONTOURS);
	STDERR_PRINT("\t-%c\tBalance contour data before processing.\n",
			OPTION_BALANCE_DATA);
	STDERR_PRINT("\t-%c\tPrint problem mode (print matrix and hvalues vector "
			"to the file).", OPTION_PRINT_PROBLEM);
	STDERR_PRINT("\t-%c\tRecover polyhedron.", OPTION_RECOVER);
	STDERR_PRINT("\t-%c\tEnable matrix scaling.", OPTION_SCALE_MATRIX);
	STDERR_PRINT("Possible synthetic models are:\n");
	for (int iModel = 0; iModel < RECOVERER_TEST_MODELS_NUMBER; ++iModel)
	{
		RecovererTestModel *model = &recovererTestModels[iModel];
		STDERR_PRINT("\t%d.  \"%s\"\t- %s\n", model->id, model->name,
			model->description);
	}
	DEBUG_END;
}

/**
 * Parses command line string to obtain options from it.
 * 
 * @param argc	Standard Linux argc
 * @param argv	Standard Linux argv
 *
 * TODO: We need to implement some common option parser (like in busybox)
 * to avoid such huge tons of code in test implementations.
 */
CommandLineOptions* parseCommandLine(int argc, char** argv)
{
	DEBUG_START;
	CommandLineOptions* options = new CommandLineOptions();
	bool ifOptionFileName = false;
	bool ifOptionModelName = false;
	bool ifOptionNumContours = false;
	bool ifOptionFirstAngle = false;
	long int charCurr;
	opterr = 0;

	/*
	 * Iterate command line arguments using standard Libc function getopt.
	 * See http://www.gnu.org/software/libc/manual/html_node/Getopt.html
	 * for details.
	 */
	options->ifBuildDualNonConvexPolyhedron = false;
	options->ifBuildContours = false;
	options->ifBalancing = false;
	options->ifPrintProblem = false;
	options->ifRecover = false;
	options->ifScaleMatrix = false;
	while ((charCurr = getopt(argc, argv, RECOVERER_OPTIONS_GETOPT_DESCRIPTION))
		!= GETOPT_FAILURE)
	{
		char *charMistaken = NULL;

		switch (charCurr)
		{
		case OPTION_FILE_NAME:
			if (ifOptionFileName)
			{
				STDERR_PRINT("Option \"-%c\" cannot be specified more than one"
					" time.\n", OPTION_FILE_NAME);
				printUsage(argc, argv);
				DEBUG_END;
				exit(EXIT_FAILURE);
			}

			options->mode = RECOVERER_REAL_TESTING;
			options->input.fileName = optarg;
			ifOptionFileName = true;
			break;
		case OPTION_MODEL_NAME:
			if (ifOptionModelName)
			{
				STDERR_PRINT("Option \"-%c\" cannot be specified more than one"
					" time.\n", OPTION_MODEL_NAME);
				printUsage(argc, argv);
				DEBUG_END;
				exit(EXIT_FAILURE);
			}

			options->mode = RECOVERER_SYNTHETIC_TESTING;
			for (int iModel = 0; iModel < RECOVERER_TEST_MODELS_NUMBER;
				++iModel)
			{
				if (strcmp(optarg, recovererTestModels[iModel].name) == 0)
				{
					options->input.model.id = (RecovererTestModelID) iModel;
					ifOptionModelName = true;
					break;
				}
			}
			if (!ifOptionModelName)
			{
				STDERR_PRINT("Invalid name of model - %s\n", optarg);
				printUsage(argc, argv);
				DEBUG_END;
				exit(EXIT_FAILURE);
			}
			break;
		case OPTION_CONTOURS_NUMBER:
			if (ifOptionNumContours)
			{
				STDERR_PRINT("Option \"-%c\" cannot be specified more than one"
					" time.\n", OPTION_CONTOURS_NUMBER);
				printUsage(argc, argv);
				DEBUG_END;
				exit(EXIT_FAILURE);
			}

			/* Parse digital number from input argument. */
			options->input.model.numContours =
					strtol(optarg, &charMistaken, 10);

			/* If user gives invalid character, the charMistaken is set to it */
			/*
			 * From "man strtol":
			 *
			 * In particular, if *nptr is not '\0' but **endptr is '\0' on
			 * return, the entire string is valid.
			 */
			if (charMistaken && *charMistaken)
			{
				STDERR_PRINT("Cannot parse number. Invalid character %s\n",
					charMistaken);
				printUsage(argc, argv);
				DEBUG_END;
				exit(EXIT_FAILURE);
			}

			/* In case of underflow or overflow errno is set to ERANGE. */
			if (errno == ERANGE)
			{
				STDERR_PRINT("Given number of contours is out of range\n");
				printUsage(argc, argv);
				DEBUG_END;
				exit(EXIT_FAILURE);
			}

			ifOptionNumContours = true;
			break;
		case OPTION_FIRST_ANGLE:
			if (ifOptionFirstAngle)
			{
				STDERR_PRINT("Option \"-%c\" cannot be specified more than one"
					"time.\n", OPTION_FIRST_ANGLE);
				printUsage(argc, argv);
				DEBUG_END;
				exit(EXIT_FAILURE);
			}
			/* Parse floating-point number from input argument. */
			options->input.model.shiftAngleFirst =
					strtod(optarg, &charMistaken);

			/* If user gives invalid character, the charMistaken is set to it */
			if (charMistaken && *charMistaken)
			{
				STDERR_PRINT("Cannot parse number. Invalid character %s\n",
					charMistaken);
				printUsage(argc, argv);
				DEBUG_END;
				exit(EXIT_FAILURE);
			}

			/* In case of underflow or overflow errno is set to ERANGE. */
			if (errno == ERANGE)
			{
				STDERR_PRINT("Given value of angle is out of range\n");
				printUsage(argc, argv);
				DEBUG_END;
				exit(EXIT_FAILURE);
			}

			ifOptionFirstAngle = true;
			break;
		case OPTION_DUAL_NONCONVEX_POLYHEDRON:
			options->ifBuildDualNonConvexPolyhedron = true;
			break;
		case OPTION_CONTOURS:
			options->ifBuildContours = true;
			break;
		case OPTION_BALANCE_DATA:
			options->ifBalancing = true;
			break;
		case OPTION_PRINT_PROBLEM:
			options->ifPrintProblem = true;
			break;
		case OPTION_RECOVER:
			options->ifRecover = true;
			break;
		case OPTION_SCALE_MATRIX:
			options->ifScaleMatrix = true;
			break;
		case GETOPT_QUESTION:
			switch (optopt)
			{
			case OPTION_FILE_NAME:
				STDERR_PRINT("Option \"-%c\" requires an argument - the name of"
					" input file.\n", OPTION_FILE_NAME);
				printUsage(argc, argv);
				DEBUG_END;
				exit(EXIT_FAILURE);
				break;
			case OPTION_MODEL_NAME:
				STDERR_PRINT("Option \"-%c\" requires an argument - the name of"
					" synthetic model to be tested.\n", OPTION_MODEL_NAME);
				printUsage(argc, argv);
				DEBUG_END;
				exit(EXIT_FAILURE);
				break;
			case OPTION_CONTOURS_NUMBER:
				STDERR_PRINT("Option \"-%c\" requires an argument - the number "
					"contour generated from original model (in synthetic "
					"testing mode)\n", OPTION_CONTOURS_NUMBER);
				printUsage(argc, argv);
				DEBUG_END;
				exit(EXIT_FAILURE);
				break;
			case OPTION_FIRST_ANGLE:
				STDERR_PRINT("Option \"-%c\" requires an argument -- the value "
					"of the first angle from which the first shadow contour is "
					"generated.\n", OPTION_FIRST_ANGLE);
				printUsage(argc, argv);
				DEBUG_END;
				exit(EXIT_FAILURE);
				break;
			default:
				STDERR_PRINT("Unknown option \"-%c\"\n", optopt);
				printUsage(argc, argv);
				DEBUG_END;
				exit(EXIT_FAILURE);
				break;
			}
			break;
		default:
			STDERR_PRINT("Failed to parse command line.\n");
			printUsage(argc, argv);
			DEBUG_END;
			exit(EXIT_FAILURE);
			break;
		}
	}

	/* Check that options "-f" and "-m" are not given at the same time. */
	if (ifOptionFileName && ifOptionModelName)
	{
		STDERR_PRINT("Options \"-%c\" and \"-%c\" cannot be given"
			" simultaneously.\n", OPTION_FILE_NAME, OPTION_MODEL_NAME);
		printUsage(argc, argv);
		DEBUG_END;
		exit(EXIT_FAILURE);
	}

	/* Check that option "-n" is provided for option "-m". */
	if ((ifOptionModelName && (!ifOptionNumContours || !ifOptionFirstAngle))
		|| (!ifOptionModelName && ifOptionNumContours))
	{
		STDERR_PRINT("Option \"-%c\" requires options \"-%c\" and \"-%c\" to be"
			" specified.\n", OPTION_MODEL_NAME, OPTION_CONTOURS_NUMBER,
			OPTION_FIRST_ANGLE);
		printUsage(argc, argv);
		DEBUG_END;
		exit(EXIT_FAILURE);
	}

	if (!ifOptionFileName && !ifOptionModelName)
	{
		STDERR_PRINT("At least one of options \"-%c\" and \"-%c\" must be "
			"specified.\n", OPTION_FILE_NAME, OPTION_MODEL_NAME);
		printUsage(argc, argv);
		DEBUG_END;
		exit(EXIT_FAILURE);
	}

	if (optind != argc)
	{
		STDERR_PRINT("Non-option arguments are not required!\n");
		printUsage(argc, argv);
		DEBUG_END;
		exit(EXIT_FAILURE);
	}

	DEBUG_END;
	return options;
}

/**
 * Created a polyhedron of the specified type.
 *
 * @param id	The type of the polyhedron
 */
PolyhedronPtr makePolyhedron(RecovererTestModelID id)
{
	DEBUG_START;
	switch (id)
	{
	case MODEL_CUBE:
	{
		PolyhedronPtr p = make_shared<Cube>(1., 0., 0., 0.);
		DEBUG_END;
		return p;
	}
		break;
	case MODEL_PYRAMID:
	{
		PolyhedronPtr p = make_shared<Pyramid>(3, 1., 1.);
		DEBUG_END;
		return p;
	}
		break;
	case MODEL_PRISM:
	{
		PolyhedronPtr p = make_shared<Prism>(3, 1., 1.);
		DEBUG_END;
		return p;
	}
		break;
	case MODEL_CUBE_CUTTED:
	{
		PolyhedronPtr p = make_shared<CubeCutted>();
		DEBUG_END;
		return p;
	}
		break;
	default:
	{
		ASSERT(0 && "Impossible happened.");
		break;
	}
	}
	DEBUG_END;
	return NULL;
}

/**
 * Gets real shadow contour data from file.
 *
 * @param options	The result of command-line parsing.
 */
ShadeContourDataPtr getRealSCData(CommandLineOptions* options)
{
	DEBUG_START;

	/* Create fake empty polyhedron. */
	PolyhedronPtr p(new Polyhedron());

	/* Create shadow contour data and associate p with it. */
	ShadeContourDataPtr SCData(new ShadeContourData(p));

	/* Read shadow contours data from file. */
	SCData->fscanDefault(options->input.fileName);

	DEBUG_END;
	return SCData;
}

/**
 * Generates synthetic shadow contour data.
 *
 * @param options	The result of command-line parsing.
 */
ShadeContourDataPtr generateSyntheticSCData(CommandLineOptions *options)
{
	DEBUG_START;

	/* Create polyhedron based on one of possible models. */
	PolyhedronPtr p(makePolyhedron(options->input.model.id));

	/* Set the pointer to the parent polyhedron in facets. */
	p->set_parent_polyhedron_in_facets();

	/* Create shadow contour data and associate p with it. */
	ShadeContourDataPtr SCData(new ShadeContourData(p));

	/*
	 * Create shadow contour constructor and associate p and SCData with
	 * them.
	 */
	ShadeContourConstructorPtr constructor(new ShadeContourConstructor(p,
			SCData));

	/* Generate shadow contours data for given model. */
	constructor->run(options->input.model.numContours,
			options->input.model.shiftAngleFirst);

	DEBUG_END;
	return SCData;
}

/**
 * Prints the problems to files (for debugging purposes).
 *
 * @param SCData	Shadow contours data.
 * @param recoverer	The recoverer to be used.
 */
static void buildNaiveMatrix(ShadeContourDataPtr SCData, RecovererPtr recoverer)
{
	DEBUG_START;
	int numConditions = 0, numHvalues = 0;
	double *matrix = NULL, *hvalues = NULL;

	recoverer->buildNaiveMatrix(SCData, numConditions, matrix, numHvalues,
			hvalues);

	FILE* fileMatrixNaive = (FILE*) fopen("../poly-data-out/matrix-naive.txt",
			"w");
	FILE* fileHvalues = (FILE*) fopen("../poly-data-out/hvalues-naive.txt",
			"w");

	if (!fileMatrixNaive || !fileHvalues)
	{
		ERROR_PRINT("Failed to open file.");
	}

	for (int iCondition = 0; iCondition < numConditions; ++iCondition)
	{
		double constraint = 0.;
		double constraint0 = 0.;
		fprintf (fileMatrixNaive, "%d ", iCondition);
		for (int iCol = 0; iCol < numHvalues; ++iCol)
		{
			double value = matrix[iCondition * numHvalues + iCol];
			if (fabs(value) > EPS_MIN_DOUBLE)
			{
				fprintf(fileMatrixNaive, "%d %.16lf ", iCol, value);
				constraint += value * hvalues[iCol];
				constraint0 += value;
			}
		}
		fprintf(fileMatrixNaive, "\n");
		DEBUG_PRINT("constraint[%d] = %.16lf, constraint0[%d] = %.16lf",
				iCondition, constraint, iCondition, constraint0);
	}

	for (int iCol = 0; iCol < numHvalues; ++iCol)
	{
		fprintf(fileHvalues, "%lf\n", hvalues[iCol]);
	}

	fclose(fileMatrixNaive);
	fclose(fileHvalues);
	if (matrix)
	{
		delete[] matrix;
	}
	if (hvalues)
	{
		delete[] hvalues;
	}

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

	/*
	 * Parse command line arguments. This function exits the program in case of
	 * failure during the parsing.
	 */
	CommandLineOptions *options = parseCommandLine(argc, argv);

	ShadeContourDataPtr SCData;

	if (options->mode == RECOVERER_REAL_TESTING)
	{
		SCData = getRealSCData(options);
	}
	else if (options->mode == RECOVERER_SYNTHETIC_TESTING)
	{
		SCData = generateSyntheticSCData(options);
	}

	/* Create the recoverer.*/
	RecovererPtr recoverer(new Recoverer());

	/* Enable balancing if required. */
	if (options->ifBalancing)
	{
		recoverer->enableBalancing();
	}

	/* Enable matrix scaling if required. */
	if (options->ifScaleMatrix)
	{
		recoverer->enableMatrixScaling();
	}

	if (options->ifRecover)
	{
		/* Run the recoverer. */
		recoverer->run(SCData);
	}
	else if (options->ifPrintProblem)
	{
		/* Just print naive matrix and vector of hvalues. */
		buildNaiveMatrix(SCData, recoverer);
	}
	else if (options->ifBuildContours &&
			!options->ifBuildDualNonConvexPolyhedron)
	{
		/* Buid polyhedron consisting of shadow contours. */
		recoverer->buildContours(SCData);
	}
	else if (!options->ifBuildContours &&
			options->ifBuildDualNonConvexPolyhedron)
	{
		/* Just build dual non-convex polyhedron. */
		recoverer->buildDualNonConvexPolyhedron(SCData);
	}
	else if (options->ifBuildContours &&
			options->ifBuildDualNonConvexPolyhedron)
	{
		/* Buid polyhedron consisting of dual shadow contours. */
		recoverer->buildDualContours(SCData);
	}
	else
	{
		/* Run naive recovering. */
		recoverer->buildNaivePolyhedron(SCData);
	}

	DEBUG_END;
	return EXIT_SUCCESS;
}
