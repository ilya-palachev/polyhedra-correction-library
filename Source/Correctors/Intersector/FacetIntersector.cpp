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

#include <cstdlib>
#include <cmath>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Correctors/Intersector/FacetIntersector.h"
#include "Correctors/Intersector/EdgeList/EdgeList.h"

FacetIntersector::FacetIntersector() :
		Intersector(),
		facet(NULL)
{
	DEBUG_START;
	DEBUG_END;
}

FacetIntersector::FacetIntersector(shared_ptr<Polyhedron> p, Facet* f) :
		Intersector(p),
		facet(f)
{
	DEBUG_START;
	DEBUG_END;
}

FacetIntersector::FacetIntersector(Intersector* i, Facet* f) :
		Intersector(*i),
		facet(f)
{
	DEBUG_START;
	DEBUG_END;
}

FacetIntersector::~FacetIntersector()
{
	DEBUG_START;
	DEBUG_END;
}

int FacetIntersector::signum(int i, Plane plane)
{
	DEBUG_START;
	if (auto polyhedron = facet->parentPolyhedron.lock())
	{
		DEBUG_END;
		return polyhedron->signum(polyhedron->vertices[facet->indVertices[i]],
			plane);
	}
	else
	{
		ERROR_PRINT("parentPolyhedron expired");
		ASSERT(0 && "parentPolyhedron expired");
		DEBUG_END;
		return -RAND_MAX;
	}
}

bool FacetIntersector::run(Plane iplane, FutureFacet& ff, int& n_components)
{
	DEBUG_START;

	int i;
	int nintrsct;
	int i0, i1;
	int v0_first, v1_first;
	int v0, v1;
	int next_f, next_d;
	int i_curr, i_next, i_prev;
	int sign_curr = 0, sign_next, sign_prev;
	int sign0;
	int i_step;

	DEBUG_PRINT("**************Пересечение грани %d*************", facet->id);

	double err;
	err = qmod(facet->plane.norm - iplane.norm)
			+ (facet->plane.dist - iplane.dist) * (facet->plane.dist - iplane.dist);
	if (fabs(err) > 1e-16)
	{
		err = qmod(facet->plane.norm + iplane.norm)
				+ (facet->plane.dist + iplane.dist) * (facet->plane.dist + iplane.dist);
	}
	if (fabs(err) < 1e-16)
	{
		DEBUG_END;
		return false;
//        return true; //2012-03-10
	}
	if (facet->id == -1)
	{
		DEBUG_END;
		return false;
	}

	EdgeList* el = &(edgeLists[facet->id]);
	FutureFacet curr_component(2 * polyhedron->numVertices);

	el->null_isUsed();
	//	el->my_fprint(stdout);

	n_components = 0;
	nintrsct = el->get_num();
	if (nintrsct == 0)
	{
		n_components = 0;
		for (i = 0; i < polyhedron->numVertices; ++i)
		{
			sign_curr = signum(i, iplane);
			if (sign_curr != 0)
				break;
		}
		if (sign_curr == 1)
		{
			DEBUG_END;
			return true;
		}
		else
		{
			DEBUG_END;
			return false;
		}
	}

	while (nintrsct > 0)
	{
		DEBUG_PRINT("Поиск компоненты %d", n_components);
		el->get_first_edge(v0, v1);
		DEBUG_PRINT("\tПервое ребро : %d %d", v0, v1);
		v0_first = v0;
		v1_first = v1;
		--nintrsct;

		do
		{
			next_d = 1;
			do
			{
				curr_component.add_edge(v0, v1, facet->id);
				DEBUG_PRINT("\tДобавлено ребро : %d %d", v0, v1);
				next_f = facet->id; //2012-03-10
				el->get_next_edge(iplane, v0, v1, i0, i1, next_f, next_d);
				DEBUG_PRINT("\t\t За ним следует ребро : %d %d", v0, v1);
				--nintrsct;
				if (i0 != i1)
					break;
				i_next = (i0 + 1) % polyhedron->numVertices;
				i_prev = (polyhedron->numVertices + i0 - 1) % polyhedron->numVertices;
				sign_next = signum(i_next, iplane);
				sign_prev = signum(i_prev, iplane);
			} while (sign_prev == 0 && sign_next == 0);
			curr_component.add_edge(v0, v1, facet->id);
			DEBUG_PRINT("\tДобавлено ребро : %d %d", v0, v1);

			if (i0 == i1)
			{
				i_next = (i0 + 1) % polyhedron->numVertices;
				i_prev = (polyhedron->numVertices + i0 - 1) % polyhedron->numVertices;
				sign_next = signum(i_next, iplane);
				sign_prev = signum(i_prev, iplane);
				//Утверждение. sign_next != 0 || sign_prev != 0
				if (sign_next <= 0 && sign_prev <= 0)
				{
					// Построение компоненты прекращается
					v0 = v0_first;
					v1 = v1_first;
					curr_component.free();
					continue;
				}
				//Утверждение. sign_next == 1 || sign_prev == 1
				//Утверждение. sign_next == 1 && sign_prev == 1 не может быть
				i_step = sign_next == 1 ? 1 : -1;
				i_curr = (i0 + i_step + polyhedron->numVertices) % polyhedron->numVertices;
				sign_curr = signum(i_curr, iplane);
			}
			else
			{
				sign0 = signum(i0, iplane);
				//Утверждение. sign0 == 1 || sign1 == 1
				if (sign0 == 1)
				{
					i_curr = i0;
					sign_curr = sign0;
					i_step = (i1 + 1) % polyhedron->numVertices == i0 ? 1 : -1;
				}
				else
				{
					i_curr = i1;
					sign_curr = sign0;
					i_step = (i0 + 1) % polyhedron->numVertices == i1 ? 1 : -1;
				}
			}

			do
			{
				curr_component.add_edge(facet->indVertices[i_curr],
						facet->indVertices[i_curr], facet->id);
				DEBUG_PRINT(
						"\tДобавлено ребро : %d %d",
						facet->indVertices[i_curr], facet->indVertices[i_curr]);
				i_curr = (i_curr + i_step + polyhedron->numVertices) % polyhedron->numVertices;
				sign_curr = signum(i_curr, iplane);
			} while (sign_curr == 1);
			//Утверждение. sign_curr == 0 || sign_curr == -1
			if (sign_curr == 0)
				v0 = v1 = facet->indVertices[i_curr];
			else
			{
				v0 = facet->indVertices[(i_curr - i_step + polyhedron->numVertices) % polyhedron->numVertices];
				v1 = facet->indVertices[i_curr];
				if (v0 > v1)
				{
					int tmp = v0;
					v0 = v1;
					v1 = tmp;
				}
			}
		} while (!(v0 == v0_first && v1 == v1_first));

		if (curr_component.get_nv() > 0)
		{
			DEBUG_PRINT("Facet %d. Component %d : ", facet->id, n_components);
			curr_component.my_fprint(stdout);
			curr_component.add_edge(-1, -1, facet->id);
			ff += curr_component;
			curr_component.free();
			++n_components;
		}
	}
	el->null_isUsed();
	DEBUG_END;
	return false;

}
