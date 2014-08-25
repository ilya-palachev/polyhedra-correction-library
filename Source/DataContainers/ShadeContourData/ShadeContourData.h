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

/**
 * @file ShadeContourData.h
 * @brief Declaration of class that contains array of shadow contours.
 */

#ifndef SHADECONTOURDATA_H_
#define SHADECONTOURDATA_H_

#include "DataContainers/PData/PData.h"

/* Forward declaration. */
class SContour;

/**
 * Shared pointer to the shadow contour data.
 */
typedef std::shared_ptr<ShadeContourData> ShadeContourDataPtr;

/**
 * Container of shadow contour data used for polyhedral reconstruction.
 * Usually this data is obtained from experimental measurements or from
 * synthetic generation from simple models.
 */
class ShadeContourData : public PData
{
public:
	/**
	 * The number if contours.
	 */
	int numContours;

	/**
	 * The array that contains shadow contours.
	 */
	SContour* contours;

	/**
	 * Constructor that does nothing but setting parent polyhedron to the
	 * given reference.
	 *
	 * @param p	The reference to the parent polyhedron
	 *
	 * TODO: shade contour data can be not associated with any polyhedron!
	 */
	ShadeContourData(shared_ptr<Polyhedron> p);

	/**
	 * Copy constructor from shared pointer
	 *
	 * @param data	The data to be copied
	 */
	ShadeContourData(const ShadeContourDataPtr data);

	/**
	 * Copy constructor from reference
	 *
	 * @param data	The data to be copied
	 */
	ShadeContourData(const ShadeContourData& data);

	/**
	 * Destructor for the freeing of data.
	 */
	~ShadeContourData();

	/**
	 * Scans shadow contour data from the file specified by its name.
	 *
	 * @param fileNameContours	The name of file with shadow contour data to be
	 * read in.
	 */
	bool fscanDefault(const char* fileNameContours);

	/**
	 * Scans shadow contour data from the file descriptor
	 *
	 * @param file	File descriptor of file with shadow contour data to be read
	 * in.
	 *
	 * TODO: Not implemented yet!
	 */
	bool fscanDefault(FILE* file);

	/**
	 * Dumps internal structures specifying shadow contour data to the given
	 * file descriptor.
	 *
	 * @param file	File descriptor to be written to.
	 */
	void fprint(FILE* file);

	/**
	 * Writes shadow contour data to the file in standard format.
	 *
	 * @param file	File descriptor to be written to.
	 */
	void fprintDefault(FILE* file);

	/**
	 * Overloaded operator "==" for comparing of shadow contour data.
	 *
	 * @param contourData	Shadow contour data to be compared with.
	 */
	bool operator ==(const ShadeContourData& contourData) const;

	/**
	 * Overloaded operator "!=" for comparing of shadow contour data.
	 *
	 * @param contourData	Shadow contour data to be compared with.
	 */
	bool operator !=(const ShadeContourData& contourData) const;

};

#endif /* SHADECONTOURDATA_H_ */
