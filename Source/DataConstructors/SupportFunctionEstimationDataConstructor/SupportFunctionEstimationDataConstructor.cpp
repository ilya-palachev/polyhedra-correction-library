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
#include "DataContainers/SupportFunctionEstimationData/GardnerKiderlenSupportMatrix.h"
#include "DataConstructors/SupportFunctionEstimationDataConstructor/SupportFunctionEstimationDataConstructor.h"
#include "SparseMatrixEigen.h"
#include "halfspaces_intersection.h"
#include "PCLDumper.h"
#include "Polyhedron/Polyhedron.h"

#define UNUSED __attribute__((unused))

/**
 * Builds support matrix.
 *
 * @param data	The support function data.
 * @param type	The requested type of support matrix.
 *
 * @return	Support matrix.
 */
static SupportMatrix *buildSupportMatrix(SupportFunctionDataPtr data,
	SupportMatrixType type);

/**
 * Builds starting vector for the estimation process.
 *
 * @param data			The support function data.
 * @param startingBodyType	Starting bod type.
 *
 * @return			Starting vector.
 */
static VectorXd buildStartingVector(SupportFunctionDataPtr data,
		SupportFunctionEstimationStartingBodyType startingBodyType);

/**
 * Checks starting vector.
 *
 * @param vector	Starting vector.
 * @param matrix	Support matrix.
 * @param data		Support function data.
 * @return		Number of incorrect (negative) elements in product.
 */
static long int checkStartingVector(VectorXd startingVector,
		SupportMatrix matrix, SupportFunctionDataPtr data) UNUSED;

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
	SupportFunctionDataPtr data, SupportMatrixType supportMatrixType,
	SupportFunctionEstimationStartingBodyType startingBodyType)
{
	DEBUG_START;
	/* Remove equal items from data (and normalize all items). */
	data = data->removeEqual();

	/** Build support matrix. */
	SupportMatrix *supportMatrix = buildSupportMatrix(data,
		supportMatrixType);

	/* Build support vector. */
	VectorXd supportVector = data->supportValues();
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
		".support-vector-original.mat") << supportVector;
	if (checkStartingVector(supportVector, *supportMatrix, data) == 0)
	{
		std::cerr << "Original support vector satisfies consistency "
			<< "conditions!" << std::endl;
	}

	/* Build starting vector. */
	VectorXd startingVector = buildStartingVector(data, startingBodyType);
	ASSERT(checkStartingVector(startingVector, *supportMatrix, data) == 0);

	/* Get support directions from data. */
	std::vector<Vector3d> supportDirections = data->supportDirections();

	/* Construct data. */
	SupportFunctionEstimationDataPtr estimationData(new
		SupportFunctionEstimationData(*supportMatrix, supportVector,
				startingVector, supportDirections));
	delete supportMatrix;
	DEBUG_END;
	return estimationData;
}

static SupportMatrix *buildSupportMatrix(SupportFunctionDataPtr data,
	SupportMatrixType type)
{
	DEBUG_START;
	SupportMatrix *matrix = NULL;
	
	switch (type)
	{
	case SUPPORT_MATRIX_TYPE_KKVW:
	case SUPPORT_MATRIX_TYPE_KKVW_OPT:
		ASSERT(0 && "Not implemented yet!");
		break;
	case SUPPORT_MATRIX_TYPE_GK_OPT:
		matrix = constructReducedGardnerKiderlenSupportMatrix(data);
		break;
	case SUPPORT_MATRIX_TYPE_GK:
		matrix = constructGardnerKiderlenSupportMatrix(data);
		break;
	}
	if (!matrix)
		exit(EXIT_FAILURE);
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, ".support-matrix.mat")
		<< *matrix;
	DEBUG_END;
	return matrix;
}

/**
 * Calculates support values for known support directions.
 *
 * @param directions	Support directions
 * @param vertices	Vertices of the polyhedron.
 *
 * @return		Support values
 */
