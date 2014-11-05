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
 * @param data		The support function data.
 * @return		Starting vector.
 */
static VectorXd buildStartingVector(SupportFunctionDataPtr data);

/**
 * Checks starting vector.
 *
 * @param vector	Starting vector.
 * @param matrix	Support matrix.
 * @return		True, if the starting vector is correct.
 */
static bool checkStartingVector(VectorXd startingVector, SupportMatrix matrix)
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
	SupportFunctionDataPtr data, SupportMatrixType supportMatrixType)
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
		".support-vector.mat") << supportVector;

	/* Build starting vector. */
	VectorXd startingVector = buildStartingVector(data);
	ASSERT(checkStartingVector(startingVector, *supportMatrix));

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
	DEBUG_END;
	return matrix;
}

/**
 * Calculates support values for known support directions.
 *
 * @param directions	Support directions
 * @param p		Polyhedron
 *
 * @return		Support values
 */
static VectorXd calculateSupportValues(std::vector<Point_3> directions,
	Polyhedron_3 p)
{
	DEBUG_START;
	VectorXd values(directions.size());

	int i = 0;
	for (auto direction = directions.begin(); direction != directions.end();
			++direction)
	{
		Vector_3 vDirection = *direction - CGAL::ORIGIN;
		double scalarProductMax = 0.;
		for (auto vertex = p.vertices_begin();
			vertex != p.vertices_end(); ++vertex)
		{
			Vector_3 vVertex = vertex->point() - CGAL::ORIGIN;
			double scalarProduct = vDirection * vVertex;
			if (scalarProduct > scalarProductMax)
				scalarProductMax = scalarProduct;
		}
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

static VectorXd buildStartingVector(SupportFunctionDataPtr data)
{
	DEBUG_START;
	VectorXd startingVector(data->size());

#if 0	
	/* Construct intersection of support halfspaces represented by planes */
	std::vector<Plane_3> planes = data->supportPlanes();
	Polyhedron_3 intersection;
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
			".support-planes-for-halfspaces_intersection.txt")
		<< planes;
	CGAL::internal::halfspaces_intersection(planes.begin(), planes.end(),
		intersection, Kernel());

	ASSERT(is_strongly_convex_3(intersection));

	/*
	 * TODO: This assertion fails:
	 * ASSERT(is_strongly_convex_3(intersection));
	 * Why?
	 *
	 * As a workaround for this issue the hull is called.
	 */
	Polyhedron_3 hull;
	std::vector<Point_3> points;
	for (auto vertex = intersection.vertices_begin();
			vertex != intersection.vertices_end(); ++vertex)
	{
		points.push_back(vertex->point());
	}
	CGAL::convex_hull_3(points.begin(), points.end(), hull);
	ASSERT(is_strongly_convex_3(hull));
#endif
	
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
		data->supportDirectionsCGAL(), hull);
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
		".starting-vector.mat") << startingVector;
	DEBUG_END;
	return startingVector;
}


const double EPS_NEGATIVE_VIOLATION = 5e-4;

static bool checkStartingVector(VectorXd startingVector, SupportMatrix matrix)
{
	DEBUG_START;
	VectorXd product(matrix.rows());
	VectorXd units(matrix.cols());
	bool ifAllPositive = true;

	for (unsigned int i = 0; i < units.rows(); ++i)
		units(i) = 1.;
	product = matrix * units;
	for (unsigned int i = 0; i < startingVector.rows(); ++i)
		if (product(i) < 0.)
		{
			DEBUG_PRINT("product on units (%d) = %le", i,
					product(i));
			DEBUG_PRINT("matrix row #%d:", i);
			std::cerr << std::setprecision(16)
				<< matrix.block(i, 0, 1, matrix.cols());
			ifAllPositive = false;
		}

	product = matrix * startingVector;
	for (unsigned int i = 0; i < startingVector.rows(); ++i)
	{
		DEBUG_PRINT("matrix row #%d:", i);
		std::cerr << std::setprecision(16)
			<< matrix.block(i, 0, 1, matrix.cols());
		if (product(i) < -EPS_NEGATIVE_VIOLATION)
		{
			DEBUG_PRINT(COLOUR_RED "product(%d) = %le" COLOUR_NORM,
					i, product(i));
			ifAllPositive = false;
		}
	}
	DEBUG_END;
	return ifAllPositive;
}
