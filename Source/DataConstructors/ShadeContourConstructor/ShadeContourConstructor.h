/*
 * ShadeContourConstructor.h
 *
 *  Created on: 17.05.2013
 *      Author: iliya
 */

#ifndef SHADECONTOURCONSTRUCTOR_H_
#define SHADECONTOURCONSTRUCTOR_H_

/*
 *
 */
class ShadeContourConstructor: public PDataConstructor
{
private:
	ShadeContourData* data;
	bool* bufferBool;
	int* bufferInt0;
	int* bufferInt1;

	EdgeDataPtr edgeData;

	SContour& createContour(int idOfContour, Plane planeOfProjection);
	bool edgeIsVisibleOnPlane(Edge edge, Plane planeOfProjection);
	bool collinearVisibility(int v0processed, int v1processed,
			Plane planeOfProjection, int ifacet);

public:
	ShadeContourConstructor(Polyhedron* p, ShadeContourData* d);
	~ShadeContourConstructor();
	void run(int numContoursNeeded, double firstAngle);
};

#endif /* SHADECONTOURCONSTRUCTOR_H_ */
