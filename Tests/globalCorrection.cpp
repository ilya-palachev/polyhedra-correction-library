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

int main(
		int argc,
		char** argv) {

	char* figure = new char[255];
	int numContours;
	int indFacetMoved;
	double maxMoveDelta;

	if (argc != 5) {
		ERROR_PRINT("Wrong number of arguments!");
		printUsage();
		return 1;
	}

	bool ifCorrectInput = sscanf(argv[1], "%s", figure)
			&& sscanf(argv[2], "%d", &numContours)
			&& sscanf(argv[3], "%d", &indFacetMoved)
			&& sscanf(argv[4], "%lf", &maxMoveDelta);
	if (!ifCorrectInput) {
		ERROR_PRINT("Incorrect input!");
		printUsage();
	}

	Polyhedron poly;
	nameFigure figureParsed = parse_figureName(figure);

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
		return -1;
	}

	poly.corpolTest(numContours, indFacetMoved, maxMoveDelta);

	if (figure) {
		delete[] figure;
		figure = NULL;
	}
	return 0;
}

void printUsage() {
	printf("Usage: \n"
			"./globalCorrectionCube <figure name> <number_of_contours> "
			"<index of facet to be moved> <max_move_delta>\n");
	printf("\nPossible figures: cube pyramid prism cube-cutted\n");
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
