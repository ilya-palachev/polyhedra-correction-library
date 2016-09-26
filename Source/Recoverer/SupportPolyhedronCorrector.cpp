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

static unsigned counter;

/**
 * Describes the topology of the given (polyhedron, s-data) pair.
 */
struct FixedTopology
{
	/**
	 * Vector of sets, i-th element of which contains IDs of support
	 * directions for which i-th vertex of the polyhedron is tangient.
	 */
	std::vector<std::set<int>> tangient;

	/**
	 * Vector of sets, i-th element of which contains IDs of vertices
	 * incident to the i-th facet.
	 */
	std::vector<std::set<int>> incident;

	/**
	 * Vector of sets, i-th element of which contains IDs of vertices
	 * incident to the neighbor facets of the i-th facet, but not incident
	 * to the i-th facet.
	 */
	std::vector<std::set<int>> semiIncident;

	/**
	 * Vector of sets, i-th element of which contains IDs of vertices
	 * incident to some edge that is incident to the i-th vertex.
	 */
	std::vector<std::set<int>> neighbors;

	FixedTopology(Polyhedron_3 initialP, SupportFunctionDataPtr SData) :
		tangient(initialP.size_of_vertices()),
		incident(initialP.size_of_facets()),
		semiIncident(initialP.size_of_facets()),
		neighbors(initialP.size_of_vertices())
	{
		DEBUG_START;
		SupportFunctionDataConstructor constructor;
		constructor.run(SData->supportDirections<Point_3>(), initialP);
		auto IDs = constructor.getTangientIDs();
		for (unsigned i = 0; i < IDs.size(); ++i)
		{
			tangient[IDs[i]].insert(i);
		}

		initialP.initialize_indices();

		unsigned iVertex = 0;
		for (auto I = initialP.vertices_begin(),
				E = initialP.vertices_end(); I != E; ++I)
		{
			std::cout << "Constructing vertrex #" << iVertex
				<< ": ";
			auto C = I->vertex_begin();
			do
			{
				int iNeighbor = C->opposite()->vertex()->id;
				std::cout << iNeighbor << " ";
				neighbors[iVertex].insert(iNeighbor);
			} while (++C != I->vertex_begin());
			++iVertex;
			std::cout << std::endl;
		}
		
		unsigned iFacet = 0;
		for (auto I = initialP.facets_begin(),
				E = initialP.facets_end(); I != E; ++I)
		{
			std::cout << "Constructing facet #" << iFacet << ": ";
			auto C = I->facet_begin();
			do
			{
				int iVertex = C->vertex()->id;
				std::cout << iVertex << " ";
				incident[iFacet].insert(iVertex);
			} while (++C != I->facet_begin());
			std::cout << " semi ";

			ASSERT(C == I->facet_begin());
			do
			{
				int iVertex = C->vertex()->id;
				for (int i : neighbors[iVertex])
					if (!incident[iFacet].count(i))
					{
						std::cout << i << " ";
						semiIncident[iFacet].insert(i);
					}
			} while (++C != I->facet_begin());

			++iFacet;
			std::cout << std::endl;
		}
		DEBUG_END;
	}
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
	const FixedTopology *FT;

	/** The number of consistensy constraints. */
	unsigned numConsistencyConstraints;

	/** The number of planarity constraints. */
	unsigned numPlanarityConstraints;

	/** The number of convexity constraints. */
	unsigned numConvexityConstraints;

	/** The number of normality constraints. */
	unsigned numNormalityConstraints;
public:
	/** Simple by-value constructor. */
	FixedTopologyNLP(const std::vector<Vector_3> &u,
			const std::vector<double> &h,
			const std::vector<Vector_3> &U,
			const std::vector<double> &H,
			const std::vector<Vector_3> &pointsInitial,
			const FixedTopology *FT) :
		u(u),
		h(h),
		U(U),
		directions(U.size()),
		H(H),
		values(H.size()),
		pointsInitial(pointsInitial),
		points(pointsInitial.size()),
		FT(FT)
	{
		DEBUG_START;
		numConsistencyConstraints = 0;
		for (unsigned i = 0; i < pointsInitial.size(); ++i)
			numConsistencyConstraints += FT->tangient[i].size()
				* FT->neighbors[i].size();
		std::cout << "Number of consistency constraints: "
			<< numConsistencyConstraints << std::endl;

		numPlanarityConstraints = 0;
		for (unsigned i = 0; i < U.size(); ++i)
			numPlanarityConstraints += FT->incident[i].size();
		std::cout << "Number of planarity constraints: "
			<< numPlanarityConstraints << std::endl;

		numConvexityConstraints = 0;
		for (unsigned i = 0; i < U.size(); ++i)
			numConvexityConstraints += FT->semiIncident[i].size();
		std::cout << "Number of convexity constraints: "
			<< numConvexityConstraints << std::endl;

		numNormalityConstraints = U.size();
		std::cout << "Number of normality constraints: "
			<< numNormalityConstraints << std::endl;
		DEBUG_END;
	}

