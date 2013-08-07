/*
 * GSAssociator.h
 *
 *  Created on: 17.05.2013
 *      Author: iliya
 */

#ifndef GSASSOCIATOR_H_
#define GSASSOCIATOR_H_

enum Orientation
{
	ORIENTATION_LEFT, ORIENTATION_RIGHT
};

class GSAssociator: public GlobalShadeCorrector
{
private:
	int iContour;
	int iFacet;
	int iEdge;
	EdgeSetIterator edge;

	Polyhedron* polyhedronTmp;
	double* bufDouble;

	Vector3d v0_projected;
	Vector3d v1_projected;

	Vector3d v0_nearest;
	Vector3d v1_nearest;

	int iSideDistMin0;
	int iSideDistMin1;


	int init();
	int checkVisibility();
	double calculateVisibility();
	int checkAlreadyAdded();
	void projectEdge();
	int checkExtinction();

	int findNearestPoint(Vector3d v_projected, Vector3d& v_nearest,
			double& distMin);

	double calculateArea(Orientation orientation);

	void add(Orientation orientation);
	void findBounds(Orientation orientation, int& iResultBegin,
			int& iResultEnd);

	double calculateWeight();

public:
	GSAssociator(GlobalShadeCorrector* corrector);
	~GSAssociator();

	void preinit();
	void run(int iContourIn, int iFacetIn, int iEdgeIn);

};


#endif /* GSASSOCIATOR_H_ */
