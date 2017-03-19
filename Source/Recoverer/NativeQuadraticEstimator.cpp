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
#include <Eigen/LU>

struct TangientVertex
{
	Point_3 point;
	double distance;
	std::set<unsigned> associations;
};

typedef CGAL::Triangulation_vertex_base_with_info_3<unsigned, Kernel> TVertexBase;
typedef CGAL::Triangulation_cell_base_with_info_3<TangientVertex, Kernel> TCellBase;
typedef CGAL::Triangulation_data_structure_3<TVertexBase, TCellBase> TDataStructure;
typedef CGAL::Delaunay_triangulation_3<Kernel, TDataStructure> TDelaunay_3;
typedef TDelaunay_3::Vertex_handle Vertex_handle;
typedef TDelaunay_3::Cell_handle Cell_handle;

struct SupportItem
{
	Vector_3 direction;
	double value;
	Plane_3 plane;
	std::set<Cell_handle> associations;
	bool resolved;
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
	void partiallyMove(const Vector_3 &xOld,
		const Vector_3 &xNew,
		const std::vector<Vertex_handle> &vertices,
		const Vertex_handle &dominator, double alpha);
	void lift(Cell_handle cell);
	unsigned countResolvedItems() const;
public:
	typedef std::pair<Point_3, unsigned> PointIndexed_3;

	DualPolyhedron_3(const std::vector<Vector_3> &directions,
			const VectorXd &values,
			const std::vector<Plane_3> &planes,
			const std::vector<PointIndexed_3>::iterator &begin,
			const std::vector<PointIndexed_3>::iterator &end):
		TDelaunay_3(begin, end)
	{
		DEBUG_START;
		ASSERT(directions.size() == planes.size() && "Wrong input");
		ASSERT(directions.size() == unsigned(values.size())
				&& "Wrong input");
		for (unsigned i = 0; i < planes.size(); ++i)
		{
			SupportItem item;
			item.direction = directions[i];
			item.value = values[i];
			item.plane = planes[i];
			item.resolved = false;
			ASSERT(item.value == -item.plane.d() && "Conflict");
			ASSERT(item.direction.x() == item.plane.a()
					&& "Conflict");
			ASSERT(item.direction.y() == item.plane.b()
					&& "Conflict");
			ASSERT(item.direction.z() == item.plane.c()
					&& "Conflict");
			items.push_back(item);
		}
		DEBUG_END;
	}

	void initialize();
	double calculateFunctional() const;
	void makeConsistent();
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
const int NUM_FACET_VERTICES = 3;
Plane_3 getOppositeFacetPlane(const TDelaunay_3::Cell_handle &cell,
		const TDelaunay_3::Vertex_handle &vertex)
{
	DEBUG_START;
	std::vector<Point_3> points;
	ASSERT(cell->has_vertex(vertex) && "Wrong usage");
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
			currentCell->info().associations.insert(iPlane);
			items[iPlane].associations.insert(currentCell);
			items[iPlane].resolved = true;
			++circulator;
		} while (circulator != end);
	}
	else
	{
		ASSERT(!isOuterVertex(vertex) && "Wrong set");
		Vector_3 direction = items[iPlane].direction;
		Cell_handle bestCell = findBestCell(direction,
				infinite_vertex(), infinite_cell());
		bestCell->info().associations.insert(iPlane);
		/* FIXME: Maybe the association for a plane is singular? */
		items[iPlane].associations.insert(bestCell);
		items[iPlane].resolved = false;
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
#ifndef NDEBUG
		std::cout << "Cell indices: ";
		for (int iPlane : cell->info())
			std::cout << iPlane << " ";
		std::cout << std::endl;
#endif
		numEmptyCells += cell->info().associations.empty();
	}
	std::cout << "Number of empty outer cells: " << numEmptyCells
		<< std::endl;
	ASSERT(numEmptyCells == 0 && "All outer cells should be nonempty");

	for (const SupportItem &item : items)
		ASSERT(!item.associations.empty()
				&& "Need at least one association");
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

const double EPS_PRODUCT_TOLERANCE = 1e-13;
double DualPolyhedron_3::calculateFunctional() const
{
	DEBUG_START;
	double functional = 0;
	unsigned iItem = 0;
	for (const SupportItem &item : items)
	{
		std::vector<double> products;
		for (const Cell_handle &cell : item.associations)
		{
			double product = calculateProduct(item.direction, cell,
					infinite_vertex());
			products.push_back(product);
		}
		double productMin = products[0];
		double productMax = products[0];
		for (double product : products)
		{
			productMin = std::min(productMin, product);
			productMax = std::max(productMax, product);
		}
		double error = productMax - productMin;
#ifndef NDEBUG
		std::cout << "Item #" << iItem << std::endl;
		std::cout << std::setprecision(16);
		std::cout << "  Product minimal: " << productMin << std::endl;
		std::cout << "  Product maximal: " << productMax << std::endl;
		std::cout << "  Error:           " << error << std::endl;
#endif
		ASSERT(error < EPS_PRODUCT_TOLERANCE && "Bad structure");
		double difference = productMax - item.value;
		functional += difference * difference;
		++iItem;
	}
	DEBUG_END;
	return functional;
}

