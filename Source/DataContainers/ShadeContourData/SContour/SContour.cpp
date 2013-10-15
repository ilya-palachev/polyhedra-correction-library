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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PolyhedraCorrectionLibrary.h"

SContour::SContour() :
				id(-1),
				ns(0),
				plane(Plane(Vector3d(0, 0, 0), 0)),
				poly(NULL),
				sides(NULL)
{
	DEBUG_START;
	DEBUG_END;
}

SContour::SContour(const SContour& orig) :
				id(orig.id),
				ns(orig.ns),
				plane(orig.plane),
				poly(orig.poly),
				sides()
{
	DEBUG_START;
	sides = new SideOfContour[ns + 1];
	for (int i = 0; i < ns; ++i)
	{
		sides[i] = orig.sides[i];
	}
	DEBUG_END;
}

SContour::~SContour()
{
	DEBUG_START;

	DEBUG_PRINT("Attention! shade contour %d is being deleted now...\n", this->id);
	DEBUG_PRINT("polyhedron use count: %ld", poly.use_count());

	if (sides != NULL)
	{
		delete[] sides;
		sides = NULL;
	}
	DEBUG_END;
}

SContour& SContour::operator =(const SContour& scontour)
{
	DEBUG_START;
	id = scontour.id;
	ns = scontour.ns;
	plane = scontour.plane;
	poly = scontour.poly;

	if (sides)
	{
		delete[] sides;
		sides = NULL;
	}

	sides = new SideOfContour[ns];

	for (int iSide = 0; iSide < ns; ++iSide)
	{
		sides[iSide] = scontour.sides[iSide];
	}

	DEBUG_END;
	return *this;
}

bool SContour::operator ==(const SContour& scontour) const
{
	DEBUG_START;

	if (id != scontour.id)
	{
		DEBUG_END;
		return false;
	}

	if (ns != scontour.ns)
	{
		DEBUG_END;
		return false;
	}

	if (plane != scontour.plane)
	{
		DEBUG_END;
		return false;
	}

	if (poly != scontour.poly)
	{
		DEBUG_END;
		return false;
	}

	for (int iSide = 0; iSide < ns; ++iSide)
	{
		if (sides[iSide] != scontour.sides[iSide])
		{
			DEBUG_END;
			return false;
		}
	}

	DEBUG_END;
	return true;
}

bool SContour::operator !=(const SContour& scontour) const
{
	DEBUG_START;
	bool returnValue = !(*this == scontour);
	DEBUG_END;
	return returnValue;
}
