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
}

SideOfContour::SideOfContour(const SideOfContour& orig) :
				confidence(orig.confidence),
				type(orig.type),
				A1(orig.A1),
				A2(orig.A2)
{
}

SideOfContour::~SideOfContour()
{
}