	std::vector<Vector_3> getDirections()
	{
		DEBUG_START;
		DEBUG_END;
		return directions;
	}

	std::vector<double> getValues()
	{
		DEBUG_START;
		DEBUG_END;
		return values;
	}

	std::vector<Vector_3> getPoints()
	{
		DEBUG_START;
		DEBUG_END;
		return points;
	}

	bool get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
			Index& nnz_h_lag, IndexStyleEnum& index_style)
	{
		DEBUG_START;
		ASSERT(U.size() == H.size());
		n = m = nnz_jac_g = nnz_h_lag = 0;


		/* ===================================== Number of variables: */
		n = 3 * U.size() + H.size() + 3 * pointsInitial.size();
		std::cout << "Number of vertices: " << pointsInitial.size()
			<< std::endl;
		std::cout << "Number of facets: " << U.size() << std::endl;
		std::cout << "Number of variables: " << n << std::endl;
		ASSERT(m == 0 && nnz_jac_g == 0 && nnz_h_lag == 0);

		/* =================================== Number of constraints: */
		m = numConsistencyConstraints + numPlanarityConstraints
			+ numConvexityConstraints + numNormalityConstraints;
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
		nnz_jac_g = 6 * numConsistencyConstraints
			+ 7 * (numPlanarityConstraints
					+ numConvexityConstraints)
			+ 3 * numNormalityConstraints;
		std::cout << "Number of nonzeros in constraints Jacobian: "
			<< nnz_jac_g << std::endl;
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
		nnz_h_lag = 9 * pointsInitial.size()
			+ 6 * (numPlanarityConstraints
					+ numConvexityConstraints)
			+ 3 * numNormalityConstraints;
		std::cout << "Number of nonzeros in the Lagrangian Hessian: "
			<< nnz_h_lag << std::endl;
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

		unsigned iCond = 0;
		for (; iCond < numPlanarityConstraints; ++iCond)
		{
			g_l[iCond] = 0.;
			g_u[iCond] = 0.;
		}
		ASSERT(iCond == numPlanarityConstraints);

		for (; iCond < numPlanarityConstraints
				+ numConvexityConstraints; ++iCond)
		{

			g_l[iCond] = -TNLP_INFINITY;
			g_u[iCond] = 0.;
		}
		std::cout << "iCond: " << iCond << std::endl;
		std::cout << "sum: " << numPlanarityConstraints + numConvexityConstraints << std::endl;
		ASSERT(iCond == numPlanarityConstraints
				+ numConvexityConstraints);

		for (unsigned i = 0; i < numConsistencyConstraints; ++i)
		{
			g_l[iCond] = 0.;
			g_u[iCond] = +TNLP_INFINITY;
			++iCond;
		}
		ASSERT(iCond == numPlanarityConstraints
				+ numConvexityConstraints
				+ numConsistencyConstraints);

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
		double *g = new double[m];
		double *g_l = new double[m];
		double *g_u = new double[m];
		double *x_l = new double[n];
		double *x_u = new double[n];
		eval_g(n, x, false, m, g);
		get_bounds_info(n, x_l, x_u, m, g_l, g_u);
		const double tol = 1e-10;
		unsigned numViolations = 0;
		for (unsigned i = 0; i < unsigned(m); ++i)
			if (g[i] < g_l[i] - tol || g[i] > g_u[i] + tol)
			{
				++numViolations;
				std::cout << "g[" << i << "] = " << g[i]
					<< " not in [" << g_l[i] << ", "
					<< g_u[i] << "]" << std::endl;
				if (i < numConvexityConstraints)
				{
					int iFacet = i / pointsInitial.size();
					int iVertex = i % pointsInitial.size();
					std::cout
						<< "  It's condition for facet "
						<< iFacet << " and vertex "
						<< iVertex << std::endl;
				} else if (i < numConvexityConstraints
						+ numConvexityConstraints)
				{
					std::cout
						<< "  It's consistency constr."
						<< std::endl;;
				} else
				{
					int iFacet = i - numConvexityConstraints
						- numConsistencyConstraints;
					std::cout <<
						"  It's normality for facet "
						<< iFacet << std::endl;
				}
			}
		std::cout << "Violation in " << numViolations
			<< " constraints from total " << m << std::endl;
		ASSERT(numViolations == 0);
		delete[] g;
		delete[] g_l;
		delete[] g_u;
		delete[] x_l;
		delete[] x_u;
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
			for (int j : FT->tangient[i])
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

	bool eval_g(Index n, const Number *x, bool new_x, Index m, Number *g)
	{
		DEBUG_START;
		ASSERT(x && g);
		getVariables(x);
		unsigned iCond = 0;
		for (unsigned i = 0; i < U.size(); ++i)
			for (int j : FT->incident[i])
				g[iCond++] = directions[i] * points[j]
					- values[i];
		ASSERT(iCond == numPlanarityConstraints);

		for (unsigned i = 0; i < U.size(); ++i)
			for (int j : FT->semiIncident[i])
				g[iCond++] = directions[i] * points[j]
					- values[i];
		ASSERT(iCond == numPlanarityConstraints
				+ numConvexityConstraints);

		for (unsigned i = 0; i < pointsInitial.size(); ++i)
			for (int j : FT->tangient[i])
				for (int k : FT->neighbors[i])
				{
					g[iCond++] = u[j] * (points[i]
							- points[k]);
				}
		ASSERT(iCond == numPlanarityConstraints
				+ numConvexityConstraints
				+ numConsistencyConstraints);

		for (const Vector_3 &direction : directions)
		{
			g[iCond++] = direction * direction;
		}
		ASSERT(iCond == unsigned(m));
		DEBUG_END;
		return true;
	}

	bool eval_jac_g(Index n, const Number *x, bool new_x, Index m,
			Index nnz_jac_g, Index *iRow, Index *jCol,
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
			for (int j : FT->incident[i])
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
						jCol[iElem] =
							3 * pointsInitial.size()
							+ 4 * i + k;
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
		ASSERT(iCond == numPlanarityConstraints);
		ASSERT(iElem == 7 * numPlanarityConstraints);

		for (unsigned i = 0; i < U.size(); ++i)
		{
			for (int j : FT->semiIncident[i])
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
						jCol[iElem] =
							3 * pointsInitial.size()
							+ 4 * i + k;
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
		ASSERT(iCond == numPlanarityConstraints
				+ numConvexityConstraints);
		ASSERT(iElem == 7 * (numPlanarityConstraints
					+ numConvexityConstraints));

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
		ASSERT(iCond == numPlanarityConstraints
				+ numConvexityConstraints
				+ numConsistencyConstraints);
		ASSERT(iElem == 7 * (numPlanarityConstraints
					+ numConvexityConstraints)
				+ 6 * numConsistencyConstraints);

		for (unsigned i = 0; i < directions.size(); ++i)
		{
			counter = iElem;
			if (!jacValues)
			{
				for (int k = 0; k < 3; ++k)
				{
					iRow[iElem] = iCond;
					jCol[iElem] = 3 * pointsInitial.size()
						+ 4 * i + k;
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
		std::cout << "eval_jac_g done." << std::endl;
		DEBUG_END;
		return true;
	}


	bool eval_h(Index n, const Number *x, bool new_x, Number obj_factor,
			Index m, const Number *lambda, bool new_lambda,
			Index nnz_h_lag, Index *iRow, Index *jCol,
			Number *hValues)
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
							sum += u[k].cartesian(p)
							* u[k].cartesian(q);
						if (p == q)
							sum *= 2.;
						hValues[iElem] = obj_factor
							* sum;
					}
					++iElem;
				}
				ASSERT(iElem == counter + 3);
				// FIXME: Rework on this.
				unsigned numPlanarityVisited = 0;
				unsigned numConvexityVisited = 0;
				for (unsigned j = 0; j < directions.size(); ++j)
				{
					auto &inc = FT->incident[j];
					auto &sinc = FT->semiIncident[j];
					if (!inc.count(i) && !sinc.count(i))
					{
						numPlanarityVisited += inc.size();
						numConvexityVisited += sinc.size();
						continue;
					}

					if (!hValues)
					{
						iRow[iElem] = 3 * i + p;
						jCol[iElem] = 3 * points.size()
							+ 4 * j + p;
						++iElem;
					}
					else
					{
						int iCond = 0;
						auto P = inc.find(i);
						if (P != inc.end())
							iCond = numPlanarityVisited + std::distance(inc.begin(), P);
						P = sinc.find(i);
						if (P != sinc.end())
							iCond = numPlanarityConstraints + numConvexityVisited + std::distance(sinc.begin(), P);
						hValues[iElem++] =
							0.5 * lambda[iCond];
					}
					numPlanarityVisited += inc.size();
					numConvexityVisited += sinc.size();
				}
			}
		}

		for (unsigned j = 0; j < directions.size(); ++j)
		{
			auto &inc = FT->incident[j];
			auto &sinc = FT->semiIncident[j];
			// FIXME: Rework on this.
			unsigned numPlanarityVisited = 0;
			unsigned numConvexityVisited = 0;
			for (unsigned p = 0; p < 3; ++p)
			{
				int iRowCommon = 3 * points.size() + 4 * j + p;
				counter = iElem;
				for (unsigned i = 0; i < points.size(); ++i)
				{
					if (!inc.count(i) && !sinc.count(i))
						continue;

					if (!hValues)
					{
						iRow[iElem] = iRowCommon;
						jCol[iElem] = 3 * i + p;
						++iElem;
					}
					else
					{
						int iCond = 0;
						if (inc.count(i))
							iCond = numPlanarityVisited;
						if (sinc.count(i))
							iCond = numPlanarityConstraints + numConvexityVisited;
						hValues[iElem++] =
							0.5 * lambda[iCond];
					}
					if (inc.count(i))
						++numPlanarityVisited;
					if (sinc.count(i))
						++numConvexityVisited;
				}

				if (!hValues)
				{
					iRow[iElem] = iRowCommon;
					jCol[iElem] = iRowCommon;
					++iElem;
				}
				else
				{
					int iCond = numPlanarityConstraints
						+ numConvexityConstraints
						+ numConsistencyConstraints + j;
					hValues[iElem++] = 2. * lambda[iCond];
				}
			}
		}
		ASSERT(iElem == unsigned(nnz_h_lag));
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

Polyhedron_3 obtainPolyhedron(FixedTopologyNLP *FTNLP)
{
	DEBUG_START;
	std::vector<Vector_3> directions = FTNLP->getDirections();
	std::vector<double> values = FTNLP->getValues();
	std::vector<Plane_3> planes(values.size());
	for (unsigned i = 0; i < values.size(); ++i)
		planes[i] = Plane_3(directions[i].x(), directions[i].y(),
				directions[i].z(), values[i]);
	Polyhedron_3 intersection(planes);
	std::cout << "Intersection has " << intersection.size_of_vertices()
		<< " vertices, " << intersection.size_of_facets() << " facets."
		<< std::endl;

	std::vector<Vector_3> pointsAsVectors = FTNLP->getPoints();
	std::vector<Point_3> points(pointsAsVectors.size());
	for (const Vector_3 &v : pointsAsVectors)
		points.push_back(Point_3(v.x(), v.y(), v.z()));
	Polyhedron_3 hull;
	CGAL::convex_hull_3(points.begin(), points.end(), hull);
	std::cout << "Hull has " << hull.size_of_vertices() << " vertices, "
		<< hull.size_of_facets() << " facets." << std::endl;
	DEBUG_END;
	return hull;
}

Polyhedron_3 SupportPolyhedronCorrector::run()
{
	DEBUG_START;
	SmartPtr<IpoptApplication> app = IpoptApplicationFactory();

	/* Intialize the IpoptApplication and process the options */
	if (app->Initialize() != Solve_Succeeded)
	{
		MAIN_PRINT("*** Error during initialization!");
		return initialP;
	}

	app->Options()->SetStringValue("linear_solver", "ma57");
	//app->Options()->SetStringValue("derivative_test", "only-second-order");
	//app->Options()->SetStringValue("derivative_test_print_all", "yes");

	/* Prepare the NLP for solving. */
	auto u = SData->supportDirections<Vector_3>();
	VectorXd values = SData->supportValues();
	ASSERT(u.size() == unsigned(values.size()));
	std::vector<double> h(values.size());
	for (unsigned i = 0; i < values.size(); ++i)
		h[i] = values(i);
	std::vector<Vector_3> U;
	std::vector<double> H;
	for (auto I = initialP.facets_begin(), E = initialP.facets_end();
			I != E; ++I)
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

	FixedTopology *FT = new FixedTopology(initialP, SData);

	FixedTopologyNLP *FTNLP = new FixedTopologyNLP(u, h, U, H, points, FT);

	/* Ask Ipopt to solve the problem */
	if (app->OptimizeTNLP(FTNLP) != Solve_Succeeded)
	{
		MAIN_PRINT("** The problem FAILED!");
		DEBUG_END;
		return initialP;
	}
	MAIN_PRINT("*** The problem solved!");
	Polyhedron_3 correctedP = obtainPolyhedron(FTNLP);

	delete FTNLP;
	DEBUG_END;
	return correctedP;
}
