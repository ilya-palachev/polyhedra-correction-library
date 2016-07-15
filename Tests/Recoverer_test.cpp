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
 * @file Recoverer_test.cpp
 * @brief Unit tests for Recoverer of the polyhedron.
 */

#include <getopt.h>
#include <cstring>
#include <sys/time.h>

#include "Constants.h"
#include "PolyhedraCorrectionLibrary.h"

/**
 * Option "-a" takes the angle from which the 1st shadow contour is generated.
 */
#define OPTION_FIRST_ANGLE 'a'

/** Option "-A" enable axial testing. */
#define OPTION_AXIAL_TESTING 'A'

/**
 * Option "-b" enables balancing of contour data before its processing.
 */
#define OPTION_BALANCE_DATA 'b'

/**
 * Option "-c" is used to specify the name of file that contains the polyhedron
 * that was constructed with some third-party software.
 */
#define OPTION_COMPARE_WITH_FILE 'c'

/**
 * Option "-d" sets the name of file with support directions.
 */
#define OPTION_FILE_DIRECTIONS 'd'

/**
 * Option "-e" sets the epsilon factor for reduced Gardner-Kiderlen matrix.
 */
#define OPTION_EPSILON_FACTOR 'e'

/** Option "-f" takes the argument with file name. */
#define OPTION_FILE_NAME 'f'

/**
 * Options "-i" sets the body used as starting point of the estimation process.
 */
#define OPTION_STARTING_BODY 'i'

/**
 * Option "-l" controls the absolute limit of random shift of modeled contour
 * points.
 */
#define OPTION_LIMIT_RANDOM 'l'

/** Option "-m" takes the argument with synthetic model name */
#define OPTION_MODEL_NAME 'm'

/** Option "-M" sets the name of file with model. */
#define OPTION_FILE_MODEL 'M'

/**
 * Option "-n" takes the argument with the number of generated contours (for
 * synthetic testing mode).
 */
#define OPTION_CONTOURS_NUMBER 'n'

/**
 * Option "-N" takes the argument with the maximum number of countours get from
 * the given file with shadow contours.
 */
#define OPTION_MAX_CONTOUR_NUMBER 'N'

/**
 * Option "-o" is used to control output file name(s). The default behaviour is
 * to use the name of input file as prefix.
 */
#define OPTION_OUTPUT_NAME 'o'

/**
 * Option "-p" sets the problem type of the problem for estimators that support
 * it.
 */
#define OPTION_PROBLEM_TYPE 'p'

/**
 * Option "-r" runs recovering.
 */
#define OPTION_RECOVER 'r'

/**
 * Option "-s" enables matrix scaling.
 */
#define OPTION_SCALE_MATRIX 's'

/**
 * Option "-S" is used to specify the file name with ready support function
 * data.
 */
#define OPTION_SUPPORT_FUNCTION_DATA_FILE 'S'

/**
 * Option "-t" sets the type of support matrix.
 */
#define OPTION_SUPPORT_MATRIX_TYPE 't'

/**
 * Option "-T" sets the value of threshold.
 */
#define OPTION_THRESHOLD_ADJACENT_FACETS 'T'

/**
 * Option "-v" enables verbose mode.
 */
#define OPTION_VERBOSE 'v'

/**
 * Option "-x" enables the convexification of shadow contous.
 */
#define OPTION_CONVEXIFY_CONTOURS 'x'

/**
 * Option "-z" is controlling the Z minimal norm.
 */
#define OPTION_Z_MINIMAL_NORM 'z'

/** Getopt returns '?' in case of parsing error (missing argument). */
#define GETOPT_QUESTION '?'

/**
 * Definition of the option set for recoverer test.
 */
#define RECOVERER_OPTIONS_GETOPT_DESCRIPTION \
	"a:Abc:d:e:f:i:l:m:M:n:N:o:p:r:sS:t:T:vxz:"

struct PCLOption: public option
{
public:
	const char *comment;

	PCLOption(): option(), comment(NULL) {}

	PCLOption(option o, const char *s) :
		option(o), comment(s) {}

	~PCLOption() {}
};

/**
 * The definition of corresponding long options list.
 */
