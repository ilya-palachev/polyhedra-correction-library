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
 * @file TrustedEdgesDetector
 * @brief The detector of edges that have iamges on multiple (at least three)
 * support directions, i. e. groups of planes that are intersecting (with some
 * precision) by some spatial line (implementation).
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "halfspaces_intersection.h"
#include "Recoverer/TrustedEdgesDetector.h"

TrustedEdgesDetector::TrustedEdgesDetector(std::vector<Plane_3> planes,
		double threshold) :
	planes_(planes),
	directions_(std::vector<Point_3>(planes.size())),
	values_(VectorXd(planes.size())),
	clusters_(),
	index_(std::vector<int>(planes.size())),
	sphere_(),
	vertices_(),
	thresholdClusterError_(threshold)
{
	DEBUG_START;
	for (int i = 0; i < (int) planes.size(); ++i)
	{
		Plane_3 plane = planes[i];
		Point_3 direction(plane.a(), plane.b(), plane.c());
		directions_[i] = direction;
		values_[i] = -plane.d();
	}
	DEBUG_END;
}

TrustedEdgesDetector::~TrustedEdgesDetector()
{
	DEBUG_START;
	DEBUG_END;
}

void TrustedEdgesDetector::initialize()
{
	DEBUG_START;
	/* 1. Build the hull of directions. */
	Polyhedron_3 hull;
	CGAL::convex_hull_3(directions_.begin(), directions_.end(), hull);
	if (hull.size_of_vertices() != directions_.size())
	{
		std::cerr << "Hull contains only " << hull.size_of_vertices()
			<< " of " << directions_.size() << " verices!"
			<< std::endl;
		exit(EXIT_FAILURE);
	}

	/* 2. Build vertices iterator access array: */
	sphere_ = hull;
	int iVertex = 0;
	vertices_ = std::vector<Polyhedron_3::Vertex_iterator>(
			directions_.size());
	CGAL::Origin origin;
	for (auto vertex = sphere_.vertices_begin();
			vertex != sphere_.vertices_end(); ++vertex)
	{
		double lengthMinimal = 1e100;
		int iDirectionBest = 0;
		Vector_3 vector = vertex->point() - origin;
		for (int i = 0; i < (int) directions_.size(); ++i)
		{
			Vector_3 difference = vector - directions_[i];
			double length = sqrt(difference.squared_length());
			if (length < lengthMinimal)
			{
				lengthMinimal = length;
				iDirectionBest = i;
			}
		}
		vertices_[iDirectionBest] = vertex;
		++iVertex;
	}
	DEBUG_END;
}

std::vector<std::pair<Point_3, Point_3>>
TrustedEdgesDetector::getSortedSegments(Polyhedron_3 polyhedron)
{
	DEBUG_START;
	std::vector<Point_3> points(polyhedron.size_of_vertices());
	int iVertex = 0;
	for (auto vertex = polyhedron.vertices_begin();
			vertex != polyhedron.vertices_end(); ++vertex)
	{
		points[iVertex] = vertex->point();
		++iVertex;
	}

	std::vector<std::pair<std::pair<int, int>, double>>
		edges(polyhedron.size_of_halfedges() / 2);
	int iEdge = 0;
	for (auto halfedge = polyhedron.halfedges_begin();
			halfedge != polyhedron.halfedges_end(); ++halfedge)
	{
		int iVertex = halfedge->vertex()->id;
		int iVertexOpposite = halfedge->opposite()->vertex()->id;
		if (iVertex < iVertexOpposite)
		{
			Vector_3 vector = points[iVertexOpposite]
				- points[iVertex];
			double length = sqrt(vector.squared_length());
			edges[iEdge] = std::make_pair(
					std::make_pair(iVertex,
						iVertexOpposite), length);
			++iEdge;
		}
	}

	std::sort(edges.begin(), edges.end(),
			[](std::pair<std::pair<int, int>, double> a,
				std::pair<std::pair<int, int>, double> b)
	{
		return a.second < b.second;
	});

	std::vector<std::pair<Point_3, Point_3>> segments(edges.size());
	int iSegment = 0;
	for (auto edge: edges)
	{
		auto segment = std::make_pair(points[edge.first.first],
				points[edge.first.second]);
		segments[iSegment] = segment;
		double length = sqrt((segment.second
					- segment.first).squared_length());
		std::cerr << "Segment #" << iSegment << ": length is " << length
			<<std::endl;
		++iSegment;
	}
	DEBUG_END;
	return segments;
}

std::vector<TrustedEdgeInformation> TrustedEdgesDetector::run(
		Polyhedron_3 polyhedron)
{
	DEBUG_START;
	initialize();
	auto segments = getSortedSegments(polyhedron);
	std::vector<TrustedEdgeInformation> nothing;
	DEBUG_END;
	return nothing;
}
