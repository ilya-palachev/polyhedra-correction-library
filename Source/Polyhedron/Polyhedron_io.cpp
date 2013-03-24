#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "PolyhedraCorrectionLibrary.h"

void Polyhedron::my_fprint(const char* filename) {

	FILE* file;
	file = (FILE*)fopen(filename, "w");
	if (!file) {
		fprintf(stdout, "Polyhedron::fprint_default_0. Error. Cannot open file %s\n",
				  filename);
		return;
	}
	this->my_fprint(file);
	fclose(file);
}

void Polyhedron::my_fprint(FILE* file) {
	int i;
	fprintf(file, "Polyhedron:\n");
	fprintf(file, "numv = %d\nnumf = %d\n", numVertices, numFacets);
	for(i = 0; i < numVertices; ++i)
		fprintf(file, "   vertex[%d] = (%f, %f, %f)\n",
				i, vertices[i].x, vertices[i].y, vertices[i].z);
	for(i = 0; i < numFacets; ++i)
		facets[i].my_fprint_all(file);
	if(vertexInfos != NULL) {
		for(i = 0; i < numVertices; ++i)
			vertexInfos[i].my_fprint_all(file);
	}
}


/////////////////////////////////////
// Format:
//
// numv numf
//
// x_i y_i z_i
//
// nv_j a_j b_j c_j d_j   k_j_1 k_j_2 ... k_j_(nv_j)
//
/////////////////////////////////////

void Polyhedron::fscan_default_0(const char *filename)
{
	int i, j, nv, *index;
	double xx, yy, zz, a, b, c, d;
	Plane plane;

	FILE* file;
	file = (FILE*)fopen(filename, "rt");
	if(!file)
	{
		fprintf(stdout, "Polyhedron::fscan_default_0. Error. Cannot open file %s\n",
				  filename);
		return;
	}

	if(fscanf(file, "%d", &i) != 1 ||
		fscanf(file, "%d", &j) != 1)
	{
		fprintf(stdout, "Polyhedron::fscan_default_0. Error. Invalid file\n");
		fclose(file);
		return;
	}


	numVertices = i;
	numFacets = j;
	vertices = new Vector3d[10 * numFacets];   //MORE MEMORY!!!
	facets = new Facet[10 * numFacets];

	for(i = 0; i < numVertices; ++i)
	{
		if((fscanf(file, "%lf", &xx) != 1) ||
			(fscanf(file, "%lf", &yy) != 1) ||
			(fscanf(file, "%lf", &zz) != 1))
		{
			fprintf(stdout, "Polyhedron::fscan_default_0. Error. Invalid file. v %d\n",
					  i);
			fprintf(stdout, "xx = %f, yy = %f, zz = %f\n", xx, yy, zz);
			fclose(file); return;
		}
		vertices[i].x = xx; vertices[i].y = yy; vertices[i].z = zz;
	}
	for(i = 0; i < numFacets; ++i)
	{
		if(fscanf(file, "%d", &nv) != 1 ||
			nv < 3 ||
			fscanf(file, "%lf", &a) != 1 ||
			fscanf(file, "%lf", &b) != 1 ||
			fscanf(file, "%lf", &c) != 1 ||
			fscanf(file, "%lf", &d) != 1
		)
		{
			fprintf(stdout, "Polyhedron::fscan_default_0. Error. Invalid file. f %d\n",
					  i);
			fclose(file); return;
		}
		plane = Plane(Vector3d(a, b, c), d);
		index = new int[3*nv + 1];
		for(j = 0; j < nv; ++j)
		{
			if(fscanf(file, "%d", index + j) != 1)
			{
				fprintf(stdout, "Polyhedron::fscan_default_0. Error. Invalid file. f%d v%d\n",
						  i, j);
				fclose(file); return;
			}
		}
		facets[i] = Facet(i, nv, plane, index, this, false);
		delete[] index;
	}
	fclose(file);
}

/////////////////////////////////////
// Format:
//
// numv numf
//
// i x_i y_i z_i
//
// j nv_j a_j b_j c_j d_j   k_j_1 k_j_2 ... k_j_(nv_j)
//
/////////////////////////////////////

