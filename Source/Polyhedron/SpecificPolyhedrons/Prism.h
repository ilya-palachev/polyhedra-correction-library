/*
 * Prism.h
 *
 *  Created on: 17.06.2013
 *      Author: ilya
 */

#ifndef PRISM_H_
#define PRISM_H_

class Prism: public Polyhedron {
private:
	int numVerticesBase;
	double height;
	double radius;

	void init();
public:
	Prism();
	Prism(int nv, double h, double r);
	~Prism();
};

#endif /* PRISM_H_ */
