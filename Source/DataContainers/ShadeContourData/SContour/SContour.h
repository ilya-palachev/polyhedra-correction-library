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

#ifndef SCONTOUR_H
#define	SCONTOUR_H

class SContour
{
public:
	int id; // Indetifier

	int ns; // Number of sides

	Plane plane; // Plane of contour. Usually it includes (0, 0, 0),
				 // so in its equation a*x + b*y + c*z + d = 0
				 // coefficient d = 0

	shared_ptr<Polyhedron> poly; // Pointer to parent polyhedon which given contour
					  // is the part of.

	SideOfContour* sides; // Array of contour sides

	// SContour.cpp :

	SContour();
	SContour(const SContour& orig);
	~SContour();

	SContour& operator =(const SContour& scontour);

	bool operator ==(const SContour& scontour) const;
	bool operator !=(const SContour& scontour) const;

	// SContour_io.cpp :

	void my_fprint(FILE* file);
};

#endif	/* SCONTOUR_H */

