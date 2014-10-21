/*
 * Copyright (c) 2009-2014 Ilya Palachev <iliyapalachev@gmail.com>
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

#ifndef GSASSOCIATOR_H_
#define GSASSOCIATOR_H_

#include <memory>

#include "Vector3d.h"
#include "DataContainers/EdgeData/EdgeData.h"
#include "Polyhedron/Polyhedron.h"
#include "Correctors/GlobalShadowCorrector/GlobalShadowCorrector.h"

enum Orientation
{
	ORIENTATION_LEFT, ORIENTATION_RIGHT
};

class GSAssociator: public GlobalShadowCorrector
{
private:
	int iContour;
	int iFacet;
	int iEdge;
	EdgeSetIterator edge;

	PolyhedronPtr polyhedronTmp;
	double* bufDouble;

	Vector3d v0_projected;
	Vector3d v1_projected;

	Vector3d v0_nearest;
	Vector3d v1_nearest;

	int iSideDistMin0;
	int iSideDistMin1;


	int init();
	int checkVisibility();
	double calculateVisibility();
	int checkAlreadyAdded();
	void projectEdge();
	int checkExtinction();

	int findNearestPoint(Vector3d v_projected, Vector3d& v_nearest,
			double& distMin);

	double calculateArea(Orientation orientation);

	void add(Orientation orientation);
	void findBounds(Orientation orientation, int& iResultBegin,
			int& iResultEnd);

	double calculateWeight();

public:
	GSAssociator(GlobalShadowCorrector* corrector);
	~GSAssociator();

	void preinit();
	void run(int iContourIn, int iFacetIn, EdgeSetIterator edgeIn);

};


#endif /* GSASSOCIATOR_H_ */
