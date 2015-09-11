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
 * @file PlaneFromFacet.h
 * @brief Plane generator that uses information about initial planes position
 * to set planes for facets (declaration).
 */

#ifndef PLANEFROMFACET_H_
#define PLANEFROMFACET_H_

#include "Polyhedron_3/Polyhedron_3.h"

/**
 * Plane generator that uses information about initial planes position
 * to set planes for facets.
 */
struct PlaneFromFacet
{
private:
	/** Original planes of the input data (in proper order). */
	std::vector<Plane_3> planes_;
public:
	/**
	 * The default constructor.
	 *
	 * @param planes	Original planes.
	 */
	PlaneFromFacet(std::vector<Plane_3> planes);

	/**
	 * The plane generator operator.
	 *
	 * @param facet		The processed facet.
	 * @return		The best plane.
	 */
	Polyhedron_3::Plane_3 operator()(Polyhedron_3::Facet& facet);

	/**
	 * Constructs the indices of planes that are the closest to the facets
	 * of the  given polyhedron.
	 */
	std::vector<int> constructPlanesIndex(Polyhedron_3 polyhedron);
};

#endif /* PLANEFROMFACET_H_ */
