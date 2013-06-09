#include "PolyhedraCorrectionLibrary.h"

enum nameFigure
{
	FIGURE_UNKNOWN,
	FIGURE_CUBE,
	FIGURE_PYRAMID,
	FIGURE_PRISM,
	FIGURE_CUBE_CUTTED
};

enum nameMethod
{
	METHOD_UNKNOWN, METHOD_GRADIENT_DESCENT, METHOD_GRADIENT_DESCENT_FAST
};

struct testParameters
{
	nameFigure figure;
	nameMethod method;
	int numContours;
	int indFacetMoved;
	double maxMoveDelta;
	double shiftAngleFirst;
};

void printUsage();
nameFigure parse_figureName(char* figureNameInput);
nameMethod parse_methodName(char* methodNameInput);
int parse_commandLine(int argc, char** argv, testParameters& parameters);
void makePolyhedron(Polyhedron& poly, nameFigure figureParsed);
inline void moveFacetRandom(Polyhedron& polyhedron, double maxMoveDelta,
		int ifacet);

int main(int argc, char** argv)
{
	testParameters parameters;

	if (parse_commandLine(argc, argv, parameters) != EXIT_SUCCESS)
		return EXIT_FAILURE;
	Polyhedron polyhedron;
	makePolyhedron(polyhedron, parameters.figure);
	ShadeContourData* contourData = new ShadeContourData(&polyhedron);
	ShadeContourConstructor* scConstructor = new ShadeContourConstructor(
			&polyhedron, contourData);
	scConstructor->run(parameters.numContours, parameters.shiftAngleFirst);
	moveFacetRandom(polyhedron, parameters.maxMoveDelta, parameters.indFacetMoved);

	polyhedron.correctGlobal(contourData);

	return EXIT_SUCCESS;
}

void printUsage()
{
	printf(
			"Usage: \n"
					"./globalCorrectionCube <figure name> <method> <number_of_contours> "
					"<index of facet to be moved> <max_move_delta> <first angle shift>\n");
	printf("\nPossible figures: cube pyramid prism cube-cutted\n");
	printf("\nPossible methods: gd (gradient descent), gdf (gradient descent - fast)");
}

int parse_commandLine(int argc, char** argv, testParameters& parameters)
{

	if (argc != 7)
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
			&& sscanf(argv[6], "%lf", &parameters.shiftAngleFirst);

	parameters.figure = parse_figureName(figure);
	parameters.method = parse_methodName(method);

	if (!ifCorrectInput)
	{
		ERROR_PRINT("Incorrect input!");
		printUsage();
		return EXIT_FAILURE;
	}
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

	return EXIT_SUCCESS;
}

nameFigure parse_figureName(char* figureNameInput)
{
	if (strcmp(figureNameInput, "cube") == 0)
	{
		return FIGURE_CUBE;
	}
	else if (strcmp(figureNameInput, "pyramid") == 0)
	{
		return FIGURE_PYRAMID;
	}
	else if (strcmp(figureNameInput, "prism") == 0)
	{
		return FIGURE_PRISM;
	}
	else if (strcmp(figureNameInput, "cube-cutted") == 0)
	{
		return FIGURE_CUBE_CUTTED;
	}
	else
	{
		return FIGURE_UNKNOWN;
	}
}

nameMethod parse_methodName(char* methodNameInput)
{
	if (strcmp(methodNameInput, "gd") == 0)
	{
		return METHOD_GRADIENT_DESCENT;
	}
	else if (strcmp(methodNameInput, "gdf") == 0)
	{
		return METHOD_GRADIENT_DESCENT_FAST;
	}
	else
	{
		return METHOD_UNKNOWN;
	}
}

void makePolyhedron(Polyhedron& poly, nameFigure figureParsed)
{
	switch (figureParsed)
	{
	case FIGURE_CUBE:
		poly.makeCube(1., 0., 0., 0.);
		break;
	case FIGURE_PYRAMID:
		poly.makePyramid(3, 1., 1.);
		break;
	case FIGURE_PRISM:
		poly.makePrism(3, 1., 1.);
		break;
	case FIGURE_CUBE_CUTTED:
		poly.makeCubeCutted();
		break;
	default:
		ERROR_PRINT("Unknown figure name!");
		printUsage();
	}
}

static double genRandomDouble(double maxDelta)
{
	srand((unsigned) time(0));
	int randomInteger = rand();
	double randomDouble = randomInteger;
	const double halfRandMax = RAND_MAX * 0.5;
	randomDouble -= halfRandMax;
	randomDouble /= halfRandMax;

	randomDouble *= maxDelta;

	return randomDouble;
}

inline void moveFacetRandom(Polyhedron& polyhedron, double maxMoveDelta,
		int ifacet)
{
	Plane& plane = polyhedron.facets[ifacet].plane;
	plane.norm.x += genRandomDouble(maxMoveDelta);
	plane.norm.y += genRandomDouble(maxMoveDelta);
	plane.norm.z += genRandomDouble(maxMoveDelta);
	plane.dist += genRandomDouble(maxMoveDelta);
	double newNorm = sqrt(qmod(plane.norm));
	plane.norm.norm(1.);
	plane.dist /= newNorm;
}
