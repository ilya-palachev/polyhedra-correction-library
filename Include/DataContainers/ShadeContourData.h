/*
 * ShadeContourData.h
 *
 *  Created on: 17.05.2013
 *      Author: iliya
 */

#ifndef SHADECONTOURDATA_H_
#define SHADECONTOURDATA_H_

/*
 *
 */
class ShadeContourData : public PData
{
private:
	int numContours;
	SContour* contours;

public:
	ShadeContourData(const Polyhedron* p);
	~ShadeContourData();

	void photographContours(int numContoursNeeded, double firstAngle);
};

#endif /* SHADECONTOURDATA_H_ */
