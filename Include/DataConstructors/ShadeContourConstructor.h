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

public:
	ShadeContourConstructor(const Polyhedron* p, const ShadeContourData* d);
	~ShadeContourConstructor();
	void run(int numContoursNeeded, double firstAngle);
};

#endif /* SHADECONTOURCONSTRUCTOR_H_ */
