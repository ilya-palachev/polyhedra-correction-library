/*
 * Copyright (c) 2009-2015 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file PlaneFromFacet.cpp
 * @brief Plane generator that uses information about initial planes position
 * to set planes for facets (implementation).
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Recoverer/PlaneFromFacet.h"

PlaneFromFacet::PlaneFromFacet(std::vector<Plane_3> planes) :
	planes_(planes)
{
	DEBUG_START;
	DEBUG_END;
}

/**
 * Looks for the best plane that is the most close to the given one.
 *
 * @param planesOriginal	The array of original planes.
 * @param facet			The processed facet.
 * @return			The ID of the best plane.
 */
static int findBestPlaneOriginal(std::vector<Plane_3> planesOriginal,
		Polyhedron_3::Facet& facet)
{
	DEBUG_START;
	double minimum = 1e100;
	int iPlaneBest = -1;
	for (int i = 0; i < (int) planesOriginal.size(); ++i)
	{
		Plane_3 plane = planesOriginal[i];
		auto halfedgeBegin = facet.facet_begin();
		auto halfedge = halfedgeBegin;
		double errorSumSquares = 0.;
		bool ifAllOnPlane = true;
		do
		{
			auto point = halfedge->vertex()->point();
			ifAllOnPlane &= plane.has_on(point);
			double error = point.x() * plane.a()
				+ point.y() * plane.b()
				+ point.z() * plane.c() + plane.d();
			error = error * error;
			errorSumSquares += error;
			++halfedge;
		}
		while (halfedge != halfedgeBegin);

		if (ifAllOnPlane)
		{
			std::cerr << "Found exact plane: " << plane
				<< std::endl;
			iPlaneBest = i;
			break;
		}

		if (errorSumSquares < minimum)
		{
			iPlaneBest = i;
			minimum = errorSumSquares;
		}
	}
	if (iPlaneBest < 0)
	{
		ERROR_PRINT("Failed to find best plane");
		exit(EXIT_FAILURE);
	}
	DEBUG_END;
	return iPlaneBest;
}

Polyhedron_3::Plane_3 PlaneFromFacet::operator()(Polyhedron_3::Facet& facet)
{
	DEBUG_START;
	int iPlane = findBestPlaneOriginal(planes_, facet);
	Plane_3 planeBest = planes_[iPlane];
	double a = planeBest.a();
	double b = planeBest.b();
	double c = planeBest.c();
	double d = planeBest.d();
	double norm = sqrt(a * a + b * b + c * c);
	a /= norm;
	b /= norm;
	c /= norm;
	d /= norm;
	if (d > 0.)
	{
		a = -a;
		b = -b;
		c = -c;
		d = -d;
	}
	planeBest = Plane_3(a, b, c, d);
	DEBUG_END;
	return planeBest;
}


std::vector<int> PlaneFromFacet::constructPlanesIndex(Polyhedron_3 polyhedron)
{
	DEBUG_START;
	int iFacet = 0;
 	std::vector<int> index(polyhedron.size_of_facets());
	std::set<int> usedIndices;
	for (auto facet = polyhedron.facets_begin();
			facet != polyhedron.facets_end(); ++facet)
	{
		int iBestPlane = findBestPlaneOriginal(planes_, *facet);
		index[iFacet] = iBestPlane;
		usedIndices.insert(iBestPlane);
		++iFacet;
	}
	if (usedIndices.size() < index.size())
	{
		std::cerr << "Equal indices: " << usedIndices.size() << " < "
			<< index.size() << std::endl;
		exit(EXIT_FAILURE);
	}
	DEBUG_END;
	return index;
}
