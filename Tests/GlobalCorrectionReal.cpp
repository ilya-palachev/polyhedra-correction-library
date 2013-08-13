/*
 * GlobalCorrectionReal.cpp
 *
 *  Created on: 01.07.2013
 *      Author: iliya
 */

#include "PolyhedraCorrectionLibrary.h"

struct _TestParameters
{
	char* fileNamePolyhedron;
	char* fileNameShadeContours;
	MethodCorrector method;
	double epsLoopStop;
	double deltaGardientStep;
};
typedef struct _TestParameters TestParameters;

void printUsage();
int parse_commandLine(int argc, char** argv, TestParameters& parameters);
MethodCorrector parse_methodName(char* methodNameInput);

int main(int argc, char** argv)
{
	DEBUG_START;
	TestParameters parameters;

	if (parse_commandLine(argc, argv, parameters) != EXIT_SUCCESS)
	{
		DEBUG_END;
		return EXIT_FAILURE;
	}

	Polyhedron* polyhedron = new Polyhedron();
	if (!polyhedron->fscan_default_1_2(parameters.fileNamePolyhedron))
	{
		DEBUG_END;
		return EXIT_FAILURE;
	}
	ShadeContourData* contourData = new ShadeContourData(polyhedron);
	if (!contourData->fscanDefault(parameters.fileNameShadeContours))
	{
		DEBUG_END;
		return EXIT_FAILURE;
	}

	GSCorrectorParameters gsParameters;
	gsParameters = {parameters.method, parameters.epsLoopStop,
			parameters.deltaGardientStep};

	polyhedron->fprint_ply_scale(1000., "poly-data-out/before.ply", "before");

	polyhedron->correctGlobal(contourData, &gsParameters);

	polyhedron->fprint_ply_scale(1000., "poly-data-out/after.ply", "after");

	if (polyhedron != NULL)
	{
		delete polyhedron;
		polyhedron = NULL;
	}

	if (parameters.fileNamePolyhedron != NULL)
	{
		delete[] parameters.fileNamePolyhedron;
		parameters.fileNamePolyhedron = NULL;
	}

	if (parameters.fileNameShadeContours != NULL)
	{
		delete[] parameters.fileNameShadeContours;
		parameters.fileNameShadeContours = NULL;
	}

	DEBUG_END;
	return EXIT_SUCCESS;
}

void printUsage()
{
	DEBUG_START;
	printf(
			"Usage: \n"
					"./globalCorrectionReal <file with polyhedron> "
					"<file with shade contours> <method name>"
					"<eps max loop> <delta gradient descend>\n");
	printf("\nPossible methods: gd (gradient descent), "
			"gdf (gradient descent - fast),"
			"cg (conjugate gradient).\n");
	DEBUG_END;
}

int parse_commandLine(int argc, char** argv, TestParameters& parameters)
{
	DEBUG_START;
	if (argc != 6)
	{
		ERROR_PRINT("Wrong number of arguments!");
		printUsage();
		DEBUG_END;
		return EXIT_FAILURE;
	}
	parameters.fileNamePolyhedron = new char[255];
	parameters.fileNameShadeContours = new char[255];
	char* method = new char[255];

	bool ifCorrectInput = sscanf(argv[1], "%s", parameters.fileNamePolyhedron)
			&& sscanf(argv[2], "%s", parameters.fileNameShadeContours)
			&& sscanf(argv[3], "%s", method)
			&& sscanf(argv[4], "%lf", &parameters.epsLoopStop)
			&& sscanf(argv[5], "%lf", &parameters.deltaGardientStep);

	parameters.method = parse_methodName(method);

	if (method)
	{
		delete[] method;
		method = NULL;
	}
	if (!ifCorrectInput)
	{
		ERROR_PRINT("Incorrect input!");
		printUsage();
		DEBUG_END;
		return EXIT_FAILURE;
	}

	DEBUG_END;
	return EXIT_SUCCESS;
}

MethodCorrector parse_methodName(char* methodNameInput)
{
	DEBUG_START;
	if (strcmp(methodNameInput, "gd") == 0)
	{
		DEBUG_END;
		return METHOD_GRADIENT_DESCENT;
	}
	else if (strcmp(methodNameInput, "gdf") == 0)
	{
		DEBUG_END;
		return METHOD_GRADIENT_DESCENT_FAST;
	}
	else if (strcmp(methodNameInput, "cg") == 0)
	{
		DEBUG_END;
		return METHOD_CONJUGATE_GRADIENT;
	}
	else if (strcmp(methodNameInput, "all") == 0)
	{
		DEBUG_END;
		return METHOD_ALL;
	}
	else
	{
		DEBUG_END;
		return METHOD_UNKNOWN;
	}
}
