/*
 * Copyright (c) 2009-2014 Ilya Palachev <iliyapalachev@gmail.com>
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

/**
 * @file SizeCalculator.h
 * @brief The declaration of class representing the engine for calculation of
 * geometric characteristics of the polyhedron.
 */

#ifndef SIZECALCULATOR_H_
#define SIZECALCULATOR_H_

#include <list>

#include "Analyzers/PAnalyzer/PAnalyzer.h"
#include "Polyhedron/Facet/Facet.h"

/**
 * The engine for calculation of geometric characteristics of the polyhedron.
 */
class SizeCalculator: public PAnalyzer
{
private:

	/**
	 * Calculates the sum of lengths of line segments of intersection between
	 * the polyhedron and the line. The line is always collinear with Ox axis,
	 * i. e. its directing vector looks as (1., 0., 0.) and it intersects plane
	 * Oyz in the point (0., y, z), where y and z are the parameters of the
	 * function.
	 *
	 * @param y	Parameter y mentioned above.
	 * @param z Parameter z mentioned above.
	 */
	double consection_x(double y, double z);

public:
	/**
	 * Empty constructor.
	 */
	SizeCalculator();

	/**
	 * Constructor that associates the size-calculator with a given polyhedorn
	 * specified by its shared pointer.
	 *
	 * @param p	Shared pointer to the associated polyhedron.
	 */
	SizeCalculator(shared_ptr<Polyhedron> p);

	/**
	 * Empty destructor.
	 */
	~SizeCalculator();

	/**
	 * Calculates (1, 1) element of the matrix of inertia using the method of
	 * approximation of the polyhedron with parallelepipeds.
	 *
	 * @param N	The number of points used for approximation.
	 *
	 * NOTE: Do not use it! It produces large errors.
	 */
	double calculate_J11(int N);

	/**
	 * Calculates the volume of the polyhedron.
	 */
	double volume();

	/**
	 * Calculates the area of the polyhedron's surface.
	 */
	double areaOfSurface();

	/**
	 * Calcualtes the area of the given facet of the polyhedron.
	 *
	 * @param iFacet	The ID of the facet.
	 */
	double areaOfFacet(int iFacet);

	/**
	 * Calculates the matrix of inertia of the polyhedron.
	 *
	 * @param Jxx	Output reference to the (1, 1) element of the matrix.
	 * @param Jyy	Output reference to the (2, 2) element of the matrix.
	 * @param Jzz	Output reference to the (3, 3) element of the matrix.
	 * @param Jxy	Output reference to the (1, 2) element of the matrix.
	 * @param Jyz	Output reference to the (2, 3) element of the matrix.
	 * @param Jxz	Output reference to the (1, 3) element of the matrix.
	 */
	void J(double& Jxx, double& Jyy, double& Jzz, double& Jxy, double& Jyz,
			double& Jxz);

	/**
	 * Calculates the mass center of the polyhedron.
	 *
	 * @param xc	Output reference to the x-coordinate of the mass center.
	 * @param yc	Output reference to the y-coordinate of the mass center.
	 * @param zc	Output reference to the z-coordinate of the mass center.
	 */
	void get_center(double& xc, double& yc, double& zc);

	/**
	 * Calculates eigenvalues and eigenvectors of the matrix of inertia of the
	 * polyhedron.
	 *
	 * @param l0	Output reference to the 1st eigenvalue.
	 * @param l1	Output reference to the 2nd eigenvalue.
	 * @param l2	Output reference to the 3rd eigenvalue.
	 * @param v0	Output reference to the 1st eigenvector.
	 * @param v1	Output reference to the 2nd eigenvector.
	 * @param v2	Output reference to the 3rd eigenvector.
	 *
	 * NOTE: (As well as i remember) They are sorted so that l0 >= l1 => l2
	 */
	void inertia(double& l0, double& l1, double& l2, Vector3d& v0, Vector3d& v1,
			Vector3d& v2);

	/**
	 * Prints the list of facets sorted by their areas.
	 */
	void printSortedByAreaFacets(void);

	/**
	 * Calculates the list of facets with their areas sorted by their areas.
	 */
	list<struct FacetWithArea> getSortedByAreaFacets(void);

	/**
	 * Calculates the mass center of facet considered as a solid object.
	 *
	 * @param iFacet	The ID of the considered facet.
	 */
	Vector3d calculateFacetCenter(int iFacet);

	/**
	 * Calculate the mass center of the polyhedron's surface.
	 */
	Vector3d calculateSurfaceCenter();
};

#endif /* SIZECALCULATOR_H_ */
