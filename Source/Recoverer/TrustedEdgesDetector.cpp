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
#include "PCLDumper.h"
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

std::vector<Segment_3> TrustedEdgesDetector::getSortedSegments(
		Polyhedron_3 polyhedron)
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

	std::vector<Segment_3> segments(polyhedron.size_of_halfedges() / 2);
	int iSegment = 0;
	for (auto halfedge = polyhedron.halfedges_begin();
			halfedge != polyhedron.halfedges_end(); ++halfedge)
	{
		int iVertex = halfedge->vertex()->id;
		int iVertexOpposite = halfedge->opposite()->vertex()->id;
		if (iVertex < iVertexOpposite)
		{
			Segment_3 segment(points[iVertexOpposite],
					points[iVertex]);
			segments[iSegment] = segment;
			++iSegment;
		}
	}

	std::sort(segments.begin(), segments.end(),
			[](Segment_3 a, Segment_3 b)
	{
		return a.squared_length() < b.squared_length();
	});
	DEBUG_END;
	return segments;
}

static void printPolyhedronWithColouredBigEdges(Polyhedron_3 polyhedron)
{
	DEBUG_START;
	polyhedron.initialize_indices();
	char *thresholdEdgeLengthString = getenv("THRESHOLD_EDGE_LENGTH");
	Colour red;
	red.red = 255; red.green = 0; red.blue = 0;
	if (thresholdEdgeLengthString)
	{
		char *mistake = NULL;
		double thresholdEdgeLength = strtod(thresholdEdgeLengthString,
				&mistake);
		if (mistake && *mistake)
		{
			std::cerr << "mistake: " << mistake << std::endl;
			exit(EXIT_FAILURE);
		}
		int iHalfedge = 0;
		for (auto halfedge = polyhedron.halfedges_begin();
				halfedge != polyhedron.halfedges_end();
				++halfedge)
		{
			Vector_3 edge = halfedge->vertex()->point()
				- halfedge->opposite()->vertex()->point();
			double length = sqrt(edge.squared_length());
			if (length >= thresholdEdgeLength)
			{
				polyhedron.halfedgeColours[iHalfedge] = red;
			}
			++iHalfedge;
		}
		globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
				"coloured-big-edges.ply") << polyhedron;
	}
	DEBUG_END;
}

/**
 * Calculates the distance between the plane and the segment as a quadratic mean
 * of distance between segment points and plane.
 *
 * @param segment		The segment.
 * @param plane			The plane.
 * @return			The distance between them.
 */
double distance(Segment_3 segment, Plane_3 plane)
{
	DEBUG_START;
	Point_3 direction(plane.a(), plane.b(), plane.c());
	double length = sqrt(direction.squared_length());
	double value = -plane.d();
	double a = (direction * segment.source() - value) / length;
	double b = (direction * segment.target() - value) / length;
	DEBUG_END;
	return std::max(fabs(a), fabs(b));
}

void TrustedEdgesDetector::buildFirstClusters(std::vector<Segment_3> segments)
{
	DEBUG_START;
	std::vector<std::vector<int>> clustersFirst(segments.size());
	for (int iPlane = 0; iPlane < (int) planes_.size(); ++iPlane)
	{
		int iSegmentNearest = INDEX_PLANE_NOT_PROCESSED;
		double errorMinimal = ALPHA_PLANE_CLUSTER_INFINITY;
		Plane_3 plane = planes_[iPlane];
		for (int iSegment = 0; iSegment < (int) segments.size();
				++iSegment)
		{
			Segment_3 segment = segments[iSegment];
			double error = distance(segment, plane);
			if (error < errorMinimal)
			{
				errorMinimal = error;
				iSegmentNearest = iSegment;
			}
		}
		if (errorMinimal <= thresholdClusterError_)
		{
			clustersFirst[iSegmentNearest].push_back(iPlane);
		}
	}
	for (int iSegment = 0; iSegment < (int) segments.size(); ++iSegment)
	{
		std::cerr << "Found " << clustersFirst[iSegment].size()
			<< " planes for segment #" << iSegment << " of length "
			<< sqrt(segments[iSegment].squared_length())
			<< std::endl;
	}
	DEBUG_END;
}

std::vector<TrustedEdgeInformation> TrustedEdgesDetector::run(
		Polyhedron_3 polyhedron)
{
	DEBUG_START;
	initialize();
	auto segments = getSortedSegments(polyhedron);
	printPolyhedronWithColouredBigEdges(polyhedron);
	buildFirstClusters(segments);
	std::vector<TrustedEdgeInformation> nothing;
	DEBUG_END;
	return nothing;
}
