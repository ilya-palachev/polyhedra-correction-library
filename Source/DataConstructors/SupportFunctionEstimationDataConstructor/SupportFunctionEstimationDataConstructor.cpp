/*
 * Copyright (c) 2009-2014 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file SupportFunctionEstimationDataConstructor.cpp
 * @brief Constructor of data used for support function estimation process
 * - implementation.
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "DataConstructors/SupportFunctionEstimationDataConstructor/SupportFunctionEstimationDataConstructor.h"
#include "DataConstructors/SupportFunctionEstimationDataConstructor/SupportFunctionEstimationDataConditionConstructor.h"
#include "SparseMatrixEigen.h"
#include "halfspaces_intersection.h"

#define UNUSED __attribute__((unused))

/**
 * Builds hull of directions by given support directions.
 *
 * @param directions	Given support directions.
 * @return 		Hull of directions.
 */
static Polyhedron_3 buildDirectionsHull(std::vector<Point_3> directions);

/**
 * Checks the hull of directions for correctness.
 *
 * @param hull	The hull of directions.
 * @return	True, if the hull is correct.
 */
static bool checkDirectionsHull(Polyhedron_3 hull) UNUSED;

/**
 * Builds support matrix from given hull of support directions.
 *
 * @param hull		Hull of directions.
 * @param ifScaleMatrix	Whether the matrix should be scaled.
 * @return 		Support matrix.
 */
static SparseMatrix buildSupportMatrix(Polyhedron_3 hull, bool ifScaleMatrix);

/**
 * Checks support matrix for correctness.
 *
 * @param matrix	The support matrix to be tested.
 * @param hull		The hull it was constructed from.
 * @return		True, if the matrix is correct.
 */
static bool checkSupportMatrix(SparseMatrix matrix, Polyhedron_3 hull) UNUSED;

/**
 * Builds starting vector for the estimation process.
 *
 * @param data		The support function data.
 * @param matrix	The support matrix.
 * @return		Starting vector.
 */
static VectorXd buildStartingVector(SupportFunctionDataPtr data,
		SparseMatrix matrix);

/**
 * Checks starting vector.
 *
 * @param vector	Starting vector.
 * @param matrix	Support matrix.
 * @return		True, if the starting vector is correct.
 */
static bool checkStartingVector(VectorXd startingVector, SparseMatrix matrix)
	UNUSED;

SupportFunctionEstimationDataConstructor::SupportFunctionEstimationDataConstructor() :
	ifScaleMatrix(false)
{
	DEBUG_START;
	DEBUG_END;
}

SupportFunctionEstimationDataConstructor::~SupportFunctionEstimationDataConstructor()
{
	DEBUG_START;
	DEBUG_END;
}

void SupportFunctionEstimationDataConstructor::enableMatrixScaling()
{
	DEBUG_START;
	ifScaleMatrix = true;
	DEBUG_END;
}

SupportFunctionEstimationDataPtr SupportFunctionEstimationDataConstructor::run(
		SupportFunctionDataPtr data)
{
	DEBUG_START;
	/* Remove equal items from data (and normalize all items). */
	data = data->removeEqual();

	/* Build hull of directions. */
	Polyhedron_3 hull = buildDirectionsHull(data->supportDirectionsCGAL());

	/* Build support matrix. */
	SparseMatrix supportMatrix = buildSupportMatrix(hull, ifScaleMatrix);

	/* Build support vector. */
	VectorXd supportVector = data->supportValues();

	/* Build starting vector. */
	VectorXd startingVector = buildStartingVector(data, supportMatrix);

	/* Get support directions from data. */
	std::vector<Vector3d> supportDirections = data->supportDirections();

	/* Construct data. */
	SupportFunctionEstimationDataPtr estimationData(new
		SupportFunctionEstimationData(supportMatrix, supportVector,
				startingVector, supportDirections));
	DEBUG_END;
	return estimationData;
}

