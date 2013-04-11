#include "PolyhedraCorrectionLibrary.h"

using namespace std;

void Edge::my_fprint(
		FILE* file) {
#ifndef NDEBUG
	fprintf(file, "Printing content of edge #%d", id);
	fprintf(file, "\tid = %d (id of the edge)\n", id);
	fprintf(file, "\tv0 = %d (first vertex)\n", v0);
	fprintf(file, "\tv1 = %d (second vertex)\n", v1);
	fprintf(file, "\tf0 = %d (first facet including this "
			"edge)\n", f0);
	fprintf(file, "\tf1 = %d (second facet including this"
			"edge)\n", f1);
	fprintf(file, "\tnumc = %d (number of contours, from which this"
			"edge is visible)\n", assocList.size());
	fprintf(file, "\tThese are that contours:\n");
	fprintf(file,
			"\ti\t|\tid of contour\t|\tnearest side\t|\tdirection\t|\tweight\n");

	int i = 0;
	for (list<EdgeContourAssociation>::iterator iter = assocList.begin();
			iter != assocList.end(); ++iter, ++i) {

		fprintf(file, "\t%d\t|\t%d\t\t|\t%d\t\t|\t%d\t\t|\t%lf\n", i,
				iter->indContour, iter->indNearestSide, iter->ifProperDirection,
				iter->weight);
	}
#endif
}
