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

#ifndef EDGEREDUCER_H_
#define EDGEREDUCER_H_

#include <set>
#include <queue>

#include "Correctors/PCorrector/PCorrector.h"
#include "DataContainers/EdgeData/EdgeData.h"

class EdgesWorkingSets
{
public:
	std::set<pair<int, int>> edgesErased;
	std::set<pair<int, int>> edgesAdded;
	std::set<pair<int, int>> edgesEdited;
};

class EdgeReducer: public PCorrector
{
private:
	EdgeDataPtr edgeData;
	EdgeSetIterator edge;

	int iVertexReduced;
	int iVertexStayed;
	VertexInfo* vertexInfoReduced;
	VertexInfo* vertexInfoStayed;
	std::set<int> facetsUpdated;

	EdgesWorkingSets edgesWS;

	void init();
	bool updateFacets();
	void cutDegeneratedFacet(int iFacet);
	bool rePreprocessFacets();
	bool updateEdges();
	bool updateVertexInfos();
	void cutDegeneratedVertex(int iVertex, queue<int>& facetsQueue);

	bool verifyEdgeData();

public:
	EdgeReducer();
	EdgeReducer(PolyhedronPtr p);
	~EdgeReducer();

	bool run(EdgeSetIterator _edge, EdgeDataPtr _edgeData,
			EdgesWorkingSets& _edgesWS);
};

#endif /* EDGEREDUCER_H_ */
