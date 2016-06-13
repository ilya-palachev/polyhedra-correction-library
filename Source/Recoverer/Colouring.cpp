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

/**
 * @file Colouring.cpp
 * @brief Different procedures for the printing of coloured polyhedrons
 * (implementation).
 */

#include <vector>
#include <set>
#include "DebugPrint.h"
#include "DebugAssert.h"
#include "PCLDumper.h"
#include "DataContainers/SupportFunctionData/SupportFunctionData.h"
#include "Recoverer/Colouring.h"
#include "Polyhedron/Polyhedron.h"
#include "Polyhedron/Facet/Facet.h"

static int findBestPlaneID(Plane_3 plane, std::vector<Plane_3> planes)
{
	DEBUG_START;
	double minimal = 1e100;
	int iPlaneMinimal = 0;
	for (int iPlane = 0; iPlane < (int) planes.size(); ++iPlane)
	{
		double a = plane.a() - planes[iPlane].a();
		double b = plane.b() - planes[iPlane].b();
		double c = plane.c() - planes[iPlane].c();
		double d = plane.d() - planes[iPlane].d();
		double difference = a * a + b * b + c * c + d * d;
		if (difference < minimal)
		{
			minimal = difference;
			iPlaneMinimal = iPlane;
		}
	}
	DEBUG_END;
	return iPlaneMinimal;
}

static std::vector<std::vector<int>> getClustersFacetsIndices(
		PolyhedronPtr polyhedron,
		std::vector<Plane_3> planes,
		std::vector<std::vector<int>> clustersIndices)
{
	DEBUG_START;
	std::vector<std::vector<int>> clustersFacetsIDs(clustersIndices.size());
	for (int iFacet = 0; iFacet < polyhedron->numFacets; ++iFacet)
	{
		Facet *facet = &polyhedron->facets[iFacet];
		Plane_3 plane(facet->plane);
		int id = findBestPlaneID(plane, planes);
		int iCluster = 0;
		for (auto clusterIndices: clustersIndices)
		{
			bool ifFound = false;
			for (int iItem = 0; iItem < (int) clusterIndices.size();
					++iItem)
			{	if (clusterIndices[iItem] == id)
				{
					ifFound = true;
					break;
				}
			}
			if (ifFound)
			{
				clustersFacetsIDs[iCluster].push_back(iFacet);
				break;
			}
			++iCluster;
		}
	}
	DEBUG_END;
	return clustersFacetsIDs;
}

void printColouredIntersection(std::vector<Plane_3> planes,
		std::vector<std::vector<int>> clustersIndices,
		const char *suffix)
{
	DEBUG_START;
	Polyhedron_3 intersection(planes);
	PolyhedronPtr polyhedron(new Polyhedron(intersection));
	auto clustersFacetsIDs = getClustersFacetsIndices(polyhedron, planes,
			clustersIndices);
	srand(time(NULL));
	for (auto clusterFacetsIDs: clustersFacetsIDs)
	{
		unsigned char red = rand() % 256;
		unsigned char green = rand() % 256;
		unsigned char blue = rand() % 256;
		for (int iFacet: clusterFacetsIDs)
		{
			polyhedron->facets[iFacet].set_rgb(red, green, blue);
		}
	}
	Polyhedron *polyhedronCopy = new Polyhedron(polyhedron);
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, suffix) << *polyhedronCopy;
	DEBUG_END;
}

void printColouredIntersection(std::vector<Plane_3> planes,
		std::vector<std::set<int>> clustersIndices,
		const char *suffix)
{
	DEBUG_START;
	std::vector<std::vector<int>> semiClusters;
	int iCluster = 0;
	for (auto cluster: clustersIndices)
	{
		std::vector<int> semiCluster;
		std::cerr << "Cluster #" << iCluster << ": ";
		for (int id: cluster)
		{
			std::cerr << id << " ";
			semiCluster.push_back(id);
		}
		std::cerr << std::endl;
		semiClusters.push_back(semiCluster);
		++iCluster;
	}
	printColouredIntersection(planes, semiClusters, suffix);
	DEBUG_END;
}

void printColouredPolyhedron(Polyhedron_3 polyhedron,
		std::vector<std::set<int>> clustersIndices,
		const char *suffix)
{
	DEBUG_START;
	PolyhedronPtr polyhedronPCL(new Polyhedron(polyhedron));
	srand(time(NULL));
	for (auto clusterIndices: clustersIndices)
	{
		unsigned char red = rand() % 256;
		unsigned char green = rand() % 256;
		unsigned char blue = rand() % 256;

		for (int iFacet: clusterIndices)
		{
			polyhedronPCL->facets[iFacet].set_rgb(red, green, blue);
		}
	}
	Polyhedron *polyhedronCopy = new Polyhedron(polyhedronPCL);
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, suffix) << *polyhedronCopy;
	DEBUG_END;
}

void printColouredPolyhedron(Polyhedron_3 polyhedron,
		std::set<int> clusterIndices,
		const char *suffix)
{
	DEBUG_START;
	std::vector<std::set<int>> cluster;
	cluster.push_back(clusterIndices);
	printColouredPolyhedron(polyhedron, cluster, suffix);
	DEBUG_END;
}

void printColouredPolyhedronAndLoadParaview(Polyhedron_3 polyhedron,
		std::vector<std::set<int>> clustersIndices)
{
	DEBUG_START;
	PolyhedronPtr polyhedronPCL(new Polyhedron(polyhedron));
	srand(time(NULL));
	for (auto clusterIndices: clustersIndices)
	{
		unsigned char red = rand() % 256;
		unsigned char green = rand() % 256;
		unsigned char blue = rand() % 256;

		for (int iFacet: clusterIndices)
		{
			polyhedronPCL->facets[iFacet].set_rgb(red, green, blue);
		}
	}
	Polyhedron *polyhedronCopy = new Polyhedron(polyhedronPCL);
	char *polyhedronTmpFileName = strdup(tmpnam(NULL));
	std::string polyhedronFileName(polyhedronTmpFileName);
	polyhedronFileName += ".ply";
	std::ofstream polyhedronFile;
	polyhedronFile.open(polyhedronFileName);
	polyhedronFile << *polyhedronCopy;
	polyhedronFile.close();
	std::string paraviewCommand("paraview --data=");
	paraviewCommand += polyhedronFileName;
	int exitValueParaview = system(paraviewCommand.c_str());
	std::cerr << "Paraview exit with value " << exitValueParaview
		<< std::endl;
	remove(polyhedronFileName.c_str());
	if (exitValueParaview != 0)
		exit(EXIT_FAILURE);
	DEBUG_END;
}

void printColouredPolyhedronAndLoadParaview(Polyhedron_3 polyhedron,
		std::set<int> clusterIndices)
{
	DEBUG_START;
	std::vector<std::set<int>> cluster;
	cluster.push_back(clusterIndices);
	printColouredPolyhedronAndLoadParaview(polyhedron, cluster);
	DEBUG_END;
}
