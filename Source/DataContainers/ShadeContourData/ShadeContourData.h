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
public:
	int numContours;
	SContour* contours;

	ShadeContourData(const Polyhedron* p);
	~ShadeContourData();

	void fscanDefault(char* fileNameContours);
};

#endif /* SHADECONTOURDATA_H_ */
