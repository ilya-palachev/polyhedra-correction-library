/* 
 * Copyright (c) 2009-2013 Ilya Palachev <iliyapalachev@gmail.com>
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

#ifndef SIZECALCULATOR_H_
#define SIZECALCULATOR_H_

#include "Analyzers/PAnalyzer/PAnalyzer.h"

class SizeCalculator: public PAnalyzer
{
private:

	double consection_x(double y, double z);

public:
	SizeCalculator();
	SizeCalculator(shared_ptr<Polyhedron> p);
	~SizeCalculator();

	double calculate_J11(int N);

	double volume();
	double areaOfSurface();
	double areaOfFacet(int iFacet);
	void J(double& Jxx, double& Jyy, double& Jzz, double& Jxy, double& Jyz,
			double& Jxz);
	void get_center(double& xc, double& yc, double& zc);
	void inertia(double& l0, double& l1, double& l2, Vector3d& v0, Vector3d& v1,
			Vector3d& v2);
	
	void printSortedByAreaFacets(void);
};

#endif /* SIZECALCULATOR_H_ */
