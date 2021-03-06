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
 * @file SupportFunctionEstimationDataConstructor.cpp
 * @brief Constructor of data used for support function estimation process
 * - implementation.
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "DataContainers/SupportFunctionEstimationData/GardnerKiderlenSupportMatrix.h"
#include "DataConstructors/SupportFunctionEstimationDataConstructor/SupportFunctionEstimationDataConstructor.h"
#include "SparseMatrixEigen.h"
#include "PCLDumper.h"
#include "Polyhedron/Polyhedron.h"

#define UNUSED __attribute__((unused))

/**
 * Builds support matrix.
 *
 * @param data			The support function data.
 * @param type			The requested type of support matrix.
 * @param startingVector	The starting vector of the algorithm.
 * @param startingEpsilon	The maximum displacement of h_i for for starting
 * 				vector.
 *
 * @return	Support matrix.
 */
static SupportMatrix *buildSupportMatrix(SupportFunctionDataPtr data,
										 SupportMatrixType type,
										 VectorXd startingVector,
										 double startingEpsilon,
										 bool ifShadowHeuristics);

/**
 * Builds starting vector for the estimation process.
 *
 * @param data			The support function data.
 * @param startingBodyType	Starting bod type.
 *
 * @return			Starting vector.
 */
static VectorXd
buildStartingVector(SupportFunctionDataPtr data,
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
									SupportMatrix *matrix,
									SupportFunctionDataPtr data) UNUSED;

SupportFunctionEstimationDataConstructor::
	SupportFunctionEstimationDataConstructor() :
	ifScaleMatrix(false), ifShadowHeuristics_(false)
{
	DEBUG_START;
	DEBUG_END;
}

SupportFunctionEstimationDataConstructor::
	~SupportFunctionEstimationDataConstructor()
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

void SupportFunctionEstimationDataConstructor::enableShadowHeuristics()
{
	DEBUG_START;
	ifShadowHeuristics_ = true;
	DEBUG_END;
}

static bool supportVectorAlreadyConsistent(VectorXd supportVector,
										   SupportMatrix *supportMatrix,
										   SupportFunctionDataPtr data)
{
	DEBUG_START;
	if (!supportMatrix)
	{
		DEBUG_END;
		return true;
	}
	auto directions = data->supportDirections<Vector3d>();
	VectorXd startingFromSupportVector(3 * directions.size());
	for (unsigned int i = 0; i < directions.size(); ++i)
	{
		startingFromSupportVector(3 * i) = directions[i].x * supportVector(i);
		startingFromSupportVector(3 * i + 1) =
			directions[i].y * supportVector(i);
		startingFromSupportVector(3 * i + 2) =
			directions[i].z * supportVector(i);
	}
	int numNegative =
		checkStartingVector(startingFromSupportVector, supportMatrix, data);
	DEBUG_END;
	return numNegative == 0;
}

static double calculateEpsilon(SupportFunctionDataPtr data,
							   VectorXd startingVector)
{
	DEBUG_START;
	auto directions = data->supportDirections<Vector3d>();
	VectorXd supportVector = data->supportValues();
	int numDirections = directions.size();
	double max = 0.;
	for (int i = 0; i < numDirections; ++i)
	{
		Vector3d point(startingVector(3 * i), startingVector(3 * i + 1),
					   startingVector(3 * i + 2));
		double difference = fabs(supportVector(i) - point * directions[i]);
		if (difference > max)
		{
			max = difference;
		}
	}
	DEBUG_PRINT("result: %lf", max);
	DEBUG_END;
	return max;
}

