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

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>

#include "Constants.h"
#include "DebugPrint.h"
#include "DebugAssert.h"
#include "PCLDumper.h"
#include "Recoverer/TrustedEdgesDetector.h"
#include "Recoverer/Colouring.h"
#include "DataConstructors/ShadowContourConstructor/ShadowContourConstructor.h"

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

#if 0
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
#endif

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
#if 0
	double aMaximal = std::max(distance(a.source(), b),
			distance(a.target(), b));
	double bMaximal = std::max(distance(b.source(), a),
			distance(b.target(), a));
	double result = std::min(aMaximal, bMaximal);
#endif
#if 0
	double forward = distance(a.source(), b.source())
		+ distance(a.target(), b.target());
	double backward = distance(a.source(), b.target())
		+ distance(a.target(), b.source());
	double result = std::min(forward, backward);
#endif
	Vector_3 u = a.target() - a.source();
	Vector_3 v = b.target() - b.source();
	double forward = ((u - v).squared_length());
	double backward = ((u + v).squared_length());
	double result = std::min(forward, backward);
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
	std::vector<std::set<int>> clustersFirst(segments.size());
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
			clustersFirst[iSegmentNearest].insert(iItem);
		}
	}
	printColouredIntersection(planes_, clustersFirst,
			"first-trusted-clusters.ply");

	for (int iSegment = 0; iSegment < (int) segments.size(); ++iSegment)
	{
		std::cerr << "Found " << clustersFirst[iSegment].size()
			<< " items for segment #" << iSegment << " of length "
			<< sqrt(segments[iSegment].squared_length())
			<< std::endl;
	}
	DEBUG_END;
}

std::ostream &operator<<(std::ostream &stream, std::vector<Point_3> points)
{
	DEBUG_START;
	stream << "ply" << std::endl;
	stream << "format ascii 1.0" << std::endl;
	stream << "comment generated by Polyhedra Correction Library"
		<< std::endl;
	stream << "element vertex " << points.size() << std::endl;
	stream << "property float x" << std::endl;
	stream << "property float y" << std::endl;
	stream << "property float z" << std::endl;
	stream << "element face " << 1 << std::endl;
	stream << "property list uchar uint vertex_indices" << std::endl;
	stream << "end_header" << std::endl;
	double scale = 1e+6;

	for (Point_3 point: points)
	{
		stream << (int) std::floor(scale * point.x()) << " "
			<< (int) std::floor(scale * point.y()) << " "
			<< (int) std::floor(scale * point.z());
		stream << std::endl;
	}
	stream << points.size();
	for (int i = 0; i < (int) points.size(); ++i)
	{
		stream << " " << i;
	}
	stream << std::endl;
	DEBUG_END;
	return stream;
}

inline std::string generateRandomString(const int length)
{
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	std::string string;
	for (int i = 0; i < length; ++i)
	{
		string += alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	return string;
}

inline bool checkFileExists(std::string path)
{
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

inline std::string generateRandomFileName(const char *prefix, int length)
{
	std::string name;
	do
	{
		name = prefix + generateRandomString(length);
	}
	while (checkFileExists(name));
	return name;
}

typedef struct
{
	std::string comment;
	Vector_3 normal;
	std::vector<Point_3> points;
} ContourInfo;

std::ostream &operator<<(std::ostream &stream,
		std::vector<ContourInfo> contours)
{
	DEBUG_START;
	std::vector<std::string> paths;
	for (auto contour: contours)
	{
		std::ofstream temporary;
		auto path = generateRandomFileName("contour_", 16);
		paths.push_back(path);
		temporary.open(path);
		Vector_3 normal = contour.normal;
		for (auto point: contour.points)
		{
			temporary << project(point, normal) << std::endl;
		}
		temporary.close();
	}
	auto imagePath = generateRandomFileName("image_", 16);
	std::string gnuplotCommand;
	gnuplotCommand += "gnuplot -e \"";
	gnuplotCommand += "set term pngcairo ; set output \'";
	gnuplotCommand += imagePath;
	gnuplotCommand += "\' ; plot";
	for (int i = 0; i < (int) contours.size(); ++i)
	{
		if (i != 0)
			gnuplotCommand += " , ";
		gnuplotCommand += " \'";
		gnuplotCommand += paths[i];
		gnuplotCommand += "\' with lines title \'";
		gnuplotCommand += contours[i].comment;
		gnuplotCommand += "\'";
	}
	gnuplotCommand += "\"";
	std::cerr << gnuplotCommand << std::endl;
	int code = system(gnuplotCommand.c_str());
	if (code != 0)
	{
		ERROR_PRINT("gnuplot failed!");
	}
	for (auto path: paths)
		std::remove(path.c_str());
	std::ifstream tmpstream;
	tmpstream.open(imagePath, std::ifstream::in);
	stream << tmpstream.rdbuf();
	tmpstream.close();
	std::remove(imagePath.c_str());
	DEBUG_END;
	return stream;
}

void dumpContours(Polyhedron_3 polyhedron,
		std::vector<std::vector<int>> indexContours,
		std::vector<Vector_3> directionContours,
		std::vector<std::vector<Point_3>> pointContours)
{
	DEBUG_START;
	for (int iContour = 0; iContour < (int) indexContours.size();
			++iContour)
	{
		std::cerr << "Contour #" << iContour << ": ";
		for (int i: indexContours[iContour])
		{
			std::cerr << i << " ";
		}
		std::cerr << std::endl;
		std::cerr << "   normal " << directionContours[iContour]
			<< std::endl;
		std::cerr << "   number of sides "
			<< indexContours[iContour].size() << std::endl;
		std::string name;
		name += "contour-";
		name += std::to_string(iContour);
		name += ".ply";
		globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, name.c_str())
			<< pointContours[iContour];
		
	}
	for (int iContour = 0; iContour < (int) indexContours.size();
			++iContour)
	{
		std::string name;
		name += "contours-comparative-view-";
		name += std::to_string(iContour);
		name += ".png";
		std::vector<ContourInfo> contours;
		ContourInfo contourInitial;
		contourInitial.comment = "initial";
		contourInitial.normal = directionContours[iContour];
		contourInitial.points = pointContours[iContour];
		contours.push_back(contourInitial);
		ContourInfo contourRude;
		contourRude.comment = "rude";
		contourRude.normal = directionContours[iContour];
		contourRude.points = generateProjection(polyhedron,
				directionContours[iContour]).first;
		contours.push_back(contourRude);
		globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, name.c_str())
			<< contours;
	}
	DEBUG_END;
}