static Polyhedron_3 buildDirectionsHull(std::vector<Point_3> directions)
{
	DEBUG_START;
	/* Construct convex hull of support directions. */
	Polyhedron_3 hull;
	CGAL::convex_hull_3(directions.begin(), directions.end(), hull);
	ASSERT(hull.size_of_vertices() == directions.size());

	/* Find correpondance between directions and vertices of polyhedron. */
	for (auto vertex = hull.vertices_begin();
		vertex != hull.vertices_end(); ++vertex)
	{
		auto point = vertex->point();
		/* Find the direction that is nearest to the point. */
		double distMin = 0.; /* Minimal distance. */
		long int iMin = 0;   /* ID os nearest direction. */
		for (unsigned int i = 0; i < directions.size(); ++i)
		{
			double dist = (directions[i] - point).squared_length();
			if (dist < distMin || i == 0)
			{
				distMin = dist;
				iMin = i;
			}
		}
		vertex->id = iMin; /* Save ID of nearest direction in vertex */
		ASSERT(equal(distMin, 0.));
	}
	ASSERT(checkDirectionsHull(hull));
	DEBUG_END;
	return hull;
}

static bool checkDirectionsHull(Polyhedron_3 hull)
{
	DEBUG_START;
	/* Check that all vertices have different IDs. */
	for (auto vertex = hull.vertices_begin(); vertex != hull.vertices_end();
			++vertex)
	{
		for (auto vertexPrev = hull.vertices_begin();
				vertexPrev != vertex; ++vertexPrev)
			if (vertexPrev->id == vertex->id)
			{
				DEBUG_END;
				return false;
			}
	}

	/* Check that all facets are triangles. */
	for (auto facet = hull.facets_begin(); facet != hull.facets_end();
			++facet)
		if (!facet->is_triangle())
		{
			DEBUG_END;
			return false;
		}

	DEBUG_END;
	return true;
}

static SparseMatrix buildSupportMatrix(Polyhedron_3 hull, bool ifScaleMatrix)
{
	DEBUG_START;

	/* Construct the set of condition constructors. */
	std::set<SupportFunctionEstimationDataConditionConstructor> conditions;
	int numConditionsNotInserted = 0;
	for (auto halfedge = hull.halfedges_begin();
			halfedge != hull.halfedges_end(); ++halfedge)
	{
		SupportFunctionEstimationDataConditionConstructor condition(
				halfedge);
		auto pairInsertResult = conditions.insert(condition);
		if (!pairInsertResult.second)
		{
			DEBUG_PRINT("condition was not inserted");
			auto existed = *(pairInsertResult.first);
			DEBUG_PRINT("already exists: [%ld %ld %ld %ld]",
				existed[0]->id, existed[1]->id,
				existed[2]->id,	existed[3]->id);
			DEBUG_PRINT("tried to insert: [%ld %ld %ld %ld]",
				condition[0]->id, condition[1]->id,
				condition[2]->id, condition[3]->id);

			++numConditionsNotInserted;
		}
	}
	DEBUG_PRINT("=== %d conditions were not inserted.",
			numConditionsNotInserted);

	/* Construct sparse matrix from triplets. */
	std::vector<Eigen::Triplet<double>> triplets;
	int iCondition = 0;
	for (auto condition = conditions.begin(); condition != conditions.end();
			++condition)
	{
		auto pairs = condition->constructCondition(ifScaleMatrix);
		for (auto pair = pairs.begin(); pair != pairs.end(); ++pair)
			triplets.push_back(Eigen::Triplet<double>(iCondition,
						pair->first, pair->second));
		++iCondition;
	}
	SparseMatrix matrix(conditions.size(), hull.size_of_vertices());
	matrix.setFromTriplets(triplets.begin(), triplets.end());
	ASSERT((unsigned int) matrix.rows() == conditions.size());

	ASSERT(checkSupportMatrix(matrix, hull));
	DEBUG_END;
	return matrix;
}

const double EPS_EIGEN_CHECK_VIOLATION = 1e-4;

