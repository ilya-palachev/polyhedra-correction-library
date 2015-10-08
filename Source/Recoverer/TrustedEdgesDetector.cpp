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

#include "Constants.h"
#include "DebugPrint.h"
#include "DebugAssert.h"
#include "PCLDumper.h"
#include "halfspaces_intersection.h"
#include "Recoverer/TrustedEdgesDetector.h"

double distance(Segment_3 a, Segment_3 b);

TrustedEdgesDetector::TrustedEdgesDetector(SupportFunctionDataPtr data,
		double threshold) :
	data_(data),
	planes_(std::vector<Plane_3>(data->size())),
	directions_(std::vector<Point_3>(data->size())),
	values_(VectorXd(data->size())),
	clusters_(),
	index_(std::vector<int>(data->size())),
	sphere_(),
	vertices_(),
	thresholdClusterError_(threshold)
{
	DEBUG_START;
	for (int i = 0; i < (int) data->size(); ++i)
	{
		auto item = (*data)[i];
		Point_3 direction(item.direction);
		double value = item.value;
		Plane_3 plane(direction.x(), direction.y(), direction.z(),
				-value);
		planes_[i] = plane;
		directions_[i] = direction;
		values_[i] = value;
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

double distance(Point_3 a, Point_3 b)
{
	DEBUG_START;
	double distanceSquared = (a - b).squared_length();
	double result = sqrt(distanceSquared);
	DEBUG_END;
	return result;
}

double distance(Point_3 point, Segment_3 segment)
{
	DEBUG_START;
	Point_3 begin = segment.source();
	Point_3 end = segment.target();
	Vector_3 vector = end - begin;
	double result = 0.;
	if (vector * (point - begin) <= 0.)
	{
		result = distance(point, begin);
	}
	else if (vector * (point - end) >= 0.)
	{
		result = distance(point, end);
	}
	else
	{
		double left = distance(point, begin);
		double right = distance(point, end);
		double difference = fabs(left - right);
		while (difference > EPS_MIN_DOUBLE * 100.)
		{
			Point_3 middle = 0.5 * (begin + end);
			if (left > right)
			{
				begin = middle;
			}
			else
			{
				end = middle;
			}
			left = distance(point, begin);
			right = distance(point, end);
			difference = fabs(left - right);
		}
		result = left;
	}
	DEBUG_END;
	return result;
}

double distance(Segment_3 a, Segment_3 b)
{
	DEBUG_START;
	double aMaximal = std::max(distance(a.source(), b),
			distance(a.target(), b));
	double bMaximal = std::max(distance(b.source(), a),
			distance(b.target(), a));
	double result = std::min(aMaximal, bMaximal);
	DEBUG_END;
	return result;
}

double distance(Segment_3 segment, SupportFunctionDataItemInfo info)
{
	DEBUG_START;
	Point_3 source = segment.source() - (segment.source()
			* info.normalShadow) * info.normalShadow;
	Point_3 target = segment.target() - (segment.target()
			* info.normalShadow) * info.normalShadow;
	Segment_3 segmentProjected(source, target);
	double result = distance(segmentProjected, info.segment);
	DEBUG_END;
	return result;
}

void TrustedEdgesDetector::buildFirstClusters(std::vector<Segment_3> segments)
{
	DEBUG_START;
	std::vector<std::vector<int>> clustersFirst(segments.size());
	for (int iItem = 0; iItem < (int) data_->size(); ++iItem)
	{
		std::cerr << "Processing item " << iItem << std::endl;
		auto item = (*data_)[iItem];
		int iSegmentNearest = 0;
		double distanceMinimal = ALPHA_PLANE_CLUSTER_INFINITY;
		for (int iSegment = 0; iSegment < (int) segments.size();
				++iSegment)
		{
			double length = sqrt(
					segments[iSegment].squared_length());
			double distanceCurrent = distance(segments[iSegment],
					*(item.info));
			if (distanceCurrent < distanceMinimal)
			{
				distanceMinimal = distanceCurrent;
				iSegmentNearest = iSegment;
			}
			if (distanceCurrent <= thresholdClusterError_)
			{
				std::cerr << "   For segment #" << iSegment <<
					" of length " << length <<
					" distance is " << distanceCurrent
					<< std::endl;
			}
		}
		if (distanceMinimal <= thresholdClusterError_)
		{
			clustersFirst[iSegmentNearest].push_back(iItem);
		}
	}

	for (int iSegment = 0; iSegment < (int) segments.size(); ++iSegment)
	{
		std::cerr << "Found " << clustersFirst[iSegment].size()
			<< " items for segment #" << iSegment << " of length "
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

	Polyhedron_3 intersection;
	CGAL::internal::halfspaces_intersection(planes_.begin(), planes_.end(),
			intersection, Kernel());
	intersection.initialize_indices();
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
			"intersection-inside-detector.ply") << intersection;

	std::vector<TrustedEdgeInformation> nothing;
	DEBUG_END;
	return nothing;
}
