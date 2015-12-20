
#include <fstream>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>
#include "Polyhedron_3/Polyhedron_3.h"

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
Polyhedron_3 cutPyramid(Polyhedron_3 pyramid)
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
	for (auto plane: planes)
		std::cout << plane << std::endl;
	Polyhedron_3 intersection(planes.begin(), planes.end());
	return intersection;
}

typedef Polyhedron_3::HalfedgeDS HalfedgeDS;

int main(int argc, char **argv) {
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <pyramid sides number>"
			<< std::endl;
		exit(EXIT_FAILURE);
	}
	int numSidesPyramid = atoi(argv[1]);
	
	Polyhedron_3 P;
	BuildPyramid<HalfedgeDS> pyramid(numSidesPyramid);
	P.delegate(pyramid);
	Polyhedron_3 intersection = cutPyramid(P);

	std::string output_name = "polyhedron." + std::to_string(getpid()) + ".ply";
	std::cout << "Dumping to file " << output_name << "... ";
	std::ofstream output;
	output.open(output_name, std::ostream::out);
	output << intersection;
	output.close();
	std::cout << "done" << std::endl;
	return 0;
}
