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
 * @file FixedTopology.cpp
 * @brief NLP for Ipopt engine. It's used for the correction of polyhedrons
 * in the fixed topology.
 */

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Recoverer/IpoptTopologicalCorrector.h"

static unsigned counter;

#define TNLP_INFINITY 2e19

/** Simple by-value constructor. */
IpoptTopologicalCorrector::IpoptTopologicalCorrector(
	const std::vector<Vector_3> &u, const std::vector<double> &h,
	const std::vector<Vector_3> &U, const std::vector<double> &H,
	const std::vector<Vector_3> &pointsInitial, const FixedTopology *FT) :
	u(u),
	h(h),
	U(U),
	directions(U.size()),
	H(H),
	values(H.size()),
	pointsInitial(pointsInitial),
	points(pointsInitial.size()),
	FT(FT),
	modeZfixed(false)
{
	DEBUG_START;
	numConsistencyConstraints = 0;
	for (unsigned i = 0; i < pointsInitial.size(); ++i)
		numConsistencyConstraints +=
			FT->tangient[i].size() * FT->neighbors[i].size();
	std::cout << "Number of consistency constraints: "
			  << numConsistencyConstraints << std::endl;

	numPlanarityConstraints = 0;
	for (unsigned i = 0; i < U.size(); ++i)
		numPlanarityConstraints += FT->incident[i].size();
	std::cout << "Number of planarity constraints: " << numPlanarityConstraints
			  << std::endl;

	numConvexityConstraints = 0;
	for (unsigned i = 0; i < U.size(); ++i)
		numConvexityConstraints +=
			FT->influent[i].size() - FT->incident[i].size();
	std::cout << "Number of convexity constraints: " << numConvexityConstraints
			  << std::endl;

	numNormalityConstraints = U.size();
	std::cout << "Number of normality constraints: " << numNormalityConstraints
			  << std::endl;
	DEBUG_END;
}

std::vector<Vector_3> IpoptTopologicalCorrector::getDirections()
{
	DEBUG_START;
	DEBUG_END;
	return directions;
}

std::vector<double> IpoptTopologicalCorrector::getValues()
{
	DEBUG_START;
	DEBUG_END;
	return values;
}

std::vector<Vector_3> IpoptTopologicalCorrector::getPoints()
{
	DEBUG_START;
	DEBUG_END;
	return points;
}

bool IpoptTopologicalCorrector::get_nlp_info(Index &n, Index &m,
											 Index &nnz_jac_g, Index &nnz_h_lag,
											 IndexStyleEnum &index_style)
{
	DEBUG_START;
	ASSERT(U.size() == H.size());
	n = m = nnz_jac_g = nnz_h_lag = 0;

	/* ===================================== Number of variables: */
	n = 3 * U.size() + H.size() + 3 * pointsInitial.size();
	std::cout << "Number of vertices: " << pointsInitial.size() << std::endl;
	std::cout << "Number of facets: " << U.size() << std::endl;
	std::cout << "Number of variables: " << n << std::endl;
	ASSERT(m == 0 && nnz_jac_g == 0 && nnz_h_lag == 0);

	/* =================================== Number of constraints: */
	m = numConsistencyConstraints + numPlanarityConstraints +
		numConvexityConstraints + numNormalityConstraints;
	std::cout << "Number of constraints: " << m << std::endl;
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
	nnz_jac_g = 6 * numConsistencyConstraints +
				7 * (numPlanarityConstraints + numConvexityConstraints) +
				3 * numNormalityConstraints;
	std::cout << "Number of nonzeros in constraints Jacobian: " << nnz_jac_g
			  << std::endl;
	ASSERT(nnz_h_lag == 0);

	/* ==== Number of non-zeros in the Hessian of the Lagrangian: */
	/*
	 * 0. The minimized functional gives 9 non-zeros to the hessian
	 * for each point.
	 * 1. Consitency constraints are linear.
	 * 2. Convexity + planarity constraints contain 3 quadratic
	 * summands each, and they are of type "a * b" => each of them
	 * gives two non-zeros to the hessian:
	 * 3. Normality constraints contain 3 quadratic summands each
	 * (which are squares of U_i, so they each of them give only one
	 * non-zero to the hessian):
	 */
	nnz_h_lag = 9 * pointsInitial.size() +
				6 * (numPlanarityConstraints + numConvexityConstraints) +
				3 * numNormalityConstraints;
	std::cout << "Number of nonzeros in the Lagrangian Hessian: " << nnz_h_lag
			  << std::endl;
	DEBUG_END;
	return true;
}

