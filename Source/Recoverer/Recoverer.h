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
 * @file Recoverer.h
 * @brief Declaration of the main recovering engine, based on support function
 * estimation tuned for our specific case.
 */

#ifndef RECOVERER_H
#define RECOVERER_H

#include <vector>

#include <CGAL/basic.h>
#include <CGAL/Filtered_kernel.h>

#include "PCLKernel/PCLKernel.h"
#include "PCLKernel/PCLPoint_2_iostream.h"

#include "Polyhedron/Polyhedron.h"
#include "DataContainers/ShadeContourData/ShadeContourData.h"
#include "Recoverer/SupportFunctionEstimationData.h"


/**
 * Sets the estimator to be used for support function estimation.
 */
typedef enum
{
#ifdef USE_TSNNLS
	/** TSNNLS estimator (not working). */
	TSNNLS_ESTIMATOR,
#endif /* USE_TSNNLS */

#ifdef USE_IPOPT
	/** Ipopt estimator. */
	IPOPT_ESTIMATOR,

	/** Linear Ipopt estimator. */
	IPOPT_ESTIMATOR_LINEAR,
#endif /* USE_IPOPT */

	/** Quadratic primal CGAL estimator.  */
	CGAL_ESTIMATOR,

	/** Linear CGAL estimator. */
	CGAL_ESTIMATOR_LINEAR
} RecovererEstimator;

#define DEFAULT_MAX_COORDINATE 1000000.

struct PCL_Point_3 : public Point_3
{
	long int id;
	
	PCL_Point_3(double x, double y, double z, long int i)
		: Point_3(x, y, z), id(i)
	{};
};

auto PCL_Point_3_comparer = [](PCL_Point_3 a, PCL_Point_3 b)
{
	return a < b || (a <= b && a.id < b.id);
};

typedef std::set<PCL_Point_3, decltype(PCL_Point_3_comparer)> CGALpointsSet;

/**
 * Main recovering engine, based on support function estimation tuned for our
 * specific needs.
 */
class Recoverer
{
private:

	/** Output file name. */
	char *outputName;

	/** Estimator to be used. */
	RecovererEstimator estimator;

	/** Whether to balance the vertical center of contours. */
	bool ifBalancing;

	/** Whether to convexify shadow contours. */
	bool ifConvexifyContours;

	/** Whether to regularize the support matrix. */
	bool ifRegularize;

	/** Whether to scale the matrix of problem. */
	bool ifScaleMatrix;

	/** ID of vertex with maximal X coordinate. */
	int iXmax;

	/** ID of vertex with maximal Y coordinate. */
	int iYmax;

	/** ID of vertex with maximal Z coordinate. */
	int iZmax;

	/** Regularizing vector. */
	Vector_3 vectorRegularizing;

	/** Vector with saved 3 h-values that correspond to iXmax, iYmax, iZmax */
	Vector_3 vectorMaxHValues;

	/** Map from initial support directions's ID's and points on hull. */
	long int *mapID;

	/** The size of map. */
	long int mapIDsize;

	/** Support values in initial (before hull construction) order. */
	double *hvaluesInit;

	/** Inverse map of the previous map (not a function). */
	std::list<long int> *mapIDinverse;

	/** Shadow contour data (initial data). */
	ShadeContourDataPtr shadowDataInit;

	/** Shadow contour data (preprocessed). */
	ShadeContourDataPtr shadowDataPrep;

	/**
	 * Initializes fields shadowDataInit and shadowDataPrep
	 *
	 * @param SCData	Shadow contour data
	 */
	void initData(ShadeContourDataPtr SCData);

	/**
	 * Extracts support planes from shadow contours.
	 *
	 * @param SCData	Shadow contour data
	 */
	vector<Plane> extractSupportPlanes(ShadeContourDataPtr SCData);

	/**
	 * Extracts support planes from one shadow contour.
	 *
	 * @param contour	Shadow contour
	 */
	vector<Plane> extractSupportPlanes(SContour* contour);

	/**
	 * Performs the transformation of polar duality for the given set of planes,
	 * i. e. maps each plane as follows:
	 *
	 * (ax + by + cz + d = 0) |--> (-a/d, -b/d, -c/d)
	 *
	 * @param planes	The vector of planes
	 */
	vector<Vector3d> mapPlanesToDualSpace(vector<Plane> planes);

	/**
	 * Constructs ID maps (used in constructHullAndIDmaps)
	 *
	 * @param pointsHull	Obtained hull points
	 * @param pointsCGAL	Initial CGAL points
	 */
	void constructIDmaps(std::set<PCL_Point_3> pointsHull,
        	CGALpointsSet pointsCGAL);

	/**
	 * Constructs convex hull of the point set using CGAL API and calls
	 * constructIDmaps to construct ID maps.
	 *
	 * @param points	Vector of points
	 */
	Polyhedron_3 constructHullAndIDmaps(vector<Vector3d> pointsPCL);

	/**
	 * Constructs convex hull of the point set using CGAL API
	 *
	 * @param points	Vector of points
	 */
	Polyhedron_3 constructConvexHullCGAL(vector<Vector3d> pointsPCL);

	/**
	 * Constructs convex hull of the point set using CGAL API.
	 *
	 * @param points	Vector of points
	 */
	PolyhedronPtr constructConvexHull(vector<Vector3d> pointsPCL);

