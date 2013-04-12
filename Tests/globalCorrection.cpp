#include "PolyhedraCorrectionLibrary.h"

enum nameFigure {
	NAME_UNKNOWN = -1,
	NAME_CUBE = 0,
	NAME_PYRAMID = 1,
	NAME_PRISM = 2,
	NAME_CUBE_CUTTED = 3
};

void printUsage();

nameFigure parse_figureName(
		char* figureNameInput);

int parse_commandLine(
		int argc,
		char** argv,
		char* figure,
		int& numContours,
		int& indFacetMoved,
		double& maxMoveDelta,
		double& shiftAngleFirst);

int makePolyhedron(
		Polyhedron& poly,
		nameFigure figureParsed);

int main(
		int argc,
		char** argv) {

	char* figure = new char[255];
	int numContours;
	int indFacetMoved;
	double maxMoveDelta;
	double shiftAngleFirst;

	if (parse_commandLine(argc, argv, figure, numContours, indFacetMoved,
			maxMoveDelta, shiftAngleFirst) != EXIT_SUCCESS) {
		return EXIT_FAILURE;
	}
	nameFigure figureParsed = parse_figureName(figure);
	Polyhedron poly;
	if (makePolyhedron(poly, figureParsed) != EXIT_SUCCESS) {
		return EXIT_FAILURE;
	}
	poly.corpolTest(numContours, indFacetMoved, maxMoveDelta, shiftAngleFirst);

	if (figure) {
		delete[] figure;
		figure = NULL;
	}
	return EXIT_SUCCESS;
}

void printUsage() {
	printf("Usage: \n"
			"./globalCorrectionCube <figure name> <number_of_contours> "
			"<index of facet to be moved> <max_move_delta> <first angle shift>\n");
	printf("\nPossible figures: cube pyramid prism cube-cutted\n");
}

int parse_commandLine(
		int argc,
		char** argv,
		char* figure,
		int& numContours,
		int& indFacetMoved,
		double& maxMoveDelta,
		double& shiftAngleFirst) {

	if (argc != 5) {
		ERROR_PRINT("Wrong number of arguments!");
		printUsage();
		return EXIT_FAILURE;
	}

	bool ifCorrectInput = sscanf(argv[1], "%s", figure)
			&& sscanf(argv[2], "%d", &numContours)
			&& sscanf(argv[3], "%d", &indFacetMoved)
			&& sscanf(argv[4], "%lf", &maxMoveDelta)
			&& sscanf(argv[4], "%lf", &shiftAngleFirst);
	if (!ifCorrectInput) {
		ERROR_PRINT("Incorrect input!");
		printUsage();
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

nameFigure parse_figureName(
		char* figureNameInput) {
	if (strcmp(figureNameInput, "cube") == 0) {
		return NAME_CUBE;
	} else if (strcmp(figureNameInput, "pyramid") == 0) {
		return NAME_PYRAMID;
	} else if (strcmp(figureNameInput, "prism") == 0) {
		return NAME_PRISM;
	} else if (strcmp(figureNameInput, "cube-cutted") == 0) {
		return NAME_CUBE_CUTTED;
	} else {
		return NAME_UNKNOWN;
	}
}

int makePolyhedron(
		Polyhedron& poly,
		nameFigure figureParsed) {
	switch (figureParsed) {
	case NAME_CUBE:
		poly.makeCube(1., 0., 0., 0.);
		break;
	case NAME_PYRAMID:
		poly.makePyramid(3, 1., 1.);
		break;
	case NAME_PRISM:
		poly.makePrism(3, 1., 1.);
		break;
	case NAME_CUBE_CUTTED:
		poly.makeCubeCutted();
		break;
	default:
		ERROR_PRINT("Unknown figure name!");
		printUsage();
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
