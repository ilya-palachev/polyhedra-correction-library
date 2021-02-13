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
 * @file EdgeInfo.cpp
 * @brief Information about polyhedron edges, comming from different contours
 * (shadow, reflect) and from planes.
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "DefaultScaner.h"
#include "DataContainers/EdgeInfo/EdgeInfo.h"

bool readEdgeInfoFile(const char *path, std::vector<EdgeInfo> &data)
{
	DefaultScaner scaner(path);
	if (!scaner.open())
	{
		ERROR_PRINT("Failed to open file %s", path);
		return false;
	}

	char *line = scaner.getline();
	unsigned numEdges = 0;
	if (!line || sscanf(line, "%u", &numEdges) != 1)
	{
		ERROR_PRINT("Failed to read the number of edges");
		return false;
	}
	std::cout << "Edge infos number: " << numEdges << std::endl; 

	for (unsigned iEdge = 0; iEdge < numEdges; ++iEdge)
	{
		EdgeInfo info;
		line = scaner.getline();
		unsigned isReady = 0;
		if (!line || sscanf(line, "%u %u %u %lf", &info.id, &isReady,
			&info.type, &info.weight) != 4 || info.id != iEdge)
		{
			ERROR_PRINT("Wrong format, edge header #%d", iEdge);
			std::cout << "Line: " << line << std::endl;
			std::cout << "Length of line: " << strlen(line)
				<< std::endl;
			return false;
		}
		info.isReady = (isReady != 0);

		line = scaner.getline();
		if (!line || sscanf(line, "%u %u", &info.facetID1,
					&info.facetID2)
			!= 2)
		{
			ERROR_PRINT("Wrong format, edge facet IDs #%d", iEdge);
			return false;
		}

		line = scaner.getline();
		double x, y, z;
		if (!line || sscanf(line, "%lf %lf %lf", &x, &y, &z) != 3)
		{
			ERROR_PRINT("Wrong format, edge #%d's 1st correcting "
				"vertex", iEdge);
			return false;
		}
		Point_3 source(x, y, z);

		line = scaner.getline();
		if (!line || sscanf(line, "%lf %lf %lf", &x, &y, &z) != 3)
		{
			ERROR_PRINT("Wrong format, edge #%d's 2st correcting "
				"vertex", iEdge);
			return false;
		}
		Point_3 destination(x, y, z);

		info.correctingEdge = Segment_3(source, destination);

		line = scaner.getline();
		unsigned numPlanes = 0;
		if (!line || sscanf(line, "%u", &numPlanes) != 1)
		{
			ERROR_PRINT("Wrong format, edge #%d's planes number",
					iEdge);
			return false;
		}

		for (unsigned iPlane = 0; iPlane < numPlanes; ++iPlane)
		{
			line = scaner.getline();
			double a, b, c, d;
			unsigned type = 0;
			double weight = 0.;
			if (!line || sscanf(line, "%u %lf %lf %lf %lf %lf",
				&type, &weight, &a, &b, &c, &d) != 6)
			{
				ERROR_PRINT("Wrong format, edge %d, plane %d",
						iEdge, iPlane);
				return false;
			}
			EdgePlane_3 plane(a, b, c, d);
			plane.type = static_cast<EdgePlane_3::EdgePlaneType>(
					type);
			plane.weight = weight;
			info.planes.push_back(plane);
		}

		data.push_back(info);
	}

	return true;
}

bool getInitialPosition(Polyhedron_3 &p, std::vector<EdgeInfo> &data)
{
	p.initialize_indices();
	unsigned iInfo = 0;
	for (EdgeInfo &info : data)
	{
		auto facet1 = *std::next(p.facets_begin(), info.facetID1);
		ASSERT(unsigned(facet1.id) == info.facetID1);
		auto facet2 = *std::next(p.facets_begin(), info.facetID2);
		ASSERT(unsigned(facet2.id) == info.facetID2);
		auto circulator = facet1.facet_begin();
		auto finish = circulator;
		bool found = false;
		do
		{
			if (circulator->opposite()->facet()->id == facet2.id)
			{
				found = true;
				break;
			}

			++circulator;

		} while(circulator != finish);

		if (!found)
		{
			std::cerr << "Failed to find initial edge for info #"
				<< iInfo << std::endl;
			return false;
		}

		Point_3 start = circulator->vertex()->point();
		Point_3 end = circulator->opposite()->vertex()->point();
		Segment_3 edge(start, end);
		info.initialEdge = edge;

		++iInfo;
	}

	return true;
}

