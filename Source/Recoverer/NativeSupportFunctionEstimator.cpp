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
 * @file NativeSupportFunctionEstimator.cpp
 * @brief Native estimation engine (implementation).
 */

#include <Eigen/LU>
#include "DebugPrint.h"
#include "DebugAssert.h"
#include "PCLDumper.h"
#include "Recoverer/NativeSupportFunctionEstimator.h"
#include "Polyhedron_3/Polyhedron_3.h"
#include <CGAL/linear_least_squares_fitting_3.h>
#include "Recoverer/Colouring.h"

NativeSupportFunctionEstimator::NativeSupportFunctionEstimator(
		SupportFunctionEstimationDataPtr data) :
	SupportFunctionEstimator(data),
	problemType_(DEFAULT_ESTIMATION_PROBLEM_NORM)
{
	DEBUG_START;
	DEBUG_END;
}

NativeSupportFunctionEstimator::~NativeSupportFunctionEstimator()
{
	DEBUG_START;
	DEBUG_END;
}

void NativeSupportFunctionEstimator::setProblemType(EstimationProblemNorm type)
{
	DEBUG_START;
	problemType_ = type;
	DEBUG_END;
}

#include <CGAL/Bbox_3.h>
#include <CGAL/Delaunay_triangulation_3.h>
typedef CGAL::Triangulation_data_structure_3<
	CGAL::Triangulation_vertex_base_3<Kernel>,
		CGAL::Triangulation_cell_base_3<Kernel>,
			CGAL::Parallel_tag> Tds;
typedef CGAL::Delaunay_triangulation_3<Kernel, Tds> Delaunay;

int findPoint(std::vector<Point_3> points, Point_3 query)
{
	DEBUG_START;
	int iMinimum = 0;
	double distanceMinimal = (query - points[0]).squared_length();
	int numPoints = points.size();
	for (int i = 0; i < numPoints; ++i)
	{
		double distance = (query - points[i]).squared_length();
		if (distance < distanceMinimal)
		{
			distanceMinimal = distance;
			iMinimum = i;
		}
	}
	DEBUG_END;
	return iMinimum;
}

static std::vector<int> collectOuterPlanesIDs(SupportFunctionDataPtr data,
		double epsilon)
{
	DEBUG_START;
	/* Get duals of higher and lower support planes */
	auto pointsHigher = data->getShiftedDualPoints_3(epsilon);
	auto pointsLower = data->getShiftedDualPoints_3(-epsilon);

	/* Construct 3D Delaunay triangulation of points. */
	Delaunay::Lock_data_structure locking_ds(
			CGAL::Bbox_3(-1000., -1000., -1000., 1000., 1000.,
				1000.), 50);
	Delaunay triangulation(pointsHigher.begin(), pointsHigher.end(),
			&locking_ds);
	auto infinity = triangulation.infinite_vertex();

	std::vector<int> outerPlanesIDs;
	int numPlanes = data->size();
	for (int i = 0; i < numPlanes; ++i)
	{
		Point_3 point = pointsLower[i];
		auto cell = triangulation.locate(point, infinity);
		if (!triangulation.is_infinite(cell))
		{
			outerPlanesIDs.push_back(i);
		}
	}
	std::cerr << "IDs of outer planes: ";
	for (int &id: outerPlanesIDs)
	{
		std::cerr << id << " ";
	}
	std::cerr << std::endl;
	DEBUG_END;
	return outerPlanesIDs;
}

static VectorXd calculateSolution(SupportFunctionDataPtr data, double epsilon)
{
	DEBUG_START;
	VectorXd solution(3 * data->size());
	for (int i = 0; i < data->size(); ++i)
	{
		auto item = (*data)[i];
		Vector3d tangient = item.direction * (item.value - epsilon);
		solution(3 * i) = tangient.x;
		solution(3 * i + 1) = tangient.y;
		solution(3 * i + 2) = tangient.z;
	}
	DEBUG_END;
	return solution;
}

const double BEST_EPSILON_PRECISION = 1e-6;
const double SEARCH_MULTIPLIER = 0.5;