	/**
	 * Builds dual polyhedron to the given polyhedron.
	 *
	 * @param p	Initial polyhedron
	 */
	PolyhedronPtr buildDualPolyhedron(PolyhedronPtr p);

	/**
	 * Shifts all points to the given vector displacement.
	 *
	 * @param SCData	Shadow contour data
	 * @param shift		Vector displacement
	 */
	void shiftAllContours(ShadeContourDataPtr SCData, Vector3d shift);

	/**
	 * Balances contours so that their common mass center lies right at the
	 * center of coordinate system. We assume that contours are displaced from
	 * this position only on a vertical vector and report error otherwise.
	 */
	void balanceAllContours(ShadeContourDataPtr SCData);

	/**
	 * Builds polyhedron consisting of facets constructed from shadow contours,
	 * or from their dual images.
	 *
	 * @param ifDual	Whether to build dual contours
	 * @param SCData	Shadow contour data
	 */
	PolyhedronPtr buildMaybeDualContours(bool ifDual,
			ShadeContourDataPtr SCData);

	/**
	 * Finds IDs of vertices that have maximal coordinates of polyhedron's
	 * vertices.
	 *
	 * Columns of transposed support matrix which have these IDs will be then
	 * eliminated.
	 *
	 * @param polyhedron	The polyhedron
	 */
	Vector_3 findMaxVertices(Polyhedron_3 polyhedron,
		int& imax0, int& imax1, int& imax2);
	
	/**
	 * Regularizes support matrix by eliminating columns that have IDs equal to
	 * iXmax, iYmax, iZmax and regularizes also the h-values vector.
	 * 
	 * @param matrix		The transpose of support matrix
	 * @param polyhedron	The polyhedron (convex hull of supporting
	 * directions)
	 */
	void regularizeSupportMatrix(SupportFunctionEstimationData* data,
			Polyhedron_3 polyhedron);

public:

	/**
	 * Empty constructor.
	 */
	Recoverer();

	/**
	 * Empty destructor.
	 */
	~Recoverer();

	/**
	 * Sets output file name.
	 *
	 * @param name	The name of output file
	 */
	void setOutputName(char *name);

	/**
	 * Enables balancing in all functions.
	 */
	void enableBalancing(void);

	/**
	 * Enables the convexification of shadow contours.
	 */
	void enableContoursConvexification(void);

	/**
	 * Disables the convexification of shadow contours.
	 */
	void disableContoursConvexification(void);

	/**
	 * Enables the regularization of support matrix based on knowledge about
	 * eigenvectors eigenvalues of support matrix Q
	 */
	void enableRegularization(void);

	/**
	 * Enables the scaling of the matrix of the problem.
	 */
	void enableMatrixScaling(void);

	/**
	 * Prints the problems to files (for debugging purposes).
	 *
	 * @param SCData	Shadow contours data.
	 */
	void buildNaiveMatrix(ShadeContourDataPtr SCData);

	/**
	 * Builds naive polyhedron using naive approach that intersect half-spaces
	 * corresponding to support planes.
	 *
	 * @param SCData	Shadow contour data
	 */
	PolyhedronPtr buildNaivePolyhedron(ShadeContourDataPtr SCData);

	/**
	 * Builds non-convex polyhedron in dual space that contains all dual images
	 * of support planes.
	 * It will be used for visualization to analyze the level of noise in
	 * support planes measurements.
	 *
	 * @param SCData	Shadow contour data
	 */
	PolyhedronPtr buildDualNonConvexPolyhedron(ShadeContourDataPtr SCData);

	/**
	 * Builds polyhedron consisting of facets constructed from shadow contours
	 * from original data. Is used for debugging purposes.
	 *
	 * @param SCData	Shadow contour data
	 */
	PolyhedronPtr buildContours(ShadeContourDataPtr SCData);

	/**
	 * Build polyhedron consisting of facets constructed from dual images of
	 * shadow contours (more precisely, from contours that are the images of
	 * dual cylinders).
	 *
	 * @param SCData	Shadow contour data
	 */
	PolyhedronPtr buildDualContours(ShadeContourDataPtr SCData);

	/**
	 * Builds naive matrix for support function estimation problem.
	 *
	 * @param SCData	Shadow contour data
	 */
	SupportFunctionEstimationData* buildSupportMatrix(
			ShadeContourDataPtr SCData);

	/**
	 * Sets the estimator that will be used for support function estimation.
	 *
	 * @param estimator	The type of estimator
	 */
	void setEstimator(RecovererEstimator e);

	/**
	 * Produces corrected shadow contour data from initial data and vector
	 * of estimated support values
	 *
	 * @param estData	Support function estimation data used by
	 * 			estimator
	 * @param estimate	The result of SFE's work
	 */
	ShadeContourDataPtr produceCorrectedData(
		SupportFunctionEstimationData *estData, VectorXd estimate);

	/**
	 * Runs the recovering procedure.
	 *
	 * @param SCData	Shadow contour data
	 */
	ShadeContourDataPtr run(ShadeContourDataPtr SCData);
};

typedef shared_ptr<Recoverer> RecovererPtr;

char *makeNameWithSuffix(char *outputName, const char *suffix);

#endif /* RECOVERER_H */
