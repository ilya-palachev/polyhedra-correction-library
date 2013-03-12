/* 
 * File:   FutureFacet.h
 * Author: ilya
 *
 * Created on 12 Ноябрь 2012 г., 11:32
 */

#ifndef FUTUREFACET_H
#define	FUTUREFACET_H

#include "PolyhedraCorrectionLibrary.h"

class FutureFacet {
private:
	int id;
	int len;
	int nv;
	int* edge0;
	int* edge1;
	int* src_facet;
	int* id_v_new;
public:
	//FutureFacet.cpp
	FutureFacet();
	~FutureFacet();
	FutureFacet(int nv_orig);
	FutureFacet(const FutureFacet& orig);
	FutureFacet& operator = (const FutureFacet& orig);
	FutureFacet& operator += (const FutureFacet& v);
	void free();
	

	int get_nv();

	void set_id(int val);

	void add_edge(int v0, int v1, int src_f);
	void get_edge(int pos, int& v0, int& v1, int& src_f, int& id_v);
	
	//FutureFacet_io.cpp
	void my_fprint(FILE* file);
	
	//FutureFacet_intersection.cpp
	void generate_facet(
		Facet& facet,
		int fid,
		Plane& iplane,
		int numv,
		EdgeSet* es);
};

#endif	/* FUTUREFACET_H */

