/*
 * Copyright (c) 2009-2017 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file NativeQuadraticEstimator.cpp
 * @brief Native quadratic estimation engine (implementation).
 */
#include "NativeQuadraticEstimator.h"
#include "Recoverer/NativeEstimatorCommonFunctions.h"
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <CGAL/Triangulation_cell_base_with_info_3.h>
#include <CGAL/Triangulation_data_structure_3.h>

typedef CGAL::Triangulation_vertex_base_with_info_3<unsigned, Kernel> TVertexBase;
typedef CGAL::Triangulation_cell_base_with_info_3<std::set<unsigned>, Kernel> TCellBase;
typedef CGAL::Triangulation_data_structure_3<TVertexBase, TCellBase> TDataStructure;
typedef CGAL::Delaunay_triangulation_3<Kernel, TDataStructure> TDelaunay_3;
typedef TDelaunay_3::Vertex_handle Vertex_handle;
typedef TDelaunay_3::Cell_handle Cell_handle;

struct SupportItem
{
	Vector_3 direction;
	Plane_3 plane;
	std::set<Cell_handle> associations;
};

class DualPolyhedron_3 : public TDelaunay_3
{
private:
	std::vector<SupportItem> items;
	std::set<Vertex_handle> outerVertices;
	std::set<Vertex_handle> innerVertices;

	bool isOuterVertex(const Vertex_handle &vertex) const;
	void associateVertex(const Vertex_handle &vertex);
	void initializeVertices();
	void verify() const;
public:
	typedef std::pair<Point_3, unsigned> PointIndexed_3;

	DualPolyhedron_3(const std::vector<Vector_3> &directions,
			const std::vector<Plane_3> &planes,
			const std::vector<PointIndexed_3>::iterator &begin,
			const std::vector<PointIndexed_3>::iterator &end):
		TDelaunay_3(begin, end)
	{
		DEBUG_START;
		ASSERT(directions.size() == planes.size() && "Wrong input");
		for (unsigned i = 0; i < planes.size(); ++i)
		{
			SupportItem item;
			item.direction = directions[i];
			item.plane = planes[i];
			items.push_back(item);
		}
		DEBUG_END;
	}

	void initialize();
};

NativeQuadraticEstimator::NativeQuadraticEstimator(
		SupportFunctionEstimationDataPtr data) :
	SupportFunctionEstimator(data)
{
	DEBUG_START;
	DEBUG_END;
}

NativeQuadraticEstimator::~NativeQuadraticEstimator()
{
	DEBUG_START;
	DEBUG_END;
}

const int NUM_CELL_VERTICES = 4;
Plane_3 getOppositeFacetPlane(const TDelaunay_3::Cell_handle &cell,
		const TDelaunay_3::Vertex_handle &vertex)
{
	DEBUG_START;
	std::vector<Point_3> points;
	int iVertex = cell->index(vertex);
	for (int i = 0; i < NUM_CELL_VERTICES; ++i)
		if (i != iVertex)
			points.push_back(cell->vertex(i)->point());
	Plane_3 plane(points[0], points[1], points[2]);
	DEBUG_END;
	return plane;
}

bool DualPolyhedron_3::isOuterVertex(const Vertex_handle &vertex) const
{
	DEBUG_START;
	Cell_handle unusedCell;
	int unusedIndex0, unusedIndex1;
	bool result = is_edge(vertex, infinite_vertex(), unusedCell,
			unusedIndex0, unusedIndex1);
	DEBUG_END;
	return result;
}

static double calculateProduct(const Vector_3 &direction,
		const Cell_handle &cell, const Vertex_handle &infinity)
{
	DEBUG_START;
	Plane_3 plane = getOppositeFacetPlane(cell, infinity);
	Point_3 point = ::dual(plane);
	double product = direction * (point - CGAL::Origin());
	DEBUG_END;
	return product;
}

static Cell_handle findBestCell(const Vector_3 &direction,
		const Vertex_handle &infinity,
		const Cell_handle &startingCell)
{
	DEBUG_START;
	/* FIXME: Maybe some hint will be useful here */
	ASSERT(startingCell->has_vertex(infinity) && "Wrong input");
	Cell_handle bestCell = startingCell;
	Cell_handle nextCell = bestCell;
	double maxProduct = calculateProduct(direction, bestCell, infinity);
	unsigned numIterations = 0;
	do
	{
		++numIterations;
		bestCell = nextCell;
		ASSERT(bestCell->has_vertex(infinity) && "Wrong iteration");
		int infinityIndex = bestCell->index(infinity);
		for (int i = 0; i < NUM_CELL_VERTICES; ++i)
		{
			if (i == infinityIndex)
				continue;
			Cell_handle neighbor = bestCell->neighbor(i);
			double product = calculateProduct(direction,
					neighbor, infinity);
			if (product > maxProduct)
			{
				nextCell = neighbor;
				maxProduct = product;
			}
		}
	} while (nextCell != bestCell);
	ASSERT(bestCell->has_vertex(infinity) && "Wrong result");
	DEBUG_END;
	return bestCell;
}

