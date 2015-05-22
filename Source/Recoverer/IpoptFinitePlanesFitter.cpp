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


#include <Eigen/LU>

#include <CGAL/point_generators_3.h>
typedef CGAL::Creator_uniform_3<double, Point_3> PointCreator;

IpoptFinitePlanesFitter::IpoptFinitePlanesFitter(
		SupportFunctionDataPtr data, int numFinitePlanes) :
	data_(data),
	numFinitePlanes_(numFinitePlanes),
	problemType_(DEFAULT_ESTIMATION_PROBLEM_NORM),
	variables_(new double[4 * numFinitePlanes]),
	directions_(numFinitePlanes),
	values_(numFinitePlanes),
	planes_(numFinitePlanes)
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
	n = 4 * numFinitePlanes_; /* number of variables */
	m = 2 * numFinitePlanes_; /* number of constraints */
	/* Number of non-zero elements in the Jacobian of constraints: */
	nnz_jac_g = 8 * numFinitePlanes_ * numFinitePlanes_;
	/* Number of non-zero elements in the Hessian of the Lagrangian: */
	nnz_h_lag = 8 * numFinitePlanes_ * numFinitePlanes_;
	/* Use the C style indexing (0-based): */
	index_style = TNLP::C_STYLE;
	DEBUG_END;
	return true;
}

#define TNLP_INFINITY 2e19

bool IpoptFinitePlanesFitter::get_bounds_info(Index n, Number* x_l,
		Number* x_u, Index m, Number* g_l, Number* g_u)
{
	DEBUG_START;
	ASSERT(n == 4 * numFinitePlanes_);
	ASSERT(m == 2 * numFinitePlanes_);

	/* All variables are free: */
	for (Index i = 0; i < n; ++i)
	{
		x_l[i] = -TNLP_INFINITY;
		x_u[i] = +TNLP_INFINITY;
	}

	/* Bounds for consistency conditions: */
	for (Index i = 0; i < numFinitePlanes_; ++i)
	{
		g_l[i] = 0.;
		g_u[i] = +TNLP_INFINITY;
	}

	/* Bounds for normality conditions: */
	for (Index i = numFinitePlanes_; i < m; ++i)
	{
		g_l[i] = 1.;
		g_u[i] = 1.;
	}
	DEBUG_END;
	return true;
}

