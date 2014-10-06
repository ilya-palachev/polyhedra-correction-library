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
#include <Gauss_test.h>

#define TEST_PARAMETERS_NUMBER 6
struct _TestParameters
{
	char* fileNamePolyhedron;
	char* fileNameShadowContours;
	MethodCorrector method;
	double epsLoopStop;
	double deltaGardientStep;
	int numFacetsCorrected;
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

	shared_ptr<Polyhedron> polyhedron(new Polyhedron());
	if (!polyhedron->fscan_default_1_2(parameters.fileNamePolyhedron))
	{
		DEBUG_END;
		return EXIT_FAILURE;
	}
	polyhedron->my_fprint(stderr);

	shared_ptr<ShadowContourData> contourData(new ShadowContourData(polyhedron));
	if (!contourData->fscanDefault(parameters.fileNameShadowContours))
	{
		DEBUG_END;
		return EXIT_FAILURE;
	}

	GSCorrectorParameters gsParameters;
	gsParameters = {parameters.method, parameters.epsLoopStop,
			parameters.deltaGardientStep};
	
	list< FacetWithArea > listSortedFacets =
		polyhedron->getSortedByAreaFacets();
		
	list<int> facetsCorrected;
	auto itFacet = listSortedFacets.rend();
	++itFacet;
	for (int iFacet = 0; iFacet < parameters.numFacetsCorrected; ++itFacet,
		++iFacet)
	{
		DEBUG_PRINT("Adding facet %d with area %lf to be corrected",
			itFacet->facet->id, itFacet->area);
		facetsCorrected.push_back(itFacet->facet->id);
	}

	polyhedron->fprint_ply_scale(1000., "poly-data-out/before.ply", "before");

	polyhedron->correctGlobal(contourData, &gsParameters, &facetsCorrected);

	polyhedron->fprint_ply_scale(1000., "poly-data-out/after.ply", "after");

	if (parameters.fileNamePolyhedron != NULL)
	{
		delete[] parameters.fileNamePolyhedron;
		parameters.fileNamePolyhedron = NULL;
	}

	if (parameters.fileNameShadowContours != NULL)
	{
		delete[] parameters.fileNameShadowContours;
		parameters.fileNameShadowContours = NULL;
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
					"<file with shadow contours> <method name>"
					"<eps max loop> <delta gradient descend> <number of "
					"corrected facets>\n");
	printf("\nPossible methods: gd (gradient descent), "
			"gdf (gradient descent - fast),"
			"cg (conjugate gradient).\n");
	DEBUG_END;
}

int parse_commandLine(int argc, char** argv, TestParameters& parameters)
{
	DEBUG_START;
	if (argc != TEST_PARAMETERS_NUMBER + 1)
	{
		ERROR_PRINT("Wrong number of arguments!");
		printUsage();
		DEBUG_END;
		return EXIT_FAILURE;
	}
	parameters.fileNamePolyhedron = new char[255];
	parameters.fileNameShadowContours = new char[255];
	char* method = new char[255];

	bool ifCorrectInput = sscanf(argv[1], "%s", parameters.fileNamePolyhedron)
			&& sscanf(argv[2], "%s", parameters.fileNameShadowContours)
			&& sscanf(argv[3], "%s", method)
			&& sscanf(argv[4], "%lf", &parameters.epsLoopStop)
			&& sscanf(argv[5], "%lf", &parameters.deltaGardientStep)
			&& sscanf(argv[6], "%d", &parameters.numFacetsCorrected);

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