bool IpoptTopologicalCorrector::get_bounds_info(Index n, Number *x_l,
												Number *x_u, Index m,
												Number *g_l, Number *g_u)
{
	DEBUG_START;
	for (int i = 0; i < n; ++i)
	{
		x_l[i] = -TNLP_INFINITY;
		x_u[i] = +TNLP_INFINITY;
	}

	if (modeZfixed)
	{
		int iPoint = 0;
		for (const auto &v : pointsInitial)
		{
			int iCurr = 3 * iPoint + 2;
			x_l[iCurr] = x_u[iCurr] = v.z();
			++iPoint;
		}

		for (unsigned i = 0; i < U.size(); ++i)
		{
			int iCurr = pointsInitial.size() * 3 + 4 * i + 3;
			x_l[iCurr] = 0.;
		}
	}

	unsigned iCond = 0;
	for (unsigned j = 0; j < U.size(); ++j)
	{
		for (int i : FT->influent[j])
		{
			if (FT->incident[j].count(i))
				g_l[iCond] = 0.;
			else
				g_l[iCond] = -TNLP_INFINITY;
			g_u[iCond] = 0.;
			++iCond;
		}
	}
	ASSERT(iCond == numPlanarityConstraints + numConvexityConstraints);

	for (unsigned i = 0; i < numConsistencyConstraints; ++i)
	{
		g_l[iCond] = 0.;
		g_u[iCond] = +TNLP_INFINITY;
		++iCond;
	}
	ASSERT(iCond == numPlanarityConstraints + numConvexityConstraints +
						numConsistencyConstraints);

	for (unsigned i = 0; i < numNormalityConstraints; ++i)
	{
		g_l[iCond] = 1.;
		g_u[iCond] = 1.;
		++iCond;
	}
	ASSERT(iCond == unsigned(m));
	DEBUG_END;
	return true;
}

void IpoptTopologicalCorrector::checkStartingPoint(int n, int m,
												   const double *x)
{
	DEBUG_START;
	double *g = new double[m];
	double *g_l = new double[m];
	double *g_u = new double[m];
	double *x_l = new double[n];
	double *x_u = new double[n];
	eval_g(n, x, false, m, g);
	get_bounds_info(n, x_l, x_u, m, g_l, g_u);
	const double tol = 1e-6;
	unsigned numViolations = 0;
	for (unsigned i = 0; i < unsigned(m); ++i)
		if (g[i] < g_l[i] - tol || g[i] > g_u[i] + tol)
		{
			++numViolations;
			std::cout << "g[" << i << "] = " << g[i] << " not in [" << g_l[i]
					  << ", " << g_u[i] << "] -- it is ";
			if (i < numPlanarityConstraints + numConvexityConstraints)
				std::cout << "planarity/convexity constraint";
			else if (i < numPlanarityConstraints + numConvexityConstraints +
							 numConsistencyConstraints)
				std::cout << "consistency constraint";
			else if (i < numPlanarityConstraints + numConvexityConstraints +
							 numConsistencyConstraints +
							 numNormalityConstraints)
				std::cout << "planarity constraint";
			else
				ASSERT(0 && "Go and fix checking function");
			std::cout << std::endl;
		}

	for (unsigned i = 0; i < unsigned(n); ++i)
		if (x[i] < x_l[i] - tol || x[i] > x_u[i] + tol)
		{
			++numViolations;
			std::cout << "x[" << i << "] = " << x[i] << " not in [" << x_l[i]
					  << ", " << x_u[i] << "] -- it is ";
			if (i >= pointsInitial.size())
			{
				unsigned ii = i - 3 * pointsInitial.size();
				unsigned iPlane = ii / 4;
				unsigned iCoord = ii % 4;
				switch (iCoord)
				{
				case 0:
					std::cout << "a";
					break;
				case 1:
					std::cout << "b";
					break;
				case 2:
					std::cout << "c";
					break;
				case 3:
					std::cout << "d";
					break;
				default:
					ASSERT(0 && "Impossible happened");
					break;
				}
				std::cout << "-violation in " << iPlane << "-th plane"
						  << std::endl;
			}
			else
			{
				unsigned iPoint = i / 3;
				unsigned iCoord = i % 3;
				ASSERT(iCoord == 2 && "Impossible happened");
				switch (iCoord)
				{
				case 0:
					std::cout << "x";
					break;
				case 1:
					std::cout << "y";
					break;
				case 2:
					std::cout << "z";
					break;
				default:
					ASSERT(0 && "Impossible happened");
					break;
				}
				std::cout << "-violation in " << iPoint << "-th point"
						  << std::endl;
			}
		}
	std::cout << "Violation in " << numViolations << " constraints from total "
			  << m << std::endl;
	ASSERT(numViolations == 0);
	delete[] g;
	delete[] g_l;
	delete[] g_u;
	delete[] x_l;
	delete[] x_u;
	DEBUG_END;
}