bool IpoptFinitePlanesFitter::get_starting_point(Index n, bool init_x,
		Number* x, bool init_z, Number* z_L, Number* z_U, Index m,
		bool init_lambda, Number* lambda)
{
	DEBUG_START;
	CGAL::Random_points_on_sphere_3<Point_3, PointCreator> generator(1.0);
	std::vector<Point_3> points;
	CGAL::cpp11::copy_n(generator, numFinitePlanes_,
			std::back_inserter(points));
	for (int i = 0; i < numFinitePlanes_; ++i)
	{
		DEBUG_PRINT("Generated plane #%d: %lf %lf %lf %lf", i,
				points[i].x(), points[i].y(),
				points[i].z(), 1.);
		x[4 * i] = points[i].x();
		x[4 * i + 1] = points[i].y();
		x[4 * i + 2] = points[i].z();
		x[4 * i + 3] = 1.;
	}

	if (init_z | init_lambda)
	{
		ASSERT(0 && "Not implemented yet");
	}
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

/**
 * A functor computing the plane containing a triangular facet
 */
struct Plane_from_facet
{
	Polyhedron_3::Plane_3 operator()(Polyhedron_3::Facet& f)
	{
		Polyhedron_3::Halfedge_handle h = f.halfedge();
		Polyhedron_3::Plane_3 plane(h->vertex()->point(),
				h->next()->vertex()->point(),
				h->opposite()->vertex()->point());
		Vector_3 normal(plane.a(), plane.b(), plane.c());
		double length = sqrt(normal.squared_length());
		length = plane.d() > 0. ? -length : length;
		Polyhedron_3::Plane_3 planeNormalized(plane.a() / length,
				plane.b() / length,
				plane.c() / length,
				plane.d() / length);
		return planeNormalized;
	}
};

void IpoptFinitePlanesFitter::recalculateParameters(void)
{
	DEBUG_START;
	for (int i = 0; i < numFinitePlanes_; ++i)
	{
		Vector3d direction(variables_[4 * i], variables_[4 * i + 1],
				variables_[4 * i + 2]);
		double value = variables_[4 * i + 3];
		/* FIXME: should we check that value >= 0 here? */
		Plane_3 plane(direction.x, direction.y, direction.z, -value);
#ifndef NDEBUG
		std::cerr << "Plane #" << i << ": " << plane << std::endl;
#endif
		directions_[i] = direction;
		values_[i] = value;
		planes_[i] = plane;
	}
	Polyhedron_3 intersection;
	CGAL::internal::halfspaces_intersection(planes_.begin(), planes_.end(),
			intersection, Kernel());
	std::transform(intersection.facets_begin(), intersection.facets_end(),
			intersection.planes_begin(), Plane_from_facet());
	recalculateFunctional(&intersection);

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

const double EPSILON_EQUAL_PLANES = 1e-16;
const int TAG_PLANE_OUTSIDE = -1;

static void initializeIndices(Polyhedron_3 *intersection,
		std::vector<Plane_3> planes)
{
	DEBUG_START;
	for (auto facet = intersection->facets_begin();
			facet != intersection->facets_end(); ++facet)
	{
		Plane_3 planeFitted = facet->plane();
#ifndef NDEBUG
		std::cerr << "Searching nearest plane for " << planeFitted
			<< std::endl;
#endif
		double distanceMinimal = distancePlanes(planeFitted, planes[0]);
		int iPlaneArgmin = 0, i = 0;
		for (auto &plane: planes)
		{
			double distance = distancePlanes(planeFitted, plane);
			if (distanceMinimal > distance)
			{
				distanceMinimal = distance;
				iPlaneArgmin = i;
			}
			++i;
		}
		if (distanceMinimal < EPSILON_EQUAL_PLANES)
			facet->id = iPlaneArgmin;
		else
			facet->id = TAG_PLANE_OUTSIDE;
#ifndef NDEBUG
		std::cerr << "... Found nearest: " << planes[iPlaneArgmin]
			<< std::endl;
		std::cerr << "... with minimal distance = " << distanceMinimal
			<< std::endl;
#endif
	}
#ifndef NDEBUG
	for (auto facet = intersection->facets_begin();
			facet != intersection->facets_end(); ++facet)
	{
		DEBUG_PRINT("facet id: %ld", facet->id);
	}
#endif /* NDEBUG */

	int i = 0;
	for (auto vertex = intersection->vertices_begin();
			vertex != intersection->vertices_end(); ++vertex)
	{
		vertex->id = i++;
	}
	DEBUG_END;
}

/**
 * Contains preliminary caluclated slice of gradient.
 */
struct GradientSlice
{
	/** The ID of the quadruple. */
	int id_;

	/** The values of the quadruple. */
	double values_[4];
};

/**
 * Contains preliminary calculated slice of the hessian.
 */
struct HessianSlice
{
	/** The ID of the block row. */
	int idRow_;

	/** The Id of the block column. */
	int idColumn_;

	/** The matrix block. */
	Eigen::Matrix4d values_;
};

/**
 * The information about tangient points required during the re-calculating of
 * functional and conditions of the problem.
 */
struct TangientPointInformation
{
	/** The ID of the point. */
	int iPoint_;

	/** The tangient point. */
	Vector_3 point_;

	/** The ID of the direction. */
	int iDirection_;

	/** The support direction. */
	Vector_3 direction_;

	/** The array of planes IDs. */
	int indices_[3];

	/** The array of planes. */
	Plane_3 planes_[3];

	/** The inverse of the planes normals matrix. */
	Eigen::Matrix3d inverse_;

	/**
	 * Calculates the inverse of the planes normals matrix.
	 *
	 * @return		The inverse of the planes normals matrix.
	 */
	Eigen::Matrix3d calculateInverse(void)
	{
		DEBUG_START;
		Eigen::Matrix3d fromDirections;
		for (int i = 0; i < 3; ++i)
		{
			fromDirections(i, 0) = planes_[i].a();
			fromDirections(i, 1) = planes_[i].b();
			fromDirections(i, 2) = planes_[i].c();
		}

		Eigen::Matrix3d inverse = fromDirections.inverse();
		DEBUG_END;
		return inverse;
	}

	/**
	 * Creates the information about tangient points.
	 *
	 * @param iDirection	The ID of the direction.
	 * @param direction	The direction.
	 * @param vertex	The vertex iterator pointing to the vertex.
	 */
	TangientPointInformation(int iDirection, Vector_3 direction,
			Polyhedron_3::Vertex_iterator vertex)
	{
		DEBUG_START;
		iPoint_ = vertex->id;
		iDirection_ = iDirection;
		point_ = vertex->point() - CGAL::ORIGIN;
		direction_ = direction;
		int iHalfedge = 0;
		auto halfedge = vertex->vertex_begin();
		auto halfedgeFirst = halfedge;
		do
		{
			auto facet = halfedge->facet();
			indices_[iHalfedge] = facet->id;
			planes_[iHalfedge] = facet->plane();
			++halfedge;
			++iHalfedge;
		} while (halfedge != halfedgeFirst);
#ifndef NDEBUG
		std::cerr << "... The best is #" << vertex->id << " : "
			<< *this << std::endl;
#endif
		inverse_ = calculateInverse();
		DEBUG_END;
	}

	/**
	 * Prints the information to the stream.
	 *
	 * @param stream	The stream.
	 * @param info		The information.
	 *
	 * @return		The srteam ready for further printing.
	 */
	friend std::ostream &operator <<(std::ostream &stream,
			TangientPointInformation &info)
	{
		DEBUG_START;
		stream << "information:" << std::endl;
		stream << "   tangient point is #" << info.iPoint_ << ": "
			<< info.point_ << std::endl;
		stream << "   support direction is #" << info.iDirection_
			<< ": " << info.direction_ << std::endl;
		stream << "   facets:" << std::endl;
		for (int i = 0; i < 3; ++i)
		{
			stream << "      " << info.indices_[i] << ": "
				<< info.planes_[i] << std::endl;
		}
		DEBUG_END;
		return stream;
	}

	/**
	 * Calculates 3 slices (consecutive qeuadruples of the gradient
	 * components) of the gradient.
	 *
	 * @return 		The pointer to slices array.
	 */
	GradientSlice *calculateFirstDerivative(void)
	{
		DEBUG_START;

		double products[3];
		for (int i = 0; i < 3; ++i)
		{
			Vector_3 column = Vector_3(inverse_(0, i),
					inverse_(1, i),
					inverse_(2, i));
			products[i] = direction_ * column;
		}

		GradientSlice *result = new GradientSlice[3];
		for (int i = 0; i < 3; ++i)
		{
			result[i].id_ = indices_[i];
			result[i].values_[0] = -point_.x() * products[i];
			result[i].values_[1] = -point_.y() * products[i];
			result[i].values_[2] = -point_.z() * products[i];
			result[i].values_[3] = products[i];
		}

		DEBUG_END;
		return result;
	}

	/**
	 * Calculates 9 slices (4x4 matrix blocks) of the hessian.
	 *
	 * @return		The pointer to the slices array.
	 */
	HessianSlice *calculateSecondDerivative(void)
	{
		DEBUG_START;

		/* FIXME: make this part of code static. */
		Eigen::Matrix3d **E = new Eigen::Matrix3d*[3];
		for (int i = 0; i < 3; ++i)
		{
			E[i] = new Eigen::Matrix3d[3];
			for (int j = 0; j < 3; ++j)
			{
				for (int k = 0; k < 3; ++k)
					for (int l = 0; l < 3; ++l)
						E[i][j](k, l) = 0.;
				E[i][j](i, j) = 1.;
			}
		}

		Eigen::Vector3d *e = new Eigen::Vector3d[3];
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				e[i](j) = 0.;
			}
			e[i](i) = 1.;
		}

		HessianSlice *result = new HessianSlice[9];

		Eigen::Vector3d tangient;
		tangient(0) = point_.x();
		tangient(1) = point_.y();
		tangient(2) = point_.z();
		Eigen::Matrix3d matrix;
		for (int s = 0; s < 3; ++s)
		{
			for (int t = 0; t < 3; ++t)
			{
				auto slice = &result[3 * s + t];
				slice->idRow_ = s;
				slice->idColumn_ = t;
				for (int i = 0; i < 3; ++i)
				{
					for (int j = 0; j < 3; ++j)
					{
						matrix = E[s][i] * inverse_
							* E[t][j] + E[t][j]
							* inverse_ * E[s][i];
						matrix = inverse_ * matrix;
						auto v = matrix * tangient;
						double value = -direction_
							* Vector_3(v(0), v(1),
									v(2));
						slice->values_(i, j) = value;
					}
					matrix = inverse_ * E[s][i] * inverse_;
					auto v = matrix * e[t];
					double value = direction_
						* Vector_3(v(0), v(1), v(2));
					slice->values_(i, 3) = value;
				}
				slice->values_(3, 3) = 0.;
			}
		}

		DEBUG_END;
		return result;
	}
};