VectorXd runLinfEstimation(SupportFunctionEstimationDataPtr data)
{
	DEBUG_START;
	SupportFunctionDataPtr supportData = data->supportData();
	double startingEpsilon = data->startingEpsilon();

	double badEpsilon = 0.;
	double goodEpsilon = startingEpsilon;
	int iIteration = 0;
	std::cout << "startingEpsilon = " << startingEpsilon <<std::endl;
	while (goodEpsilon - badEpsilon > BEST_EPSILON_PRECISION)
	{
		double epsilon = badEpsilon +
			(goodEpsilon - badEpsilon) * SEARCH_MULTIPLIER;
		std::cerr << "Native estimator: iteration #" << iIteration <<
			": epsilon = " << epsilon << "... ";
		std::vector<int> outerPlanesIDs =
			collectOuterPlanesIDs(supportData, epsilon);
		if (outerPlanesIDs.size() == 0)
		{
			goodEpsilon = epsilon;
			std::cerr << "SUCCESS" << std::endl;
		}
		else
		{
			badEpsilon = epsilon;
		}
		++iIteration;
	}

	VectorXd solution = calculateSolution(supportData, goodEpsilon);
	DEBUG_END;
	return solution;
}

std::vector<int> collectInnerPointsIDs(std::vector<Point_3> points)
{
	DEBUG_START;
	Delaunay::Lock_data_structure locking_ds(
			CGAL::Bbox_3(-1000., -1000., -1000., 1000.,
				1000., 1000.), 50);
	Delaunay triangulation(points.begin(), points.end(),
			&locking_ds);
	auto infinity = triangulation.infinite_vertex();

	std::vector<int> outerPlanesIDs;
	int numPlanes = points.size();
	for (int i = 0; i < numPlanes; ++i)
	{
		Point_3 point = points[i];
		Delaunay::Locate_type lt;
		int li, lj;
		Delaunay::Cell_handle c = triangulation.locate(point,
				lt, li, lj);

		auto vertex = c->vertex(li);
		ASSERT(lt == Delaunay::VERTEX
					&& vertex->point() == point);
		if (!triangulation.is_edge(vertex, infinity, c, li, lj))
		{
			outerPlanesIDs.push_back(i);
		}
	}
	DEBUG_END;
	return outerPlanesIDs;
}

int countInnerPoints(std::vector<Point_3> points)
{
	DEBUG_START;
	int numInnerPoints = collectInnerPointsIDs(points).size();
	DEBUG_END;
	return numInnerPoints;
}

void runContoursCounterDiagnostics(SupportFunctionEstimationDataPtr data,
		std::vector<int> index)
{
	DEBUG_START;
	SupportFunctionDataPtr supportData = data->supportData();
	auto planes = supportData->supportPlanes();
	std::vector<int> contoursCounter(planes.size());
	for (int i = 0; i < (int) planes.size(); ++i)
	{
		contoursCounter[i] = -1;
	}
	std::vector<int> contoursNumSides(planes.size());
	for (auto &i: index)
	{
		auto item = (*supportData)[i];
		int iContour = item.info->iContour;
		if (contoursCounter[iContour] < 0)
		{
			contoursCounter[iContour] = 1;
			contoursNumSides[iContour] = item.info->numSidesContour;
		}
		else
		{
			contoursCounter[iContour] += 1;
			if (contoursNumSides[iContour]
					!= item.info->numSidesContour)
			{
				std::cerr << "Wrong info about sides number "
					<< "in contour" << iContour
					<< std::endl;
			}
		}
	}

	for (int i = 0; i < (int) contoursCounter.size(); ++i)
	{
		if (contoursCounter[i] > 0)
			std::cerr << "Contour " << i << " has "
				<< contoursCounter[i] << " consistent of "
				<< contoursNumSides[i] << " sides."
				<< std::endl;
	}
	DEBUG_END;
}