bool IpoptTopologicalCorrector::get_starting_point(Index n, bool init_x,
												   Number *x, bool init_z,
												   Number *z_L, Number *z_U,
												   Index m, bool init_lambda,
												   Number *lambda)
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
		double a = U[i].x();
		double b = U[i].y();
		double c = U[i].z();
		double d = H[i];
		double norm = sqrt(a * a + b * b + c * c);
		ASSERT(norm > 1e-10);
		a /= norm;
		b /= norm;
		c /= norm;
		d /= norm;
		if (H[i] >= 0.)
		{
			x[iVariable++] = a;
			x[iVariable++] = b;
			x[iVariable++] = c;
			x[iVariable++] = d;
		}
		else
		{
			x[iVariable++] = -a;
			x[iVariable++] = -b;
			x[iVariable++] = -c;
			x[iVariable++] = -d;
		}
	}
	ASSERT(iVariable == n);
	if (getenv("CHECK_STARTING_POINT"))
		checkStartingPoint(n, m, x);
	DEBUG_END;
	return true;
}

void IpoptTopologicalCorrector::getVariables(const Number *x)
{
	DEBUG_START;
	ASSERT(x);
	for (unsigned i = 0; i < pointsInitial.size(); ++i)
		points[i] = Vector_3(x[3 * i], x[3 * i + 1], x[3 * i + 2]);
	x = x + 3 * pointsInitial.size();
	for (unsigned i = 0; i < U.size(); ++i)
	{
		directions[i] = Vector_3(x[4 * i], x[4 * i + 1], x[4 * i + 2]);
		values[i] = x[4 * i + 3];
	}
	DEBUG_END;
}

bool IpoptTopologicalCorrector::eval_f(Index n, const Number *x, bool new_x,
									   Number &obj_value)
{
	DEBUG_START;
	ASSERT(x);
	getVariables(x);
	obj_value = 0.;
	for (unsigned i = 0; i < pointsInitial.size(); ++i)
	{
		for (int j : FT->tangient[i])
		{
			double diff = u[j] * points[i] - h[j];
			obj_value += diff * diff;
		}
	}
	DEBUG_END;
	return true;
}