SupportFunctionEstimationDataPtr SupportFunctionEstimationDataConstructor::run(
	SupportFunctionDataPtr data, SupportMatrixType supportMatrixType,
	SupportFunctionEstimationStartingBodyType startingBodyType)
{
	DEBUG_START;
	/* Remove equal items from data (and normalize all items). */
	if (!ifShadowHeuristics_)
	{
		std::cerr << "Removing equal items..." << std::endl;
		data = data->removeEqual();
	}

	/* Build starting vector. */
	std::cerr << "Building starting vector..." << std::endl;
	VectorXd startingVector = buildStartingVector(data, startingBodyType);

	/** Build starting epsilon. */
	std::cerr << "Building starting epsilon..." << std::endl;
	double startingEpsilon = calculateEpsilon(data, startingVector);

	/** Build support matrix. */
	std::cerr << "Building support matrix..." << std::endl;
	SupportMatrix *supportMatrix =
		buildSupportMatrix(data, supportMatrixType, startingVector,
						   startingEpsilon, ifShadowHeuristics_);

	/* Build support vector. */
	VectorXd supportVector = data->supportValues();
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "support-vector-original.mat")
		<< supportVector;
	if (supportMatrix)
	{
		std::cerr << "Checking whether the support vector already "
					 "satisfies consistency conditions..."
				  << std::endl;
		if (supportVectorAlreadyConsistent(supportVector, supportMatrix, data))
		{
			std::cerr << "Original support vector satisfies "
						 "consistency conditions!"
					  << std::endl;
		}
	}

	/* Check starting vector. */
#ifndef NDEBUG
	std::cerr << "Checking whether the starting vector already satisfies "
				 "consistency conditions..."
			  << std::endl;
	int numViolations =
		checkStartingVector(startingVector, supportMatrix, data);
	ASSERT(numViolations == 0 && "Starting point is infeasible");
#endif

	/* Get support directions from data. */
	std::cerr << "Finally constructing the data structures..." << std::endl;
	auto supportDirections = data->supportDirections<Vector3d>();

	/* Construct data. */
	if (!supportMatrix)
	{
		supportMatrix = new SupportMatrix();
	}
	SupportFunctionEstimationDataPtr estimationData(
		new SupportFunctionEstimationData(
			*supportMatrix, supportVector, startingVector, supportDirections,
			startingEpsilon, data, ifShadowHeuristics_));
	delete supportMatrix;
	DEBUG_END;
	return estimationData;
}

static SupportMatrix *buildSupportMatrix(SupportFunctionDataPtr data,
										 SupportMatrixType type,
										 VectorXd startingVector,
										 double startingEpsilon,
										 bool ifShadowHeuristics)
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
		matrix = constructReducedGardnerKiderlenSupportMatrix(
			data, startingEpsilon, ifShadowHeuristics);
		break;
	case SUPPORT_MATRIX_TYPE_GK:
		matrix = constructGardnerKiderlenSupportMatrix(data);
		break;
	case SUPPORT_MATRIX_TYPE_EMPTY:
		matrix = NULL;
		break;
	}
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
	std::vector<Vector_3> points;

	for (auto direction = directions.begin(); direction != directions.end();
		 ++direction)
	{
		Vector_3 vDirection = *direction - CGAL::ORIGIN;
		double scalarProductMax = 0.;
		Vector_3 supportPoint;
		for (auto vertex = vertices.begin(); vertex != vertices.end(); ++vertex)
		{
			double scalarProduct = vDirection * *vertex;
			if (scalarProduct > scalarProductMax)
			{
				scalarProductMax = scalarProduct;
				supportPoint = *vertex;
			}
		}
		ASSERT(scalarProductMax > 0);
		points.push_back(supportPoint);
	}
	VectorXd values(3 * points.size());
	for (unsigned int i = 0; i < points.size(); ++i)
	{
		values(3 * i) = points[i].x();
		values(3 * i + 1) = points[i].y();
		values(3 * i + 2) = points[i].z();
	}
	DEBUG_END;
	return values;
}

std::ostream &operator<<(std::ostream &stream, std::vector<Plane_3> planes)
{
	DEBUG_START;
	for (auto plane = planes.begin(); plane != planes.end(); ++plane)
		stream << plane->a() << " " << plane->b() << " " << plane->c() << " "
			   << plane->d() << std::endl;
	DEBUG_END;
	return stream;
}

static VectorXd buildCylindersIntersection(SupportFunctionDataPtr data)
{
	DEBUG_START;
	VectorXd startingVector(3 * data->size());
	/* Construct intersection of support halfspaces represented by planes */
	std::vector<Plane_3> planes = data->supportPlanes();
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
					"support-planes-for-halfspaces_intersection.txt")
		<< planes;

	if (planes.empty())
	{
		ERROR_PRINT("No support planes are given!");
		exit(EXIT_FAILURE);
	}

	DEBUG_PRINT("Computing intersection of %ld halfspaces.", planes.size());
	ASSERT(planes.size() > 3);
	Polyhedron_3 intersection(planes);

	startingVector = calculateSupportValues(data->supportDirections<Point_3>(),
											intersection.getVertices());
	DEBUG_END;
	return startingVector;
}

