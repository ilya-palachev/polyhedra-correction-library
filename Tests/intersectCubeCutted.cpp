/* 
 * Copyright (c) 2009-2013 Ilya Palachev <iliyapalachev@gmail.com>
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

#include "PolyhedraCorrectionLibrary.h"

void test_cube_cutted();

int main(int argc, char** argv)
{
	DEBUG_START;
	test_cube_cutted();
	DEBUG_END;
}

void test_cube_cutted()
{
	DEBUG_START;
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
	DEBUG_END;
}