static VectorXd calculateSupportValues(std::vector<Point_3> directions,
	std::vector<Vector_3> vertices)
{
	DEBUG_START;
	VectorXd values(directions.size());

	int i = 0;
	for (auto direction = directions.begin(); direction != directions.end();
			++direction)
	{
		Vector_3 vDirection = *direction - CGAL::ORIGIN;
		double scalarProductMax = 0.;
		for (auto vertex = vertices.begin();
			vertex != vertices.end(); ++vertex)
		{
			double scalarProduct = vDirection * *vertex;
			if (scalarProduct > scalarProductMax)
				scalarProductMax = scalarProduct;
		}
		ASSERT(scalarProductMax > 0);
		values(i++) = scalarProductMax;
	}
	DEBUG_END;
	return values;
}

std::ostream &operator<<(std::ostream &stream, std::vector<Plane_3> planes)
{
	DEBUG_START;
	for (auto plane = planes.begin(); plane != planes.end(); ++plane)
		stream << plane->a() << " " << plane->b() << " " << plane->c()
			<< " " << plane->d() << std::endl;
	DEBUG_END;
	return stream;
}

static VectorXd buildCylindersIntersection(SupportFunctionDataPtr data)
{
	DEBUG_START;
	VectorXd startingVector(data->size());
	/* Construct intersection of support halfspaces represented by planes */
	std::vector<Plane_3> planes = data->supportPlanes();
	Polyhedron_3 intersection;
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
			".support-planes-for-halfspaces_intersection.txt")
		<< planes;
	CGAL::internal::halfspaces_intersection(planes.begin(), planes.end(),
		intersection, Kernel());

	startingVector = calculateSupportValues(
		data->supportDirectionsCGAL(), intersection.getVertices());
	DEBUG_END;
	return startingVector;
}

static VectorXd buildPointsHull(SupportFunctionDataPtr data)
{
	DEBUG_START;
	VectorXd startingVector(data->size());
	VectorXd supportVector = data->supportValues();
	std::vector<Vector3d> supportDirections = data->supportDirections();
	std::vector<Point_3> points;
	for (unsigned int i = 0; i < supportDirections.size(); ++i)
	{
		Vector3d point = supportDirections[i] * supportVector(i);
		points.push_back(Point_3(point.x, point.y, point.z));
	}
	Polyhedron_3 hull;

	CGAL::convex_hull_3(points.begin(), points.end(), hull);
	ASSERT(is_strongly_convex_3(hull));
	startingVector = calculateSupportValues(
		data->supportDirectionsCGAL(), hull.getVertices());
	DEBUG_END;
	return startingVector;
}

static VectorXd buildVectorOfUnits(SupportFunctionDataPtr data)
{
	DEBUG_START;
	VectorXd startingVector(data->size());
	for (int i = 0; i < data->size(); ++i)
		startingVector(i) = 1.;
	DEBUG_END;
	return startingVector;
}

static VectorXd buildVectorFromCube(SupportFunctionDataPtr data)
{
	DEBUG_START;
	VectorXd startingVector(data->size());

	std::vector<Point_3> points;
	points.push_back(Point_3(-1., -1., -1.));
	points.push_back(Point_3(-1., -1.,  1.));
	points.push_back(Point_3(-1.,  1., -1.));
	points.push_back(Point_3(-1.,  1.,  1.));
	points.push_back(Point_3( 1., -1., -1.));
	points.push_back(Point_3( 1., -1.,  1.));
	points.push_back(Point_3( 1.,  1., -1.));
	points.push_back(Point_3( 1.,  1.,  1.));
	Polyhedron_3 cube;
	CGAL::convex_hull_3(points.begin(), points.end(), cube);
	ASSERT(is_strongly_convex_3(cube));

	startingVector = calculateSupportValues(
		data->supportDirectionsCGAL(), cube.getVertices());

	DEBUG_END;
	return startingVector;
}

