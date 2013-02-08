/* 
 * File:   VertexInfo.h
 * Author: ilya
 *
 * Created on 12 Ноябрь 2012 г., 11:24
 */


#ifndef VERTEXINFO_H
#define	VERTEXINFO_H

#include "Polyhedron.h"
#include "Vector3d.h"

class VertexInfo
{
public:
	int id;
	int nf;
	Vector3d vector;
	int* index;
	Polyhedron* poly;

public:
	VertexInfo();
	VertexInfo(
			const int id_orig,
			const int nf_orig,
			const Vector3d vector_orig,
			const int* index_orig,
			Polyhedron* poly_orig);
	VertexInfo(
		const int id_orig,
		const Vector3d vector_orig,
		Polyhedron* poly_orig);

	VertexInfo& operator = (const VertexInfo& orig);
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