static Cell_handle iterate(std::vector<Cell_handle> &cells,
		const std::vector<SupportItem> &items,
		const Vertex_handle &infinity)
{
	DEBUG_START;
	double distanceMin = 1e10;
	Cell_handle nextCell;
	for (Cell_handle &cell : cells)
	{
		const auto &associations = cell->info().associations;
		ASSERT(associations.size() >= NUM_CELL_VERTICES - 1
				&& "Bad structure");
		if (associations.size() < NUM_CELL_VERTICES)
			continue;
		Plane_3 plane = getOppositeFacetPlane(cell, infinity);
		Point_3 point = dual(plane);
		cell->info().point = point;
		
		double distanceMax = 0.;
		for (unsigned iPlane : associations)
		{
			SupportItem item = items[iPlane];
			Vector_3 u = item.direction;
			double value = item.value;
			double distance = value - u * (point - CGAL::Origin());
			ASSERT(distance >= 0 && "Wrong outer cells list");
			distanceMax = std::max(distanceMax, distance);
		}
		cell->info().distance = distanceMax;
		if (distanceMax < distanceMin)
		{
			distanceMin = distanceMax;
			nextCell = cell;
		}
	}
	DEBUG_END;
	return nextCell;
}

static Vector_3 leastSquaresPoint(std::vector<SupportItem> items)
{
	DEBUG_START;
	Eigen::Matrix3d matrix;
	Eigen::Vector3d vector;
	for (unsigned i = 0; i < 3; ++i)
	{
		vector(i) = 0.;
		for (unsigned j = 0; j < 3; ++j)
			matrix(i, j) = 0.;
	}

	for (const SupportItem &item : items)
	{
		Vector_3 u = item.direction;
		double value = item.value;
		for (unsigned i = 0; i < 3; ++i)
		{
			for (unsigned j = 0; j < 3; ++j)
				matrix(i, j) += u.cartesian(i)
					* u.cartesian(j);
			vector(i) += u.cartesian(i) * value;
		}
	}
	Eigen::Vector3d solution = matrix.inverse() * vector;

	return Vector_3(solution(0), solution(1), solution(2));
}

static double calculateAlpha(const Vector_3 &xOld, const Vector_3 &xNew,
		const Plane_3 &planeOuter)
{
	DEBUG_START;
	Plane_3 plane = planeOuter;
	Vector_3 u(plane.a(), plane.b(), plane.c());
	double value = -plane.d();
	if (value < 0.)
	{
		u = -u;
		value = -value;
	}
	double length = sqrt(u.squared_length());
	u = u / length;
	value = value / length;
	double productNew = xNew * u;
	double productOld = xOld * u;
	double alpha = (productNew - value) / (productNew - productOld);
	DEBUG_END;
	return alpha;
}

bool isPositivelyDecomposable(const Point_3 &a, const Point_3 &b,
		const Point_3 &c, const Point_3 &decomposed)
{
	DEBUG_START;
	DEBUG_END;
	Eigen::Matrix3d matrix;
	matrix << a.x(), b.x(), c.x(),
	       a.y(), b.y(), c.y(),
	       a.z(), b.z(), c.z();
	Eigen::Vector3d vector;
	vector << decomposed.x(), decomposed.y(), decomposed.z();
	Eigen::Vector3d coefficients = matrix.inverse() * vector;
	return coefficients(0) >= 0.
		&& coefficients(1) >= 0.
		&& coefficients(2) >= 0.;
}

Point_3 calculateMove(const Vertex_handle &vertex,
		const Vector_3 &tangient)
{
	DEBUG_START;
	Plane_3 plane = dual(vertex->point());
	Vector_3 u(plane.a(), plane.b(), plane.c());
	if (plane.d() > 0.)
		u = -u;
	double value = tangient * u;
	Plane_3 planeNew(u.x(), u.y(), u.z(), -value);
	Point_3 point = dual(planeNew);
	DEBUG_END;
	return point;
}

static inline unsigned indexModulo(unsigned i)
{
	return (NUM_FACET_VERTICES + i) % NUM_FACET_VERTICES;
}

