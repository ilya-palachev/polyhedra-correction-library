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
 * @file IpoptSupportFunctionEstimator.h
 * @brief Declaration of Iptopt-based non-linear optimizer in case of support
 * function estimation.
 */

#ifdef USE_IPOPT

#include <coin/IpTNLP.hpp>
#include "Recoverer/SupportFunctionEstimator.h"

using namespace Ipopt;

#ifndef IPOPTSUPPORTFUNCTIONESTIMATOR_H_
#define IPOPTSUPPORTFUNCTIONESTIMATOR_H_

/**
 * Iptopt-based non-linear optimizer in case of support
 * function estimation.
 */
class IpoptSupportFunctionEstimator:
	public TNLP,
	public SupportFunctionEstimator
{
private:
	/** Type of problem. */
	EstimationProblemNorm problemType_;

	/** Number of regular variables: */
	int numVariablesX;

	/** Number of epsilon variables: */
	int numVariablesEpsilon;

	/** Number of consistency conditions: */
	int numConsistencyConditions;

	/** Number of locality conditions: */
	int numLocalityConditions;

	/** The solution returned by Ipopt. */
	VectorXd solution;
public:
	/**
	 * Default constructor
	 *
	 * @param data	Support function estimation data (input)
	 */
	IpoptSupportFunctionEstimator(SupportFunctionEstimationDataPtr data);

	/**
	 * Sets the type of problem.
	 *
	 * @param type	The problem type.
	 */
	void setProblemType(EstimationProblemNorm type);

	/** Default destructor. */
	~IpoptSupportFunctionEstimator();

	/**
	 * Gives Ipopt the information about the size of the problem (and hence,
	 * the size of arrays it needs to allocate).
	 *
	 * @param n		(out) the number of variables in the problem
	 * 			(dimension of x).
	 * @param m		(out), the number of constraints in the problem
	 * 			(dimension of g(x)).
	 * @param nnz_jac_g	(out), the number of nonzero entries in the
	 * 			Jacobian.
	 * @param nnz_h_lag	(out), the number of nonzero entries in the
	 * 			Hessian.
	 * @param index style:	(out), the numbering style used for row/col
	 * 			entries in the sparse matrix format.
	 *
	 * @return Always true.
	 */
	virtual bool get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
			Index& nnz_h_lag, IndexStyleEnum& index_style);

	/**
	 * Gives Ipopt the value of the bounds on the variables and constraints.
	 *
	 * @param n		(in), the number of variables in the problem
	 * 			(dimension of x).
	 * @param x_l		(out) the lower bounds x^{L} for x.
	 * @param x_u		(out) the upper bounds x^{U} for x.
	 * @param m		(in), the number of constraints in the problem
	 * 			(dimension of g(x)).
	 * @param g_l		(out) the lower bounds g^{L} for g(x).
	 * @param g_u		(out) the upper bounds g^{U} for g(x).
	 *
	 * @return		Always true.
	 */
	virtual bool get_bounds_info(Index n, Number* x_l, Number* x_u, Index m,
			Number* g_l, Number* g_u);

	/**
	 * Gives Ipopt the starting point before it begins iterating.
	 *
	 * @param n		(in), the number of variables in the problem
	 * 			(dimension of x).
	 * @param init_x	(in), if true, this method must provide an
	 * 			initial	value for x.
	 * @param x		(out), the initial values for the primal
	 * 			variables x.
	 * @param init_z	(in), if true, this method must provide an
	 * 			initial value for the bound multipliers z_L and
	 * 			z_U.
	 * @param z_L		(out), the initial values for the bound
	 * 			multipliers, z_L.
	 * @param z_U		(out), the initial values for the bound
	 * 			multipliers, z_U.
	 * @param m		(in), the number of constraints in the problem
	 * 			(dimension of g(x)).
	 * @param init_lambda	(in), if true, this method must provide an
	 * 			initial value for the constraint multipliers, λ.
	 * @param lambda	(out), the initial values for the constraint.
	 * 			multipliers, λ.
	 *
	 * @return		Always true.
	 */
	virtual bool get_starting_point(Index n, bool init_x, Number* x,
			bool init_z, Number* z_L, Number* z_U, Index m,
			bool init_lambda, Number* lambda);

	/**
	 * Returns the value of the objective function at the point x.
	 *
	 * @param n		(in), the number of variables in the problem
	 * 			(dimension of x).
	 * @param x		(in), the values for the primal variables, x, at
	 * 			which f (x) is to be evaluated.
	 * @param new_x		(in), false if any evaluation method was
	 * 			previously called with the same values in x,
	 * 			true otherwise.
	 * @param obj_value	(out) the value of the objective function (f(x))
	 *
	 * @return		Always true.
	 */
	virtual bool eval_f(Index n, const Number* x, bool new_x,
			Number& obj_value);

	/**
	 * Returns the gradient of the objective function at the point x.
	 *
	 * @param n		(in), the number of variables in the problem
	 * 			(dimension of x).
	 * @param x		(in), the values for the primal variables, x, at
	 * 			which \nabla f (x) is to be evaluated.
	 * @param new_x		(in), false if any evaluation method was
	 * 			previously called with the same values in x,
	 * 			true otherwise.
	 * @param grad_f	(out) the array of values for the gradient of
	 * 			the objective function (\nabla f (x)).
	 *
	 * @return		Always true.
	 */
	virtual bool eval_grad_f(Index n, const Number* x, bool new_x,
			Number* grad_f);

	/**
	 * Returns the value of the constraint function at the point x.
	 *
	 * @param n		(in), the number of variables in the problem
	 * 			(dimension of x).
	 * @param x		(in), the values for the primal variables, x, at
	 * 			which the constraint functions, g(x), are to be
	 * 			evaluated.
	 * @param new_x		(in), false if any evaluation method was
	 * 			previously called with the same values in x,
	 * 			true otherwise.
	 * @param m		(in), the number of constraints in the problem
	 * 			(dimension of g(x)).
	 * @param g		(out) the array of constraint function values,
	 * 			g(x).
	 *
	 * @return		Always true.
	 */
	virtual bool eval_g(Index n, const Number* x, bool new_x, Index m,
			Number* g);

	/**
	 * Returns either the sparsity structure of the Jacobian of the
	 * constraints, or the values for the Jacobian of the constraints at
	 * the point x.
	 *
	 * @param n		(in), the number of variables in the problem
	 * 			(dimension of x).
	 * @param x		(in), the values for the primal variables, x, at
	 * 			which the constraint Jacobian,. \nabla g(x)^{T},
	 * 			is to be evaluated.
	 * @param new_x		(in), false if any evaluation method was
	 * 			previously called with the same values in x,
	 * 			true otherwise.
	 * @param m:		(in), the number of constraints in the problem
	 * 			(dimension of g(x)).
	 * @param n_ele_jac	(in), the number of nonzero elements in the
	 * 			Jacobian (dimension of iRow, jCol, and values).
	 * @param iRow		(out), the row indices of entries in the
	 * 			Jacobian of the constraints.
	 * @param jCol		(out), the column indices of entries in the
	 * 			Jacobian of the constraints.
	 * @param values	(out), the values of the entries in the Jacobian
	 * 			of the constraints.
	 *
	 * @return 		Always true.
	 */
	virtual bool eval_jac_g(Index n, const Number* x, bool new_x, Index m,
			Index n_ele_jac, Index* iRow, Index *jCol,
			Number* values);

	/**
	 * Returns either the sparsity structure of the Hessian of the
	 * Lagrangian, or the values of the Hessian of the Lagrangian (9) for
	 * the given values for x, σ_{f} , and λ.
	 *
	 * @param n		(in), the number of variables in the problem
	 * 			(dimension of x).
	 * @param x		(in), the values for the primal variables, x, at
	 * 			which the Hessian is to be evaluated.
	 * @param new_x		(in), false if any evaluation method was
	 * 			previously called with the same values in x,
	 * 			true otherwise.
	 * @param obj_factor	(in), factor in front of the objective term in
	 * 			the Hessian, σ_f .
	 * @param m		(in), the number of constraints in the problem
	 * 			(dimension of g(x)).
	 * @param lambda	(in), the values for the constraint multipliers,
	 * 			λ, at which the Hessian is to be evaluated.
	 * @param new_lambda	(in), false if any evaluation method was
	 * 			previously called with the same values in
	 * 			lambda, true otherwise.
	 * @param n_ele_hess	(in), the number of nonzero elements in the
	 * 			Hessian (dimension of iRow, jCol, and values).
	 * @param iRow		(out), the row indices of entries in the
	 * 			Hessian.
	 * @param jCol		(out), the column indices of entries in the
	 * 			Hessian.
	 * @param values	(out), the values of the entries in the Hessian.
	 *
	 * @return 		Always true.
	 */
	virtual bool eval_h(Index n, const Number* x, bool new_x,
			Number obj_factor, Index m, const Number* lambda,
			bool new_lambda, Index n_ele_hess, Index* iRow,
			Index* jCol, Number* values);

	/**
	 * This method is called by Ipopt after the algorithm has finished
	 * (successfully or even with most errors).
	 *
	 * @param status	(in), gives the status of the algorithm as
	 * 			specified in IpAlgTypes.hpp
	 * @param n		(in), the number of variables in the problem
	 * 			(dimension of x).
	 * @param x		(in), the final values for the primal variables
	 * @param z_L		(in), the final values for the lower bound
	 * 			multipliers
	 * @param z_U		(in), the final values for the upper bound
	 * 			multipliers
	 * @param m		(in), the number of constraints in the problem
	 * 			(dimension of g(x)).
	 * @param g		(in), the final value of the constraint function
	 * 			values, g(x∗ ).
	 * @param lambda	(in), the final values of the constraint
	 * 			multipliers, λ∗ .
	 * @param obj_value	(in), the final value of the objective, f (x∗ ).
	 * @param ip_data and ip_cq are provided for expert users.
	 *
	 * @return		Always true.
	 */
	virtual void finalize_solution(SolverReturn status, Index n,
			const Number* x, const Number* z_L, const Number* z_U,
			Index m, const Number* g, const Number* lambda,
			Number obj_value, const IpoptData* ip_data,
			IpoptCalculatedQuantities* ip_cq);

	/**
	 * Runs the Ipopt algorithm.
	 *
	 * @return		The solution vector.
	 */
	virtual VectorXd run(void);
};

#endif /* IPOPTSUPPORTFUNCTIONESTIMATOR_H_ */
#endif /* USE_IPOPT */