void DualPolyhedron_3::associateVertex(const Vertex_handle &vertex)
{
	DEBUG_START;
	int iPlane = vertex->info();
	if (outerVertices.find(vertex) != outerVertices.end())
	{
		Cell_handle cell;
		int iVertex, iInfinity;
		bool result = is_edge(vertex, infinite_vertex(), cell,
				iVertex, iInfinity);
		ASSERT(result && "Wrong set");
		
		TDelaunay_3::Edge edge(cell, iVertex, iInfinity);
		auto circulator = incident_cells(edge);
		auto end = circulator;
		do
		{
			Cell_handle currentCell = circulator;
			ASSERT(currentCell->has_vertex(vertex));
			ASSERT(currentCell->has_vertex(infinite_vertex()));
			currentCell->info().insert(iPlane);
			++circulator;
		} while (circulator != end);
	}
	else
	{
		ASSERT(!isOuterVertex(vertex) && "Wrong set");
		Vector_3 direction = items[iPlane].direction;
		Cell_handle bestCell = findBestCell(direction,
				infinite_vertex(), infinite_cell());
		bestCell->info().insert(iPlane);
	}
	DEBUG_END;
}

void DualPolyhedron_3::initializeVertices()
{
	DEBUG_START;
	for (auto I = finite_vertices_begin(), E = finite_vertices_end();
			I != E; ++I)
	{
		Vertex_handle vertex = I;
		if (isOuterVertex(vertex))
			outerVertices.insert(vertex);
		else
			innerVertices.insert(vertex);
	}
	std::cout << "Number of outer vertices: " << outerVertices.size()
		<< std::endl;
	std::cout << "Number of inner vertices: " << innerVertices.size()
		<< std::endl;
	ASSERT(outerVertices.size() + innerVertices.size() == items.size()
		&& "Different numbers of dual vertices and primal planes");
	DEBUG_END;
}

void DualPolyhedron_3::verify() const
{
	DEBUG_START;
	std::vector<Cell_handle> outerCells;
	incident_cells(infinite_vertex(), std::back_inserter(outerCells));
	std::cout << "Number of outer cells: " << outerCells.size()
		<< std::endl;

	std::vector<Plane_3> planes;
	for (const SupportItem &item : items)
		planes.push_back(item.plane);
	Polyhedron_3 intersection(planes);
	ASSERT(intersection.size_of_vertices() == outerCells.size()
		&& "Two methods must produce the same topology");

	unsigned numEmptyCells = 0;
	for (Cell_handle cell : outerCells)
	{
		std::cout << "Cell indices: ";
		for (int iPlane : cell->info())
			std::cout << iPlane << " ";
		std::cout << std::endl;
		numEmptyCells += cell->info().empty();
	}
	std::cout << "Number of empty outer cells: " << numEmptyCells
		<< std::endl;
	ASSERT(numEmptyCells == 0 && "All outer cells should be nonempty");
	DEBUG_END;
}

void DualPolyhedron_3::initialize()
{
	DEBUG_START;
	initializeVertices();

	for (auto I = finite_vertices_begin(), E = finite_vertices_end();
			I != E; ++I)
		associateVertex(I);

	if (getenv("INTERNAL_CHECK"))
		verify();
	DEBUG_END;
}

static VectorXd runL2Estimation(SupportFunctionEstimationDataPtr SFEData)
{
	DEBUG_START;
	auto data = SFEData->supportData();
	auto planes = data->supportPlanes();
	auto directions = data->supportDirections<Vector_3>();

	std::vector<DualPolyhedron_3::PointIndexed_3> points;
	for (unsigned i = 0; i < planes.size(); ++i)
		points.push_back(std::make_pair(dual(planes[i]), i));

	DualPolyhedron_3 dualP(directions, planes, points.begin(),
			points.end());
	dualP.initialize();

	/*
	 * FIXME: Change this to actual values, when the algorithm will be
	 * implemented
	 */
	auto values = data->supportValues();
	auto solution = calculateSolution(data, values);
	DEBUG_END;
	return solution;
}

VectorXd NativeQuadraticEstimator::run()
{
	DEBUG_START;
	VectorXd solution = runL2Estimation(data);
	DEBUG_END;
	return solution;
}
