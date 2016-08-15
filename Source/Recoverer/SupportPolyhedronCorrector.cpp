/*
 * Copyright (c) 2009-2015 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file SupportPolyhedronCorrector.cpp
 * @brief Correction of polyhedron based on support function measurements
 * (implementation).
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "SupportPolyhedronCorrector.h"
#include "DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataConstructor.h"
#include <coin/IpTNLP.hpp>
#include <coin/IpIpoptApplication.hpp>

using namespace Ipopt;

static int counter;

/**
 * Describes the topology of the given (polyhedron, s-data) pair.
 */
struct FixedTopology
{
	/**
	 * Vector of vectors, i-th element of which contains IDs of support
	 * directions for which i-th vertex of the polyhedron is tangient.
	 */
	std::vector<std::set<int>> tangient;

	/**
	 * Vector of vectors, i-th element of which contains IDs of vertices
	 * incident to the i-th facet.
	 */
	std::vector<std::set<int>> incident;
};

#define TNLP_INFINITY 2e19

class FixedTopologyNLP : public TNLP
{
	/** Support directions. */
	const std::vector<Vector_3> &u;

	/** Support values. */
	const std::vector<double> &h;

	/** Vectors "U" from planes of polyhedron facets {(U, x) = H}. */
	const std::vector<Vector_3> &U;

	/** The same as above, but changed during the optimization. */
	std::vector<Vector_3> directions;

	/** Values "H" from planes of polyhedron facets {(U, x) = H}. */
	const std::vector<double> &H;

	/** The same as above, but changed during the optimization. */
	std::vector<double> values;

	/** Vertices of the polyhedron. */
	const std::vector<Vector_3> &pointsInitial;

	/** The same as above, but changed during the optimization. */
	std::vector<Vector_3> points;

	/** Topology of the problem. */
	const FixedTopology &FT;

	/** The number of consistensy constraints. */
	int numConsistencyConstraints;

	/** The number of convexity + planarity constraints. */
	int numConvexityConstraints;

	/** The number of normality constraints. */
	int numNormalityConstraints;
public:
	/** Simple by-value constructor. */
	FixedTopologyNLP(const std::vector<Vector_3> &u,
			const std::vector<double> &h,
			const std::vector<Vector_3> &U,
			const std::vector<double> &H,
			const std::vector<Vector_3> &pointsInitial,
			const FixedTopology &FT) :
		u(u),
		h(h),
		U(U),
		directions(U.size()),
		H(H),
		values(H.size()),
		pointsInitial(pointsInitial),
		points(pointsInitial.size()),
		FT(FT),
		numConsistencyConstraints(u.size()
				* (pointsInitial.size() - 1)),
		numConvexityConstraints(pointsInitial.size() * U.size()),
		numNormalityConstraints(U.size())
	{
		DEBUG_START;
		DEBUG_END;
	}

