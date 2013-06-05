/*
 * EdgeData.cpp
 *
 *  Created on: 17.05.2013
 *      Author: iliya
 */

#include "PolyhedraCorrectionLibrary.h"

EdgeData::EdgeData() :
				edges(NULL),
				numEdges(0)
{
}

EdgeData::~EdgeData()
{
	if (edges != NULL)
	{
		delete[] edges;
		edges = NULL;
	}
}

