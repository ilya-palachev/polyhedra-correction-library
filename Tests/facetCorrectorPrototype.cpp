
#include <fstream>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>
#include <CGAL/squared_distance_3.h>
#include <coin/IpTNLP.hpp>
#include <coin/IpIpoptApplication.hpp>
using namespace Ipopt;

#include "Polyhedron_3/Polyhedron_3.h"
#include "DataConstructors/ShadowContourConstructor/ShadowContourConstructor.h"

// A modifier creating a pyramid with the incremental builder.
template <class HDS>
class BuildPyramid: public CGAL::Modifier_base<HDS>
{
private:
	int numSideFacets_;
public:
	BuildPyramid(int numSideFacets): numSideFacets_(numSideFacets) {}

	void operator()(HDS& hds)
	{
		// Postcondition: hds is a valid polyhedral surface.
		CGAL::Polyhedron_incremental_builder_3<HDS> B(hds, true);
		B.begin_surface(numSideFacets_ + 1, numSideFacets_ + 1,
				4 * numSideFacets_);
		for (int i = 0; i < numSideFacets_; ++i)
		{
			double angle = 2. * M_PI * i / numSideFacets_;
			double x = cos(angle);
			double y = sin(angle);
			double z = -1.;
#if 0
			double rotation = 0.1;
			double yy = y * cos(rotation) + z * sin(rotation);
			double zz = y * sin(rotation) + z * cos(rotation);
			y = yy;
			z = zz;
#endif
			B.add_vertex(Point_3(x, y, z));
		}
		B.add_vertex(Point_3(0., 0., 1.));
		for (int i = 0; i < numSideFacets_; ++i)
		{
			B.begin_facet();
			B.add_vertex_to_facet(numSideFacets_);
			B.add_vertex_to_facet(i);
			B.add_vertex_to_facet((i + 1) % numSideFacets_);
			B.end_facet();
		}
		B.begin_facet();
		for (int i = numSideFacets_ - 1; i >= 0; --i)
			B.add_vertex_to_facet(i);
		B.end_facet();
		B.end_surface();
	}
};

/**
 * Generates random number d such that |d| <= maxDelta
 *
 * @param maxDelta	maximum absolute limit of generated number
 */
static double genRandomDouble(double maxDelta)
{
	DEBUG_START;
	//srand((unsigned) time(0));
	struct timeval t1;
	gettimeofday(&t1, NULL);
	srand(t1.tv_usec * t1.tv_sec);
	
	int randomInteger = rand();
	double randomDouble = randomInteger;
	const double halfRandMax = RAND_MAX * 0.5;
	randomDouble -= halfRandMax;
	randomDouble /= halfRandMax;

	randomDouble *= maxDelta;

	DEBUG_END;
	return randomDouble;
}

const double MEAN_VALUE = 0.5;
const double RAND_MAX_VALUE = 0.1;
const double MAX_COORDINATE = 1.;
std::pair<Polyhedron_3, int> cutPyramid(Polyhedron_3 pyramid)
{
	pyramid = Polyhedron_3(Polyhedron(pyramid));
	double a = genRandomDouble(MAX_COORDINATE);
	double b = genRandomDouble(MAX_COORDINATE);
	double c = 1.;
	double norm = sqrt(a * a + b * b + c * c);
	a /= norm;
	b /= norm;
	c /= norm;
	double h = MEAN_VALUE + genRandomDouble(RAND_MAX_VALUE);
	Plane_3 planeCutting(a, b, c, -h);

	std::vector<Plane_3> planes(pyramid.size_of_facets() + 1);
	int iPlane = 0;
	for (auto facet = pyramid.facets_begin();
			facet != pyramid.facets_end(); ++facet)
	{
		planes[iPlane++] = facet->plane();
	}
	planes[pyramid.size_of_facets()] = planeCutting;
	Polyhedron_3 intersection(planes.begin(), planes.end());

	int iPlaneCutting = intersection.size_of_facets();
	iPlane = 0;
	for (auto facet = intersection.facets_begin();
			facet != intersection.facets_end(); ++facet)
	{
		if (facet->plane() == planeCutting)
		{
			iPlaneCutting = iPlane;
			break;
		}
		++iPlane;
	}
	if (iPlaneCutting == (int) intersection.size_of_facets())
		std::cerr << "Failed to find cutting plane in intersection"
			<< std::endl;
	else
		std::cout << "Found cutting plane as the " << iPlaneCutting
			<< "-th plane in the intersection." << std::endl;

	return std::make_pair(intersection, iPlaneCutting);
}

