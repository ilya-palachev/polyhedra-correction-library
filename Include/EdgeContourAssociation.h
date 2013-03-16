/*
 * EdgeContourAssociation.h
 *
 *  Created on: 12.03.2013
 *      Author: iliya
 */

#ifndef EDGECONTOURASSOCIATION_H_
#define EDGECONTOURASSOCIATION_H_

#include "PolyhedraCorrectionLibrary.h"

class EdgeContourAssociation {
public:
	int indContour;
	int indNearestSide;
	bool ifProperDirection;
	double weight;

	EdgeContourAssociation();
	EdgeContourAssociation(
			int indContour_orig,
			int indNearestSide_orig,
			bool ifProperDirection_orig,
			double weight_orig);
	EdgeContourAssociation(
			int indContour_orig);
	EdgeContourAssociation(
			const EdgeContourAssociation& orig);

	~EdgeContourAssociation();
};

#endif /* EDGECONTOURASSOCIATION_H_ */
