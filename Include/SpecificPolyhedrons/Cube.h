/*
 * Cube.h
 *
 *  Created on: 17.06.2013
 *      Author: ilya
 */

#ifndef CUBE_H_
#define CUBE_H_

class Cube : public Polyhedron
{
private:
	double height;
	Vector3d center;

	void init();
public:
	Cube();
	Cube(double h);
	Cube(double h, double x, double y, double z);
	Cube(double h, Vector3d c);
	~Cube();
};

#endif /* CUBE_H_ */