void Polyhedron::fscan_default_1(const char *filename)
{
	int i, j, nv, *index, ii;
	double xx, yy, zz, a, b, c, d;
	Plane plane;

	FILE* file;
	file = (FILE*)fopen(filename, "rt");
	if(!file)
	{
		fprintf(stdout, "Polyhedron::fscan_default_1. Error. Cannot open file %s\n",
				  filename);
		return;
	}

	if(fscanf(file, "%d", &i) != 1 ||
		fscanf(file, "%d", &j) != 1)
	{
		fprintf(stdout, "Polyhedron::fscan_default_1. Error. Invalid file\n");
		fclose(file); return;
	}

	numVertices = i;
	numFacets = j;
	vertices = new Vector3d[2 * numFacets];   //MORE MEMORY!!!
	facets = new Facet[numFacets];

	for(i = 0; i < numVertices; ++i)
	{
		if(fscanf(file, "%d", &ii) != 1 ||
			fscanf(file, "%lf", &xx) != 1 ||
			fscanf(file, "%lf", &yy) != 1 ||
			fscanf(file, "%lf", &zz) != 1)
		{
			fprintf(stdout, "Polyhedron::fscan_default_1. Error. Invalid file\n");
			fclose(file); return;
		}
		vertices[ii].x = xx; vertices[ii].y = yy; vertices[ii].z = zz;
	}
	for(i = 0; i < numFacets; ++i)
	{
		if(fscanf(file, "%d", &ii) != 1 ||
			fscanf(file, "%d", &nv) != 1 ||
			nv < 3 ||
			fscanf(file, "%lf", &a) != 1 ||
			fscanf(file, "%lf", &b) != 1 ||
			fscanf(file, "%lf", &c) != 1 ||
			fscanf(file, "%lf", &d) != 1
		)
		{
			fprintf(stdout, "Polyhedron::fscan_default_1. Error. Invalid file\n");
			fclose(file); return;
		}
		plane = Plane(Vector3d(a, b, c), d);
		index = new int[3*nv + 1];
		for(j = 0; j < nv; ++j)
		{
			if(fscanf(file, "%d", index + j) != 1)
			{
				fprintf(stdout, "Polyhedron::fscan_default_1. Error. Invalid file\n");
				fclose(file); return;
			}
		}
		facets[ii] = Facet(ii, nv, plane, index, this, false);
		delete[] index;
	}
	fclose(file);
}

/////////////////////////////////////
// Format:
//
// #WPolyhedron
// #version 1
// #dimension
// numv numf
// #vertices
// i x_i y_i z_i
// #facets
// j nv_j a_j b_j c_j d_j   k_j_1 k_j_2 ... k_j_(nv_j)
//
/////////////////////////////////////
//Example: polyhedron-2010-11-25.dat
/////////////////////////////////////

void Polyhedron::fscan_default_1_1(const char *filename)
{
	int i, j, nv, *index, ii;
	double xx, yy, zz, a, b, c, d;
	Plane plane;
	char* tmp;
	FILE* file;

	file = (FILE*)fopen(filename, "rt");
        
        if(!file)
	{
		fprintf(stdout, "Polyhedron::fscan_default_1_1. Error. Cannot open file %s\n",
				  filename);
		return;
	}
	tmp = new char[255];
	for(i = 0; i < 4; ++i)
		if(fscanf(file, "%s", tmp) != 1)
		{
			fprintf(stdout, "Polyhedron::fscan_defualt_1_1. Error. Invalid file\n");
			fclose(file); return;
		}

	if(!file)
	{
		fprintf(stdout, "Polyhedron::fscan_default_1_1. Error. Cannot open file %s\n",
				  filename);
		return;
	}

	if(fscanf(file, "%d", &i) != 1 ||
		fscanf(file, "%d", &j) != 1)
	{
		fprintf(stdout, "Polyhedron::fscan_default_1_1. Error. Invalid file\n");
		fclose(file); return;
	}

	if(fscanf(file, "%s", tmp) != 1)
	{
		fprintf(stdout, "Polyhedron::fscan_defualt_1_1. Error. Invalid file\n");
		fclose(file); return;
	}

	numVertices = i;
	numFacets = j;
	vertices = new Vector3d[10 * numFacets];   //MORE MEMORY!!!
	facets = new Facet[10 * numFacets];

	for(i = 0; i < numVertices; ++i)
	{
		if(fscanf(file, "%d", &ii) != 1 ||
			fscanf(file, "%lf", &xx) != 1 ||
			fscanf(file, "%lf", &yy) != 1 ||
			fscanf(file, "%lf", &zz) != 1)
		{
			fprintf(stdout, "Polyhedron::fscan_default_1_1. Error. Invalid file\n");
			fclose(file); return;
		}
		vertices[ii].x = xx; vertices[ii].y = yy; vertices[ii].z = zz;
	}

	if(fscanf(file, "%s", tmp) != 1)
	{
		fprintf(stdout, "Polyhedron::fscan_defualt_1_1. Error. Invalid file\n");
		fclose(file); return;
	}

	for(i = 0; i < numFacets; ++i)
	{
		if(fscanf(file, "%d", &ii) != 1 ||
			fscanf(file, "%d", &nv) != 1 ||
			nv < 3 ||
			fscanf(file, "%lf", &a) != 1 ||
			fscanf(file, "%lf", &b) != 1 ||
			fscanf(file, "%lf", &c) != 1 ||
			fscanf(file, "%lf", &d) != 1
		)
		{
			fprintf(stdout, "Polyhedron::fscan_default_1_1. Error. Invalid file\n");
			fclose(file); return;
		}
		plane = Plane(Vector3d(a, b, c), d);
		index = new int[3*nv + 1];
		for(j = 0; j < nv; ++j)
		{
			if(fscanf(file, "%d", index + j) != 1)
			{
				fprintf(stdout, "Polyhedron::fscan_default_1_1. Error. Invalid file\n");
				fclose(file); return;
			}
		}
		facets[ii] = Facet(ii, nv, plane, index, this, false);
		delete[] index;
	}
	delete[] tmp;
	fclose(file);
}