static VectorXd buildStartingVector(SupportFunctionDataPtr data,
		SupportFunctionEstimationStartingBodyType startingBodyType)
{
	DEBUG_START;
	VectorXd startingVector(data->size());

	switch (startingBodyType)
	{
	case SUPPORT_FUNCTION_ESTIMATION_STARTING_BODY_TYPE_CYLINDERS_INTERSECTION:
		startingVector = buildCylindersIntersection(data);
		break;
	case SUPPORT_FUNCTION_ESTIMATION_STARTING_BODY_TYPE_POINTS_HULL:
		startingVector = buildPointsHull(data);
		break;
	case SUPPORT_FUNCTION_ESTIMATION_STARTING_BODY_TYPE_SPHERE:
		startingVector = buildVectorOfUnits(data);
		break;
	case SUPPORT_FUNCTION_ESTIMATION_STARTING_BODY_TYPE_CUBE:
		startingVector = buildVectorFromCube(data);
		break;
	case SUPPORT_FUNCTION_ESTIMATION_STARTING_BODY_TYPE_PYRAMID:
	case SUPPORT_FUNCTION_ESTIMATION_STARTING_BODY_TYPE_PRISM:
	default:
		ERROR_PRINT("This type of starting body is not implemented "
				"yet!");
		exit(EXIT_FAILURE);
	}

	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
		".starting-vector.mat") << startingVector;
	DEBUG_END;
	return startingVector;
}

static std::set<int> findNonZeroElementsInRow(SupportMatrix &matrix, int idRow)
{
	std::set<int> idColNonZero;
	for (int k = 0; k < matrix.outerSize(); ++k)
	{
		for (Eigen::SparseMatrix<double>::InnerIterator
			it(matrix, k); it; ++it)
		{
			if (it.row() == idRow)
			{
				DEBUG_PRINT("Nonzero element [%d][%d]",
						it.row(), it.col());
				idColNonZero.insert(it.col());
			}
		}
	}
	return idColNonZero;
}

static long int checkStartingVector(VectorXd startingVector,
		SupportMatrix matrix, SupportFunctionDataPtr data)
{
	DEBUG_START;
	VectorXd product(matrix.rows());
	VectorXd units(matrix.cols());
	long int numNegative = 0;

	for (unsigned int i = 0; i < units.rows(); ++i)
		units(i) = 1.;
	product = matrix * units;
	for (unsigned int i = 0; i < product.rows(); ++i)
		if (product(i) < 0.)
		{
			DEBUG_PRINT("product on units (%d) = %le", i,
					product(i));
			DEBUG_PRINT("matrix row #%d:", i);
#ifndef NDEBUG
			std::cerr << std::setprecision(16)
				<< matrix.block(i, 0, 1, matrix.cols());
#endif
		}

	auto directions = data->supportDirections();
	product = matrix * startingVector;
	for (unsigned int i = 0; i < product.rows(); ++i)
	{
		if (product(i) < 0.)
		{
			DEBUG_PRINT(COLOUR_RED "product(%d) = %le" COLOUR_NORM,
					i, product(i));
			DEBUG_PRINT("matrix row #%d:", i);
			std::set<int> idColNonZero =
				findNonZeroElementsInRow(matrix, i);
			for (auto &iCol: idColNonZero)
			{
				DEBUG_PRINT("(%.16le) * (%.16le) = (%.16le)",
						matrix.coeffRef(i, iCol),
						startingVector(iCol),
						matrix.coeffRef(i, iCol)
						* startingVector(iCol));
				std::cerr << "u[" << iCol << "] = "
					<< directions[iCol] << std::endl;
				std::cerr << "uh0[" << iCol << "] = "
					<< directions[iCol]
					* startingVector(iCol) << std::endl;
			}
#ifndef NDEBUG
			std::cerr << std::setprecision(16)
				<< matrix.block(i, 0, 1, matrix.cols());
#endif
			++numNegative;
		}
	}

	DEBUG_PRINT("(%d x %d) * (%ld x %ld) = (%ld x %ld)",
			matrix.rows(), matrix.cols(),
			startingVector.rows(), startingVector.cols(),
			product.rows(), product.cols());
	DEBUG_PRINT("Check: %ld of %ld product elements are negative",
			numNegative, product.rows());
	DEBUG_END;
	return numNegative;
}