	bool get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
			Index& nnz_h_lag, IndexStyleEnum& index_style)
	{
		DEBUG_START;
		ASSERT(U.size() == H.size());
		n = m = nnz_jac_g = nnz_h_lag = 0;


		/* ===================================== Number of variables: */
		n = 3 * U.size() + H.size() + 3 * pointsInitial.size();
		ASSERT(m == 0 && nnz_jac_g == 0 && nnz_h_lag == 0);

		/* =================================== Number of constraints: */
		m = numConsistencyConstraints + numConvexityConstraints
			+ numNormalityConstraints;
		ASSERT(nnz_jac_g == 0 && nnz_h_lag == 0);

		/* ========= Number of non-zeros in the constraints Jacobian: */
		/*
		 * 1. Each consistency constraint contains 6 variables:
		 *    (u_j, x_i) >= (u_j, x_k),
		 *    where only u_j is fixed and known.
		 * 2. Each convexity + planarity constraint contains 7
		 *    variables:
		 *    (U_j, x_i) = (or <= ) H_j,
		 *    where all variables are unknown.
		 * 3. Each normality constraint contains 3 variables:
		 *    (U_j, U_j) = 1
		 */
		nnz_jac_g = 6 * numConsistencyConstraints
			+ 7 * numConvexityConstraints
			+ 3 * numNormalityConstraints;
		ASSERT(nnz_h_lag == 0);

		/* ==== Number of non-zeros in the Hessian of the Lagrangian: */
		/*
		 * 1. Consitency constraints are linear.
		 * 2. Convexity + planarity constraints contain 3 quadratic
		 * summands each:
		 * 3. Normality constraints contain 3 quadratic summands each:
		 */
		nnz_h_lag = 3 * numConvexityConstraints
			+ 3 * numNormalityConstraints;
		DEBUG_END;
		return true;
	}

	bool get_bounds_info(Index n, Number *x_l, Number *x_u, Index m,
			Number *g_l, Number *g_u)
	{
		DEBUG_START;
		for (int i = 0; i < n; ++i)
		{
			x_l[i] = -TNLP_INFINITY;
			x_u[i] = +TNLP_INFINITY;
		}

		int iCond = 0;
		for (; iCond < numConsistencyConstraints; ++iCond)
		{
			g_l[iCond] = 0.;
			g_u[iCond] = +TNLP_INFINITY;
		}
		for (const auto &facet : FT.incident)
		{
			int iVertexPrev = 0;
			for (const auto &iVertex : facet)
			{
				ASSERT(iVertexPrev <= iVertex);
				for (; iVertexPrev < iVertex; ++iVertexPrev)
				{
					g_l[iCond] = -TNLP_INFINITY;
					g_u[iCond] = 0.;
					++iCond;
				}
				g_l[iCond] = 0.;
				g_u[iCond] = 0.;
				++iCond;
				iVertexPrev = iVertex;
			}
		}
		for (int i = 0; i < numConsistencyConstraints; ++i)
		{
			g_l[iCond] = 0.;
			g_u[iCond] = +TNLP_INFINITY;
			++iCond;
		}
		for (int i = 0; i < numNormalityConstraints; ++i)
		{
			g_l[iCond] = 1.;
			g_u[iCond] = 1.;
			++iCond;
		}
		DEBUG_END;
		return true;
	}

	bool get_starting_point(Index n, bool init_x,
		Number *x, bool init_z, Number *z_L, Number *z_U, Index m,
		bool init_lambda, Number *lambda)
	{
		DEBUG_START;
		ASSERT(x && init_x && !init_z && !init_lambda);
		ASSERT(U.size() == H.size());
		int iVariable = 0;
		for (const auto &v : pointsInitial)
		{
			x[iVariable++] = v.x();
			x[iVariable++] = v.y();
			x[iVariable++] = v.z();
		}
		for (unsigned i = 0; i < U.size(); ++i)
		{
			x[iVariable++] = U[i].x();
			x[iVariable++] = U[i].y();
			x[iVariable++] = U[i].z();
			x[iVariable++] = H[i];
		}
		ASSERT(iVariable == n);
		DEBUG_END;
		return true;
	}

	void getVariables(const Number *x)
	{
		DEBUG_START;
		ASSERT(x);
		for (unsigned i = 0; i < pointsInitial.size(); ++i)
			points[i] = Vector_3(x[3 * i], x[3 * i + 1],
					x[3 * i + 2]);
		x = x + 3 * pointsInitial.size();
		for (unsigned i = 0; i < U.size(); ++i)
		{
			directions[i] = Vector_3(x[4 * i], x[4 * i + 1],
					x[4 * i + 2]);
			values[i] = x[4 * i + 3];
		}
		DEBUG_END;
	}

	bool eval_f(Index n, const Number *x, bool new_x, Number& obj_value)
	{
		DEBUG_START;
		ASSERT(x);
		getVariables(x);
		obj_value = 0.;
		for (unsigned i = 0; i < pointsInitial.size(); ++i)
		{
			for (int j : FT.tangient[i])
			{
				double diff = u[j] * points[i] - h[j];
				obj_value += diff * diff;
			}
		}
		DEBUG_END;
		return true;
	}


	bool eval_grad_f(Index n, const Number *x, bool new_x, Number *grad_f)
	{
		DEBUG_START;
		ASSERT(x && grad_f);
		getVariables(x);
		for (int i = 0; i < n; ++i)
			grad_f[i] = 0.;
		for (unsigned i = 0; i < pointsInitial.size(); ++i)
		{
			for (int j : FT.tangient[i])
			{
				Vector_3 direction = u[j];
				double diff = direction * points[i] - h[j];
				grad_f[3 * i + 0] += 2. * diff * direction.x();
				grad_f[3 * i + 1] += 2. * diff * direction.y();
				grad_f[3 * i + 2] += 2. * diff * direction.z();
			}
		}
		DEBUG_END;
		return true;
	}

	bool eval_g(Index n, const Number *x, bool new_x, Index m, Number *g)
	{
		DEBUG_START;
		ASSERT(x && g);
		int iCond = 0;
		for (unsigned i = 0; i < U.size(); ++i)
			for (int j : FT.incident[i])
				g[iCond++] = directions[i] * points[j]
					- values[i];
		ASSERT(iCond == numConvexityConstraints);

		for (unsigned i = 0; i < pointsInitial.size(); ++i)
			for (int j : FT.tangient[i])
				for (unsigned k = 0; k < pointsInitial.size();
						++k)
				{
					if (k == i)
						continue;
					g[iCond++] = u[j] * (points[i]
							- points[k]);
				}
		ASSERT(iCond == numConvexityConstraints
				+ numConsistencyConstraints);

		for (const Vector_3 &direction : directions)
		{
			g[iCond++] = direction * direction;
		}
		ASSERT(iCond == m);
		DEBUG_END;
		return true;
	}

	bool eval_jac_g(Index n, const Number *x, bool new_x, Index m,
			Index n_ele_jac, Index *iRow, Index *jCol,
			Number *values)
	{
		DEBUG_START;
		ASSERT((iRow && jCol) || values);
		if (x)
			getVariables(x);
		int iElem = 0;
		int iCond = 0;
		for (unsigned i = 0; i < U.size(); ++i)
		{
			for (int j : FT.incident[i])
			{
				counter = iElem;
				if (!values)
				{
					for (int k = 0; k < 3; ++k)
					{
						iRow[iElem] = iCond;
						jCol[iElem] = 3 * j + k;
						++iElem;
					}
					for (int k = 0; k < 4; ++k)
					{
						iRow[iElem] = iCond;
						jCol[iElem] = 4 * i + k;
						++iElem;
					}
				}
				else
				{
					values[iElem++] = directions[i].x();
					values[iElem++] = directions[i].y();
					values[iElem++] = directions[i].z();
					values[iElem++] = points[j].x();
					values[iElem++] = points[j].y();
					values[iElem++] = points[j].z();
					values[iElem++] = -1.;
				}
				ASSERT(iElem == counter + 7);
				++iCond;
			}
		}
		ASSERT(iCond == numConvexityConstraints);
		ASSERT(iElem == 7 * numConvexityConstraints);

		for (unsigned i = 0; i < points.size(); ++i)
			for (int j : FT.tangient[i])
				for (unsigned k = 0; k < points.size(); ++k)
				{
					if (k == i)
						continue;
					counter = iElem;
					if (!values)
					{
						for (int l = 0; l < 3; ++l)
						{
							iRow[iElem] = iCond;
							jCol[iElem] = 3 * i + l;
							++iElem;
						}
						for (int l = 0; l < 3; ++l)
						{
							iRow[iElem] = iCond;
							jCol[iElem] = 3 * k + l;
							++iElem;
						}
					}
					else
					{
						values[iElem++] = u[j].x();
						values[iElem++] = u[j].y();
						values[iElem++] = u[j].z();
						values[iElem++] = -u[j].x();
						values[iElem++] = -u[j].y();
						values[iElem++] = -u[j].z();
					}
					ASSERT(iElem == counter + 6);
					++iCond;
				}
		ASSERT(iCond == numConvexityConstraints
				+ numConsistencyConstraints);
		ASSERT(iElem == 7 * numConvexityConstraints
				+ 6 * numConsistencyConstraints);

		for (unsigned i = 0; i < directions.size(); ++i)
		{
			counter = iElem;
			if (!values)
			{
				for (int k = 0; k < 3; ++k)
				{
					iRow[iElem] = iCond;
					jCol[iElem] = 3 * i;
					++iElem;
				}
			}
			else
			{
				values[iElem++] = 2. * directions[i].x();
				values[iElem++] = 2. * directions[i].y();
				values[iElem++] = 2. * directions[i].z();
			}
			ASSERT(iElem == counter + 3);
			++iCond;
		}
		ASSERT(iCond == m);
		ASSERT(iElem == nnz_jac_g);
		DEBUG_END;
		return true;
	}


	bool eval_h(Index n, const Number *x, bool new_x, Number obj_factor,
			Index m, const Number *lambda, bool new_lambda,
			Index n_ele_hess, Index *iRow, Index *jCol,
			Number *values)
	{
		DEBUG_START;
		int iElem = 0;
		if (x)
			getVariables(x);
		for (unsigned i = 0; i < points.size(); ++i)
		{
			counter = iElem;
			for (unsigned p = 0; p < 3; ++p)
			{
				for (unsigned q = 0; q < 3; ++q)
				{
					if (!values)
					{
						iRow[iElem] = 3 * i + p;
						jCol[iElem] = 3 * i + q;
					}
					else
					{
						double sum = 0.;
						for (int k : FT.tangient[i])
							sum += u[k].cartesian(p)
							* u[k].cartesian(q);
						values[iElem] = obj_factor
							* sum;
					}
					++iElem;
				}
				ASSERT(iElem == counter + 3);
				for (unsigned j = 0; j < directions.size(); ++j)
				{
					if (!values)
					{
						iRow[iElem] = 3 * i + p;
						jCol[iElem] = 3 * points.size()
							+ 4 * j + p;
						++iElem;
					}
					else
					{
						int iCond = points.size() * j
							+ i;
						values[iElem++] = lambda[iCond];
					}
				}
				ASSERT(iElem == counter + 3
						+ directions.size());
			}
		}

		for (unsigned j = 0; j < directions.size(); ++j)
		{
			for (unsigned p = 0; p < 3; ++p)
			{
				int iRowCommon = 3 * points.size() + 4 * j + p;
				counter = iElem;
				for (unsigned i = 0; i < points.size(); ++i)
				{
					if (!values)
					{
						iRow[iElem] = iRowCommon;
						jCol[iElem] = 3 * i + p;
						++iElem;
					}
					else
					{
						int iCond = points.size() * j
							+ i;
						values[iElem++] = lambda[iCond];
					}
				}
				ASSERT(iElem == counter + points.size());

				if (!values)
				{
					iRow[iElem] = iRowCommon;
					jCol[iElem] = iRowCommon;
					++iElem;
				}
				else
				{
					int iCond = numConvexityConstraints
						+ numConsistencyConstraints + j;
					values[iElem++] = 2. * lambda[iCond];
				}
				ASSERT(iElem == counter + points.size() + 1);
			}
		}
		ASSERT(iElem == nnz_h_lag);
		DEBUG_END;
		return true;
	}

	
	void finalize_solution(SolverReturn status, Index n, const Number *x,
			const Number *z_L, const Number *z_U, Index m,
			const Number *g, const Number *lambda, Number obj_value,
			const IpoptData *ip_data,
			IpoptCalculatedQuantities *ip_cq)
	{
		DEBUG_START;
		switch (status)
		{
		case SUCCESS:
			MAIN_PRINT("SUCCESS");
			getVariables(x);
			break;
		case MAXITER_EXCEEDED:
			MAIN_PRINT("MAXITER_EXCEEDED");
			break;
		case CPUTIME_EXCEEDED:
			MAIN_PRINT("CPUTIME_EXCEEDED");
			break;
		case STOP_AT_TINY_STEP:
			MAIN_PRINT("STOP_AT_TINY_STEP");
			break;
		case STOP_AT_ACCEPTABLE_POINT:
			MAIN_PRINT("STOP_AT_ACCEPTABLE_POINT");
			break;
		case LOCAL_INFEASIBILITY:
			MAIN_PRINT("LOCAL_INFEASIBILITY");
			break;
		case USER_REQUESTED_STOP:
			MAIN_PRINT("USER_REQUESTED_STOP");
			break;
		case FEASIBLE_POINT_FOUND:
			MAIN_PRINT("FEASIBLE_POINT_FOUND");
			break;
		case DIVERGING_ITERATES:
			MAIN_PRINT("DIVERGING_ITERATES");
			break;
		case RESTORATION_FAILURE:
			MAIN_PRINT("RESTORATION_FAILURE");
			break;
		case ERROR_IN_STEP_COMPUTATION:
			MAIN_PRINT("ERROR_IN_STEP_COMPUTATION");
			break;
		case INVALID_NUMBER_DETECTED:
			MAIN_PRINT("INVALID_NUMBER_DETECTED");
			break;
		case TOO_FEW_DEGREES_OF_FREEDOM:
			MAIN_PRINT("TOO_FEW_DEGREES_OF_FREEDOM");
			break;
		case INVALID_OPTION:
			MAIN_PRINT("INVALID_OPTION");
			break;
		case OUT_OF_MEMORY:
			MAIN_PRINT("OUT_OF_MEMORY");
			break;
		case INTERNAL_ERROR:
			MAIN_PRINT("INTERNAL_ERROR");
			break;
		case UNASSIGNED:
			MAIN_PRINT("UNASSIGNED");
			break;
		}
		DEBUG_END;
	}
};

