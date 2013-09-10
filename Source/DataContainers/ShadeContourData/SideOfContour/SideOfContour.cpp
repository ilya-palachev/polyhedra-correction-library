/* 
 * File:   SideOfContour.cpp
 * Author: ilya
 * 
 * Created on 13 Ноябрь 2012 г., 10:01
 */

#include "PolyhedraCorrectionLibrary.h"

SideOfContour::SideOfContour() :
				confidence(0),
				type(EEdgeUnknown),
				A1(Vector3d(0, 0, 0)),
				A2(Vector3d(0, 0, 0))
{
	DEBUG_START;
	DEBUG_END;
}

SideOfContour::SideOfContour(const SideOfContour& orig) :
				confidence(orig.confidence),
				type(orig.type),
				A1(orig.A1),
				A2(orig.A2)
{
	DEBUG_START;
	DEBUG_END;
}

SideOfContour::~SideOfContour()
{
	DEBUG_START;
	DEBUG_END;
}

bool SideOfContour::operator ==(const SideOfContour& side) const
{
	DEBUG_START;

	if (fabs(confidence - side.confidence) > EPS_MIN_DOUBLE)
	{
		DEBUG_END;
		return false;
	}

	if (type != side.type)
	{
		DEBUG_END;
		return false;
	}

	if (A1 != side.A1)
	{
		DEBUG_END;
		return false;
	}

	if (A2 != side.A2)
	{
		DEBUG_END;
		return false;
	}

	DEBUG_END;
	return true;
}

bool SideOfContour::operator !=(const SideOfContour& side) const
{
	DEBUG_START;
	bool returnValue = !(*this == side);
	DEBUG_END;
	return returnValue;
}