void analyzeContours(Polyhedron_3 polyhedron, SupportFunctionDataPtr data)
{
	DEBUG_START;
	int iContourMax = 0;
	for (int i = 0; i < (int) data->size(); ++i)
	{
		auto item = (*data)[i];
		int iContour = item.info->iContour;
		if (iContour > iContourMax)
		{
			iContourMax = iContour;
		}
	}
	std::vector<std::vector<int>> indexContours(iContourMax + 1);
	std::vector<Vector_3> directionContours(iContourMax + 1);
	std::vector<std::vector<Point_3>> pointContours(iContourMax + 1);
	for (int i = 0; i < (int) data->size(); ++i)
	{
		auto item = (*data)[i];
		int iContour = item.info->iContour;
		indexContours[iContour].push_back(i);
		directionContours[iContour] = item.info->normalShadow;
		pointContours[iContour].push_back(item.info->segment.source());
		pointContours[iContour].push_back(item.info->segment.target());
	}
	dumpContours(polyhedron, indexContours, directionContours,
			pointContours);
	DEBUG_END;
}

void dumpClustersRaw(std::vector<std::vector<int>> clusters,
		std::vector<Plane_3> planes)
{
	DEBUG_START;
	int numClusters = clusters.size();
	for (int iCluster = 0; iCluster < numClusters; ++iCluster)
	{
		std::cerr << "Cluster #" << iCluster << ":"
			<< std::endl;
		for (int iFacet: clusters[iCluster])
		{
			std::cerr << " " << iFacet << " -> ";
			std::cerr << planes[iFacet] << std::endl;

		}
		std::cerr << std::endl;
	}
	DEBUG_END;
}

void dumpClusters(Polyhedron_3 polyhedron,
		std::vector<std::vector<int>> clusters,
		std::vector<Plane_3> planes)
{
	DEBUG_START;
	int iFacetCluster = 0;
	for (auto facet = polyhedron.facets_begin();
			facet != polyhedron.facets_end(); ++facet)
	{
		std::cerr << "facet-cluster " << iFacetCluster
			<< std::endl;
		std::cerr << "facet->id = " << facet->id << std::endl;
		std::cerr << "polyhedron.indexPlanes_[" << facet->id
			<< "] = " << polyhedron.indexPlanes_[facet->id]
			<< std::endl;
		int iCluster = polyhedron.indexPlanes_[facet->id];
		if (iCluster > (int) clusters.size())
		{
			ERROR_PRINT("%d > %ld", iCluster,
					clusters.size());
		}
		std::cerr << "Cluster #" << iCluster << ":"
			<< std::endl;
		for (int iFacet: clusters[iCluster])
		{
			std::cerr << " " << iFacet << " -> ";
			std::cerr << planes[iFacet] << std::endl;

		}
		std::cerr << "   cluster plane: " << facet->plane()
			<< std::endl;
		std::cerr << std::endl;
		++iFacetCluster;
	}
	DEBUG_END;
}

