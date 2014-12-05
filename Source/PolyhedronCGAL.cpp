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
 * @file PolyhedronCGAL.cpp
 * @brief Inclusions from CGAL library (implementation of additional features).
 */

#include "PolyhedronCGAL.h"
#include "DebugPrint.h"

std::vector<Vector_3> Polyhedron_3::getVertices()
{
	DEBUG_START;
	std::vector<Vector_3> vertices;
	for (auto v = vertices_begin(); v != vertices_end(); ++v)
	{
		vertices.push_back(v->point() - CGAL::ORIGIN);
	}
	DEBUG_END;
	return vertices;
}