static VectorXd buildPointsHull(SupportFunctionDataPtr data)
{
	DEBUG_START;
	VectorXd startingVector(3 * data->size());
	VectorXd supportVector = data->supportValues();
	auto supportDirections = data->supportDirections<Vector3d>();

	if (supportDirections.empty())
	{
		ERROR_PRINT("No support directions are given!");
		exit(EXIT_FAILURE);
	}

	std::vector<Point_3> points;
	for (unsigned int i = 0; i < supportDirections.size(); ++i)
	{
		Vector3d point = supportDirections[i] * supportVector(i);
		points.push_back(Point_3(point.x, point.y, point.z));
	}
	Polyhedron_3 hull;

	CGAL::convex_hull_3(points.begin(), points.end(), hull);
	ASSERT(is_strongly_convex_3(hull));
	startingVector = calculateSupportValues(data->supportDirections<Point_3>(),
											hull.getVertices());
	DEBUG_END;
	return startingVector;
}

static VectorXd buildVectorOfUnits(SupportFunctionDataPtr data)
{
	DEBUG_START;
	VectorXd startingVector(3 * data->size());
	auto supportDirections = data->supportDirections<Vector3d>();
	for (int i = 0; i < data->size(); ++i)
	{
		startingVector(3 * i) = supportDirections[i].x;
		startingVector(3 * i + 1) = supportDirections[i].y;
		startingVector(3 * i + 2) = supportDirections[i].z;
	}
	DEBUG_END;
	return startingVector;
}

static VectorXd buildVectorFromCube(SupportFunctionDataPtr data)
{
	DEBUG_START;
	VectorXd startingVector(3 * data->size());

	std::vector<Point_3> points;
	points.push_back(Point_3(-1., -1., -1.));
	points.push_back(Point_3(-1., -1., 1.));
	points.push_back(Point_3(-1., 1., -1.));
	points.push_back(Point_3(-1., 1., 1.));
	points.push_back(Point_3(1., -1., -1.));
	points.push_back(Point_3(1., -1., 1.));
	points.push_back(Point_3(1., 1., -1.));
	points.push_back(Point_3(1., 1., 1.));
	Polyhedron_3 cube;
	CGAL::convex_hull_3(points.begin(), points.end(), cube);
	ASSERT(is_strongly_convex_3(cube));

	startingVector = calculateSupportValues(data->supportDirections<Point_3>(),
											cube.getVertices());

	DEBUG_END;
	return startingVector;
}

static VectorXd
buildStartingVector(SupportFunctionDataPtr data,
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

	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "starting-vector.mat")
		<< startingVector;
	DEBUG_END;
	return startingVector;
}

static std::set<int> findNonZeroElementsInRow(SupportMatrix &matrix, int idRow)
{
	std::set<int> idColNonZero;
	for (int k = 0; k < matrix.outerSize(); ++k)
	{
		for (Eigen::SparseMatrix<double>::InnerIterator it(matrix, k); it; ++it)
		{
			if (it.row() == idRow)
			{
				DEBUG_PRINT("Nonzero element [%ld][%ld]", it.row(), it.col());
				idColNonZero.insert(it.col());
			}
		}
	}
	return idColNonZero;
}

const double EPS_SUPPORT_CONSISTENCY_LIMIT = 1e-15;