/////////////////////////////////////
// Format:
//
// numv numf
//
// x_i y_i z_i
//
// nv_j a_j b_j c_j d_j   k_j_1 k_j_2 ... k_j_(3 * nv_j + 1)
//
// nf_j    k_j_1 k_j_2 ... k_j_(3 * nf_j + 1)
//
/////////////////////////////////////

void Polyhedron::fscan_my_format(const char *filename)
{
	int i, j, nv, nf, *index;
	double xx, yy, zz, a, b, c, d;
	Plane plane;

	FILE* file;
	file = (FILE*)fopen(filename, "rt");
	if(!file)
	{
		fprintf(stdout, "Polyhedron::fscan_my_format. Error. Cannot open file %s\n",
				  filename);
		return;
	}

	if(fscanf(file, "%d", &i) != 1 ||
		fscanf(file, "%d", &j) != 1)
	{
		fprintf(stdout, "Polyhedron::fscan_my_format. Error. Invalid file\n");
		fclose(file); return;
	}

	numVertices = i;
	numFacets = j;
	vertices = new Vector3d[2 * numFacets];   //MORE MEMORY!!!
	facets = new Facet[numFacets];

	for(i = 0; i < numVertices; ++i)
	{
		if(fscanf(file, "%lf", &xx) != 1 ||
			fscanf(file, "%lf", &yy) != 1 ||
			fscanf(file, "%lf", &zz) != 1)
		{
			fprintf(stdout, "Polyhedron::fscan_my_format. Error. Invalid file\n");
			fclose(file); return;
		}
		vertices[i].x = xx; vertices[i].y = yy; vertices[i].z = zz;
	}
	for(i = 0; i < numFacets; ++i)
	{
		if(fscanf(file, "%d", &nv) != 1 ||
			nv < 3 ||
			fscanf(file, "%lf", &a) != 1 ||
			fscanf(file, "%lf", &b) != 1 ||
			fscanf(file, "%lf", &c) != 1 ||
			fscanf(file, "%lf", &d) != 1
		)
		{
			fprintf(stdout, "Polyhedron::fscan_my_format. Error. Invalid file\n");
			fclose(file); return;
		}
		plane = Plane(Vector3d(a, b, c), d);
		index = new int[3*nv + 1];
		for(j = 0; j < 3*nv + 1; ++j)
		{
			if(fscanf(file, "%d", index + j) != 1)
			{
				fprintf(stdout, "Polyhedron::fscan_my_format. Error. Invalid file\n");
				fclose(file); return;
			}
		}
		facets[i] = Facet(i, nv, plane, index, this, false);
		delete[] index;
	}
	for(i = 0; i < numVertices; ++i)
	{
		if(fscanf(file, "%d", &nf) != 1 ||
			nf < 3)
		{
			fprintf(stdout, "Polyhedron::fscan_my_format. Error. Invalid file\n");
			fclose(file); return;
		}
		index = new int[2*nf + 1];
		for(j = 0; j < 2*nf + 1; ++j)
		{
			if(fscanf(file, "%d", index + j) != 1)
			{
				fprintf(stdout, "Polyhedron::fscan_my_format. Error. Invalid file\n");
				fclose(file); return;
			}
		}
		vertexInfos[i] = VertexInfo(i, nf, vertices[i], index, this);
		delete[] index;
	}
	fclose(file);
}

