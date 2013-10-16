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

#ifndef VERTEXINFO_H
#define	VERTEXINFO_H

class VertexInfo
{
public:
	int id;
	int numFacets;
	Vector3d vector;
	int* indFacets;
	shared_ptr<Polyhedron> parentPolyhedron;

public:
	VertexInfo();
	VertexInfo(const int id_orig, const int nf_orig, const Vector3d vector_orig,
			const int* index_orig, shared_ptr<Polyhedron> poly_orig);
	VertexInfo(const int id_orig, const Vector3d vector_orig,
			shared_ptr<Polyhedron> poly_orig);

	VertexInfo& operator =(const VertexInfo& orig);
	~VertexInfo();

	int get_nf();
	void preprocess();

	void find_and_replace_facet(int from, int to);
	void find_and_replace_vertex(int from, int to);

	void fprint_my_format(FILE* file);
	void my_fprint_all(FILE* file);

	int intersection_find_next_facet(Plane iplane, int facet_id);
};

#endif	/* VERTEXINFO_H */

