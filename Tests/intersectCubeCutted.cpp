#include "PolyhedraCorrectionLibrary.h"

void test_cube_cutted();

int main(int argc, char** argv)
{
	test_cube_cutted();
}

void test_cube_cutted()
{
	Plane iplane;

	Polyhedron* poly = new CubeCutted();
	poly->fprint_ply_scale(1e3, "../poly-data-out/cube_cutted.ply", "poly");
	poly->preprocessAdjacency();
	iplane = Plane(Vector3d(0., 0., 1), -1.);
	poly->intersect(iplane);
	poly->fprint_ply_scale(1e3, "../poly-data-out/cube_cutted_0x0y1z_1.ply",
			"poly");
	delete poly;

	poly = new CubeCutted();
	poly->preprocessAdjacency();
	iplane = Plane(Vector3d(0., 0., 1), -2.);
	poly->intersect(iplane);
	poly->fprint_ply_scale(1e3, "../poly-data-out/cube_cutted_0x0y1z_2.ply",
			"poly");
	delete poly;

	poly = new CubeCutted();
	poly->preprocessAdjacency();
	iplane = Plane(Vector3d(0., 0., 1), -3.);
	poly->intersect(iplane);
	poly->fprint_ply_scale(1e3, "../poly-data-out/cube_cutted_0x0y1z_3.ply",
			"poly");
	delete poly;
}