template <class PlaneIterator>
double calculateFunctional(
		PlaneIterator planesBegin,
		PlaneIterator planesEnd,
		std::vector<Point_3> directions,
		VectorXd values)
{
	DEBUG_START;
	Polyhedron_3 intersection(planesBegin, planesEnd);
	auto data = intersection.calculateSupportData(directions);
	VectorXd valuesNew = data->supportValues();
	double functional = 0.;
	int numItems = directions.size();
	for (int i = 0; i < numItems; ++i)
	{
		double error = valuesNew(i) - values(i);
		functional += error * error;
	}
	DEBUG_END;
	return functional;
}

void analyzeClustersQuality(Polyhedron_3 polyhedron,
		std::vector<std::vector<int>> clusters,
		std::vector<Plane_3> planes,
		std::vector<Point_3> directions,
		VectorXd values)
{
	DEBUG_START;
	std::list<Plane_3> planesBig;
	for (auto facet = polyhedron.facets_begin();
			facet != polyhedron.facets_end(); ++facet)
		planesBig.push_back(facet->plane());

	int iIntersection = 0;
	typedef std::pair<int, double> BadnessInfo;
	auto comparator = [](BadnessInfo a, BadnessInfo b)
	{
		return a.second < b.second;
	};
	std::set<BadnessInfo, decltype(comparator)> badnesses(comparator);
	double functionalInitial = calculateFunctional(planes.begin(),
			planes.end(), directions, values);
	double functionalJoined = calculateFunctional(planesBig.begin(),
			planesBig.end(), directions, values);
	double diff = functionalJoined - functionalInitial;
	for (auto facet = polyhedron.facets_begin();
			facet != polyhedron.facets_end(); ++facet)
	{
		int iCluster = polyhedron.indexPlanes_[facet->id];
		auto first = planesBig.begin();
		Plane_3 planeCurrent = *first;
		planesBig.erase(first);
		int numPlanes = 0;
		for (int iFacet: clusters[iCluster])
		{
			planesBig.push_back(planes[iFacet]);
			++numPlanes;
		}
		double functional = calculateFunctional(planesBig.begin(),
				planesBig.end(), directions, values);
		badnesses.insert(BadnessInfo(iIntersection, functional));
		while(numPlanes--)
			planesBig.pop_back();
		planesBig.push_back(planeCurrent);
		++iIntersection;
	}
	double diffMinimal = badnesses.begin()->second - functionalJoined;
	for (auto badness: badnesses)
	{
		int iIntersection = badness.first;
		double functional = badness.second;
		double diffInitial = functional - functionalInitial;
		double diffJoined = functional - functionalJoined;
		double percent = diffJoined / diff * 100.;
		std::cerr << "Intersection #" << iIntersection << std::endl;
		std::cerr << "     functional  : " << functional << std::endl;
		std::cerr << "     diff initial: " << diffInitial << std::endl;
		std::cerr << "     diff joined : " << diffJoined << std::endl;
		std::cerr << "     percent     : " << percent << std::endl;
		double ratio = 1. - diffJoined / diffMinimal;
		std::cerr << "     ratio       : " << ratio << std::endl;
		Colour colour;
		if (ratio > 0)
		{
			unsigned char component = (unsigned char)
				(255. * ratio);
			colour.red = component;
			colour.green = component;
			colour.blue = component;
			std::cerr << "     component   : " << (int) component
				<< std::endl;
		}
		else
		{
			colour.red = 255;
			colour.green = 0;
			colour.blue = 0;
		}
		polyhedron.facetColours[iIntersection] = colour;
	}
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
			"coloured-badness.ply") << polyhedron;
	DEBUG_END;
}

std::vector<TrustedEdgeInformation> TrustedEdgesDetector::run(
		Polyhedron_3 polyhedron,
		std::vector<std::vector<int>> clusters)
{
	DEBUG_START;
	initialize();
	if (getenv("DUMP_CONTOURS"))
		analyzeContours(polyhedron, data_);
	if (getenv("DUMP_CLUSTERS_RAW"))
		dumpClustersRaw(clusters, planes_);
	if (getenv("DUMP_CLUSTERS"))
		dumpClusters(polyhedron, clusters, planes_);
	if (getenv("ANALYZE_CLUSTERS_QUALITY"))
		analyzeClustersQuality(polyhedron, clusters, planes_,
				directions_, values_);

	std::vector<TrustedEdgeInformation> nothing;
	DEBUG_END;
	return nothing;
}