typedef Polyhedron_3::HalfedgeDS HalfedgeDS;

/**
 * The structure defining the description of QP problem for facet correction
 * for one particular vertex.
 */
struct ProblemPointDescription
{
	/** The vertex ID in the initial polyhedron. */
	int iVertex;

	/**
	 * The list of target lines that should the corrected vertex should
	 * belong to (with some precision).
	 */
	std::vector<Line_3> lines;

	/**
	 * The list target planes that should the corrected vertex should
	 * belong to (precisely).
	 */
	std::vector<Plane_3> planes;

	/** The initial position of corrected vertex. */
	Point_3 initialPosition;
};

static
std::set<int>
buildFacetIndices(
		Polyhedron_3 polyhedron,
		int iFacetCutting)
{
	polyhedron.initialize_indices();
	auto facet = polyhedron.facets_begin() + iFacetCutting;
	std::set<int> indicesCutting;
	auto circulator = facet->facet_begin();
	do
	{
		indicesCutting.insert(circulator->vertex()->id);
		++circulator;
	}
	while (circulator != facet->facet_begin());
	std::cout << "Cutting facet contains " << facet->facet_degree()
		<< " vertices:";
	for (int i: indicesCutting)
		std::cout << " " << i;
	std::cout << std::endl;
	return indicesCutting;
}

static
std::vector<ProblemPointDescription>
buildPlanesDescriptions(
		Polyhedron_3 polyhedron,
		int iFacetCutting,
		std::set<int> indicesCutting)
{
	std::vector<ProblemPointDescription> descriptions;

	/* Build planes descriptions. */
	for (int iVertex: indicesCutting)
	{
		ProblemPointDescription description;
		description.iVertex = iVertex;
		auto vertex = polyhedron.vertices_begin() + iVertex;
		description.initialPosition = vertex->point();
		auto circulator = vertex->vertex_begin();
		bool ifVertexCorrect = false;
		do
		{
			auto facet = circulator->facet();
			int iFacet = facet->id;
			std::cout << "Vertex " << iVertex
				<< " is incident to facet " << iFacet
				<< std::endl;
			Plane_3 plane = facet->plane();
			double distance = CGAL::squared_distance<Kernel>(plane,
					vertex->point());
			if (iFacet != iFacetCutting)
			{
				description.planes.push_back(plane);
				std::cout << "Plane: " << plane	<< std::endl;
				std::cout << "Distance: " << distance
					<< std::endl;
				std::cout << "Adding it to vector, so now it "
					<< "contains "
					<< description.planes.size()
					<< " planes." << std::endl;
			}
			else
				ifVertexCorrect = true;
			++circulator;
		}
		while (circulator != vertex->vertex_begin());
		if (!ifVertexCorrect)
		{
			std::cerr << "Vertex #" << iVertex
				<< " is incorrect, doesn't belong to cutting "
				<< "facet!" << std::endl;
		       exit(EXIT_FAILURE);
		}
		descriptions.push_back(description);
	}
	return descriptions;
}

static
std::vector<ProblemPointDescription>
buildProblemPointDescriptions(
		Polyhedron_3 polyhedron,
		int iFacetCutting,
		int numContours)
{
	auto indicesCutting = buildFacetIndices(polyhedron, iFacetCutting);
	auto descriptions = buildPlanesDescriptions(polyhedron, iFacetCutting,
			indicesCutting);

	double angleFirst = genRandomDouble(0.1);
	/* Generate contours and build lines description by them. */
	for (int iContour = 0 ; iContour < numContours; ++iContour)
	{
		double angle = angleFirst + 2 * M_PI * iContour / numContours;
		Vector_3 normal(cos(angle), sin(angle), 0.);
		auto result = generateProjection(polyhedron, normal);
		auto contour = result.first;
		auto indices = result.second;
		std::cout << "Generated contour #" << iContour << ":";
		for (int iVertex: indices)
			std::cout << " " << iVertex;
		std::cout << std::endl;
		for (int i = 0; i < (int) indices.size(); ++i)
		{
			int iVertex = indices[i];
			int iDescription = descriptions.size();
			for (int j = 0; j < (int) descriptions.size(); ++j)
			{
				if (descriptions[j].iVertex == iVertex)
				{
					iDescription = j;
					break;
				}
			}
			if (iDescription == (int) descriptions.size())
				continue;
			Point_3 point = contour[i];
			Line_3 line(point, point + normal);
			descriptions[iDescription].lines.push_back(line);
		}
		std::cout << std::endl;
	}
	return descriptions;
}

