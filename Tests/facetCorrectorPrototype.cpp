
#include <fstream>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>
#include <coin/IpTNLP.hpp>

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
		descriptions.push_back(description);
		auto vertex = polyhedron.vertices_begin() + iVertex;
		auto circulator = vertex->vertex_begin();
		bool ifVertexCorrect = false;
		do
		{
			auto facet = circulator->facet();
			int iFacet = facet->id;
			if (iFacet != iFacetCutting)
				description.planes.push_back(facet->plane());
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

	std::string output_name = "polyhedron." + std::to_string(getpid())
		+ ".ply";
	std::cout << "Dumping to file " << output_name << "... ";
	std::ofstream output;
	output.open(output_name, std::ostream::out);
	output << pyramidCut;
	output.close();
	std::cout << "done" << std::endl;
	
	auto descriptions = buildProblemPointDescriptions(
			pyramidCut, iFacetCutting, numShadowContours);
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
	return 0;
}
