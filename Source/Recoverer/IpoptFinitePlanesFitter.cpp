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
 * @file IpoptFinitePlanesFitter.h
 * @brief Definition of Iptopt-based non-linear optimizer used for finite
 * planes fitting algorithm.
*/

#ifdef USE_IPOPT

#include "IpIpoptCalculatedQuantities.hpp"
#include "IpIpoptData.hpp"
#include "IpTNLPAdapter.hpp"
#include "IpOrigIpoptNLP.hpp"

#undef NDEBUG

#include "DebugPrint.h"
#include "DebugAssert.h"
#include <coin/IpIpoptApplication.hpp>
#include "Recoverer/IpoptFinitePlanesFitter.h"
#include "halfspaces_intersection.h"

IpoptFinitePlanesFitter::IpoptFinitePlanesFitter(
		SupportFunctionDataPtr data) :
	data_(data),
	problemType_(DEFAULT_ESTIMATION_PROBLEM_NORM),
	variables_(new double[4 * data->size()]),
	directions_(data->size()),
	values_(data->size()),
	planes_(data->size())
{
	DEBUG_START;
	DEBUG_END;
}

IpoptFinitePlanesFitter::~IpoptFinitePlanesFitter()
{
	DEBUG_START;
	delete[] variables_;
	DEBUG_END;
}

void IpoptFinitePlanesFitter::setProblemType(EstimationProblemNorm type)
{
	DEBUG_START;
	problemType_ = type;
	DEBUG_END;
}

bool IpoptFinitePlanesFitter::get_nlp_info(Index& n, Index& m,
		Index& nnz_jac_g, Index& nnz_h_lag, IndexStyleEnum& index_style)
{
	DEBUG_START;
	DEBUG_END;
	return true;
}

#define TNLP_INFINITY 2e19

bool IpoptFinitePlanesFitter::get_bounds_info(Index n, Number* x_l,
		Number* x_u, Index m, Number* g_l, Number* g_u)
{
	DEBUG_START;
	DEBUG_END;
	return true;
}

bool IpoptFinitePlanesFitter::get_starting_point(Index n, bool init_x,
		Number* x, bool init_z, Number* z_L, Number* z_U, Index m,
		bool init_lambda, Number* lambda)
{
	DEBUG_START;
	DEBUG_END;
	return true;
}

bool IpoptFinitePlanesFitter::eval_f(Index n, const Number* x, bool new_x,
		Number& obj_value)
{
	DEBUG_START;
	DEBUG_END;
	return true;
}

bool IpoptFinitePlanesFitter::eval_grad_f(Index n, const Number* x,
		bool new_x, Number* grad_f)
{
	DEBUG_START;
	DEBUG_END;
	return true;
}

bool IpoptFinitePlanesFitter::eval_g(Index n, const Number* x, bool new_x,
		Index m, Number* g)
{
	DEBUG_START;
	DEBUG_END;
	return true;
}

bool IpoptFinitePlanesFitter::eval_jac_g(Index n, const Number* x,
		bool new_x, Index m, Index n_ele_jac, Index* iRow, Index *jCol,
		Number* values)
{
	DEBUG_START;
	DEBUG_END;
	return true;
}

bool IpoptFinitePlanesFitter::eval_h(Index n, const Number* x, bool new_x,
		Number obj_factor, Index m, const Number* lambda,
		bool new_lambda, Index n_ele_hess, Index* iRow, Index* jCol,
		Number* values)
{
	DEBUG_START;
	DEBUG_END;
	return true;
}

