#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <string.h>

#include "Polyhedron.h"
#include "Gauss_test.h"

void printUsage()
{
	printf("Usage: \n"
			"./globalCorrectionCube <number_of_contours> <max_move_delta>\n");
}

int main(int argc, char** argv) {

	int numContours;
	double maxMoveDelta;

	if (argc != 3)
	{
		printUsage();
		return 1;
	}
	if( sscanf(argv[1], "%d", &numContours) != 1)
	{
		printUsage();
		return 2;
	}
	if ( sscanf(argv[1], "%lf", &maxMoveDelta) != 1)
	{
		printUsage();
		return 3;
	}
    Polyhedron poly;
    poly.corpolTest(numContours, maxMoveDelta);
    return 0;
}