static PCLOption optionsLong[] =
{
	{
		option
		{
			"file-name",
			required_argument,
			0,
			OPTION_FILE_NAME
		},
		"The name of file with shadow contours to be processed"
	},
	{
		option
		{
			"model-name",
			required_argument,
			0,
			OPTION_MODEL_NAME
		},
		"The name of synthetic model to be tested on"
	},
	{
		option
		{
			"contours-number",
			required_argument,
			0,
			OPTION_CONTOURS_NUMBER
		},
		"The number of contours to be generated from the synthetic "
			"model"
	},
	{
		option
		{
			"max-contours-number",
			required_argument,
			0,
			OPTION_MAX_CONTOUR_NUMBER
		},
		"The maximum number of contours to be analyzed"
	},
	{
		option
		{
			"first-angle",
			required_argument,
			0,
			OPTION_FIRST_ANGLE
		},
		"The fist angle from which the first shadow contour is obtained"
	},
	{
		option
		{
			"balance-data",
			no_argument,
			0,
			OPTION_BALANCE_DATA
		},
		"Balance contour data before processing"
	},
	{
		option
		{
			"limit-random",
			required_argument,
			0,
			OPTION_LIMIT_RANDOM
		},
		"The absolute limit of random shift of modeled contour"
	},
	{
		option
		{
			"output-name",
			required_argument,
			0,
			OPTION_OUTPUT_NAME
		},
		"The name of output file(s)"
	},
	{
		option
		{
			"problem-type",
			required_argument,
			0,
			OPTION_PROBLEM_TYPE
		},
		"The type of oprimization problem (not for all estimators!)"
	},
	{
		option
		{
			"recover",
			required_argument,
			0,
			OPTION_RECOVER
		},
		"Recover polyhedron using some estimator"
	},
	{
		option
		{
			"scale-matrix",
			no_argument,
			0,
			OPTION_SCALE_MATRIX
		},
		"Enable matrix scaling"
	},
	{
		option
		{
			"support-matrix-type",
			required_argument,
			0,
			OPTION_SUPPORT_MATRIX_TYPE
		},
		"Set type of support matrix"
	},
	{
		option
		{
			"verbose",
			no_argument,
			0,
			OPTION_VERBOSE
		},
		"Enable verbose mode"
	},
	{
		option
		{
			"convexify-contours",
			no_argument,
			0,
			OPTION_CONVEXIFY_CONTOURS
		},
		"Enable contours convexification"
	},
	{
		option
		{
			"starting-body",
			required_argument,
			0,
			OPTION_STARTING_BODY
		},
		"Type of starting body of the estimation process"
	},
	{
		option
		{
			"directions",
			required_argument,
			0,
			OPTION_FILE_DIRECTIONS
		},
		"The name of file with support directions"
	},
	{
		option
		{
			"file-model",
			required_argument,
			0,
			OPTION_FILE_MODEL
		},
		"The name of file with model for synthetic data construction"
	},
	{
		option
		{
			"axial",
			no_argument,
			0,
			OPTION_AXIAL_TESTING
		},
		"Axial testing: move support values, non points"
	},
	{
		option
		{
			"epsilon-factor",
			required_argument,
			0,
			OPTION_EPSILON_FACTOR
		},
		"Epsilon factor for reduced Gardner-Kiderlen matrix"
	},
	{
		option
		{
			"compare",
			required_argument,
			0,
			OPTION_COMPARE_WITH_FILE
		},
		"The name of file that contains the polyhedron constructed "
		"with some third-party software."
	},
	{
		option
		{
			"threshold",
			required_argument,
			0,
			OPTION_THRESHOLD_ADJACENT_FACETS
		},
		"The value of threshold of angle between adjacent facets."
	},
	{
		option
		{
			"support-function-data",
			required_argument,
			0,
			OPTION_SUPPORT_FUNCTION_DATA_FILE
		},
		"Support function data file."
	},
	{
		option
		{
			"z-minimal-norm",
			required_argument,
			0,
			OPTION_Z_MINIMAL_NORM
		},
		"The Z minimal norm."
	},
	{option{0, 0, 0, 0}, 0}
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

	/** Test on syntheically generated data. */
	RECOVERER_SYNTHETIC_TESTING
};

typedef struct
{
	/** The ID of model */
	RecovererTestModelID id;

	/** The name of file with model. */
	char *fileName;
			
	/**  The number of generated contours */
	int numContours;

	/** The value of first angle */
	double shiftAngleFirst;

	/** The name of file with support directions. */
	char *directionsFileName;

	/** The limit of random shift */
	double limitRandom;

	/** Whether to do axial testing. */
	bool ifAxialTesting;

} SyntheticModelDescription;

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
		struct
		{
			/**
			 * File name with existent shadow contour data or
			 * support function data.
			 */
			char *name;

			/** Whether it is support function data or not. */
			bool ifSupportFunctionData;
		} file;

		/** Parameters of synthetically generated data. */
		SyntheticModelDescription model;
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

	/** The type of starting body of the estimation process. */
	SupportFunctionEstimationStartingBodyType startingBodyType;

	/** Maximum number of contours to be analyzed. */
	int numMaxContours;

	/** Epsilon factor for reduced Gardner-Kiderlen matrix. */
	double epsilonFactor;

	/** The problem type. */
	EstimationProblemNorm problemType;

	/** The name of file with 3rd-party constructed polyhedron. */
	char *fileNamePolyhedron;

	/** The threshold of angle between adjacent facets. */
	double threshold;

	/** The Z minimal norm. */
	double zMinimalNorm;
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

/** Structure describing starting body of the estimation process. */
typedef struct
{
	SupportFunctionEstimationStartingBodyType id;	/**< The ID of body */
	const char *name;				/**< Name */
	const char *description;			/**< Description */
} RecovererStartingBody;

