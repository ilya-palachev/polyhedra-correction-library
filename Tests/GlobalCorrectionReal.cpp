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
	TestParameters parameters;

	if (parse_commandLine(argc, argv, parameters) != EXIT_SUCCESS)
		return EXIT_FAILURE;

	Polyhedron* polyhedron = new Polyhedron();
	if (!polyhedron->fscan_default_1_2(parameters.fileNamePolyhedron))
		return EXIT_FAILURE;
	ShadeContourData* contourData = new ShadeContourData(polyhedron);
	if (!contourData->fscanDefault(parameters.fileNameShadeContours))
		return EXIT_FAILURE;

	GSCorrectorParameters gsParameters;
	gsParameters = {parameters.method, parameters.epsLoopStop,
			parameters.deltaGardientStep};
	polyhedron->correctGlobal(contourData, &gsParameters);

	if (polyhedron != NULL)
	{
		delete polyhedron;
		polyhedron = NULL;
	}
	return EXIT_SUCCESS;
}

void printUsage()
{
	printf(
			"Usage: \n"
					"./globalCorrectionReal <file with polyhedron> "
					"<file with shade contours> <method name>"
					"<eps max loop> <delta gradient descend>\n");
	printf("\nPossible methods: gd (gradient descent), "
			"gdf (gradient descent - fast),"
			"cg (conjugate gradient).\n");
}

int parse_commandLine(int argc, char** argv, TestParameters& parameters)
{
	if (argc != 6)
	{
		ERROR_PRINT("Wrong number of arguments!");
		printUsage();
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

	if (!ifCorrectInput)
	{
		ERROR_PRINT("Incorrect input!");
		printUsage();
		return EXIT_FAILURE;
	}

	if (method)
	{
		delete[] method;
		method = NULL;
	}

	return EXIT_SUCCESS;
}

MethodCorrector parse_methodName(char* methodNameInput)
{
	if (strcmp(methodNameInput, "gd") == 0)
	{
		return METHOD_GRADIENT_DESCENT;
	}
	else if (strcmp(methodNameInput, "gdf") == 0)
	{
		return METHOD_GRADIENT_DESCENT_FAST;
	}
	else if (strcmp(methodNameInput, "cg") == 0)
	{
		return METHOD_CONJUGATE_GRADIENT;
	}
	else if (strcmp(methodNameInput, "all") == 0)
	{
		return METHOD_ALL;
	}
	else
	{
		return METHOD_UNKNOWN;
	}
}
