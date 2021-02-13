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

#ifndef POINTSHIFTERWEIGHTED_H_
#define POINTSHIFTERWEIGHTED_H_

#include "Correctors/PCorrector/PCorrector.h"
#include "Vector3d.h"

class PointShifterWeighted : public PCorrector
{
private:
	int n;
	double *x;
	double *x1;
	double *fx;
	double *A;
	int id;
	bool *khi;
	double *tmp0;
	double *tmp1;
	double *tmp2;
	double *tmp3;
	double K;

	void calculateFunctional();
	void calculateFuncitonalDerivative();

	void init();

public:
	PointShifterWeighted();
	PointShifterWeighted(PolyhedronPtr p);
	PointShifterWeighted(Polyhedron *p);
	~PointShifterWeighted();

	void run(int id, Vector3d delta);
};

#endif /* POINTSHIFTERWEIGHTED_H_ */