/////////////////////////////////////
// Format:
//
// ply
//      format ascii 1.0
//      comment converted from <former_filename>
//      element vertex <numv>
//      property float x
//      property float y
//      property float z
//      element face <numf>
//      property list uchar int vertex_index
//      end_header
//
// x_i y_i z_i
//
// nv_j k_j_1 k_j_2 ... k_j_(nv_j)
//
/////////////////////////////////////

void Polyhedron::fscan_ply(const char *filename)
{
	int i, j, numv, numf, nv, *index;
	double xx, yy, zz;
	char* tmp;
	Plane plane;

	FILE* file;
	file = (FILE*)fopen(filename, "rt");
	if(!file)
	{
		fprintf(stdout, "Polyhedron::fscan_ply. Error. Cannot open file %s\n",
				  filename);
		return;
	}


	tmp = new char[255];
	for(i = 0; i < 11; ++i)
		if(fscanf(file, "%s", tmp) != 1)
		{
			fprintf(stdout, "Polyhedron::fscan_ply. Error. Invalid file\n");
			fclose(file); return;
		}

	if(fscanf(file, "%d", &nv) != 1)
	{
		fprintf(stdout, "Polyhedron::fscan_ply. Error. Invalid file\n");
		fclose(file); return;
	}
	numv = nv;

	for(i = 0; i < 11; ++i)
		if(fscanf(file, "%s", tmp) != 1)
		{
			fprintf(stdout, "Polyhedron::fscan_ply. Error. Invalid file\n");
			fclose(file); return;
		}

	if(fscanf(file, "%d", &nv) != 1)
	{
		fprintf(stdout, "Polyhedron::fscan_ply. Error. Invalid file\n");
		fclose(file); return;
	}
	numf = nv;

	vertices = new Vector3d[2 * numf];   //MORE MEMORY!!!
	facets = new Facet[numf];

	for(i = 0; i < numv; ++i)
	{
		if(fscanf(file, "%lf", &xx) != 1 ||
			fscanf(file, "%lf", &yy) != 1 ||
			fscanf(file, "%lf", &zz) != 1)
		{
			fprintf(stdout, "Polyhedron::fscan_ply. Error. Invalid file\n");
			fclose(file); return;
		}
		vertices[i].x = xx; vertices[i].y = yy; vertices[i].z = zz;
	}
	for(i = 0; i < numf; ++i)
	{
		if(fscanf(file, "%d", &nv) != 1 ||
			nv < 3)
		{
			fprintf(stdout, "Polyhedron::fscan_ply. Error. Invalid file\n");
			fclose(file); return;
		}
		index = new int[3*nv + 1];
		for(j = 0; j < nv; ++j)
		{
			if(fscanf(file, "%d", index + j) != 1)
			{
				fprintf(stdout, "Polyhedron::fscan_ply. Error. Invalid file\n");
				fclose(file); return;
			}
		}
		plane = Plane(vertices[index[0]], vertices[index[1]], vertices[index[2]]);
		facets[i] = Facet(i, nv, plane, index, this, false);
		delete[] index;
	}
	fclose(file);
	delete[] tmp;
}

void Polyhedron::fprint_default_0(const char *filename)
{
	FILE* file = (FILE*)fopen(filename, "w");
	if(!file)
	{
		fprintf(stdout, "Polyhedron::fprint_default_0. Error. Cannot open file %s\n",
				  filename);
		return;
	}


	fprintf(file, "%d %d\n", numVertices, numFacets);
	for(int i = 0; i < numVertices; ++i)
		fprintf(file, "%.8f %.8f %.8f\n", vertices[i].x, vertices[i].y, vertices[i].z);

	for(int i = 0; i < numFacets; ++i)
		facets[i].fprint_default_0(file);

	fclose(file);
}

void Polyhedron::fprint_default_1(const char *filename)
{
	FILE* file = (FILE*)fopen(filename, "w");
	if(!file)
	{
		fprintf(stdout, "Polyhedron::fprint_default_1. Error. Cannot open file %s\n",
				  filename);
		fclose(file); return;
	}

	fprintf(file, "%d %d\n", numVertices, numFacets);
	for(int i = 0; i < numVertices; ++i)
		fprintf(file, "%d %.8f %.8f %.8f\n", i, vertices[i].x, vertices[i].y, vertices[i].z);

	for(int i = 0; i < numFacets; ++i)
		facets[i].fprint_default_1(file);
	fclose(file);
}

