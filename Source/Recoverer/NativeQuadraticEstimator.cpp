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

const double MINIMIZATION_STARTING_VALUE = 1e10;

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

	bool isOuterVertex(const Vertex_handle &vertex) const;
	void associateVertex(const Vertex_handle &vertex);
	void verify() const;
	void partiallyMove(const Vector_3 &xOld,
		const Vector_3 &xNew,
		const std::vector<Vertex_handle> &vertices,
		const Vertex_handle &dominator, double alpha);
	void fullyMove(const std::vector<Vertex_handle> &vertices,
		const Vector_3 &xNew, const std::vector<unsigned> &activeGroup);
	void lift(Cell_handle cell, unsigned iNearest);
	unsigned countResolvedItems() const;
	std::vector<Cell_handle> getOuterCells() const;
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
	if (isOuterVertex(vertex))
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
		Vector_3 direction = items[iPlane].direction;
		Cell_handle bestCell = findBestCell(direction,
				infinite_vertex(), infinite_cell());
		bestCell->info().associations.insert(iPlane);
		/* FIXME: Maybe the association for a plane is singular? */
		items[iPlane].associations.insert(bestCell);
	}
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
	/*
	 * FIXME: Remove these two loops when local initialization will be
	 * implemented
	 */
	for (SupportItem &item : items)
		item.associations.clear();
	for (auto I = cells_begin(), E = cells_end(); I != E; ++I)
		I->info().associations.clear();

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

static unsigned getNearestOuterItemID(const Cell_handle &cell,
		const std::vector<SupportItem> &items,
		const Vertex_handle &infinity)
{
	DEBUG_START;
	Plane_3 plane = getOppositeFacetPlane(cell, infinity);
	Point_3 point = dual(plane);
	cell->info().point = point;
	
	double distanceMin = MINIMIZATION_STARTING_VALUE;
	unsigned iNearest = 0;
	const auto &associations = cell->info().associations;
	for (unsigned iPlane : associations)
	{
		SupportItem item = items[iPlane];
		Vector_3 u = item.direction;
		double value = item.value;
		double distance = value - u * (point - CGAL::Origin());
		if (!item.resolved)
		{
			ASSERT(distance > 0. && "Incorrect resolved flag");
			if (distance < distanceMin)
			{
				iNearest = iPlane;
				distanceMin = distance;
			}
		}
	}
	ASSERT(distanceMin < MINIMIZATION_STARTING_VALUE && "Failed to find");
	cell->info().distance = distanceMin;
	DEBUG_END;
	return iNearest;
}

static void printCell(const Cell_handle &cell, const Vertex_handle &infinity)
{
	DEBUG_START;
	unsigned infinityIndex = cell->index(infinity);
	for (unsigned i = 0; i < NUM_CELL_VERTICES; ++i)
		if (i != infinityIndex)
			std::cout << cell->vertex(i)->info()
				<< " ";
	std::cout << std::endl;
	DEBUG_END;
}

static std::pair<Cell_handle, unsigned> iterate(
		const std::vector<Cell_handle> &cells,
		const std::vector<SupportItem> &items,
		const Vertex_handle &infinity)
{
	DEBUG_START;
	double distanceMin = MINIMIZATION_STARTING_VALUE;
	Cell_handle nextCell;
	/* FIXME: Use this ID to move to it only */
	unsigned iNextNearest = 0;
	for (const Cell_handle &cell : cells)
	{
		const auto &associations = cell->info().associations;
		ASSERT(associations.size() >= NUM_CELL_VERTICES - 1
				&& "Bad structure");
		if (associations.size() < NUM_CELL_VERTICES)
			continue;
		ASSERT(cell->has_vertex(infinity) && "Wrong list");

		unsigned iNearest = getNearestOuterItemID(cell, items,
				infinity);
		double distance = cell->info().distance;
		if (distance < distanceMin)
		{
			iNextNearest = iNearest;
			distanceMin = distance;
			nextCell = cell;
		}
	}
	std::cout << "Next cell to be iterated on: ";
	printCell(nextCell, infinity);
	std::cout << "Nearest plane ID: " << iNextNearest << std::endl;
	std::cout << "Nearest distance: " << distanceMin << std::endl;
	DEBUG_END;
	return std::make_pair(nextCell, iNextNearest);
}

