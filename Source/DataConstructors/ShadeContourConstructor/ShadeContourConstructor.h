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

#ifndef SHADECONTOURCONSTRUCTOR_H_
#define SHADECONTOURCONSTRUCTOR_H_

/*
 *
 */
class ShadeContourConstructor: public PDataConstructor
{
private:
	shared_ptr<ShadeContourData> data;
	bool* bufferBool;
	int* bufferInt0;
	int* bufferInt1;

	EdgeDataPtr edgeData;

	void createContour(int idOfContour, Plane planeOfProjection,
			SContour* outputContour);
	bool edgeIsVisibleOnPlane(Edge edge, Plane planeOfProjection);
	bool collinearVisibility(int v0processed, int v1processed,
			Plane planeOfProjection, int ifacet);

public:
	ShadeContourConstructor(shared_ptr<Polyhedron> p,
			shared_ptr<ShadeContourData> d);
	~ShadeContourConstructor();
	void run(int numContoursNeeded, double firstAngle);
};

#endif /* SHADECONTOURCONSTRUCTOR_H_ */
