/*
 * EdgeContourAssociation.cpp
 *
 *  Created on: 12.03.2013
 *      Author: iliya
 */

#include "PolyhedraCorrectionLibrary.h"

EdgeContourAssociation::EdgeContourAssociation() :
				indContour(-1),
				indNearestSide(-1),
				ifProperDirection(false),
				weight(0.) {
}

EdgeContourAssociation::EdgeContourAssociation(
		int indContour_orig,
		int indNearestSide_orig,
		bool ifProperDirection_orig,
		double weight_orig) :
				indContour(indContour_orig),
				indNearestSide(indNearestSide_orig),
				ifProperDirection(ifProperDirection_orig),
				weight(weight_orig) {
}

EdgeContourAssociation::EdgeContourAssociation(
		int indContour_orig) :
				indContour(indContour_orig),
				indNearestSide(-1),
				ifProperDirection(false),
				weight(0.) {
}

EdgeContourAssociation::EdgeContourAssociation(
		const EdgeContourAssociation& orig) :
				indContour(orig.indContour),
				indNearestSide(orig.indNearestSide),
				ifProperDirection(orig.ifProperDirection),
				weight(orig.weight) {
}

EdgeContourAssociation::~EdgeContourAssociation() {
}