std::set<int> findTangientPointPlanesIDs(
		Polyhedron_3 *polyhedron, Polyhedron_3::Vertex_iterator vertex,
		std::vector<int> index)
{
	DEBUG_START;
	auto circulatorFirst = vertex->vertex_begin();
	auto circulator = circulatorFirst;
	std::set<int> planesIDs;
	std::vector<int> planesIDsVector;
	do
	{
		int iFacet = circulator->facet()->id;
		if (iFacet > (int) polyhedron->size_of_facets())
		{
			ERROR_PRINT("%d > %ld", iFacet,
					polyhedron->size_of_facets());
			exit(EXIT_FAILURE);
		}
		planesIDs.insert(index[iFacet]);
		planesIDsVector.push_back(index[iFacet]);
		++circulator;
	} while (circulator != circulatorFirst);

	if (planesIDs.size() != 3)
	{
		ERROR_PRINT("%d != %d", (int) planesIDs.size(), 3);
		std::cerr << "Indices:";
		for (int i: planesIDsVector)
			std::cerr << " " << i;
		std::cerr << std::endl;
		std::cerr << "degree of vertex " << vertex->id << " is "
			<< vertex->degree() << std::endl;
		exit(EXIT_FAILURE);
	}
	DEBUG_END;
	return planesIDs;
}

Eigen::Vector3d decomposeDirection(SupportFunctionDataPtr data, int planeID,
		std::set<int> planesIDs)
{
	DEBUG_START;
	auto directions = data->supportDirections();

	Eigen::Matrix3d matrix;
	int i = 0;
	for (int id: planesIDs)
	{
		auto direction = directions[id];
		matrix(0, i) = direction.x;
		matrix(1, i) = direction.y;
		matrix(2, i) = direction.z;
		++i;
	}
	Eigen::Matrix3d inverse = matrix.inverse();

	auto direction = directions[planeID];
	Eigen::Vector3d vector;
	vector(0) = direction.x;
	vector(1) = direction.y;
	vector(2) = direction.z;

	Eigen::Vector3d solution = inverse * vector;

	DEBUG_END;
	return solution;
}

double calculateMinimalShift(SupportFunctionDataPtr data, int planeID,
		std::set<int> planesIDs)
{
	DEBUG_START;
	auto values = data->supportValues();

	Eigen::Vector3d solution = decomposeDirection(data, planeID, planesIDs);

	double numerator = values(planeID);
	int i = 0;
	for (int id: planesIDs)
	{
		numerator -= solution(i) * values(id);
		++i;
	}

	double denominator = 1.;
	for (int i = 0; i < 3; ++i)
	{
		denominator += solution(i);
	}

	double epsilon = numerator / denominator;

	DEBUG_END;
	return epsilon;
}


std::pair<SparseMatrix, VectorXd> buildMatrix(SupportFunctionDataPtr data,
		Polyhedron_3 *intersection, std::vector<int> index)
{
	DEBUG_START;
	auto outerIndex = collectInnerPointsIDs(
			data->getShiftedDualPoints_3(0.));
	auto directions = data->supportDirections();
	auto values = data->supportValues();

	typedef Eigen::Triplet<double> Triplet;
	auto comparison = [](Triplet a, Triplet b)
	{
		return a.row() < b.row() ||
			(a.row() == b.row() && a.col() < b.col());
	};
	std::set<Triplet, decltype(comparison)> triplets(comparison);
	int numValues = values.size();
	VectorXd rightSide(numValues);
	for (int i = 0; i < numValues; ++i)
	{
		rightSide(i) = values(i);
		triplets.insert(Triplet(i, i, 1.));
	}

	Polyhedron_3::Vertex_iterator tangient;
	for (int &iOuter: outerIndex)
	{
		rightSide(iOuter) = 0.;
		auto pair = intersection->findTangientVertex(
				directions[iOuter]);
		auto tangient = pair.first;

		auto planesIDs = findTangientPointPlanesIDs(intersection,
				tangient, index);
		auto coefficients = decomposeDirection(data, iOuter, planesIDs);
		int i = 0;
		for (int iPlane: planesIDs)
		{
			rightSide(iPlane) += coefficients[i] * values(iOuter);
			Triplet triplet(iOuter, iPlane, -coefficients[i]);
			triplets.insert(triplet);
			int j = 0;
			for (int jPlane: planesIDs)
			{
				double value = coefficients[i]
					* coefficients[j];
				Triplet triplet(iPlane, jPlane, value);
				auto iterator = triplets.find(triplet);
				if (iterator == triplets.end())
				{
					triplets.insert(triplet);
				}
				else
				{
					value += iterator->value();
					Triplet triplet(iPlane, jPlane, value);
					triplets.erase(iterator);
					triplets.insert(triplet);
				}
				++j;
			}
			++i;
		}
	}
	SparseMatrix matrix(numValues, numValues);
	matrix.setFromTriplets(triplets.begin(), triplets.end());
	DEBUG_END;
	return std::pair<SparseMatrix, VectorXd>(matrix, rightSide);
}