SupportPolyhedronCorrector::SupportPolyhedronCorrector(Polyhedron_3 initialP,
		SupportFunctionDataPtr SData) : initialP(initialP), SData(SData)
{
	DEBUG_START;
	DEBUG_END;
}

static FixedTopologyNLP *buildNLP(Polyhedron_3 initialP,
		SupportFunctionDataPtr SData)
{
	DEBUG_START;
	/* Prepare the NLP for solving. */
	auto u = SData->supportDirections<Vector_3>();
	VectorXd values = SData->supportValues();
	ASSERT(u.size() == values.size());
	std::vector<double> h(values.size());
	for (unsigned i = 0; i < values.size(); ++i)
		h[i] = values(i);
	std::vector<Vector_3> U;
	std::vector<double> H;
	for (auto I = initialP.facets_begin(), E = initialP.facets_end(); I != E;
			++I)
	{
		Plane_3 plane = I->plane();
		Vector_3 norm = plane.orthogonal_vector();
		double length = sqrt(norm.squared_length());
		norm = norm * (1. / length);
		double value = -plane.d() / length;
		// FIXME: Handle cases with small lengths.
		U.push_back(norm);
		H.push_back(value);
	}
	std::vector<Vector_3> points;
	for (auto I = initialP.vertices_begin(), E = initialP.vertices_end();
			I != E; ++I)
	{
		Point_3 point = I->point();
		points.push_back(Vector_3(point.x(), point.y(), point.z()));
	}

	FixedTopology FT;
	SupportFunctionDataConstructor constructor;
	constructor.run(SData->supportDirections<Point_3>(), initialP);
	auto IDs = constructor.getTangientIDs();
	for (unsigned i = 0; i < IDs.size(); ++i)
		FT.tangient[IDs[i]].insert(i);

	initialP.initialize_indices();

	// FIXME: complete also FT.incident

	FixedTopologyNLP *FTNLP = new FixedTopologyNLP(u, h, U, H, points, FT);
	DEBUG_END;
	return FTNLP;
}

Polyhedron_3 SupportPolyhedronCorrector::run()
{
	DEBUG_START;
	FixedTopologyNLP *FTNLP = buildNLP(initialP, SData);

	SmartPtr<IpoptApplication> app = IpoptApplicationFactory();

	/* Intialize the IpoptApplication and process the options */
	ApplicationReturnStatus status;
	status = app->Initialize();
	if (status != Solve_Succeeded)
	{
		MAIN_PRINT("*** Error during initialization!");
		return initialP;
	}

	app->Options()->SetStringValue("linear_solver", "ma57");

	/* Ask Ipopt to solve the problem */
	status = app->OptimizeTNLP(FTNLP);
	if (status == Solve_Succeeded)
	{
		MAIN_PRINT("*** The problem solved!");
	}
	else
	{
		MAIN_PRINT("** The problem FAILED!");
	}

	delete FTNLP;
	DEBUG_END;
	return initialP; // FIXME: Construct the proper polyhedron.
}
