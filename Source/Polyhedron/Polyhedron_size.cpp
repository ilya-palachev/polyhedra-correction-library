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

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Polyhedron/Polyhedron.h"
#include "Analyzers/SizeCalculator/SizeCalculator.h"

double Polyhedron::calculate_J11(int N)
{
	DEBUG_START;
	SizeCalculator *sizeCalculator = new SizeCalculator(get_ptr());
	double ret = sizeCalculator->calculate_J11(N);
	delete sizeCalculator;
	DEBUG_END;
	return ret;
}

double Polyhedron::volume()
{
	DEBUG_START;
	SizeCalculator *sizeCalculator = new SizeCalculator(get_ptr());
	double ret = sizeCalculator->volume();
	delete sizeCalculator;
	DEBUG_END;
	return ret;
}

double Polyhedron::areaOfSurface()
{
	DEBUG_START;
	SizeCalculator *sizeCalculator = new SizeCalculator(get_ptr());
	double ret = sizeCalculator->areaOfSurface();
	delete sizeCalculator;
	DEBUG_END;
	return ret;
}

double Polyhedron::areaOfFacet(int iFacet)
{
	DEBUG_START;
	SizeCalculator *sizeCalculator = new SizeCalculator(get_ptr());
	double ret = sizeCalculator->areaOfFacet(iFacet);
	delete sizeCalculator;
	DEBUG_END;
	return ret;
}

void Polyhedron::J(double &Jxx, double &Jyy, double &Jzz, double &Jxy,
				   double &Jyz, double &Jxz)
{
	DEBUG_START;
	SizeCalculator *sizeCalculator = new SizeCalculator(get_ptr());
	sizeCalculator->J(Jxx, Jyy, Jzz, Jxy, Jyz, Jxz);
	delete sizeCalculator;
	DEBUG_END;
}

void Polyhedron::get_center(double &xc, double &yc, double &zc)
{
	DEBUG_START;
	SizeCalculator *sizeCalculator = new SizeCalculator(get_ptr());
	sizeCalculator->get_center(xc, yc, zc);
	delete sizeCalculator;
	DEBUG_END;
}

void Polyhedron::inertia(double &l0, double &l1, double &l2, Vector3d &v0,
						 Vector3d &v1, Vector3d &v2)
{
	DEBUG_START;
	SizeCalculator *sizeCalculator = new SizeCalculator(get_ptr());
	sizeCalculator->inertia(l0, l1, l2, v0, v1, v2);
	delete sizeCalculator;
	DEBUG_END;
}

void Polyhedron::printSortedByAreaFacets(void)
{
	DEBUG_START;
	SizeCalculator *sizeCalculator = new SizeCalculator(get_ptr());
	sizeCalculator->printSortedByAreaFacets();
	delete sizeCalculator;
	DEBUG_END;
}

std::list<FacetWithArea> Polyhedron::getSortedByAreaFacets(void)
{
	DEBUG_START;
	SizeCalculator *sizeCalculator = new SizeCalculator(get_ptr());
	std::list<FacetWithArea> list = sizeCalculator->getSortedByAreaFacets();
	delete sizeCalculator;
	return list;
	DEBUG_END;
}