static long int checkStartingVector(VectorXd startingVector,
									SupportMatrix *matrix,
									SupportFunctionDataPtr data)
{
	DEBUG_START;
	if (!matrix)
	{
		DEBUG_END;
		return 0;
	}
	VectorXd product(matrix->rows());
	long int numNegative = 0;

	auto directions = data->supportDirections<Vector3d>();
	ASSERT(directions.size() * 3 == (unsigned)matrix->cols());
	ASSERT(directions.size() * 3 == (unsigned)startingVector.rows());
	product = (*matrix) * startingVector;
	for (unsigned int i = 0; i < product.rows(); ++i)
	{
		if (product(i) < -EPS_SUPPORT_CONSISTENCY_LIMIT)
		{
			DEBUG_PRINT(COLOUR_RED "product(%d) = %le" COLOUR_NORM, i,
						product(i));
			DEBUG_PRINT("matrix row #%d:", i);
			std::set<int> idColNonZero = findNonZeroElementsInRow(*matrix, i);
#ifndef NDEBUG
			for (auto &iCol : idColNonZero)
			{
				DEBUG_PRINT("(%.16le) * (%.16le) = (%.16le)",
							matrix->coeffRef(i, iCol), startingVector(iCol),
							matrix->coeffRef(i, iCol) * startingVector(iCol));
			}
#if 0
			std::cerr << std::setprecision(16)
				<< matrix->block(i, 0, 1, matrix->cols());
#endif /* 0 */
#endif
			++numNegative;
#ifdef NDEBUG
			std::cout << "Stop check after 1st fail for release run."
					  << std::endl;
			return numNegative;
#endif
		}
	}

	DEBUG_PRINT("(%ld x %ld) * (%ld x %ld) = (%ld x %ld)", matrix->rows(),
				matrix->cols(), startingVector.rows(), startingVector.cols(),
				product.rows(), product.cols());
	DEBUG_PRINT("Check: %ld of %ld product elements are negative", numNegative,
				product.rows());
	DEBUG_END;
	return numNegative;
}

const double EPSILON_MAX_VIOLATION = 1e-7;

bool SupportFunctionEstimationDataConstructor::checkResult(
	SupportFunctionEstimationDataPtr data, SupportMatrixType supportMatrixType,
	VectorXd estimate)
{
	DEBUG_START;
	if (supportMatrixType != SUPPORT_MATRIX_TYPE_GK &&
		supportMatrixType != SUPPORT_MATRIX_TYPE_GK_OPT)
	{
		ERROR_PRINT("Not implemented yet!");
		DEBUG_END;
		return true;
	}
	auto directions = data->supportDirections();
	int numDirections = directions.size();
	std::cout << "Number of directions: " << numDirections << std::endl;
	std::cout << "Size of the estimate vector: " << estimate.size()
			  << std::endl;
	ASSERT(3 * numDirections == estimate.size() ||
		   numDirections == estimate.size());

#ifndef NDEBUG
	for (int i = 0; i < estimate.rows(); ++i)
	{
		DEBUG_PRINT("estimate(%d) = %lf", i, estimate(i));
	}
#endif

	std::vector<Vector3d> points;
	for (int i = 0; i < numDirections; ++i)
	{
		points.push_back(Vector3d(estimate(3 * i), estimate(3 * i + 1),
								  estimate(3 * i + 2)));
	}

	int numViolations = 0;
	double minViolation = 0.;
	for (int i = 0; i < numDirections; ++i)
	{
		for (int j = 0; j < numDirections; ++j)
		{
			if (j == i)
				continue;
			double violation =
				directions[i] * points[i] - directions[i] * points[j];
			if (violation < 0. && violation < minViolation)
				minViolation = violation;
			if (violation < -EPSILON_MAX_VIOLATION)
			{
#ifndef NDEBUG
				std::cerr << "directions[" << i << "] = " << directions[i]
						  << std::endl;
				std::cerr << "point[" << i << "] = " << points[i] << std::endl;
				std::cerr << "point[" << j << "] = " << points[j] << std::endl;
#endif /* NDEBUG */
				DEBUG_PRINT("directions[%d] * "
							"points[%d] = %lf ; ",
							i, i, directions[i] * points[i]);
				DEBUG_PRINT("directions[%d] * "
							"points[%d] = %lf ; ",
							i, j, directions[i] * points[j]);
				DEBUG_PRINT("Violation in (%d, %d) "
							"= %.16lf\n",
							i, j, violation);
				++numViolations;
				if (violation < minViolation)
					minViolation = violation;
			}
		}
	}

	ALWAYS_PRINT(stdout, "Number of violations = %d, min = %.16lf\n",
				 numViolations, minViolation);
	DEBUG_END;
	return numViolations == 0;
}
