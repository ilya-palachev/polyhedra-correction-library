/*
 * Pyramid.h
 *
 *  Created on: 17.06.2013
 *      Author: ilya
 */

#ifndef PYRAMID_H_
#define PYRAMID_H_

class Pyramid: public Polyhedron {
private:
	int numVerticesBase;
	double height;
	double radius;

	void init();
public:
	Pyramid();
	Pyramid(int nv, double h, double r);
	~Pyramid();
};

#endif /* PYRAMID_H_ */