void DualPolyhedron_3::partiallyMove(const Vector_3 &xOld,
		const Vector_3 &xNew,
		const std::vector<Vertex_handle> &vertices,
		const Vertex_handle &dominator, double alpha)
{
	DEBUG_START;
	unsigned numDeletable = 0;
	unsigned iDeleted = 0;
	for (unsigned i = 0; i < NUM_FACET_VERTICES; ++i)
	{
		unsigned iPrev = indexModulo(i - 1);
		unsigned iNext = indexModulo(i + 1);
		if (isPositivelyDecomposable(vertices[iPrev]->point(),
					vertices[iNext]->point(),
					dominator->point(),
					vertices[i]->point()))
		{
			iDeleted = i;
			++numDeletable;
		}
	}
	ASSERT(numDeletable == 1 && "Wrong topological configuration");
	Vertex_handle vertexDeleted = vertices[iDeleted];
	Vector_3 tangient = alpha * xOld + (1. - alpha) * xNew;
	for (unsigned i = 0; i < NUM_FACET_VERTICES; ++i)
	{
		if (i == iDeleted)
			continue;

		Vertex_handle vertex = vertices[i];
		Point_3 point = calculateMove(vertex, tangient);
		move(vertex, point);
	}
	DEBUG_END;
}

void DualPolyhedron_3::lift(Cell_handle cell)
{
	DEBUG_START;
	std::vector<SupportItem> currentItems;
	for (unsigned iPlane : cell->info().associations)
		currentItems.push_back(items[iPlane]);

	Vector_3 xNew = leastSquaresPoint(currentItems);
	Vector_3 xOld = cell->info().point - CGAL::Origin();

	ASSERT(cell->has_vertex(infinite_vertex()));
	unsigned infinityIndex = cell->index(infinite_vertex());
	std::vector<Vertex_handle> vertices;
	Vertex_handle dominator;
	double alphaMax = 0.;
	for (unsigned i = 0; i < NUM_CELL_VERTICES; ++i)
	{
		if (i == infinityIndex)
			continue;

		vertices.push_back(cell->vertex(i));

		Vertex_handle vertex = mirror_vertex(cell, i);
		Plane_3 plane = ::dual(vertex->point());
		double alpha = calculateAlpha(xOld, xNew, plane);
		if (alpha > alphaMax)
		{
			alphaMax = alpha;
			dominator = vertex;
		}
	}
	
	if (alphaMax > 0.)
		partiallyMove(xOld, xNew, vertices, dominator, alphaMax);
	else
		for (unsigned i = 0; i < NUM_FACET_VERTICES; ++i)
		{
			Vertex_handle vertex = vertices[i];
			Point_3 point = calculateMove(vertex, xNew);
			move(vertex, point);
		}
	DEBUG_END;
}

unsigned DualPolyhedron_3::countResolvedItems() const
{
	DEBUG_START;
	unsigned numResolved = 0;
	for (const SupportItem &item : items)
		if (item.resolved)
			++numResolved;
	DEBUG_END;
	return numResolved;
}

void DualPolyhedron_3::makeConsistent()
{
	DEBUG_START;
	unsigned iIteration = 0;
	std::cout << "Number of resolved items: " << countResolvedItems()
		<< std::endl;
	while (countResolvedItems() < items.size())
	{
		std::cout << "Iteration #" << iIteration << std::endl;
		std::vector<Cell_handle> outerCells;
		incident_cells(infinite_vertex(),
				std::back_inserter(outerCells));
		Cell_handle cell = iterate(outerCells, items,
				infinite_vertex());
		lift(cell);

		/* FIXME: Optimize this by local graph traversal */
		initialize();
	}
	DEBUG_END;
}

static VectorXd runL2Estimation(SupportFunctionEstimationDataPtr SFEData)
{
	DEBUG_START;
	auto data = SFEData->supportData();
	auto planes = data->supportPlanes();
	auto directions = data->supportDirections<Vector_3>();
	auto values = data->supportValues();

	std::vector<DualPolyhedron_3::PointIndexed_3> points;
	for (unsigned i = 0; i < planes.size(); ++i)
		points.push_back(std::make_pair(dual(planes[i]), i));

	DualPolyhedron_3 dualP(directions, values, planes, points.begin(),
			points.end());
	dualP.initialize();
	double startingFunctional = dualP.calculateFunctional();
	std::cout << "Starting value of functional: " << startingFunctional
		<< std::endl;
	std::cout << "And square root of it: " << sqrt(startingFunctional)
		<< std::endl;

	dualP.makeConsistent();
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