void runInconsistencyDiagnostics(SupportFunctionDataPtr data,
		Polyhedron_3 *intersection, std::vector<int> index)
{
	DEBUG_START;

	auto outerIndex = collectInnerPointsIDs(
			data->getShiftedDualPoints_3(0.));
	auto directions = data->supportDirections();
	auto values = data->supportValues();

	double minimal = 0.;
	int iWorst = -1;
	Polyhedron_3::Vertex_iterator tangient;
	for (int &iOuter: outerIndex)
	{
		auto pair = intersection->findTangientVertex(
				directions[iOuter]);
		auto vertex = pair.first;
		if (!vertex->is_trivalent())
		{
			ERROR_PRINT("vertex has %ld degree %d",
					vertex->id, (int) vertex->degree());
			ERROR_PRINT("Not implemented yet!");
			exit(EXIT_FAILURE);
		}
		double value = pair.second;
		double difference = value - values(iOuter);
		if (difference < minimal)
		{
			minimal = difference;
			iWorst = iOuter;
			tangient = vertex;
		}
	}
	std::cerr << "Worst plane: " << iWorst << " " << minimal << std::endl;
	auto planesIDs = findTangientPointPlanesIDs(intersection, tangient,
			index);
	double epsilon = calculateMinimalShift(data, iWorst, planesIDs);
	std::cerr << "epsilon = " << epsilon << std::endl;

	DEBUG_END;
}


static VectorXd calculateSolution(SupportFunctionDataPtr data, VectorXd values)
{
	DEBUG_START;
	VectorXd difference = values - data->supportValues();
	std::vector<double> epsilons;
	for (int i = 0; i < (int) difference.size(); ++i)
	{
		epsilons.push_back(difference(i));
	}

	auto points = data->getShiftedDualPoints_3(epsilons);
	auto innerIndex = collectInnerPointsIDs(points);
	std::cerr << innerIndex.size() << " points are inner" << std::endl;

	std::vector<Plane_3> planes;
	auto directions = data->supportDirectionsCGAL();
	for (int i = 0; i < (int) directions.size(); ++i)
	{
		auto direction = directions[i];
		Plane_3 plane(direction.x(), direction.y(), direction.z(),
				-values(i));
		planes.push_back(plane);
	}
	Polyhedron_3 polyhedron(planes);
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
			"recovered-by-native-estimator.ply") << polyhedron;

	VectorXd solution =
		polyhedron.findTangientPointsConcatenated(directions);
	DEBUG_END;
	return solution;
}

void validateEstimate(std::vector<Plane_3> planesOld, VectorXd valuesNew)
{
	DEBUG_START;
	std::vector<Plane_3> planesNew(planesOld.size());
	for (int i = 0; i < (int) valuesNew.size(); ++i)
	{
		Plane_3 plane = planesOld[i];
		planesNew[i] = Plane_3(plane.a(), plane.b(), plane.c(),
				-valuesNew[i]);
	}
	Polyhedron_3 intersection(planesNew);
	int numOuterPlanes = 0;
	for (int i = 0; i < (int) valuesNew.size(); ++i)
	{
		Plane_3 plane = planesNew[i];
		Point_3 direction(plane.a(), plane.b(), plane.c());
		auto pair = intersection.findTangientVertex(direction);
		double value = pair.second;
		if (value < valuesNew[i])
		{
			std::cerr << "Value #" << i << ": " << value << " < "
				<< valuesNew[i] << std::endl;
			++numOuterPlanes;
		}
	}
	std::cerr << "Number of outer planes: " << numOuterPlanes <<
		" from total " << planesOld.size() << std::endl;
	DEBUG_END;
}

