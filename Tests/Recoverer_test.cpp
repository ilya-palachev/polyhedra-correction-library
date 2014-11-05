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

#include <getopt.h>
#include <cstring>
#include <sys/time.h>

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
 * Option "-b" enables balancing of contour data before its processing.
 */
#define OPTION_BALANCE_DATA 'b'

/**
 * Option "-l" controls the absolute limit of random shift of modeled contour
 * points.
 */
#define OPTION_LIMIT_RANDOM 'l'

/**
 * Option "-o" is used to control output file name(s). The default behaviour is
 * to use the name of input file as prefix.
 */
#define OPTION_OUTPUT_NAME 'o'

/**
 * Option "-r" runs recovering.
 */
#define OPTION_RECOVER 'r'

/**
 * Option "-s" enables matrix scaling.
 */
#define OPTION_SCALE_MATRIX 's'

/**
 * Option "-t" sets the type of support matrix.
 */
#define OPTION_SUPPORT_MATRIX_TYPE 't'

/**
 * Option "-v" enables verbose mode.
 */
#define OPTION_VERBOSE 'v'

/**
 * Option "-x" enables the convexification of shadow contous.
 */
#define OPTION_CONVEXIFY_CONTOURS 'x'

/** Getopt returns '?' in case of parsing error (missing argument). */
#define GETOPT_QUESTION '?'

/**
 * Definition of the option set for recoverer test.
 */
#define RECOVERER_OPTIONS_GETOPT_DESCRIPTION "f:m:n:a:bl:o:r:st:vx"

/**
 * The definition of corresponding long options std::list.
 */
static struct option optionsLong[] =
{
	{
		"file-name",
		required_argument,
		0,
		OPTION_FILE_NAME
	},
	{
		"model-name",
		required_argument,
		0,
		OPTION_MODEL_NAME
	},
	{
		"contours-number",
		required_argument,
		0,
		OPTION_CONTOURS_NUMBER
	},
	{
		"first-angle",
		required_argument,
		0,
		OPTION_FIRST_ANGLE
	},
	{
		"balance-data",
		no_argument,
		0,
		OPTION_BALANCE_DATA
	},
	{
		"limit-random",
		required_argument,
		0,
		OPTION_LIMIT_RANDOM
	},
	{
		"output-name",
		required_argument,
		0,
		OPTION_OUTPUT_NAME
	},
	{
		"recover",
		required_argument,
		0,
		OPTION_RECOVER
	},
	{
		"scale-matrix",
		no_argument,
		0,
		OPTION_SCALE_MATRIX
	},
	{
		"support-matrix-type",
		required_argument,
		0,
		OPTION_SUPPORT_MATRIX_TYPE
	},
	{
		"verbose",
		no_argument,
		0,
		OPTION_VERBOSE
	},
	{
		"convexify-contours",
		no_argument,
		0,
		OPTION_CONVEXIFY_CONTOURS
	},
	{0, 0, 0, 0}
};


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
			double limitRandom;		/**< The limit of random shift */
		} model;
	} input;

	/** The name of output file(s). */
	char *outputName;

	/** Whether to balance contour data before processing. */
	bool ifBalancing;

	/** Whether the recovering mode is enabled. */
	bool ifRecover;
	
	/** The type of estimator that will be used for recovering. */
	RecovererEstimatorType estimator;

	/** Whether to scale the matrix of problem. */
	bool ifScaleMatrix;

	/** The type of support matrix. */
	SupportMatrixType supportMatrixType;

	/** Whether to produce output data. */
	bool ifVerbose;

	/** Whether to convexify contours. */
	bool ifConvexifyContours;

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

/** Structure describing given estimator. */
typedef struct
{
	RecovererEstimatorType id;	/**< The ID of estimator */
	const char *name;		/**< Name */
	const char *description;	/**< Description */
} RecovererEstimatorTypeDescription;

