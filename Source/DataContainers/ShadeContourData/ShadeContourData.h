/*
 * ShadeContourData.h
 *
 *  Created on: 17.05.2013
 *      Author: iliya
 */

#ifndef SHADECONTOURDATA_H_
#define SHADECONTOURDATA_H_

class ShadeContourData : public PData
{
public:
	int numContours;
	SContour* contours;

	ShadeContourData(const Polyhedron* p);
	~ShadeContourData();

	bool fscanDefault(char* fileNameContours);
	bool fscanDefault(FILE* file);

	void fprint(FILE* file);
	void fprintDefault(FILE* file);

	bool operator ==(const ShadeContourData& contourData) const;
	bool operator !=(const ShadeContourData& contourData) const;

};

typedef std::shared_ptr<ShadeContourData> ShadeContourDataPtr;

#endif /* SHADECONTOURDATA_H_ */