void dumpBody(Polyhedron_3 body, const char *comment)
{
	std::string output_name = "polyhedron." + std::to_string(getpid())
		+ "." + comment + ".ply";
	std::cout << "Dumping to file " << output_name << "... ";
	std::ofstream output;
	output.open(output_name, std::ostream::out);
	output << body;
	output.close();
	std::cout << "done" << std::endl;
}

void dumpDescriptions(std::vector<ProblemPointDescription> descriptions)
{
	for (auto description: descriptions)
	{
		int iVertex = description.iVertex;
		std::cout << "Target lines for vertex #" << iVertex << ":"
			<< std::endl;
		for (Line_3 line: description.lines)
			std::cout << "    " << line << std::endl;
		std::cout << "    " << description.lines.size() << " in total"
			<< std::endl;
		std::cout << "Target planes for vertex #" << iVertex << ":"
			<< std::endl;
		for (Plane_3 plane: description.planes)
			std::cout << "        " << plane << std::endl;
		std::cout << "         " << description.planes.size()
			<< " in total" << std::endl;
	}
}

/** The final position of plane. */
static Plane_3 planeFinal_;

class FacetCorrectionNLP: public TNLP
{
private:
	/** The descriptions of the problem objects. */
	std::vector<ProblemPointDescription> descriptions_;

	/** The initial position of plane. */
	Plane_3 planeInitial_;

public:
	/** default constructor */
	FacetCorrectionNLP(std::vector<ProblemPointDescription> descriptions,
			Plane_3 planeInitial):
		descriptions_(descriptions),
		planeInitial_(planeInitial)
	{}

	/** default destructor */
	virtual ~FacetCorrectionNLP() {}


	/**@name Overloaded from TNLP */
	//@{
	/** Method to return some info about the nlp */
	virtual bool get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
		Index& nnz_h_lag, IndexStyleEnum& index_style)
	{
		n = 4 +                            /* u, h */
			3 * descriptions_.size();  /* A_i */

		int numPlanesConstraints = 0;
		for (auto description: descriptions_)
			numPlanesConstraints += description.planes.size();
		m = 1 +                             /* (u, u) = 1 */
			descriptions_.size() +      /* (u,   A_i) = h   */
			numPlanesConstraints;       /* (u_k, A_i) = h_k */

		nnz_jac_g = 3 +                     /* (u, u) = 1 */
			7 * descriptions_.size() +  /* (u,   A_i) = h   */
			3 * numPlanesConstraints;   /* (u_k, A_i) = h_k */

		nnz_h_lag = 3 + 15 * descriptions_.size();

		index_style = TNLP::C_STYLE;
		return true;
	}