RecovererEstimatorTypeDescription estimatorDescriptions[] =
{
	{
		ZERO_ESTIMATOR,
		"zero",
		"Estimator that does nothing"
	},
#ifdef USE_TSNNLS
	{
		TSNNLS_ESTIMATOR,
		"tsnnls",
		"TSNNLS nonnegative least squares solver (not working)"
	},
#endif /* USE_TSNNLS */
#ifdef USE_IPOPT
	{
		IPOPT_ESTIMATOR,
		"ipopt",
		"Ipopt interior-point method for quadratic programming"
	},
	{
		IPOPT_ESTIMATOR_LINEAR,
		"ipopt-lp",
		"Ipopt interior-point method for linear programming"
	},
#endif /* USE_IPOPT */
	{
		CGAL_ESTIMATOR,
		"cgal",
		"CGAL solver of quadratic programming problems"
	},
	{
		CGAL_ESTIMATOR_LINEAR,
		"cgal-lp",
		"CGAL solver of linear programming problems"
	}
};

/** Structure describing given support matrix type. */
typedef struct
{
	SupportMatrixType id;	/**< Support matric type ID. */
	const char *name;		/**< Name */
	const char *description;	/**< Description */
} SupportMatrixTypeDescription;

SupportMatrixTypeDescription supportMatrixTypeDescriptions[] =
{
	{
		SUPPORT_MATRIX_TYPE_KKVW,
		"kkvw",
		"Karl-Kulkarni-Verhese-Willsky conditions"
	},
	{
		SUPPORT_MATRIX_TYPE_KKVW_OPT,
		"kkvw-opt",
		"Optimized Karl-Kulkarni-Verhese-Willsky conditions"
	},
	{
		SUPPORT_MATRIX_TYPE_GK,
		"gk",
		"Gardner-Kiderlen conditions"
	},
	{
		SUPPORT_MATRIX_TYPE_GK_OPT,
		"gk-opt",
		"Optimized Gardner-Kiderlen conditions"
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
	int i = 0;
	STDERR_PRINT("\t-%c --%s\tThe name of file with shadow contours to be "
		"processed\n", OPTION_FILE_NAME, optionsLong[i++].name);
	STDERR_PRINT("\t-%c --%s\tThe name of synthetic model to be tested on.\n",
		OPTION_MODEL_NAME, optionsLong[i++].name);
	STDERR_PRINT("\t-%c --%s\tThe number of contours to be generated from the "
		"synthetic model\n", OPTION_CONTOURS_NUMBER, optionsLong[i++].name);
	STDERR_PRINT("\t-%c --%s\tThe fist angle from which the first shadow contour is "
		"obtained\n", OPTION_FIRST_ANGLE, optionsLong[i++].name);
	STDERR_PRINT("\t-%c --%s\tBalance contour data before processing.\n",
		OPTION_BALANCE_DATA, optionsLong[i++].name);
	STDERR_PRINT("\t-%c --%s\tThe absolute limit of random shift of modeled "
		"contour.\n", OPTION_LIMIT_RANDOM, optionsLong[i++].name);
	STDERR_PRINT("\t-%c --%s\tThe name of output file(s).\n",
		OPTION_OUTPUT_NAME, optionsLong[i++].name);
	STDERR_PRINT("\t-%c --%s\tRecover polyhedron using some estimator.\n",
		OPTION_RECOVER, optionsLong[i++].name);
	STDERR_PRINT("\t-%c --%s\tEnable matrix scaling.\n",
		OPTION_SCALE_MATRIX, optionsLong[i++].name);
	STDERR_PRINT("\t-%c --%s\tSet type of support matrix.\n",
		OPTION_SUPPORT_MATRIX_TYPE, optionsLong[i++].name);
	STDERR_PRINT("\t-%c --%s\tEnable verbose mode.\n",
		OPTION_VERBOSE, optionsLong[i++].name);
	STDERR_PRINT("\t-%c --%s\tEnable contours convexification.\n",
		OPTION_CONVEXIFY_CONTOURS, optionsLong[i++].name);
	STDERR_PRINT("\nPossible synthetic models are:\n");
	for (int iModel = 0; iModel < RECOVERER_TEST_MODELS_NUMBER; ++iModel)
	{
		RecovererTestModel *model = &recovererTestModels[iModel];
		STDERR_PRINT("\t%d.  \"%s\"\t- %s\n", model->id, model->name,
			model->description);
	}
	STDERR_PRINT("\nPossible estimators are:\n");
	int numEstimators = sizeof(estimatorDescriptions)
		/ sizeof(RecovererEstimatorTypeDescription);
	DEBUG_PRINT("Number of estimators: %d", numEstimators);
	for (int iEstimator = 0; iEstimator < numEstimators;
		++iEstimator)
	{
		RecovererEstimatorTypeDescription *desc =
			&estimatorDescriptions[iEstimator];
		STDERR_PRINT("\t%d. \"%s\"\t - %s\n", desc->id, desc->name,
			desc->description);
	}

	STDERR_PRINT("\nPossible types of support matrix are:\n");
	int numSupportMatrixTypes = sizeof(supportMatrixTypeDescriptions)
		/ sizeof(SupportMatrixTypeDescription);
	DEBUG_PRINT("Number of support matrix types: %d", numSupportMatrixTypes);
	for (int iType = 0; iType < numSupportMatrixTypes; ++iType)
	{
		SupportMatrixTypeDescription *desc =
			&supportMatrixTypeDescriptions[iType];
		STDERR_PRINT("\t%d. \"%s\"\t - %s", desc->id, desc->name,
			desc->description);
		if (desc->id == DEFAULT_SUPPORT_MATRIX_TYPE)
			STDERR_PRINT(" (default)");
		STDERR_PRINT("\n");
	}
	DEBUG_END;
}

static void errorOptionTwice(int argc, char** argv, char option)
{
	STDERR_PRINT("Option \"-%c\" cannot be specified more than one"
		" time.\n", option);
	printUsage(argc, argv);
	exit(EXIT_FAILURE);
}

static void errorCannotParseNumber(int argc, char** argv, char* strBuggy)
{
	STDERR_PRINT("Cannot parse number. Invalid character %s\n",
		strBuggy);
	printUsage(argc, argv);
	exit(EXIT_FAILURE);
}

static void errorOutOfRange(int argc, char** argv)
{
	STDERR_PRINT("Given number is out of range\n");
		printUsage(argc, argv);
	exit(EXIT_FAILURE);
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
	bool ifOptionLimitRandom = false;
	bool ifOptionRecover = false;
	bool ifOptionSupportMatrixType = false;
	long int charCurr;
	char *modelName = NULL;
	opterr = 0;

	int numEstimators = sizeof(estimatorDescriptions)
		/ sizeof(RecovererEstimatorTypeDescription);
	DEBUG_PRINT("Number of estimators: %d", numEstimators);

	int numSupportMatrixTypes = sizeof(supportMatrixTypeDescriptions)
		/ sizeof(SupportMatrixTypeDescription);
	DEBUG_PRINT("Number of support matrix types: %d", numSupportMatrixTypes);

	/*
	 * Iterate command line arguments using standard Libc function getopt.
	 * See http://www.gnu.org/software/libc/manual/html_node/Getopt.html
	 * for details.
	 */
	options->ifBalancing = false;
	options->ifRecover = false;
	options->ifVerbose = false;
	options->ifConvexifyContours = false;
	options->ifScaleMatrix = false;
	options->outputName = NULL;
	int optionIndex = 0;
	while ((charCurr = getopt_long(argc, argv,
		RECOVERER_OPTIONS_GETOPT_DESCRIPTION, optionsLong,
		&optionIndex)) != GETOPT_FAILURE)
	{
		char *charMistaken = NULL;

		switch (charCurr)
		{
		case 0:
			STDERR_PRINT("Impossible happened!");
			printUsage(argc, argv);
			DEBUG_END;
			exit(EXIT_FAILURE);
			break;
		case OPTION_FILE_NAME:
			if (ifOptionFileName)
			{
				errorOptionTwice(argc, argv, OPTION_FILE_NAME);
			}

			options->mode = RECOVERER_REAL_TESTING;
			options->input.fileName = optarg;
			ifOptionFileName = true;
			break;
		case OPTION_MODEL_NAME:
			if (ifOptionModelName)
			{
				errorOptionTwice(argc, argv, OPTION_MODEL_NAME);
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
			modelName = strdup(optarg);
			break;
		case OPTION_CONTOURS_NUMBER:
			if (ifOptionNumContours)
			{
				errorOptionTwice(argc, argv, OPTION_CONTOURS_NUMBER);
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
				errorCannotParseNumber(argc, argv, charMistaken);
			}

			/* In case of underflow or overflow errno is set to ERANGE. */
			if (errno == ERANGE)
			{
				errorOutOfRange(argc, argv);
			}

			ifOptionNumContours = true;
			break;
		case OPTION_FIRST_ANGLE:
			if (ifOptionFirstAngle)
			{
				errorOptionTwice(argc, argv, OPTION_FIRST_ANGLE);
			}
			/* Parse floating-point number from input argument. */
			options->input.model.shiftAngleFirst =
					strtod(optarg, &charMistaken);

			/* If user gives invalid character, the charMistaken is set to it */
			if (charMistaken && *charMistaken)
			{
				errorCannotParseNumber(argc, argv, charMistaken);
			}
			
			/* In case of underflow or overflow errno is set to ERANGE. */
			if (errno == ERANGE)
			{
				errorOutOfRange(argc, argv);
			}

			ifOptionFirstAngle = true;
			break;
		case OPTION_BALANCE_DATA:
			options->ifBalancing = true;
			break;
		case OPTION_LIMIT_RANDOM:
			if (ifOptionLimitRandom)
			{
				errorOptionTwice(argc, argv, OPTION_LIMIT_RANDOM);
			}
			/* Parse floating-point number from input argument. */
			options->input.model.limitRandom =
					strtod(optarg, &charMistaken);

			/* If user gives invalid character, the charMistaken is set to it */
			if (charMistaken && *charMistaken)
			{
				errorCannotParseNumber(argc, argv, charMistaken);
			}

			/* In case of underflow or overflow errno is set to ERANGE. */
			if (errno == ERANGE)
			{
				errorOutOfRange(argc, argv) ;
			}

			ifOptionLimitRandom = true;
			break;
		case OPTION_OUTPUT_NAME:
			if (options->outputName)
			{
				errorOptionTwice(argc, argv, OPTION_OUTPUT_NAME);
			}
			options->outputName = optarg;
			break;
		case OPTION_RECOVER:
			if (ifOptionRecover)
			{
				errorOptionTwice(argc, argv, OPTION_RECOVER);
			}

			options->ifRecover = true;
			for (int iEstimator = 0; iEstimator < numEstimators;
				++iEstimator)
			{
				if (strcmp(optarg, estimatorDescriptions[iEstimator].name) == 0)
				{
					options->estimator = (RecovererEstimatorType) iEstimator;
					ifOptionRecover = true;
					break;
				}
			}
			if (!ifOptionRecover)
			{
				STDERR_PRINT("Invalid name of estimator - %s\n", optarg);
				printUsage(argc, argv);
				DEBUG_END;
				exit(EXIT_FAILURE);
			}
			break;
		case OPTION_SCALE_MATRIX:
			options->ifScaleMatrix = true;
			break;
		case OPTION_SUPPORT_MATRIX_TYPE:
			if (ifOptionSupportMatrixType)
			{
				errorOptionTwice(argc, argv,
					OPTION_SUPPORT_MATRIX_TYPE);
			}

			for (int iType = 0; iType < numSupportMatrixTypes;
				++iType)
			{
				if (!strcmp(optarg,
					supportMatrixTypeDescriptions[
						iType].name))
				{
					options->supportMatrixType =
						supportMatrixTypeDescriptions[
							iType].id;
					ifOptionSupportMatrixType = true;
					break;
				}
			}
			if (!ifOptionSupportMatrixType)
			{
				STDERR_PRINT("Invalid name of support matrix "
					"type - %s\n", optarg);
				printUsage(argc, argv);
				DEBUG_END;
				exit(EXIT_FAILURE);
			}
			break;
		case OPTION_VERBOSE:
			options->ifVerbose = true;
			break;
		case OPTION_CONVEXIFY_CONTOURS:
			options->ifConvexifyContours = true;
			break;
		case GETOPT_QUESTION:
			STDERR_PRINT("Option \"-%c\" requires an argument "
				" (or there is unknown error...).\n", optopt);
			printUsage(argc, argv);
			DEBUG_END;
			exit(EXIT_FAILURE);
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

	/*
	 * Check that all 4 options "-m", "-n", "-a" and "-l" are given
	 * simultaneously.
	 */
	if (!(ifOptionModelName && ifOptionNumContours &&
		ifOptionFirstAngle && ifOptionLimitRandom) &&
		(ifOptionModelName || ifOptionNumContours ||
		ifOptionFirstAngle || ifOptionLimitRandom))
	{
		STDERR_PRINT("Option \"-%c\" requires options \"-%c\","
			"\"-%c\" and \"-%c\" to be specified, "
			"and vice versa.\n",
			OPTION_MODEL_NAME, OPTION_CONTOURS_NUMBER,
			OPTION_FIRST_ANGLE, OPTION_LIMIT_RANDOM);
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

	if (!options->outputName)
	{
		if (ifOptionFileName)
		{
			options->outputName = strdup(options->input.fileName);
		}
		else
		{
			ASSERT(modelName);
			options->outputName = strdup(modelName);
		}
	}

	if (!ifOptionSupportMatrixType)
	{
		options->supportMatrixType = DEFAULT_SUPPORT_MATRIX_TYPE;
	}

	if (modelName)
		free(modelName);

	DEBUG_END;
	return options;
}

/**
 * Created a polyhedron of the specified type.
 *
 * @param id	The type of the polyhedron
 */
Polyhedron *makePolyhedron(RecovererTestModelID id)
{
	DEBUG_START;
	switch (id)
	{
	case MODEL_CUBE:
	{
		Polyhedron *p = new Cube(1., 0., 0., 0.);
		DEBUG_END;
		return p;
	}
		break;
	case MODEL_PYRAMID:
	{
		Polyhedron *p = new Pyramid(3, 1., 1.);
		DEBUG_END;
		return p;
	}
		break;
	case MODEL_PRISM:
	{
		Polyhedron *p = new Prism(3, 1., 1.);
		DEBUG_END;
		return p;
	}
		break;
	case MODEL_CUBE_CUTTED:
	{
		Polyhedron *p = new CubeCutted();
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
ShadowContourDataPtr getRealSCData(CommandLineOptions* options)
{
	DEBUG_START;

	/* Create fake empty polyhedron. */
	PolyhedronPtr p(new Polyhedron());

	/* Create shadow contour data and associate p with it. */
	ShadowContourDataPtr SCData(new ShadowContourData(p));

	/* Read shadow contours data from file. */
	SCData->fscanDefault(options->input.fileName);

	DEBUG_END;
	return SCData;
}

/**
 * Generates random number d such that |d| <= maxDelta
 *
 * @param maxDelta	maximum absolute limit of generated number
 */
static double genRandomDouble(double maxDelta)
{
	DEBUG_START;
	//srand((unsigned) time(0));
	struct timeval t1;
	gettimeofday(&t1, NULL);
	srand(t1.tv_usec * t1.tv_sec);
	
	int randomInteger = rand();
	double randomDouble = randomInteger;
	const double halfRandMax = RAND_MAX * 0.5;
	randomDouble -= halfRandMax;
	randomDouble /= halfRandMax;

	randomDouble *= maxDelta;

	DEBUG_END;
	return randomDouble;
}

/**
 * Shifts all points of contours on random double std::vectors
 *
 * @param data		Shadow contours data
 * @param maxDelta	maximum delta in shift std::vectors' coordinates
 */
void shiftContoursRandom(ShadowContourDataPtr data, double maxDelta)
{
	DEBUG_START;
	for (int iContour = 0; iContour < data->numContours; ++iContour)
	{
		SContour* contour = &data->contours[iContour];
		for (int iSide = 0; iSide < contour->ns; ++iSide)
		{
			SideOfContour* side = &contour->sides[iSide];
			Vector3d A1_backup DEBUG_VARIABLE = side->A1;
			side->A1 += Vector3d(genRandomDouble(maxDelta),
				genRandomDouble(maxDelta),
				genRandomDouble(maxDelta));
			DEBUG_PRINT("shift: (%lf, %lf, %lf) -> "
				"(%lf, %lf, %lf)",
				A1_backup.x, A1_backup.y, A1_backup.z,
				side->A1.x, side->A1.y, side->A1.z);
			side->A1 = contour->plane.project(side->A1);
		}
		for (int iSide = 0; iSide < contour->ns - 1; ++iSide)
		{
			contour->sides[iSide].A2 = contour->sides[iSide + 1].A1;
		}
		contour->sides[contour->ns - 1].A2 = contour->sides[0].A1;
	}
	DEBUG_END;
}

/**
 * Generates synthetic shadow contour data.
 *
 * @param options	The result of command-line parsing.
 */
ShadowContourDataPtr generateSyntheticSCData(CommandLineOptions *options)
{
	DEBUG_START;

	/* Create polyhedron based on one of possible models. */
	PolyhedronPtr p(makePolyhedron(options->input.model.id));
	Polyhedron *pCopy = makePolyhedron(options->input.model.id);

	/* Set the pointer to the parent polyhedron in facets. */
	p->set_parent_polyhedron_in_facets();
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
		".original-polyhedron.ply") << *pCopy;

	/* Create shadow contour data and associate p with it. */
	ShadowContourDataPtr SCData(new ShadowContourData(p));

	/*
	 * Create shadow contour constructor and associate p and SCData with
	 * them.
	 */
	ShadowContourConstructorPtr constructor(new ShadowContourConstructor(p,
			SCData));

	/* Generate shadow contours data for given model. */
	constructor->run(options->input.model.numContours,
			options->input.model.shiftAngleFirst);
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
		".original-unshifted-contours.dat") << SCData;
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
		".original-unshifted-contours.ply") << Polyhedron(SCData);

	/* Shift points contours on random std::vectors. */
	shiftContoursRandom(SCData, options->input.model.limitRandom);

	DEBUG_END;
	return SCData;
}

/**
 * Makes shadow contour data requested by user: synthetic or real, and dumps it
 * if it has been requested
 *
 * @param options	Parsed command-line options
 * @param recoverer	The used recoverer
 */
static ShadowContourDataPtr makeRequestedData(CommandLineOptions* options)
{
	DEBUG_START;
	ShadowContourDataPtr SCData;

	if (options->mode == RECOVERER_REAL_TESTING)
	{
		SCData = getRealSCData(options);
	}
	else if (options->mode == RECOVERER_SYNTHETIC_TESTING)
	{
		SCData = generateSyntheticSCData(options);
	}
	DEBUG_END;
	return SCData;
}

/**
 * Makes recoverer with requested properties.
 *
 * @param options	Parsed command-line options
 */
static RecovererPtr makeRequestedRecoverer(CommandLineOptions* options)
{
	DEBUG_START;
	RecovererPtr recoverer(new Recoverer());

	/* Set the name of output file. */
	recoverer->setOutputName(options->outputName);

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

	/* Enable contours convexification if required. */
	if (options->ifConvexifyContours)
	{
		recoverer->enableContoursConvexification();
	}

	/* Set support matrix type. */
	recoverer->setSupportMatrixType(options->supportMatrixType);
	DEBUG_END;
	return recoverer;
}

/**
 * Runs the recovering with requested mode.
 *
 * @param options	Parsed command-line options
 * @param recoverer	The recoverer to be used
 * @param data		Shadow contours data
 */
void runRequestedRecovery(CommandLineOptions* options,
	RecovererPtr recoverer, ShadowContourDataPtr data)
{
	DEBUG_START;
	/* In verbose mode we dump all output. */
	if (options->ifVerbose)
	{
		globalPCLDumper.enableVerboseMode();
	}

	if (options->ifRecover)
		recoverer->setEstimatorType(options->estimator);
	else
		recoverer->setEstimatorType(ZERO_ESTIMATOR);	

	/* Run the recoverer. */
	PolyhedronPtr p = recoverer->run(data);
	globalPCLDumper(PCL_DUMPER_LEVEL_OUTPUT, ".recovered.ply") << *p;
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

	/* Parse command line arguments. */
	CommandLineOptions *options = parseCommandLine(argc, argv);

	/* Create the recoverer with requested properties. */
	RecovererPtr recoverer = makeRequestedRecoverer(options);
	
	/* Read or generate data depending on requested option. */
	ShadowContourDataPtr data = makeRequestedData(options);

	/* Run the recovery. */
	runRequestedRecovery(options, recoverer, data);

	DEBUG_END;
	return EXIT_SUCCESS;
}