static std::vector<unsigned> calculateActiveGroup(const Cell_handle &cell,
		unsigned iNearest,
		const std::vector<SupportItem> &items)
{
	DEBUG_START;
	Vector_3 xOld = cell->info().point - CGAL::Origin();
	std::vector<unsigned> activeGroup;
	unsigned numUnresolvedCurrent = 0;
	bool nearestFound = false;
	for (unsigned iPlane : cell->info().associations)
	{
		const SupportItem &item = items[iPlane];
		double delta = item.direction * xOld - item.value;
		std::cout << "  delta for plane #" << iPlane << "= " << delta
			<< "; resolved: " << item.resolved << std::endl;
		if (!item.resolved)
		{
			++numUnresolvedCurrent;
			if (iPlane == iNearest)
			{
				activeGroup.push_back(iPlane);
				nearestFound = true;
			}
		}
		else
			activeGroup.push_back(iPlane);
	}
	std::cout << "  Number of current unresolved items: "
		<< numUnresolvedCurrent << std::endl;
	ASSERT(numUnresolvedCurrent > 0 && "Nothing to be resolved");
	ASSERT(nearestFound && "Failed to find nearest point inside given "
			"cell");
	ASSERT(activeGroup.size() > NUM_FACET_VERTICES && "Not enough planes");

	DEBUG_END;
	return activeGroup;
}

static Vector_3 leastSquaresPoint(const std::vector<unsigned> activeGroup,
		const std::vector<SupportItem> &items)
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

	std::cout << "Calculating least squares points for the following items"
		<< std::endl;
	for (unsigned iPlane : activeGroup)
	{
		SupportItem item = items[iPlane];
		Vector_3 u = item.direction;
		double value = item.value;
		std::cout << "  Item #" << iPlane << ": u = " << u << "; h = "
			<< value << std::endl;
		for (unsigned i = 0; i < 3; ++i)
		{
			for (unsigned j = 0; j < 3; ++j)
				matrix(i, j) += u.cartesian(i)
					* u.cartesian(j);
			vector(i) += u.cartesian(i) * value;
		}
	}
	Eigen::Vector3d solution = matrix.inverse() * vector;

	Vector_3 result(solution(0), solution(1), solution(2));
	std::cout << "Result: " << result << std::endl;
	for (unsigned iPlane : activeGroup)
	{
		SupportItem item = items[iPlane];
		double delta = item.direction * result - item.value;
		std::cout << "  delta = " << delta << std::endl;
	}
	DEBUG_END;
	return result;
}

const double EPS_PRODUCT_DIFF_TOLERANCE = 1e-5;
static double calculateAlpha(const Vector_3 &xOld, const Vector_3 &xNew,
		const Plane_3 &planeOuter)
{
	DEBUG_START;
	std::cout << "Calculating alpha for plane " << planeOuter << std::endl; 
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
	std::cout << "  u = " << u << std::endl;
	value = value / length;
	std::cout << "  h initial = " << value << std::endl;
	double productOld = xOld * u;
	std::cout << "  h current = " << productOld << std::endl;
	double productNew = xNew * u;
	std::cout << "  h new     = " << productNew << std::endl;
	double productDifference = productNew - productOld;
	std::cout << "  product difference: " << productDifference
		<< std::endl;
	if (productDifference < 0.
			&& productDifference >= -EPS_PRODUCT_DIFF_TOLERANCE)
	{
		std::cout << "Considering negative product difference as no "
			"difference at all" << std::endl;
		DEBUG_END;
		return 0.;
	}
	ASSERT(productNew >= productOld && "Wrong move");
	double alpha = (productNew - value) / (productNew - productOld);
	if (alpha > 1.)
		alpha = 1.;
	DEBUG_END;
	return alpha;
}