void Polyhedron::fprint_my_format(const char *filename)
{
	FILE* file = (FILE*)fopen(filename, "w");
	if(!file)
	{
		fprintf(stdout, "Polyhedron::fprint_my_format. Error. Cannot open file %s\n",
				  filename);
		fclose(file); return;
	}

	fprintf(file, "%d %d\n", numVertices, numFacets);
	for(int i = 0; i < numVertices; ++i)
		fprintf(file, "%.8f %.8f %.8f\n", vertices[i].x, vertices[i].y, vertices[i].z);

	for(int i = 0; i < numFacets; ++i)
		facets[i].fprint_my_format(file);
	fprintf(file, "\n\n\n");
	for(int i = 0; i < numVertices; ++i)
		vertexInfos[i].fprint_my_format(file);

	fclose(file);
}


void Polyhedron::fprint_ply(const char *filename, const char *comment = NULL)
		//comment must have 1 word
{
	char* comment_w;

	FILE* file = (FILE*)fopen(filename, "w");
	if(!file)
	{
		fprintf(stdout, "Polyhedron::fscan_ply. Error. Cannot open file %s\n",
				  filename);
		return;
	}


	comment_w = new char[255];
	if(!comment) 
            sprintf(comment_w, "unknown");
	else 
            sprintf(comment_w, "%s", comment);

	int numv_new = 0;
	for(int i = 0; i < numFacets; ++i)
		numv_new += facets[i].get_nv();

	fprintf(file, "ply\n");
	fprintf(file, "format ascii 1.0\n");
	fprintf(file, "comment converted from %s\n", comment_w);
	fprintf(file, "element vertex %d\n", numv_new);
	fprintf(file, "property float x\n");
	fprintf(file, "property float y\n");
	fprintf(file, "property float z\n");
	fprintf(file, "property float nx\n");
	fprintf(file, "property float ny\n");
	fprintf(file, "property float nz\n");
	fprintf(file, "element face %d\n", numFacets);
	fprintf(file, "property list uchar uint vertex_indices\n");
	fprintf(file, "end_header\n");

	for(int i = 0; i < numFacets; ++i)
		facets[i].fprint_ply_vertex(file);

	for(int i = 0; i < numFacets; ++i)
		facets[i].fprint_ply_vertex(file);
	fclose(file);
}

void Polyhedron::fprint_ply_scale(
		double scale,
		const char *filename,
		const char *comment)
//		void Polyhedron::fprint_ply_1e16(
//				const char *filename,
//				const char *comment,
//				const int n,
//				const int* col)
		//comment must have 1 word
{
	long int vx, vy, vz;
	char* comment_w;

//	int ncolored = 0;

	FILE* file = (FILE*)fopen(filename, "w");
	if(!file)
	{
		fprintf(stdout, "Polyhedron::fprint_ply_scale. Error. Cannot open file %s\n",
				  filename);
		return;
	}


	comment_w = new char[255];
	if(!comment) sprintf(comment_w, "unknown");
	else sprintf(comment_w, "%s", comment);


//	numv_new = 0;
//	for(i = 0; i < numf; ++i)
//		numv_new += facet[i].nv;

	fprintf(file, "ply\n");
	fprintf(file, "format ascii 1.0\n");
	fprintf(file, "comment converted from %s\n", comment_w);
	fprintf(file, "element vertex %d\n", numVertices);
	fprintf(file, "property float x\n");
	fprintf(file, "property float y\n");
	fprintf(file, "property float z\n");
	fprintf(file, "element face %d\n", numFacets);
	fprintf(file, "property list uchar uint vertex_indices\n");
	fprintf(file, "property uchar red\n");
	fprintf(file, "property uchar green\n");
	fprintf(file, "property uchar blue\n");
	fprintf(file, "end_header\n");

	for(int i = 0; i < numVertices; ++i)
	{
		vx = (long int)scale*vertices[i].x;
		vy = (long int)scale*vertices[i].y;
		vz = (long int)scale*vertices[i].z;
		fprintf(file, "%ld %ld %ldb\n",
				  vx, vy, vz);
	}

	for(int i = 0; i < numFacets; ++i)
		facets[i].fprint_ply_scale(file);
	fclose(file);
}