bool IpoptTopologicalCorrector::eval_grad_f(Index n, const Number *x,
											bool new_x, Number *grad_f)
{
	DEBUG_START;
	ASSERT(x && grad_f);
	getVariables(x);
	for (int i = 0; i < n; ++i)
		grad_f[i] = 0.;
	for (unsigned i = 0; i < pointsInitial.size(); ++i)
	{
		for (int j : FT->tangient[i])
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

bool IpoptTopologicalCorrector::eval_g(Index n, const Number *x, bool new_x,
									   Index m, Number *g)
{
	DEBUG_START;
	ASSERT(x && g);
	getVariables(x);
	unsigned iCond = 0;
	for (unsigned j = 0; j < U.size(); ++j)
		for (int i : FT->influent[j])
			g[iCond++] = directions[j] * points[i] - values[j];
	ASSERT(iCond == numPlanarityConstraints + numConvexityConstraints);

	for (unsigned i = 0; i < pointsInitial.size(); ++i)
		for (int j : FT->tangient[i])
			for (int k : FT->neighbors[i])
			{
				g[iCond++] = u[j] * (points[i] - points[k]);
			}
	ASSERT(iCond == numPlanarityConstraints + numConvexityConstraints +
						numConsistencyConstraints);

	for (const Vector_3 &direction : directions)
	{
		g[iCond++] = direction * direction;
	}
	ASSERT(iCond == unsigned(m));
	DEBUG_END;
	return true;
}

bool IpoptTopologicalCorrector::eval_jac_g(Index n, const Number *x, bool new_x,
										   Index m, Index nnz_jac_g,
										   Index *iRow, Index *jCol,
										   Number *jacValues)
{
	DEBUG_START;
	ASSERT((iRow && jCol) || jacValues);
	if (x)
		getVariables(x);
	unsigned iElem = 0;
	unsigned iCond = 0;
	for (unsigned i = 0; i < U.size(); ++i)
	{
		for (int j : FT->influent[i])
		{
			counter = iElem;
			if (!jacValues)
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
					jCol[iElem] = 3 * pointsInitial.size() + 4 * i + k;
					++iElem;
				}
			}
			else
			{
				jacValues[iElem++] = directions[i].x();
				jacValues[iElem++] = directions[i].y();
				jacValues[iElem++] = directions[i].z();
				jacValues[iElem++] = points[j].x();
				jacValues[iElem++] = points[j].y();
				jacValues[iElem++] = points[j].z();
				jacValues[iElem++] = -1.;
			}
			ASSERT(iElem == counter + 7);
			++iCond;
		}
	}
	ASSERT(iCond == numPlanarityConstraints + numConvexityConstraints);
	ASSERT(iElem == 7 * (numPlanarityConstraints + numConvexityConstraints));

	for (unsigned i = 0; i < points.size(); ++i)
		for (int j : FT->tangient[i])
			for (int k : FT->neighbors[i])
			{
				counter = iElem;
				if (!jacValues)
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
					jacValues[iElem++] = u[j].x();
					jacValues[iElem++] = u[j].y();
					jacValues[iElem++] = u[j].z();
					jacValues[iElem++] = -u[j].x();
					jacValues[iElem++] = -u[j].y();
					jacValues[iElem++] = -u[j].z();
				}
				ASSERT(iElem == counter + 6);
				++iCond;
			}
	ASSERT(iCond == numPlanarityConstraints + numConvexityConstraints +
						numConsistencyConstraints);
	ASSERT(iElem == 7 * (numPlanarityConstraints + numConvexityConstraints) +
						6 * numConsistencyConstraints);

	for (unsigned i = 0; i < directions.size(); ++i)
	{
		counter = iElem;
		if (!jacValues)
		{
			for (int k = 0; k < 3; ++k)
			{
				iRow[iElem] = iCond;
				jCol[iElem] = 3 * pointsInitial.size() + 4 * i + k;
				++iElem;
			}
		}
		else
		{
			jacValues[iElem++] = 2. * directions[i].x();
			jacValues[iElem++] = 2. * directions[i].y();
			jacValues[iElem++] = 2. * directions[i].z();
		}
		ASSERT(iElem == counter + 3);
		++iCond;
	}
	ASSERT(iCond == unsigned(m));
	ASSERT(iElem == unsigned(nnz_jac_g));
	DEBUG_END;
	return true;
}

