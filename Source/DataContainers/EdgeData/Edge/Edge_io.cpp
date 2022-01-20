/*
 * Copyright (c) 2009-2016 Ilya Palachev <iliyapalachev@gmail.com>
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

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "DataContainers/EdgeData/Edge/Edge.h"

const double THRESHOLD_ESSENTIAL_ASSOCIATION = 0.1;

void Edge::my_fprint(FILE *file) const
{
	DEBUG_START;
	REGULAR_PRINT(file, COLOUR_YELLOW "Printing content of edge #%d" COLOUR_WHITE, id);
	REGULAR_PRINT(file, "\tid = %d (id of the edge)\n", id);
	REGULAR_PRINT(file, "\tv0 = %d (first vertex)\n", v0);
	REGULAR_PRINT(file, "\tv1 = %d (second vertex)\n", v1);
	REGULAR_PRINT(file,
				  "\tf0 = %d (first facet including this "
				  "edge)\n",
				  f0);
	REGULAR_PRINT(file,
				  "\tf1 = %d (second facet including this"
				  "edge)\n",
				  f1);
	REGULAR_PRINT(file,
				  "\tnumc = %ld (number of contours, from which this"
				  "edge is visible)\n",
				  (long int)assocList.size());
	REGULAR_PRINT(file, "\tThese are that contours:\n");
	REGULAR_PRINT(file, "\ti\t|\tid of contour\t|\tnearest side\t|\tdirection\t|\tweight\n");

	int i = 0;
	int numContoursEssential = 0;
	for (std::list<EdgeContourAssociation>::iterator iter = assocList.begin(); iter != assocList.end(); ++iter, ++i)
	{
		REGULAR_PRINT(file, "\t%d\t|\t%d\t\t|\t%d\t\t|\t%d\t\t|\t%lf\n", i, iter->indContour, iter->indNearestSide,
					  iter->ifProperDirection, iter->weight);
		if (iter->weight >= THRESHOLD_ESSENTIAL_ASSOCIATION)
		{
			++numContoursEssential;
		}
	}
	REGULAR_PRINT(file, "\t number of essential contours: %d (weight >= 0.1)\n", numContoursEssential);
	DEBUG_END;
}
