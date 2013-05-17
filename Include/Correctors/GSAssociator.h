/*
 * GSAssociator.h
 *
 *  Created on: 17.05.2013
 *      Author: iliya
 */

#ifndef GSASSOCIATOR_H_
#define GSASSOCIATOR_H_

class GSAssociator: public GlobalShadeCorrector
{
private:
	int iContour;
	int iFacet;
	int iEdge;

	Polyhedron* polyhedronTmp;
	double* bufDouble;

	Vector3d v0_projected;
	Vector3d v1_projected;

	Vector3d v0_nearest;
	Vector3d v1_nearest;

	int iSideDistMin0;
	int iSideDistMin1;

	void preinit();

	void run(int iContourIn, int iFacetIn, int iEdgeIn);

	int init();
	int checkVisibility();
	double calculateVisibility();
	int checkAlreadyAdded();
	int projectEdge();
	int checkExtinction();

	int findNearestPoint(Vector3d v_projected, Vector3d& v_nearest);

	double calculateArea(Orientation orientation);

	void add(Orientation orientation);
	void findBounds(Orientation orientation, int& iResultBegin,
			int& iResultEnd);

	double calculateWeight();

public:
	GSAssociator(const GlobalShadeCorrector* corrector);
	~GSAssociator();
};


#endif /* GSASSOCIATOR_H_ */