static std::vector<TangientPointInformation> searchTangientVertices(
		Polyhedron_3 *intersection, SupportFunctionDataPtr data)
{
	DEBUG_START;
	int numDirections = data->size();
	std::vector<Point_3> directions = data->supportDirectionsCGAL();
	std::vector<TangientPointInformation> informations;
	for (int i = 0; i < numDirections; ++i)
	{
		Vector_3 direction = directions[i] - CGAL::ORIGIN;
#ifndef NDEBUG
		std::cerr << "Searching for tangient point for direction "
			<< direction << std::endl;
#endif

		auto vertex = intersection->vertices_begin();
		auto vertexBest = vertex;
		double productMaximal = (vertex->point() - CGAL::ORIGIN)
			* direction;
		while (vertex != intersection->vertices_end())
		{
			ASSERT(vertex->is_trivalent());
			double product = (vertex->point() - CGAL::ORIGIN)
				* direction;
			if (product > productMaximal)
			{
				productMaximal = product;
				vertexBest = vertex;
			}
			++vertex;
		}
		ASSERT(vertexBest->is_trivalent());
		TangientPointInformation info(i, direction, vertexBest);
		informations.push_back(info);
	}
	DEBUG_END;
	return informations;
}


void IpoptFinitePlanesFitter::recalculateFunctional(Polyhedron_3 *intersection)
{
	DEBUG_START;
	initializeIndices(intersection, planes_);
	searchTangientVertices(intersection, data_);
	
	DEBUG_END;
}

SupportFunctionDataPtr IpoptFinitePlanesFitter::run(void)
{
	DEBUG_START;
	get_starting_point(4 * numFinitePlanes_, true, variables_, false, NULL,
			NULL, 0, false, NULL);
	recalculateParameters();
	
#if 0
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
#endif
	DEBUG_END;
	return data_;
}

#endif /* USE_IPOPT */
