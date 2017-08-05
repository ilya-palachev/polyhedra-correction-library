/*
 * Copyright (c) 2017 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file EdgeInfo.h
 * @brief Information about polyhedron edges, comming from different contours
 * (shadow, reflect) and from planes.
 */

#ifndef EDGE_INFO_H
#define	EDGE_INFO_H
#include "Polyhedron_3/Polyhedron_3.h"

struct EdgePlane_3 : public Plane_3
{
	typedef enum
	{
		EDGE_PLANE_SHADOW = 0,
		EDGE_PLANE_REFLECT = 1,
		EDGE_PLANE_FACET = 2
	} EdgePlaneType;

	EdgePlaneType type; /* unused */
	double weight;      /* unused */

	using Plane_3::Plane_3;
};

struct EdgeInfo
{
	unsigned id;
	bool isReady;  /* unused */
	unsigned type; /* unused */
	double weight; /* unused */

	unsigned facetID1;
	unsigned facetID2;
	Segment_3 initialEdge; /* not contained in std format */
	Segment_3 correctingEdge; /* may be unused */
	std::vector<EdgePlane_3> planes;
};

bool readEdgeInfoFile(const char *path, std::vector<EdgeInfo> &data);
#endif	/* EDGE_INFO_H */