bool IpoptTopologicalCorrector::eval_h(Index n, const Number *x, bool new_x,
									   Number obj_factor, Index m,
									   const Number *lambda, bool new_lambda,
									   Index nnz_h_lag, Index *iRow,
									   Index *jCol, Number *hValues)
{
	DEBUG_START;
	unsigned iElem = 0;
	if (x)
		getVariables(x);
	for (unsigned i = 0; i < points.size(); ++i)
	{
		counter = iElem;
		for (unsigned p = 0; p < 3; ++p)
		{
			counter = iElem;
			for (unsigned q = 0; q < 3; ++q)
			{
				if (!hValues)
				{
					iRow[iElem] = 3 * i + p;
					jCol[iElem] = 3 * i + q;
				}
				else
				{
					double sum = 0.;
					for (int k : FT->tangient[i])
						sum += u[k].cartesian(p) * u[k].cartesian(q);
					if (p == q)
						sum *= 2.;
					hValues[iElem] = obj_factor * sum;
				}
				++iElem;
			}
			ASSERT(iElem == counter + 3);
			unsigned iCond = 0;
			for (unsigned j = 0; j < directions.size(); ++j)
			{
				for (int k : FT->influent[j])
				{
					if (unsigned(k) == i)
					{
						if (!hValues)
						{
							iRow[iElem] = 3 * i + p;
							jCol[iElem] = 3 * points.size() + 4 * j + p;
							++iElem;
						}
						else
						{
							hValues[iElem++] = 0.5 * lambda[iCond];
						}
					}
					++iCond;
				}
			}
		}
	}

	unsigned iCond = 0;
	for (unsigned j = 0; j < directions.size(); ++j)
	{
		for (unsigned p = 0; p < 3; ++p)
		{
			if (p != 0)
				iCond -= FT->influent[j].size();

			int iRowCommon = 3 * points.size() + 4 * j + p;
			counter = iElem;
			for (int i : FT->influent[j])
			{
				if (!hValues)
				{
					iRow[iElem] = iRowCommon;
					jCol[iElem] = 3 * i + p;
					++iElem;
				}
				else
				{
					hValues[iElem++] = 0.5 * lambda[iCond];
				}
				++iCond;
			}

			if (!hValues)
			{
				iRow[iElem] = iRowCommon;
				jCol[iElem] = iRowCommon;
				++iElem;
			}
			else
			{
				int iCondNorm = numPlanarityConstraints +
								numConvexityConstraints +
								numConsistencyConstraints + j;
				hValues[iElem++] = 2. * lambda[iCondNorm];
			}
		}
	}
	ASSERT(iElem == unsigned(nnz_h_lag));
	DEBUG_END;
	return true;
}

void IpoptTopologicalCorrector::finalize_solution(
	SolverReturn status, Index n, const Number *x, const Number *z_L,
	const Number *z_U, Index m, const Number *g, const Number *lambda,
	Number obj_value, const IpoptData *ip_data,
	IpoptCalculatedQuantities *ip_cq)
{
	DEBUG_START;
	getVariables(x);
	checkStartingPoint(n, m, x);

	std::cout << "========== finalize_solution ==========" << std::endl;
	for (unsigned i = 0; i < pointsInitial.size(); ++i)
	{
		std::cout << "Changing point #" << i << ": " << pointsInitial[i]
				  << " |--> " << points[i] << std::endl;
	}

	for (unsigned i = 0; i < U.size(); ++i)
	{
		Plane_3 planeInitial(U[i].x(), U[i].y(), U[i].z(), -H[i]);
		Plane_3 plane(directions[i].x(), directions[i].y(), directions[i].z(),
					  -values[i]);
		std::cout << "Changing plane #" << i << ": " << planeInitial << " |--> "
				  << plane << std::endl;
	}

	std::cout << "========== end ==========" << std::endl;

	switch (status)
	{
	case SUCCESS:
		MAIN_PRINT("SUCCESS");
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