VectorXd runL2Estimation(SupportFunctionEstimationDataPtr data)
{
	DEBUG_START;
	SupportFunctionDataPtr supportData = data->supportData();
	auto planes = supportData->supportPlanes();

	Polyhedron_3 intersection(planes);
	intersection.initialize_indices(planes);
	auto index = intersection.indexPlanes_;
	std::cerr << "Intersection contains " << intersection.size_of_facets()
		<< " of " << planes.size() << " planes." << std::endl;

	if (getenv("PCL_CONTOURS_COUNTER_DIAGNOSTICS"))
		runContoursCounterDiagnostics(data, index);
	if (getenv("PCL_INCONSISTENCY_DIAGNOSTICS"))
		runInconsistencyDiagnostics(supportData, &intersection, index);

	Polyhedron *pCopy = new Polyhedron(intersection);
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG,
			"intersection-for-matrix-building.ply") << *pCopy;

	auto problem = buildMatrix(supportData, &intersection, index);
	auto matrix = problem.first;
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "matrix.mat") << matrix;
	auto rightSide = problem.second;
	globalPCLDumper(PCL_DUMPER_LEVEL_DEBUG, "right-side.mat") << rightSide;

	Eigen::SparseLU<SparseMatrix> solver;
	solver.compute(matrix);
	if (solver.info() != Eigen::Success)
	{
		ERROR_PRINT("Decomposition failed!");
		exit(EXIT_FAILURE);
	}
	VectorXd values = solver.solve(rightSide);
	if (solver.info() != Eigen::Success)
	{
		ERROR_PRINT("Solving failed!");
		exit(EXIT_FAILURE);
	}
	if (getenv("PCL_NATIVE_ESTIMATOR_VALIDATOIN"))
		validateEstimate(planes, values);

	auto solution = calculateSolution(supportData, values);
	DEBUG_END;
	return solution;
}

void runSupportDataDiagnostics(SupportFunctionEstimationDataPtr data)
{
	DEBUG_START;
	auto solution = runL2Estimation(data);
	DEBUG_END;
}

void runBadPlanesSearch(SupportFunctionEstimationDataPtr data)
{
	DEBUG_START;
	SupportFunctionDataPtr supportData = data->supportData();
	auto points = supportData->getShiftedDualPoints_3(0.);
	int numPoints = points.size();
	int numInnerPoints = countInnerPoints(points);
	std::vector<int> saved;
	for (int i = 0; i < numPoints; ++i)
		saved.push_back(0);

	for (int i = 0; i < numPoints; ++i)
	{
		std::vector<Point_3> pointsTested = points;
		pointsTested.erase(pointsTested.begin() + i);
		int numInnerPointsTested = countInnerPoints(
				pointsTested);
		int numSavedPoints = numInnerPoints
			- numInnerPointsTested;
		saved[numSavedPoints]++;
	}

	for (int i = 0; i < numPoints; ++i)
		if (saved[i] > 0)
			std::cerr << saved[i] << " points can save "
				<< i << " points." << std::endl;
	DEBUG_END;
}

VectorXd NativeSupportFunctionEstimator::run(void)
{
	DEBUG_START;
	VectorXd solution;
	
	switch(problemType_)
	{
	case ESTIMATION_PROBLEM_NORM_L_INF:
		DEBUG_END;
		solution = runLinfEstimation(data);
		break;
	case ESTIMATION_PROBLEM_NORM_L_1:
		runSupportDataDiagnostics(data);
		if (getenv("PCL_BAD_PLANES_SEARCH"))
			runBadPlanesSearch(data);
		ERROR_PRINT("Not implemented yet!");
		exit(EXIT_FAILURE);
		break;
	case ESTIMATION_PROBLEM_NORM_L_2:
		solution = runL2Estimation(data);
		break;
	default:
		ERROR_PRINT("Unknown problem type!");
		exit(EXIT_FAILURE);
		break;
	}
	return solution;
	DEBUG_END;
}
