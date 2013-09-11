#include "PolyhedraCorrectionLibrary.h"

const double THRESHOLD_ESSENTIAL_ASSOCIATION = 0.1;

void Edge::my_fprint(FILE* file) const
{
	DEBUG_START;
	REGULAR_PRINT(file, COLOUR_YELLOW "Printing content of edge #%d" COLOUR_WHITE,
			id);
	REGULAR_PRINT(file, "\tid = %d (id of the edge)\n", id);
	REGULAR_PRINT(file, "\tv0 = %d (first vertex)\n", v0);
	REGULAR_PRINT(file, "\tv1 = %d (second vertex)\n", v1);
	REGULAR_PRINT(file, "\tf0 = %d (first facet including this "
			"edge)\n", f0);
	REGULAR_PRINT(file, "\tf1 = %d (second facet including this"
			"edge)\n", f1);
	REGULAR_PRINT(file, "\tnumc = %ld (number of contours, from which this"
			"edge is visible)\n", assocList.size());
	REGULAR_PRINT(file, "\tThese are that contours:\n");
	REGULAR_PRINT(file,
			"\ti\t|\tid of contour\t|\tnearest side\t|\tdirection\t|\tweight\n");

	int i = 0;
	int numContoursEssential = 0;
	for (list<EdgeContourAssociation>::iterator iter = assocList.begin();
			iter != assocList.end(); ++iter, ++i)
	{
		REGULAR_PRINT(file, "\t%d\t|\t%d\t\t|\t%d\t\t|\t%d\t\t|\t%lf\n", i,
				iter->indContour, iter->indNearestSide, iter->ifProperDirection,
				iter->weight);
		if (iter->weight >= THRESHOLD_ESSENTIAL_ASSOCIATION)
		{
			++numContoursEssential;
		}
	}
	REGULAR_PRINT(file, "\t number of essential contours: %d (weight >= 0.1)\n",
			numContoursEssential);
	DEBUG_END;
}
