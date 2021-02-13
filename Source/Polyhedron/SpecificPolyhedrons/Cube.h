/*
 * Copyright (c) 2009-2016 Ilya Palachev <iliyapalachev@gmail.com>
 *
 * This file is part of Polyhedra Correction Library.
 *
 * Polyhedra Correction Library is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Polyhedra Correction Library is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Polyhedra Correction Library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CUBE_H_
#define CUBE_H_

#include <memory>

#include "Polyhedron/Polyhedron.h"
#include "Vector3d.h"

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
	virtual ~Cube();
};

#endif /* CUBE_H_ */
