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

#ifndef POINTSHIFTER_H_
#define POINTSHIFTER_H_

#include "Correctors/PCorrector/PCorrector.h"
#include "Vector3d.h"

class PointShifter : public PCorrector
{
private:
	int n;
	double *x;
	double *x1;
	double *fx;
	double *A;
	int id;
	int *sum;
	double *tmp0;
	double *tmp1;
	double *tmp2;
	double *tmp3;

	void calculateFunctional();
	void calculateFunctionalDerivative();
	void calculateFunctionalDerivative2();

	void init();

public:
	PointShifter();
	PointShifter(PolyhedronPtr p);
	PointShifter(Polyhedron *p);
	~PointShifter();

	void run(int id, Vector3d delta);
};

#endif /* POINTSHIFTER_H_ */