bool isPositivelyDecomposable(const Point_3 &a, const Point_3 &b,
		const Point_3 &c, const Point_3 &decomposed)
{
	DEBUG_START;
	Eigen::Matrix3d matrix;
	matrix << a.x(), b.x(), c.x(),
	       a.y(), b.y(), c.y(),
	       a.z(), b.z(), c.z();
	Eigen::Vector3d vector;
	vector << decomposed.x(), decomposed.y(), decomposed.z();
	Eigen::Vector3d coefficients = matrix.inverse() * vector;
	std::cout << "Tried to decompose vector, result: " << std::endl
		<< coefficients << std::endl;
	DEBUG_END;
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
	std::cout << "Number of positively decomposable vectors: "
		<< numDeletable << std::endl;
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

void DualPolyhedron_3::fullyMove(const std::vector<Vertex_handle> &vertices,
		const Vector_3 &xNew, const std::vector<unsigned> &activeGroup)
{
	DEBUG_START;
	for (const Vertex_handle vertex : vertices)
		ASSERT(isOuterVertex(vertex));
	for (unsigned i = 0; i < NUM_FACET_VERTICES; ++i)
	{
		Vertex_handle vertex = vertices[i];
		Point_3 point = calculateMove(vertex, xNew);
		std::cout << "Moving dual point #" << vertex->info()
			<< ": " << std::endl
			<< std::setprecision(16)
			<< vertex->point() << " -> " << std::endl
			<< point << std::endl;
		unsigned numOld = number_of_vertices();
		Vertex_handle vertexNew = move(vertex, point);
		ASSERT(vertexNew == vertex);
		std::cout << vertex->point() << std::endl;
		ASSERT(number_of_vertices() == numOld);
	}
	for (const Vertex_handle vertex : vertices)
		ASSERT(isOuterVertex(vertex));

	unsigned numNewlyResolved = 0;
	for (unsigned iPlane : activeGroup)
	{
		if (!items[iPlane].resolved)
		{
			std::cout << "  Marking item #" << iPlane
				<< " as resolved" << std::endl;
			items[iPlane].resolved = true;
			++numNewlyResolved;
		}
		else
			std::cout << "  Item #" << iPlane
				<< " is already resolved" << std::endl;
	}
	ASSERT(numNewlyResolved > 0 && "Nothing has been resolved");
	ASSERT(0 && "To be implemented");
	DEBUG_END;
}

void DualPolyhedron_3::lift(Cell_handle cell, unsigned iNearest)
{
	DEBUG_START;
	Vector_3 xOld = cell->info().point - CGAL::Origin();
	std::cout << "Old tangient point: " << xOld << std::endl;
	const auto activeGroup = calculateActiveGroup(cell, iNearest, items);
	Vector_3 xNew = leastSquaresPoint(activeGroup, items);
	std::cout << "New tangient point: " << xNew << std::endl;

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
		std::cout << "Alpha #" << i << ": " << alpha << std::endl;
		ASSERT(alpha <= 1. && "Wrongly computed alpha");
		if (alpha > alphaMax)
		{
			alphaMax = alpha;
			dominator = vertex;
		}
	}
	
	std::cout << "Maximal alpha: " << alphaMax << std::endl;
	if (alphaMax > 0.)
	{
		std::cout << "Full move is impossible, performing partial move"
			<< std::endl;
		partiallyMove(xOld, xNew, vertices, dominator, alphaMax);
	}
	else
	{
		std::cout << "Performing full move" << std::endl;
		fullyMove(vertices, xNew, activeGroup);
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

std::vector<Cell_handle> DualPolyhedron_3::getOuterCells() const
{
	DEBUG_START;
	std::vector<Cell_handle> outerCells;
	incident_cells(infinite_vertex(), std::back_inserter(outerCells));

	std::vector<Cell_handle> outerCellsAnother;
	for (auto I = cells_begin(), E = cells_end(); I != E;
			++I)
		if (I->has_vertex(infinite_vertex()))
			outerCellsAnother.push_back(I);

	std::cout << "  One way to calculate outer cells gives number "
		<< outerCells.size() << std::endl;
	std::cout << "  Another one gives number " << outerCellsAnother.size()
		<< std::endl;
	ASSERT(outerCells.size() == outerCells.size()
			&& "How to calculate it?");
	DEBUG_END;
	return outerCells;
}


void DualPolyhedron_3::makeConsistent()
{
	DEBUG_START;
	unsigned iIteration = 0;
	unsigned numResolved = countResolvedItems();
	while (numResolved < items.size())
	{
		std::cout << "===== Iteration #" << iIteration << " ====="
			<< std::endl;
		std::cout << "  Resolved " << numResolved << " items from "
			<< items.size() << std::endl;

		const auto &outerCells = getOuterCells();
		auto next = iterate(outerCells, items, infinite_vertex());
		Cell_handle cell = next.first;
		unsigned iNearest = next.second;
		lift(cell, iNearest);

		/* FIXME: Optimize this by local graph traversal */
		initialize();
		unsigned numResolvedPrev = numResolved;
		numResolved = countResolvedItems();
		std::cout << "  Change in the number of resolved items: "
			<< numResolved - numResolvedPrev << std::endl;
		ASSERT(numResolved >= numResolvedPrev
				&& "Degradation happened");
		++iIteration;
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
