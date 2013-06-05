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

	inline SContour& operator[] (const int i)
	{
		return contours[i];
	}

	inline int getNumContours()
	{
		return numContours;
	}
};

#endif /* SHADECONTOURDATA_H_ */
