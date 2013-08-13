#include "PolyhedraCorrectionLibrary.h"

enum NameFigure
{
	FIGURE_UNKNOWN,
	FIGURE_CUBE,
	FIGURE_PYRAMID,
	FIGURE_PRISM,
	FIGURE_CUBE_CUTTED
};

struct TestParameters
{
	NameFigure figure;
	MethodCorrector method;
	int numContours;
	int indFacetMoved;
	double maxMoveDelta;
	double shiftAngleFirst;
	double epsLoopStop;
	double deltaGardientStep;
};

void printUsage();
NameFigure parse_figureName(char* figureNameInput);
MethodCorrector parse_methodName(char* methodNameInput);
int parse_commandLine(int argc, char** argv, TestParameters& parameters);
Polyhedron* makePolyhedron(NameFigure figureParsed);
inline void moveFacetRandom(Polyhedron* polyhedron, double maxMoveDelta,
		int ifacet);

int main(int argc, char** argv)
{
	DEBUG_START;
	TestParameters parameters;

	if (parse_commandLine(argc, argv, parameters) != EXIT_SUCCESS)
		return EXIT_FAILURE;
	Polyhedron* polyhedron = makePolyhedron(parameters.figure);

	polyhedron->fprint_ply_scale(1000.,
			"poly-data-out/globalCorrection-before.ply",
			"globalCorrection");

	ShadeContourData* contourData = new ShadeContourData(polyhedron);
	ShadeContourConstructor* scConstructor = new ShadeContourConstructor(
			polyhedron, contourData);
	scConstructor->run(parameters.numContours, parameters.shiftAngleFirst);
	moveFacetRandom(polyhedron, parameters.maxMoveDelta, parameters.indFacetMoved);

	polyhedron->fprint_ply_scale(1000.,
				"poly-data-out/globalCorrection-moved.ply",
				"globalCorrection");

	GSCorrectorParameters gsParameters;
	gsParameters = {parameters.method, parameters.epsLoopStop,
			parameters.deltaGardientStep};
	polyhedron->correctGlobal(contourData, &gsParameters);

	polyhedron->fprint_ply_scale(1000.,
			"poly-data-out/globalCorrection-after.ply",
			"globalCorrection");

	DEBUG_END;
	return EXIT_SUCCESS;
}

void printUsage()
{
	DEBUG_START;
	printf(
			"Usage: \n"
					"./globalCorrection <figure name> <method> <number_of_contours> "
					"<index of facet to be moved> <max_move_delta> <first angle shift> "
					"<eps max loop> <delta gradient descend>\n");
	printf("\nPossible figures: cube pyramid prism cube-cutted\n");
	printf("\nPossible methods: gd (gradient descent), "
				"gdf (gradient descent - fast),"
				"cg (conjugate gradient).\n");
	DEBUG_END;
}

int parse_commandLine(int argc, char** argv, TestParameters& parameters)
{
	DEBUG_START;

	if (argc != 9)
	{
		ERROR_PRINT("Wrong number of arguments!");
		printUsage();
		return EXIT_FAILURE;
	}

	char* figure = new char[255];
	char* method = new char[255];

	bool ifCorrectInput = sscanf(argv[1], "%s", figure)
			&& sscanf(argv[2], "%s", method)
			&& sscanf(argv[3], "%d", &parameters.numContours)
			&& sscanf(argv[4], "%d", &parameters.indFacetMoved)
			&& sscanf(argv[5], "%lf", &parameters.maxMoveDelta)
			&& sscanf(argv[6], "%lf", &parameters.shiftAngleFirst)
			&& sscanf(argv[7], "%lf", &parameters.epsLoopStop)
			&& sscanf(argv[8], "%lf", &parameters.deltaGardientStep);

	parameters.figure = parse_figureName(figure);
	parameters.method = parse_methodName(method);

	if (figure)
	{
		delete[] figure;
		figure = NULL;
	}

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

NameFigure parse_figureName(char* figureNameInput)
{
	DEBUG_START;
	NameFigure ret;
	if (strcmp(figureNameInput, "cube") == 0)
	{
		ret = FIGURE_CUBE;
	}
	else if (strcmp(figureNameInput, "pyramid") == 0)
	{
		ret = FIGURE_PYRAMID;
	}
	else if (strcmp(figureNameInput, "prism") == 0)
	{
		ret = FIGURE_PRISM;
	}
	else if (strcmp(figureNameInput, "cube-cutted") == 0)
	{
		ret = FIGURE_CUBE_CUTTED;
	}
	else
	{
		ret = FIGURE_UNKNOWN;
	}

	DEBUG_END;
	return ret;
}

MethodCorrector parse_methodName(char* methodNameInput)
{
	DEBUG_START;
	MethodCorrector ret;
	if (strcmp(methodNameInput, "gd") == 0)
	{
		ret = METHOD_GRADIENT_DESCENT;
	}
	else if (strcmp(methodNameInput, "gdf") == 0)
	{
		ret = METHOD_GRADIENT_DESCENT_FAST;
	}
	else if (strcmp(methodNameInput, "cg") == 0)
	{
		ret = METHOD_CONJUGATE_GRADIENT;
	}
	else if (strcmp(methodNameInput, "all") == 0)
	{
		ret = METHOD_ALL;
	}
	else
	{
		ret = METHOD_UNKNOWN;
	}

	DEBUG_END;
	return ret;
}

Polyhedron* makePolyhedron(NameFigure figureParsed)
{
	DEBUG_START;
	Polyhedron* ret;
	switch (figureParsed)
	{
	case FIGURE_CUBE:
		ret = new Cube(1., 0., 0., 0.);
		break;
	case FIGURE_PYRAMID:
		ret = new Pyramid(3, 1., 1.);
		break;
	case FIGURE_PRISM:
		ret = new Prism(3, 1., 1.);
		break;
	case FIGURE_CUBE_CUTTED:
		ret = new CubeCutted();
		break;
	default:
		ERROR_PRINT("Unknown figure name!");
		printUsage();
		ret = NULL;
		break;
	}
	DEBUG_END;
	return ret;
}

static double genRandomDouble(double maxDelta)
{
	DEBUG_START;
	srand((unsigned) time(0));
	int randomInteger = rand();
	double randomDouble = randomInteger;
	const double halfRandMax = RAND_MAX * 0.5;
	randomDouble -= halfRandMax;
	randomDouble /= halfRandMax;

	randomDouble *= maxDelta;

	DEBUG_END;
	return randomDouble;
}

inline void moveFacetRandom(Polyhedron* polyhedron, double maxMoveDelta,
		int ifacet)
{
	DEBUG_START;
	Plane& plane = polyhedron->facets[ifacet].plane;
	plane.norm.x += genRandomDouble(maxMoveDelta);
	plane.norm.y += genRandomDouble(maxMoveDelta);
	plane.norm.z += genRandomDouble(maxMoveDelta);
	plane.dist += genRandomDouble(maxMoveDelta);
	double newNorm = sqrt(qmod(plane.norm));
	plane.norm.norm(1.);
	plane.dist /= newNorm;
	DEBUG_END;
}