RecovererStartingBody recovererStartingBodies[] =
{
	{
		SUPPORT_FUNCTION_ESTIMATION_STARTING_BODY_TYPE_CYLINDERS_INTERSECTION,
		"intersection",
		"Body is intersection of halfspaces formed by shadow cylinders."
	},
	{
		SUPPORT_FUNCTION_ESTIMATION_STARTING_BODY_TYPE_POINTS_HULL,
		"hull",
		"Body is convex hull of support points (u_i * h_i)."
	},
	{
		SUPPORT_FUNCTION_ESTIMATION_STARTING_BODY_TYPE_SPHERE,
		"sphere",
		"Body is sphere (each support value is = 1)."
	},
	{
		SUPPORT_FUNCTION_ESTIMATION_STARTING_BODY_TYPE_CUBE,
		"cube",
		"Body is cube."
	},
	{
		SUPPORT_FUNCTION_ESTIMATION_STARTING_BODY_TYPE_PYRAMID,
		"pyramid",
		"Body is pyramid."
	},
	{
		SUPPORT_FUNCTION_ESTIMATION_STARTING_BODY_TYPE_PRISM,
		"prism",
		"Body is prism."
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
	{
		NATIVE_ESTIMATOR,
		"native",
		"Built-in native estimator"
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
#endif /* USE_IPOPT */
#ifdef USE_GLPK
	{
		GLPK_ESTIMATOR,
		"glpk",
		"Glpk linear programming solver"
	},
#endif /* USE_GLPK */
#ifdef USE_CLP
	{
		CLP_ESTIMATOR,
		"clp",
		"Clp linear programming solver"
	},
	{
		CLP_COMMAND_ESTIMATOR,
		"clp-cmd",
		"Clp linear programming solver (invoked from command-line)"
	},
#endif /* USE_CLP */
#ifdef USE_CPLEX
	{
		CPLEX_ESTIMATOR,
		"cplex",
		"CPLEX linear programming solver"
	},
#endif /* USE_CPLEX */
	{
		CGAL_ESTIMATOR,
		"cgal",
		"Built-in CGAL solver"
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

/** Sreucture describing problem type. */
typedef struct
{
	EstimationProblemNorm id;	/**< Problem type ID */
	const char *name;		/**< Name */
	const char *description;	/**< Description */
} ProblemType;

ProblemType problemTypes[] =
{
	{
		ESTIMATION_PROBLEM_NORM_L_INF,
		"linf",
		"L-infinity norm problem"
	},
	{
		ESTIMATION_PROBLEM_NORM_L_1,
		"l1",
		"L-1 norm problem"
	},
	{
		ESTIMATION_PROBLEM_NORM_L_2,
		"l2",
		"L-2 norm problem"
	}
};

static void printUsageOption(char option, const char *longOptionName,
	const char *comment)
{
	DEBUG_START;
	char longOption[256];
	strcpy(longOption, "--");
	strcat(longOption, longOptionName);
	STDERR_PRINT("\t-%c %*s\t%s\n", option, 23, longOption, comment);
	DEBUG_END;
}

/**
 * Prints the usage of the program.
 *
 * @param argc	Standard Linux argc
 * @param argv	Standard Linux argv
 */
static void printUsage(int argc, char** argv)
{
	DEBUG_START;
	STDERR_PRINT("%s - Unit tests for Recoverer of the polyhedron.\n\n",
		argv[0]);
	STDERR_PRINT("Usage:\n");
	STDERR_PRINT("%s [-%c <input file name>] [-%c <model name> -%c <number"
		" of contours>]\n", argv[0], OPTION_FILE_NAME,
		OPTION_MODEL_NAME, OPTION_CONTOURS_NUMBER);
	STDERR_PRINT("Options:\n");
	PCLOption *option = optionsLong;
	while (option->name)
	{
		printUsageOption((char) option->val, option->name,
				option->comment);
		++option;
	}

	STDERR_PRINT("\nPossible synthetic models are:\n");
	for (int iModel = 0; iModel < RECOVERER_TEST_MODELS_NUMBER; ++iModel)
	{
		RecovererTestModel *model = &recovererTestModels[iModel];
		STDERR_PRINT("\t%d. %*s - %s\n", model->id, 12, model->name,
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
		STDERR_PRINT("\t%d. %*s - %s\n", desc->id, 12, desc->name,
			desc->description);
	}

	STDERR_PRINT("\nPossible types of support matrix are:\n");
	int numSupportMatrixTypes = sizeof(supportMatrixTypeDescriptions)
		/ sizeof(SupportMatrixTypeDescription);
	DEBUG_PRINT("Number of support matrix types: %d",
			numSupportMatrixTypes);
	for (int iType = 0; iType < numSupportMatrixTypes; ++iType)
	{
		SupportMatrixTypeDescription *desc =
			&supportMatrixTypeDescriptions[iType];
		STDERR_PRINT("\t%d. %*s - %s", desc->id, 12, desc->name,
			desc->description);
		if (desc->id == DEFAULT_SUPPORT_MATRIX_TYPE)
			STDERR_PRINT(" (default)");
		STDERR_PRINT("\n");
	}

	STDERR_PRINT("\nPossible types of starting body of the estimation "
			"process are:\n");
	int numStartingPointTypes = sizeof(recovererStartingBodies)
		/ sizeof(RecovererStartingBody);
	DEBUG_PRINT("Number of types of starting body of the estimation "
			"process: %d", numStartingPointTypes);
	for (int iType = 0; iType < numStartingPointTypes; ++iType)
	{
		RecovererStartingBody *desc =
			&recovererStartingBodies[iType];
		STDERR_PRINT("\t%d. %*s - %s", desc->id, 12, desc->name,
			desc->description);
		if (desc->id == DEFAULT_STARTING_BODY_TYPE)
			STDERR_PRINT(" (default)");
		STDERR_PRINT("\n");
	}

	STDERR_PRINT("\nPossible type of problem:\n");
	int numProblemTypes = sizeof(problemTypes) / sizeof(ProblemType);
	DEBUG_PRINT("Number of problem types: %d", numProblemTypes);
	for (int iType = 0; iType < numProblemTypes; ++iType)
	{
		ProblemType *desc = &problemTypes[iType];
		STDERR_PRINT("\t%d. %*s - %s", desc->id, 12, desc->name,
			desc->description);
		if (desc->id == DEFAULT_ESTIMATION_PROBLEM_NORM)
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
static CommandLineOptions* parseCommandLine(int argc, char** argv)
{
	DEBUG_START;
	CommandLineOptions* options = new CommandLineOptions();
	bool ifOptionFileName = false;
	bool ifOptionModelName = false;
	bool ifOptionNumContours = false;
	bool ifOptionMaxNumContours = false;
	bool ifOptionFirstAngle = false;
	bool ifOptionLimitRandom = false;
	bool ifOptionProblemType = false;
	bool ifOptionRecover = false;
	bool ifOptionSupportMatrixType = false;
	bool ifOptionStartingBodyType = false;
	bool ifOptionFileDirections = false;
	bool ifOptionFileModel = false;
	bool ifOptionEpsilonFactor = false;
	long int charCurr;
	char *modelName = NULL;
	opterr = 0;

	int numEstimators = sizeof(estimatorDescriptions)
		/ sizeof(RecovererEstimatorTypeDescription);
	DEBUG_PRINT("Number of estimators: %d", numEstimators);

	int numSupportMatrixTypes = sizeof(supportMatrixTypeDescriptions)
		/ sizeof(SupportMatrixTypeDescription);
	DEBUG_PRINT("Number of support matrix types: %d",
			numSupportMatrixTypes);

	int numStartingPointTypes = sizeof(recovererStartingBodies)
		/ sizeof(RecovererStartingBody);
	DEBUG_PRINT("Number of types of starting body of the estimation "
			"process: %d", numStartingPointTypes);

	int numProblemTypes = sizeof(problemTypes) / sizeof(ProblemType);
	DEBUG_PRINT("Number of problem types: %d", numProblemTypes);

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
	options->epsilonFactor = -1;
	options->fileNamePolyhedron = NULL;
	options->threshold = 0.;
	options->zMinimalNorm = 0.;
	options->input.file.ifSupportFunctionData = false;
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
		case OPTION_SUPPORT_FUNCTION_DATA_FILE:
			options->input.file.ifSupportFunctionData = true;
		case OPTION_FILE_NAME:
			if (ifOptionFileName)
			{
				errorOptionTwice(argc, argv, OPTION_FILE_NAME);
			}

			options->mode = RECOVERER_REAL_TESTING;
			options->input.file.name = optarg;
			ifOptionFileName = true;
			break;
		case OPTION_MODEL_NAME:
			if (ifOptionModelName)
			{
				errorOptionTwice(argc, argv, OPTION_MODEL_NAME);
			}

			options->mode = RECOVERER_SYNTHETIC_TESTING;
			for (int iModel = 0;
				iModel < RECOVERER_TEST_MODELS_NUMBER;
				++iModel)
			{
				if (strcmp(optarg,
					recovererTestModels[iModel].name) == 0)
				{
					options->input.model.id
						= (RecovererTestModelID) iModel;
					ifOptionModelName = true;
					break;
				}
			}
			if (!ifOptionModelName)
			{
				STDERR_PRINT("Invalid name of model - %s\n",
						optarg);
				printUsage(argc, argv);
				DEBUG_END;
				exit(EXIT_FAILURE);
			}
			modelName = strdup(optarg);
			break;
		case OPTION_CONTOURS_NUMBER:
			if (ifOptionNumContours)
			{
				errorOptionTwice(argc, argv,
						OPTION_CONTOURS_NUMBER);
			}

			/* Parse digital number from input argument. */
			options->input.model.numContours =
					strtol(optarg, &charMistaken, 10);

			/*
			 *  If user gives invalid character, the charMistaken is
			 *  set to it
			 */

			/*
			 * From "man strtol":
			 *
			 * In particular, if *nptr is not '\0' but **endptr is
			 * '\0' on return, the entire string is valid.
			 */
			if (charMistaken && *charMistaken)
			{
				errorCannotParseNumber(argc, argv,
						charMistaken);
			}

			/*
			 * In case of underflow or overflow errno is set to
			 * ERANGE.
			 */
			if (errno == ERANGE)
			{
				errorOutOfRange(argc, argv);
			}


			ifOptionNumContours = true;
			break;
		case OPTION_MAX_CONTOUR_NUMBER:
			if (ifOptionMaxNumContours)
			{
				errorOptionTwice(argc, argv,
						OPTION_CONTOURS_NUMBER);
			}

			/* Parse digital number from input argument. */
			options->numMaxContours = strtol(optarg, &charMistaken,
					10);
			/*
			 *  If user gives invalid character, the charMistaken is
			 *  set to it
			 */

			/*
			 * From "man strtol":
			 *
			 * In particular, if *nptr is not '\0' but **endptr is
			 * '\0' on return, the entire string is valid.
			 */
			if (charMistaken && *charMistaken)
			{
				errorCannotParseNumber(argc, argv,
						charMistaken);
			}

			/*
			 * In case of underflow or overflow errno is set to
			 * ERANGE.
			 */
			if (errno == ERANGE)
			{
				errorOutOfRange(argc, argv);
			}

			ifOptionMaxNumContours = true;
			break;
		case OPTION_FIRST_ANGLE:
			if (ifOptionFirstAngle)
			{
				errorOptionTwice(argc, argv,
						OPTION_FIRST_ANGLE);
			}
			/* Parse floating-point number from input argument. */
			options->input.model.shiftAngleFirst =
					strtod(optarg, &charMistaken);

			/*
			 * If user gives invalid character, the charMistaken is
			 * set to it
			 */
			if (charMistaken && *charMistaken)
			{
				errorCannotParseNumber(argc, argv,
						charMistaken);
			}
			
			/*
			 * In case of underflow or overflow errno is set to
			 * ERANGE.
			 */
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
				errorOptionTwice(argc, argv,
						OPTION_LIMIT_RANDOM);
			}
			/* Parse floating-point number from input argument. */
			options->input.model.limitRandom =
					strtod(optarg, &charMistaken);

			/*
			 * If user gives invalid character, the charMistaken is
			 * set to it
			 */
			if (charMistaken && *charMistaken)
			{
				errorCannotParseNumber(argc, argv,
						charMistaken);
			}

			/*
			 * In case of underflow or overflow errno is set to
			 * ERANGE.
			 */
			if (errno == ERANGE)
			{
				errorOutOfRange(argc, argv) ;
			}

			ifOptionLimitRandom = true;
			break;
		case OPTION_OUTPUT_NAME:
			if (options->outputName)
			{
				errorOptionTwice(argc, argv,
						OPTION_OUTPUT_NAME);
			}
			options->outputName = optarg;
			break;
		case OPTION_PROBLEM_TYPE:
			if (ifOptionProblemType)
			{
				errorOptionTwice(argc, argv,
						OPTION_PROBLEM_TYPE);
			}
			for (int iType = 0; iType < numProblemTypes; ++iType)
			{
				if (strcmp(optarg,
					problemTypes[iType].name) == 0)
				{
					options->problemType
						= problemTypes[iType].id;
					ifOptionProblemType = true;
				}
			}
			if (!ifOptionProblemType)
			{
				STDERR_PRINT("Invalid name of problem type "
						"- %s\n", optarg);
				printUsage(argc, argv);
				DEBUG_END;
				exit(EXIT_FAILURE);
			}
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
				if (!strcmp(optarg,
					estimatorDescriptions[iEstimator].name))
				{
					options->estimator
						= (RecovererEstimatorType)
						iEstimator;
					ifOptionRecover = true;
					break;
				}
			}
			if (!ifOptionRecover)
			{
				STDERR_PRINT("Invalid name of estimator - %s\n",
						optarg);
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
		case OPTION_STARTING_BODY:
			if (ifOptionStartingBodyType)
			{
				errorOptionTwice(argc, argv,
						OPTION_STARTING_BODY);
			}

			for (int iType = 0; iType < numStartingPointTypes;
					++iType)
			{
				if (!strcmp(optarg, recovererStartingBodies[
							iType].name))
				{
					options->startingBodyType =
						recovererStartingBodies[
							iType].id;
					ifOptionStartingBodyType = true;
					break;
				}
			}
			if (!ifOptionStartingBodyType)
			{
				STDERR_PRINT("Invalid name of starting body "
						"type.");
				printUsage(argc, argv);
				DEBUG_END;
				exit(EXIT_FAILURE);
			}
			break;
		case OPTION_FILE_DIRECTIONS:
			if (ifOptionFileDirections)
			{
				errorOptionTwice(argc, argv,
						OPTION_FILE_DIRECTIONS);
			}
			options->input.model.directionsFileName = optarg;
			ifOptionFileDirections = true;
			break;
		case OPTION_FILE_MODEL:
			if (ifOptionFileModel)
			{
				errorOptionTwice(argc, argv,
						OPTION_FILE_MODEL);
			}
			options->mode = RECOVERER_SYNTHETIC_TESTING;
			options->input.model.fileName = optarg;
			ifOptionFileModel = true;
			break;
		case OPTION_AXIAL_TESTING:
			options->input.model.ifAxialTesting = true;
			break;
		case OPTION_EPSILON_FACTOR:
			if (ifOptionEpsilonFactor)
			{
				errorOptionTwice(argc, argv,
						OPTION_EPSILON_FACTOR);
			}
			/* Parse floating-point number from input argument. */
			options->epsilonFactor = strtod(optarg, &charMistaken);

			/*
			 * If user gives invalid character, the charMistaken is
			 * set to it
			 */
			if (charMistaken && *charMistaken)
			{
				errorCannotParseNumber(argc, argv,
						charMistaken);
			}

			/*
			 * In case of underflow or overflow errno is set to
			 * ERANGE.
			 */
			if (errno == ERANGE)
			{
				errorOutOfRange(argc, argv);
			}

			ifOptionEpsilonFactor = true;
			break;
		case OPTION_COMPARE_WITH_FILE:
			if (options->fileNamePolyhedron)
			{
				errorOptionTwice(argc, argv,
						OPTION_COMPARE_WITH_FILE);
			}
			options->fileNamePolyhedron = optarg;
			break;
			break;
		case OPTION_THRESHOLD_ADJACENT_FACETS:
			/* Parse floating-point number from input argument. */
			options->threshold = strtod(optarg, &charMistaken);

			/*
			 * If user gives invalid character, the charMistaken is
			 * set to it
			 */
			if (charMistaken && *charMistaken)
			{
				errorCannotParseNumber(argc, argv,
						charMistaken);
			}

			/*
			 * In case of underflow or overflow errno is set to
			 * ERANGE.
			 */
			if (errno == ERANGE)
			{
				errorOutOfRange(argc, argv);
			}

			ifOptionEpsilonFactor = true;
			break;
		case OPTION_Z_MINIMAL_NORM:
			options->zMinimalNorm = strtod(optarg, &charMistaken);
			if (charMistaken && *charMistaken)
				errorCannotParseNumber(argc, argv,
						charMistaken);
			if (errno == ERANGE)
				errorOutOfRange(argc, argv);
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

	bool ifAnyOptionSynthetic = ifOptionModelName
		|| ifOptionNumContours
		|| ifOptionFirstAngle
		|| ifOptionLimitRandom
		|| ifOptionFileModel
		|| ifOptionFileDirections
		|| options->input.model.ifAxialTesting;

	if (ifOptionFileName && ifAnyOptionSynthetic)
	{
		STDERR_PRINT("Option \"-%c\" cannot be used with any option "
				"related with synthetic testing.",
				OPTION_FILE_NAME);
		printUsage(argc, argv);
		DEBUG_END;
		exit(EXIT_FAILURE);
	}

	if (ifOptionModelName && ifOptionFileModel)
	{
		STDERR_PRINT("Options \"-%c\" and \"-%c\" are conflicting.",
			OPTION_MODEL_NAME, OPTION_FILE_MODEL);
		printUsage(argc, argv);
		DEBUG_END;
		exit(EXIT_FAILURE);
	}

	bool ifAnyOptionContours = ifOptionNumContours || ifOptionFirstAngle;

	if (ifOptionFileDirections && ifAnyOptionContours)
	{
		STDERR_PRINT("Option \"-%c\" conflict with options controlling"
				"contours generation", OPTION_FILE_DIRECTIONS);
		printUsage(argc, argv);
		DEBUG_END;
		exit(EXIT_FAILURE);
	}

	bool ifValidSyntheticOptions = (ifOptionModelName || ifOptionFileModel)
		&& (ifOptionFileDirections
				|| (ifOptionNumContours && ifOptionFirstAngle))
		&& ifOptionLimitRandom;

	if (!ifValidSyntheticOptions && ifAnyOptionSynthetic)
	{
		STDERR_PRINT("Invalid option set for synthetic testing.\n");
		printUsage(argc, argv);
		DEBUG_END;
		exit(EXIT_FAILURE);
	}

	if (!ifOptionFileName && !ifOptionModelName && !ifOptionFileModel)
	{
		STDERR_PRINT("At least one of options \"-%c\", \"-%c\" "
			"and \"-%c\" must be specified.\n",
			OPTION_FILE_NAME,
			OPTION_MODEL_NAME,
			OPTION_FILE_MODEL);
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
			options->outputName = strdup(options->input.file.name);
		}
		else
		{
			ASSERT(modelName);
			options->outputName = strdup(modelName);
		}
	}

	if (options->mode == RECOVERER_SYNTHETIC_TESTING
		&& !ifOptionNumContours)
	{
		options->input.model.numContours = 0;
	}

	if (!ifOptionSupportMatrixType)
	{
		options->supportMatrixType = DEFAULT_SUPPORT_MATRIX_TYPE;
	}

	if (!ifOptionStartingBodyType)
	{
		options->startingBodyType = DEFAULT_STARTING_BODY_TYPE;
	}

	if (!ifOptionMaxNumContours)
	{
		options->numMaxContours = IF_ANALYZE_ALL_CONTOURS;
	}

	if (!ifOptionProblemType)
	{
		options->problemType = DEFAULT_ESTIMATION_PROBLEM_NORM;
	}

	if (modelName)
		free(modelName);

	DEBUG_END;
	return options;
}

static Polyhedron *readPolyhedron(char *fileName)
{
	DEBUG_START;
	Polyhedron *p = new Polyhedron();
	p->fscan_ply(fileName);
	DEBUG_END;
	return p;
}

/**
 * Created a polyhedron of the specified type.
 *
 * @param id	The type of the polyhedron
 */
static Polyhedron *makePolyhedron(RecovererTestModelID id)
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

static Polyhedron_3 makeModel(CommandLineOptions *options)
{
	DEBUG_START;
	Polyhedron *p = NULL;

	if (options->input.model.fileName)
	{
		p = readPolyhedron(options->input.model.fileName);
	}
	else
	{
		p = makePolyhedron(options->input.model.id);
	}
	ASSERT(p->numVertices > 0);
	Polyhedron_3 pCGAL(*p);
	DEBUG_END;
	return pCGAL;
}

/**
 * Gets real shadow contour data from file.
 *
 * @param options	The result of command-line parsing.
 */
static ShadowContourDataPtr getRealSCData(CommandLineOptions* options)
{
	DEBUG_START;

	/* Create fake empty polyhedron. */
	PolyhedronPtr p(new Polyhedron());

	/* Create shadow contour data and associate p with it. */
	ShadowContourDataPtr SCData(new ShadowContourData(p));

	/* Read shadow contours data from file. */
	ASSERT(!options->input.file.ifSupportFunctionData);
	SCData->fscanDefault(options->input.file.name);

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
 * Shifts all points of contours on random double vectors
 *
 * @param data		Shadow contours data
 * @param maxDelta	Maximum delta in shift vectors' coordinates
 */
static void shiftContoursRandom(ShadowContourDataPtr data, double maxDelta)
{
	DEBUG_START;
	for (int iContour = 0; iContour < data->numContours; ++iContour)
	{
		SContour* contour = &data->contours[iContour];
		if (contour->ns <= 0)
			continue;

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
static ShadowContourDataPtr generateSyntheticSCData(CommandLineOptions *options)
{
	DEBUG_START;

	/* Create polyhedron based on one of possible models. */
	PolyhedronPtr p(new Polyhedron(makeModel(options)));
	ASSERT(p->nonZeroPlanes());
	Polyhedron *pCopy = new Polyhedron(makeModel(options));

	/* Set the pointer to the parent polyhedron in facets. */
	p->set_parent_polyhedron_in_facets();
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
		"original-polyhedron.ply") << *pCopy;

	/* Create shadow contour data and associate p with it. */
	ShadowContourDataPtr SCData(new ShadowContourData(p));

	/*
	 * Create shadow contour constructor and associate p and SCData with
	 * them.
	 */
	ASSERT(p->nonZeroPlanes());
	ShadowContourConstructorPtr constructor(new ShadowContourConstructor(p,
			SCData));

	/* Generate shadow contours data for given model. */
	constructor->run(options->input.model.numContours,
			options->input.model.shiftAngleFirst);
	ASSERT(!SCData->empty());

	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
		"original-unshifted-contours.dat") << SCData;
	ASSERT(!SCData->empty());
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
		"original-unshifted-contours.ply") << Polyhedron(SCData);
	ASSERT(!SCData->empty());

	/* Shift points contours on random std::vectors. */
	shiftContoursRandom(SCData, options->input.model.limitRandom);
	ASSERT(!SCData->empty());

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
static ShadowContourDataPtr makeShadowData(CommandLineOptions* options)
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

static std::vector<Point_3> readDirections(char *fileName)
{
	DEBUG_START;
	std::ifstream input(fileName);

	if (!input.is_open())
	{
		ERROR_PRINT("File %s does not exist!", fileName);
		DEBUG_END;
		exit(EXIT_FAILURE);
	}

	std::istream_iterator<Vector3d> start(input), end;
	std::vector<Vector3d> vectors(start, end);
	ASSERT(!vectors.empty());

	DEBUG_PRINT("Read %d vectors from file %s.", (int) vectors.size(),
			fileName);

	std::vector<Point_3> points;
	for (auto &v: vectors)
	{
		Point_3 p = v;
		points.push_back(p);
	}
	DEBUG_END;
	return points;
}

static SupportFunctionDataPtr makeSupportData(CommandLineOptions *options)
{
	DEBUG_START;
	if (options->mode == RECOVERER_REAL_TESTING
			&& options->input.file.ifSupportFunctionData)
	{
		std::filebuf fb;
		if (fb.open(options->input.file.name, std::ios::in))
		{
			std::istream is(&fb);
			SupportFunctionDataPtr data(new SupportFunctionData(
				is));
			fb.close();
			DEBUG_END;
			return data;
		}
		else
		{
			ERROR_PRINT("Cannot open file %s",
				options->input.file.name);
			DEBUG_END;
			exit(EXIT_FAILURE);
		}
	}

	/* Create polyhedron based on one of possible models. */
	Polyhedron_3 p = makeModel(options);
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
		"original-polyhedron.ply") << p;

	if (options->input.model.directionsFileName)
	{
		auto directions = readDirections(
				options->input.model.directionsFileName);
		auto data = p.calculateSupportData(directions);
		data->shiftValues(options->input.model.limitRandom);
		DEBUG_END;
		return data;
	}
	else
	{
		auto shadows = generateSyntheticSCData(options);
		auto data = shadows->calculateSupportData();
		data->shiftValues(options->input.model.limitRandom);
		DEBUG_END;
		return data;
	}
}

/**
 * Makes recoverer with requested properties.
 *
 * @param options	Parsed command-line options
 */
static RecovererPtr makeRecoverer(CommandLineOptions* options)
{
	DEBUG_START;
	RecovererPtr recoverer(new Recoverer());

	/* Set the name of output file. */
	globalPCLDumper.setNameBase(options->outputName);

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

	/* Set starting body type. */
	recoverer->setStartingBodyType(options->startingBodyType);

	/* Set the problem type. */
	recoverer->setProblemType(options->problemType);

	/* Set number of analyzed contours. */
	recoverer->setNumMaxContours(options->numMaxContours);

	if (options->epsilonFactor > 0.)
	{
		GardnerKiderlenSupportMatrix::epsilonFactor =
			options->epsilonFactor;
	}

	recoverer->setFileNamePolyhedron(options->fileNamePolyhedron);
	recoverer->setThreshold(options->threshold);
	recoverer->setZMinimalNorm(options->zMinimalNorm);

	DEBUG_END;
	return recoverer;
}
struct Plane_from_facet
{
	Polyhedron_3::Plane_3 operator()(Polyhedron_3::Facet& f)
	{
		Polyhedron_3::Halfedge_handle h = f.halfedge();
		auto plane = Polyhedron_3::Plane_3(h->vertex()->point(),
				h->next()->vertex()->point(),
				h->opposite()->vertex()->point());
		double a = plane.a();
		double b = plane.b();
		double c = plane.c();
		double d = plane.d();
		double norm = sqrt(a * a + b * b + c * c);
		a /= norm;
		b /= norm;
		c /= norm;
		d /= norm;
		if (d > 0.)
		{
			a = -a;
			b = -b;
			c = -c;
			d = -d;
		}
		plane = Plane_3(a, b, c, d);
		return plane;
	}
};

/**
 * Runs the recovering from shadow contours with requested mode.
 *
 * @param options	Parsed command-line options
 * @param recoverer	The recoverer to be used
 * @param data		The data
 */
template<typename DataPtr>
static void runRecovery(CommandLineOptions* options,
	RecovererPtr recoverer, DataPtr data)
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
	Polyhedron_3 polyhedron = recoverer->run(data);
	std::transform(polyhedron.facets_begin(), polyhedron.facets_end(),
		polyhedron.planes_begin(), Plane_from_facet());
#ifndef NDEBUG
	int iFacet = 0;
	for (auto facet = polyhedron.facets_begin();
			facet != polyhedron.facets_end(); ++facet)
	{
		std::cerr << "facet " << iFacet << ": " << facet->plane()
			<< std::endl;
		++iFacet;
	}
#endif

	globalPCLDumper(PCL_DUMPER_LEVEL_OUTPUT, "recovered.ply") << polyhedron;
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
	RecovererPtr recoverer = makeRecoverer(options);
	
	if ((options->mode == RECOVERER_REAL_TESTING
		&& !options->input.file.ifSupportFunctionData)
		|| (options->mode == RECOVERER_SYNTHETIC_TESTING
		&& options->input.model.numContours))
	{
		/*
		 * Read or generate shadow contour data depending on requested
		 * option.
		 */
		auto data = makeShadowData(options);

		/* Run the recovery. */
		runRecovery(options, recoverer, data);
	}
	else
	{
		/* Make support function data. */
		auto data = makeSupportData(options);

		/* Run the recovery. */
		runRecovery(options, recoverer, data);
	}


	DEBUG_END;
	return EXIT_SUCCESS;
}