void IpoptFinitePlanesFitter::finalize_solution(SolverReturn status,
		Index n, const Number* x, const Number* z_L, const Number* z_U, Index m,
		const Number* g, const Number* lambda, Number obj_value,
		const IpoptData* ip_data, IpoptCalculatedQuantities* ip_cq)
{
	DEBUG_START;
	int numVariables = 4 * data_->size();
	VectorXd result(numVariables);

	switch (status)
	{
	case SUCCESS:
		MAIN_PRINT("SUCCESS");
		for (int i = 0; i < numVariables; ++i)
		{
			result(i) = x[i];
		}
		solution_ = result;
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
	return;
}

bool IpoptFinitePlanesFitter::intermediate_callback(AlgorithmMode mode,
		   Index iter, Number obj_value,
		   Number inf_pr, Number inf_du,
		   Number mu, Number d_norm,
		   Number regularization_size,
		   Number alpha_du, Number alpha_pr,
		   Index ls_trials,
		   const IpoptData* ip_data,
		   IpoptCalculatedQuantities* ip_cq)
{
	DEBUG_START;

	Ipopt::TNLPAdapter* tnlp_adapter = NULL;
	if (!ip_cq)
	{
		DEBUG_END;
		return true;
	}

	Ipopt::OrigIpoptNLP* orignlp;
	orignlp = dynamic_cast<OrigIpoptNLP*>(GetRawPtr(ip_cq->GetIpoptNLP()));
	if (!orignlp)
	{
		ERROR_PRINT("dynamic_cast failed!");
		DEBUG_END;
		return true;
	}
	tnlp_adapter = dynamic_cast<TNLPAdapter*>(GetRawPtr(orignlp->nlp()));
	if (orignlp)
	{
		ERROR_PRINT("dynamic_cast failed!");
		DEBUG_END;
		return true;
	}
	tnlp_adapter->ResortX(*ip_data->curr()->x(), variables_);
	recalculateParameters();
	DEBUG_END;
	return true;
}

void IpoptFinitePlanesFitter::recalculateParameters(void)
{
	DEBUG_START;
	int numDirections = data_->size();
	for (int i = 0; i < numDirections; ++i)
	{
		Vector3d direction(variables_[4 * i], variables_[4 * i + 1],
				variables_[4 * i + 2]);
		double value = variables_[4 * i + 3];
		/* FIXME: should we check that value >= 0 here? */
		Plane_3 plane(direction.x, direction.y, direction.z, -value);
		directions_[i] = direction;
		values_[i] = value;
		planes_[i] = plane;
	}
	Polyhedron_3 intersection;
	CGAL::internal::halfspaces_intersection(planes_.begin(), planes_.end(),
			intersection, Kernel());
	recalculateFunctional(intersection);

#if 0
	/* FIXME: maybe we can parallelize this with openmp. */
	for (int i = 0; i < numDirections; ++i)
	{
		std::vector<Plane_3> planesWithoutOne = planes_;
		planesWithoutOne.erase(i);
		Polyhedron_3 intersectionWithoutOne;
		CGAL::internal::halfspaces_intersection(
				planesWithoutOne.begin(),
				planesWithoutOne.end(), intersectionWithoutOne,
				Kernel());
		recalculateConstraint(intersectionWithoutOne, i);
	}
	DEBUG_END;
#endif
}

static inline double distancePlanes(Plane_3 first, Plane_3 second)
{
	double deltaA = first.a() - second.a();
	double deltaB = first.b() - second.b();
	double deltaC = first.c() - second.c();
	double deltaD = first.d() - second.d();
	double distance = deltaA * deltaA + deltaB * deltaB
		+ deltaC * deltaC + deltaD * deltaD;
	return distance;
}

static void initializeIndices(Polyhedron_3 intersection,
		std::vector<Plane_3> planes)
{
	DEBUG_START;
	for (auto facet = intersection.facets_begin();
			facet != intersection.facets_end(); ++facet)
	{
		Plane_3 plane = facet->plane();
		double distanceMinimal = distancePlanes(plane, planes[0]);
		int iPlaneArgmin = 0, i = 0;
		for (auto &plane: planes)
		{
			double distance = distancePlanes(plane, planes[i]);
			if (distanceMinimal > distance)
			{
				distanceMinimal = distance;
				iPlaneArgmin = i;
			}
			++i;
		}
		facet->id = iPlaneArgmin;
		/* FIXME: add check that distanceMinimal is 0 */
	}
#ifndef NDEBUG
	for (auto facet = intersection.facets_begin();
			facet != intersection.facets_end(); ++facet)
	{
		DEBUG_PRINT("facet id: %ld", facet->id);
	}
#endif /* NDEBUG */
	DEBUG_END;
}

void IpoptFinitePlanesFitter::recalculateFunctional(Polyhedron_3 intersection)
{
	DEBUG_START;
	initializeIndices(intersection, planes_);
	
	DEBUG_END;
}

SupportFunctionDataPtr IpoptFinitePlanesFitter::run(void)
{
	DEBUG_START;
	SmartPtr<IpoptApplication> app = IpoptApplicationFactory();

	/* Intialize the IpoptApplication and process the options */
	ApplicationReturnStatus status;
	status = app->Initialize();
	if (status != Solve_Succeeded)
	{
		MAIN_PRINT("*** Error during initialization!");
		return data_;
	}

	//app->Options()->SetNumericValue("tol", 1e-3);
	//app->Options()->SetNumericValue("acceptable_tol", 1e-3);
	//app->Options()->SetIntegerValue("max_iter", 3000000);
	app->Options()->SetStringValue("linear_solver", "ma57");

	/* Ask Ipopt to solve the problem */
	status = app->OptimizeTNLP(this);
	if (status == Solve_Succeeded)
	{
		MAIN_PRINT("*** The problem solved!");
	}
	else
	{
		MAIN_PRINT("** The problem FAILED!");
	}

	DEBUG_END;
	return data_;
}

#endif /* USE_IPOPT */
