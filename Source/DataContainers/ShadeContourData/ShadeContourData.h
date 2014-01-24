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

#ifndef SHADECONTOURDATA_H_
#define SHADECONTOURDATA_H_

#include "DataContainers/PData/PData.h"

class SContour;

class ShadeContourData : public PData
{
public:
	int numContours;
	SContour* contours;

	ShadeContourData(shared_ptr<Polyhedron> p);
	~ShadeContourData();

	bool fscanDefault(const char* fileNameContours);
	bool fscanDefault(FILE* file);

	void fprint(FILE* file);
	void fprintDefault(FILE* file);

	bool operator ==(const ShadeContourData& contourData) const;
	bool operator !=(const ShadeContourData& contourData) const;

};

typedef std::shared_ptr<ShadeContourData> ShadeContourDataPtr;

#endif /* SHADECONTOURDATA_H_ */