#define IPOPT_INFINITY 2e19
	/** Method to return the bounds for my problem */
	virtual bool get_bounds_info(Index n, Number* x_l, Number* x_u,
		Index m, Number* g_l, Number* g_u)
	{
		for (Index i = 0; i < n; ++i)
		{
			x_l[i] = -IPOPT_INFINITY;
			x_u[i] = +IPOPT_INFINITY;
		}
		g_l[0] = g_u[0] = 1.;
		for (Index i = 1; i < m; ++i)
		{
			g_l[i] = 0.;
			g_u[i] = 0.;
		}
		return true;
	}

	/** Method to return the starting point for the algorithm */
	virtual bool get_starting_point(Index n, bool init_x, Number* x,
		bool init_z, Number* z_L, Number* z_U,
		Index m, bool init_lambda,
		Number* lambda)
	{
		assert(init_x == true);
		assert(init_z == false);
		assert(init_lambda == false);
		x[0] = planeInitial_.a();
		x[1] = planeInitial_.b();
		x[2] = planeInitial_.c();
		x[3] = planeInitial_.d();
		Index i = 4;
		for (auto description: descriptions_)
		{
			x[i + 0] = description.initialPosition.x();
			x[i + 1] = description.initialPosition.y();
			x[i + 2] = description.initialPosition.z();
			i += 3;
		}
		return true;
	}

	/** Method to return the objective value */
	virtual bool eval_f(Index n, const Number* x, bool new_x,
			Number& obj_value)
	{
		Index i = 4;
		obj_value = 0.;
		for (auto description: descriptions_)
		{
			Point_3 point(x[i], x[i + 1], x[i + 2]);
			for (Line_3 line: description.lines)
				obj_value += CGAL::squared_distance(point,
						line);
			i += 3;
		}
		return true;
	}

	/** Method to return the gradient of the objective */
	virtual bool eval_grad_f(Index n, const Number* x, bool new_x,
			Number* grad_f)
	{
		for (Index i = 0; i < n; ++i)
			grad_f[i] = 0.;
		Index i = 4;
		for (auto description: descriptions_)
		{
			Point_3 point(x[i], x[i + 1], x[i + 2]);
			for (Line_3 line: description.lines)
			{
				Vector_3 direction = line.to_vector();
				Point_3 projection = line.projection(point);
				grad_f[i + 0] += (projection.x() - point.x()) *
					(direction.x() * direction.x() - 1.);
				grad_f[i + 0] += (projection.y() - point.y()) * 
					(direction.y() * direction.x());
				grad_f[i + 0] += (projection.z() - point.z()) * 
					(direction.z() * direction.x());
				grad_f[i + 1] += (projection.x() - point.x()) *
					(direction.x() * direction.y());
				grad_f[i + 1] += (projection.y() - point.y()) *
					(direction.y() * direction.y() - 1.);
				grad_f[i + 1] += (projection.z() - point.z()) *
					(direction.z() * direction.y());
				grad_f[i + 2] += (projection.x() - point.x()) *
					(direction.x() * direction.z());
				grad_f[i + 2] += (projection.y() - point.y()) *
					(direction.y() * direction.z());
				grad_f[i + 2] += (projection.z() - point.z()) *
					(direction.z() * direction.z() - 1.);
			}
			grad_f[i + 0] *= 2.;
			grad_f[i + 1] *= 2.;
			grad_f[i + 2] *= 2.;
			i += 3;
		}
		return true;
	}

	/** Method to return the constraint residuals */
	virtual bool eval_g(Index n, const Number* x, bool new_x, Index m,
			Number* g)
	{
		Vector_3 normalMoved(x[0], x[1], x[2]);
		Plane_3 planeMoved(x[0], x[1], x[2], x[3]);
		Index iConstraint = 0;
		g[iConstraint++] = normalMoved.squared_length();
		for (int iPoint = 0; iPoint < (int) descriptions_.size();
				++iPoint)
		{
			Index i = 4 + 3 * iPoint;
			Vector_3 point(x[i], x[i + 1], x[i + 2]);
			g[iConstraint++] = normalMoved * point + planeMoved.d();
		}
		for (int iPoint = 0; iPoint < (int) descriptions_.size();
				++iPoint)
		{
			Index i = 4 + 3 * iPoint;
			Vector_3 point(x[i], x[i + 1], x[i + 2]);
			for (Plane_3 plane: descriptions_[iPoint].planes)
			{
				Vector_3 normal(plane.a(), plane.b(),
						plane.c());
				g[iConstraint++] = normal * point + plane.d();
			}
		}
		return true;
	}

	/** Method to return:
	*   1) The structure of the jacobian (if "values" is NULL)
	*   2) The values of the jacobian (if "values" is not NULL)
	*/
	virtual bool eval_jac_g(Index n, const Number* x, bool new_x,
		Index m, Index nele_jac, Index* iRow, Index *jCol,
		Number* values)
	{
		Index *iRow_ = new Index[nele_jac];
		Index *jCol_ = new Index[nele_jac];
		Number *x_ = new Number[n];
		if (x)
			for (int i = 0; i < n; ++i)
				x_[i] = x[i];
		else
			for (int i = 0; i < n; ++i)
				x_[i] = 0.;
		Number *values_ = new Number[nele_jac];

		Index iConstraint = 0;
		Index iNonzero = 0;
		Number planeMoved[4];
		for (Index iVariable = 0; iVariable < 3; ++iVariable)
		{
			iRow_[iNonzero] = iConstraint;
			jCol_[iNonzero] = iVariable;
			values_[iNonzero] = 2. * x_[iVariable];
			++iNonzero;
		}
		for (Index iVariable = 0; iVariable < 4; ++iVariable)
		{
			planeMoved[iVariable] = x_[iVariable];
		}
		++iConstraint;

		Index numPoints = descriptions_.size();
		for (Index iPoint = 0; iPoint < numPoints; ++iPoint)
		{
			Index iVariableFirst = 4 + 3 * iPoint;
			Number point[4];
			for (Index iVariable = 0; iVariable < 3; ++iVariable)
				point[iVariable] =
					x_[iVariableFirst + iVariable];
			point[3] = 1.;

			for (Index iVariable = 0; iVariable < 4;
					++iVariable)
			{
				iRow_[iNonzero] = iConstraint;
				jCol_[iNonzero] = iVariable;
				values_[iNonzero] = point[iVariable];
				++iNonzero;
			}
			for (Index iVariable = iVariableFirst;
					iVariable < iVariableFirst + 3;
					++iVariable)
			{
				iRow_[iNonzero] = iConstraint;
				jCol_[iNonzero] = iVariable;
				values_[iNonzero] = planeMoved[iVariable
					- iVariableFirst];
				++iNonzero;
			}
			++iConstraint;
		}

		for (Index iPoint = 0; iPoint < numPoints; ++iPoint)
		{
			for (Plane_3 plane:
					descriptions_[iPoint].planes)
			{
				Number planeFixed[4];
				planeFixed[0] = plane.a();
				planeFixed[1] = plane.b();
				planeFixed[2] = plane.c();
				planeFixed[3] = plane.d();
				Index iVariableFirst = 4 + 3 * iPoint;
				for (Index iVariable = iVariableFirst;
						iVariable < iVariableFirst + 3;
						++iVariable)
				{
					iRow_[iNonzero] = iConstraint;
					jCol_[iNonzero] = iVariable;
					values_[iNonzero] = planeFixed[iVariable
						- iVariableFirst];
					++iNonzero;
				}
				++iConstraint;
			}
		}
		assert(iNonzero == nele_jac);

		if (values)
		{
			for (Index i = 0; i < nele_jac; ++i)
				values[i] = values_[i];
		}
		else
		{
			for (Index i = 0; i < nele_jac; ++i)
			{
				iRow[i] = iRow_[i];
				jCol[i] = jCol_[i];
			}
		}
		delete[] iRow_;
		delete[] jCol_;
		delete[] x_;
		delete[] values_;
		return true;
	}

	/** Method to return:
	*   1) The structure of the hessian of the lagrangian (if "values" is NULL)
	*   2) The values of the hessian of the lagrangian (if "values" is not NULL)
	*/
	virtual bool eval_h(Index n, const Number* x, bool new_x,
		Number obj_factor, Index m, const Number* lambda,
		bool new_lambda, Index nele_hess, Index* iRow,
		Index* jCol, Number* values)
	{
		Index *iRow_ = new Index[nele_hess];
		Index *jCol_ = new Index[nele_hess];
		Number *lambda_ = new Number[m];
		if (lambda)
			for (int i = 0; i < m; ++i)
				lambda_[i] = lambda[i];
		else
			for (int i = 0; i < m; ++i)
				lambda_[i] = 0.;
		Number *values_ = new Number[nele_hess];
		int iNonzero = 0;

		int numPoints = descriptions_.size();
		for (Index i = 0; i < 3; ++i)
		{
			iRow_[iNonzero] = i;
			jCol_[iNonzero] = i;
			values_[iNonzero] = lambda_[0];
			++iNonzero;
			for (int iPoint = 0; iPoint < numPoints; ++iPoint)
			{
				iRow_[iNonzero] = i;
				jCol_[iNonzero] = 4 + 3 * iPoint + i;
				values_[iNonzero] = lambda_[1 + iPoint];
				++iNonzero;
			}

		}

		for (int iPoint = 0; iPoint < numPoints; ++iPoint)
		{
			std::vector<Number> d[3][3];
			auto lines = descriptions_[iPoint].lines;
			int numLines = lines.size();
			for (auto line: lines)
			{
				auto vector = line.to_vector();
				double x = vector.x();
				double y = vector.y();
				double z = vector.z();
				d[0][0].push_back(x * x - 1.);
				d[0][1].push_back(x * y);
				d[0][2].push_back(x * z);
				d[1][0].push_back(y * x);
				d[1][1].push_back(y * y - 1.);
				d[1][2].push_back(y * z);
				d[2][0].push_back(z * x);
				d[2][1].push_back(z * y);
				d[2][2].push_back(z * z - 1.);
			}

			for (int i = 0; i < 3; ++i)
			{
				int row = 4 + 3 * iPoint + i;
				iRow_[iNonzero] = row;
				jCol_[iNonzero] = i;
				values_[iNonzero] = lambda_[1 + iPoint];
				++iNonzero;
				
				for (int j = 0; j < 3; ++j)
				{
					int col = 4 + 3 * iPoint + j;
					iRow_[iNonzero] = row;
					jCol_[iNonzero] = col;
					values_[iNonzero] = 0.;
					for (int k = 0; k < 3; ++k)
					{
						for (int l = 0; l < numLines;
								++l)
						{
							values_[iNonzero] +=
								d[k][i][l] *
								d[k][j][l] * 2.;
						}
					}
					++iNonzero;
				}
			}
		}
		assert(iNonzero == nele_hess);

		if (values)
		{
			for (Index i = 0; i < nele_hess; ++i)
				values[i] = values_[i];
		}
		else
		{
			for (Index i = 0; i < nele_hess; ++i)
			{
				iRow[i] = iRow_[i];
				jCol[i] = jCol_[i];
			}
		}
		delete[] iRow_;
		delete[] jCol_;
		delete[] lambda_;
		delete[] values_;
		return true;
	}

	//@}

	/** @name Solution Methods */
	//@{
	/** This method is called when the algorithm is complete so the TNLP can store/write the solution */
	virtual void finalize_solution(SolverReturn status,
		Index n, const Number* x, const Number* z_L, const Number* z_U,
		Index m, const Number* g, const Number* lambda,
		Number obj_value,
		const IpoptData* ip_data,
		IpoptCalculatedQuantities* ip_cq)
	{
		planeFinal_ = Plane_3(x[0], x[1], x[2], x[3]);
	}
	//@}

	private:
	/**@name Methods to block default compiler methods.
	* The compiler automatically generates the following three methods.
	*  Since the default compiler implementation is generally not what
	*  you want (for all but the most simple classes), we usually 
	*  put the declarations of these methods in the private section
	*  and never implement them. This prevents the compiler from
	*  implementing an incorrect "default" behavior without us
	*  knowing. (See Scott Meyers book, "Effective C++")
	*  
	*/
	//@{
	//  HS071_NLP();
	FacetCorrectionNLP(const FacetCorrectionNLP&);
	FacetCorrectionNLP& operator=(const FacetCorrectionNLP&);
	//@}
};

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0]
			<< " <pyramid sides number> <shadow contours number>"
			<< std::endl;
		exit(EXIT_FAILURE);
	}
	int numSidesPyramid = atoi(argv[1]);
	int numShadowContours = atoi(argv[2]);
	std::cout << "We will build " << numShadowContours
		<< " shadow contours." << std::endl;

	Polyhedron_3 pyramid;
	BuildPyramid<HalfedgeDS> pyramidBuilder(numSidesPyramid);
	pyramid.delegate(pyramidBuilder);
	auto result = cutPyramid(pyramid);
	Polyhedron_3 pyramidCut = result.first;
	int iFacetCutting = result.second;
	dumpBody(pyramidCut, "initial");
	
	auto descriptions = buildProblemPointDescriptions(
			pyramidCut, iFacetCutting, numShadowContours);
	dumpDescriptions(descriptions);

	Plane_3 plane = (pyramidCut.facets_begin() + iFacetCutting)->plane();
	SmartPtr<TNLP> mynlp = new FacetCorrectionNLP(descriptions, plane);
	SmartPtr<IpoptApplication> app = IpoptApplicationFactory();
	app->RethrowNonIpoptException(true);
	app->Options()->SetNumericValue("tol", 1e-7);
	app->Options()->SetStringValue("mu_strategy", "adaptive");
	app->Options()->SetStringValue("output_file", "ipopt.out");
	ApplicationReturnStatus status;
	status = app->Initialize();
	if (status != Solve_Succeeded)
	{
		std::cout << std::endl << std::endl
			<< "*** Error during initialization!" << std::endl;
		return (int) status;
	}
	status = app->OptimizeTNLP(mynlp);
	if (status == Solve_Succeeded)
	{
		std::cout << std::endl << std::endl << "*** The problem solved!"
			<< std::endl;
		std::vector<Plane_3> planes;
		int iFacet = 0;
		for (auto facet = pyramidCut.facets_begin();
				facet != pyramidCut.facets_end(); ++facet)
		{
			if (iFacet == iFacetCutting)
				planes.push_back(planeFinal_);
			else
				planes.push_back(facet->plane());
			++iFacet;
		}
		Polyhedron_3 pyramidFinal(planes);
		dumpBody(pyramidFinal, "final");
	}
	else
		std::cout << std::endl << std::endl << "*** The problem FAILED!"
			<< std::endl;

	return (int) status;
}
