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

#ifndef POINTSHIFTERLINEAR_H_
#define POINTSHIFTERLINEAR_H_

#include "Vector3d.h"
#include "Correctors/PCorrector/PCorrector.h"

class PointShifterLinear: public PCorrector
{
private:
	/* The ID of shifted vertex */
	int id;

	/* Auxiliary arrays (for calculating movements of facets) */
	double* x;
	double* y;
	double* z;

	/* Auxiliary arrays (for saving initial values) */
	double* xold;
	double* yold;
	double* zold;

	/* Auxiliary arrays (for calculatind movements of vertices) */
	double* A;
	double* B;

	/* Coefficient */
	double K;


	double calculateError();
	double calculateDeform();
	void moveFacets();
	void moveVerticesGlobal();
	void moveVerticesLocal();
public:
	PointShifterLinear();
	PointShifterLinear(PolyhedronPtr p);
	PointShifterLinear(Polyhedron* p);
	~PointShifterLinear();

	void runGlobal(int id, Vector3d delta);
	void runLocal(int id, Vector3d delta);
	void runTest(int id, Vector3d delta, int mode,
		int& num_steps, double& norm_sum);
	void runPartial(int id, Vector3d delta, int num);
};

#endif /* POINTSHIFTERLINEAR_H_ */