static bool checkSupportMatrix(SparseMatrix matrix, Polyhedron_3 hull)
{
	DEBUG_START;
	/* Check the number of conditions. */
	int numEdges = hull.size_of_halfedges() / 2;
	int numTrivalentVertices = 0;
	for (auto vertex = hull.vertices_begin(); vertex != hull.vertices_end();
			++vertex) /* Trivalent vertex has only 1 condition. */
		if (vertex->is_trivalent())
			++numTrivalentVertices;
	if (numEdges - 2 * numTrivalentVertices != matrix.rows())
	{
		DEBUG_PRINT("numEdges = %d", numEdges);
		DEBUG_PRINT("numTrivalentVertices = %d", numTrivalentVertices);
		DEBUG_PRINT("numEdges - 2 * numTrivalentVertices = %d",
				numEdges - 2 * numTrivalentVertices);
		DEBUG_PRINT("matrix.rows() = %d", matrix.rows());
		DEBUG_END;
		return false;
	}

	/* Construct 3 known eigenvectors of the matrix. */
	int numVertices = hull.size_of_vertices();
	VectorXd eigenVectorX(numVertices), eigenVectorY(numVertices),
		 eigenVectorZ(numVertices);
	int i = 0;
	for (auto vertex = hull.vertices_begin();
			vertex != hull.vertices_end(); ++vertex)
	{
		auto point = vertex->point();
		int id = vertex->id;
		eigenVectorX(id) = point.x();
		eigenVectorY(id) = point.y();
		eigenVectorZ(id) = point.z();
		++i;
	}

	double errorX = (matrix * eigenVectorX).norm();
	double errorY = (matrix * eigenVectorY).norm();
	double errorZ = (matrix * eigenVectorZ).norm();
	if (errorX > EPS_EIGEN_CHECK_VIOLATION
		|| errorY > EPS_EIGEN_CHECK_VIOLATION
		|| errorZ > EPS_EIGEN_CHECK_VIOLATION)
	{
		DEBUG_PRINT("errorX = %le, errorY = %le, errorZ = %le", errorX,
				errorY, errorZ);
		DEBUG_END;
		return false;
	}
	DEBUG_END;
	return true;
}

static VectorXd buildStartingVector(SupportFunctionDataPtr data,
		SparseMatrix matrix)
{
	DEBUG_START;
	VectorXd startingVector(data->size());

	/* Construct intersection of support halfspaces represented by planes */
	std::vector<Plane_3> planes = data->supportPlanes();
	Polyhedron_3 intersection;
	CGAL::internal::halfspaces_intersection<std::vector<Plane_3>::iterator,
		Polyhedron_3, Kernel>(planes.begin(), planes.end(),
		intersection, Kernel());

	/* Calculate support values for known support directions. */
	std::vector<Point_3> directions = data->supportDirectionsCGAL();
	int i = 0;
	for (auto direction = directions.begin(); direction != directions.end();
			++direction)
	{
		Vector_3 vDirection = *direction - CGAL::ORIGIN;
		double scalarProductMax = 0.;
		for (auto vertex = intersection.vertices_begin();
				vertex != intersection.vertices_end(); ++vertex)
		{
			Vector_3 vVertex = vertex->point() - CGAL::ORIGIN;
			double scalarProduct = vDirection * vVertex;
			if (scalarProduct > scalarProductMax)
				scalarProductMax = scalarProduct;
		}
		startingVector(i++) = scalarProductMax;
	}
	ASSERT(checkStartingVector(startingVector, matrix));
	DEBUG_END;
	return startingVector;
}

static bool checkStartingVector(VectorXd startingVector, SparseMatrix matrix)
{
	DEBUG_START;
	VectorXd product = matrix * startingVector;
	for (unsigned int i = 0; i < startingVector.rows(); ++i)
		if (product(i) < 0.)
		{
			DEBUG_END;
			return false;
		}
	DEBUG_END;
	return true;
}
