/*
 * Copyright (c) 2009-2016 Ilya Palachev <iliyapalachev@gmail.com>
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

#include <iostream>
#include <fstream>

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "DefaultScaner.h"
#include "Polyhedron/Polyhedron.h"
#include "Polyhedron/Facet/Facet.h"
#include "Polyhedron/VertexInfo/VertexInfo.h"
#include "DataContainers/EdgeData/EdgeData.h"

void Polyhedron::my_fprint(const char *filename)
{
	DEBUG_START;

	FILE *file;
	file = (FILE *)fopen(filename, "w");
	if (!file)
	{
		ERROR_PRINT("Error. Cannot open file %s\n", filename);
		DEBUG_END;
		return;
	}
	this->my_fprint(file);
	fclose(file);
	DEBUG_END;
}

void Polyhedron::my_fprint(FILE *file)
{
	DEBUG_START;
	REGULAR_PRINT(file, "Polyhedron:\n");
	REGULAR_PRINT(file, "numv = %d\nnumf = %d\n", numVertices, numFacets);
	for (int i = 0; i < numVertices; ++i)
	{
		REGULAR_PRINT(file, "   vertex[%d] = (%f, %f, %f)\n", i, vertices[i].x, vertices[i].y, vertices[i].z);
	}
	for (int i = 0; i < numFacets; ++i)
	{
		facets[i].my_fprint_all(file, *this);
	}
	for (size_t i = 0; i < vertexInfos.size(); ++i)
	{
		vertexInfos[i].my_fprint_all(file);
	}
	DEBUG_END;
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
	DEBUG_START;
	int i, j, nv;
	double xx, yy, zz, a, b, c, d;
	Plane plane;

	FILE *file;
	file = (FILE *)fopen(filename, "rt");
	if (!file)
	{
		ERROR_PRINT("Error. Cannot open file %s\n", filename);
		DEBUG_END;
		return;
	}

	if (fscanf(file, "%d", &i) != 1 || fscanf(file, "%d", &j) != 1)
	{
		ERROR_PRINT("Error. Invalid file\n");
		fclose(file);
		DEBUG_END;
		return;
	}

	numVertices = i;
	numFacets = j;
	vertices.reserve(10 * numFacets); // MORE MEMORY!!!
	facets.reserve(10 * numFacets);

	for (i = 0; i < numVertices; ++i)
	{
		if ((fscanf(file, "%lf", &xx) != 1) || (fscanf(file, "%lf", &yy) != 1) || (fscanf(file, "%lf", &zz) != 1))
		{
			ERROR_PRINT("Error. Invalid file. v %d\n", i);
			DEBUG_PRINT("xx = %f, yy = %f, zz = %f\n", xx, yy, zz);
			fclose(file);
			DEBUG_END;
			return;
		}
		vertices.emplace_back(xx, yy, zz);
	}
	for (i = 0; i < numFacets; ++i)
	{
		if (fscanf(file, "%d", &nv) != 1 || nv < 3 || fscanf(file, "%lf", &a) != 1 || fscanf(file, "%lf", &b) != 1 ||
			fscanf(file, "%lf", &c) != 1 || fscanf(file, "%lf", &d) != 1)
		{
			ERROR_PRINT("Error. Invalid file. f %d\n", i);
			fclose(file);
			DEBUG_END;
			return;
		}
		plane = Plane(Vector3d(a, b, c), d);
		std::vector<int> index(3 * nv + 1);
		for (j = 0; j < nv; ++j)
		{
			if (fscanf(file, "%d", &index[j]) != 1)
			{
				ERROR_PRINT("Error. Invalid file. f%d v%d\n", i, j);
				fclose(file);
				DEBUG_END;
				return;
			}
		}
		facets.emplace_back(i, nv, plane, index, false);
	}
	fclose(file);
	DEBUG_END;
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
	DEBUG_START;
	int i, j, nv, ii;
	double xx, yy, zz, a, b, c, d;
	Plane plane;

	FILE *file;
	file = (FILE *)fopen(filename, "rt");
	if (!file)
	{
		ERROR_PRINT("Error. Cannot open file %s\n", filename);
		DEBUG_END;
		return;
	}

	if (fscanf(file, "%d", &i) != 1 || fscanf(file, "%d", &j) != 1)
	{
		ERROR_PRINT("Error. Invalid file\n");
		fclose(file);
		DEBUG_END;
		return;
	}

	numVertices = i;
	numFacets = j;
	vertices.reserve(2 * numFacets); // MORE MEMORY!!!
	facets.reserve(numFacets);

	for (i = 0; i < numVertices; ++i)
	{
		if (fscanf(file, "%d", &ii) != 1 || fscanf(file, "%lf", &xx) != 1 || fscanf(file, "%lf", &yy) != 1 ||
			fscanf(file, "%lf", &zz) != 1)
		{
			ERROR_PRINT("Error. Invalid file\n");
			fclose(file);
			DEBUG_END;
			return;
		}
		vertices.emplace_back(xx, yy, zz);
	}
	for (i = 0; i < numFacets; ++i)
	{
		if (fscanf(file, "%d", &ii) != 1 || fscanf(file, "%d", &nv) != 1 || nv < 3 || fscanf(file, "%lf", &a) != 1 ||
			fscanf(file, "%lf", &b) != 1 || fscanf(file, "%lf", &c) != 1 || fscanf(file, "%lf", &d) != 1)
		{
			ERROR_PRINT("Error. Invalid file\n");
			fclose(file);
			DEBUG_END;
			return;
		}
		plane = Plane(Vector3d(a, b, c), d);
		std::vector<int> index(3 * nv + 1);
		for (j = 0; j < nv; ++j)
		{
			if (fscanf(file, "%d", &index[j]) != 1)
			{
				ERROR_PRINT("Error. Invalid file\n");
				fclose(file);
				DEBUG_END;
				return;
			}
		}
		facets.emplace_back(ii, nv, plane, index, false);
	}
	fclose(file);
	DEBUG_END;
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
// Example: polyhedron-2010-11-25.dat
/////////////////////////////////////

void Polyhedron::fscan_default_1_1(const char *filename)
{
	DEBUG_START;
	int i, j, nv, ii;
	double xx, yy, zz, a, b, c, d;
	Plane plane;
	char *tmp;
	FILE *file;

	file = (FILE *)fopen(filename, "rt");

	if (!file)
	{
		ERROR_PRINT("Error. Cannot open file %s\n", filename);
		DEBUG_END;
		return;
	}
	tmp = new char[255];
	for (i = 0; i < 4; ++i)
		if (fscanf(file, "%s", tmp) != 1)
		{
			ERROR_PRINT("Error. Invalid file\n");
			fclose(file);
			delete[] tmp;
			DEBUG_END;
			return;
		}

	if (fscanf(file, "%d", &i) != 1 || fscanf(file, "%d", &j) != 1)
	{
		ERROR_PRINT("Error. Invalid file\n");
		fclose(file);
		delete[] tmp;
		DEBUG_END;
		return;
	}

	if (fscanf(file, "%s", tmp) != 1)
	{
		ERROR_PRINT("Polyhedron::fscan_defualt_1_1. Error. Invalid file\n");
		fclose(file);
		delete[] tmp;
		DEBUG_END;
		return;
	}

	numVertices = i;
	numFacets = j;
	vertices.reserve(10 * numFacets); // MORE MEMORY!!!
	facets.reserve(10 * numFacets);

	for (i = 0; i < numVertices; ++i)
	{
		if (fscanf(file, "%d", &ii) != 1 || fscanf(file, "%lf", &xx) != 1 || fscanf(file, "%lf", &yy) != 1 ||
			fscanf(file, "%lf", &zz) != 1)
		{
			ERROR_PRINT("Error. Invalid file\n");
			fclose(file);
			delete[] tmp;
			DEBUG_END;
			return;
		}
		vertices.emplace_back(xx, yy, zz);
	}

	if (fscanf(file, "%s", tmp) != 1)
	{
		ERROR_PRINT("Error. Invalid file\n");
		fclose(file);
		delete[] tmp;
		DEBUG_END;
		return;
	}

	for (i = 0; i < numFacets; ++i)
	{
		if (fscanf(file, "%d", &ii) != 1 || fscanf(file, "%d", &nv) != 1 || nv < 3 || fscanf(file, "%lf", &a) != 1 ||
			fscanf(file, "%lf", &b) != 1 || fscanf(file, "%lf", &c) != 1 || fscanf(file, "%lf", &d) != 1)
		{
			ERROR_PRINT("Error. Invalid file\n");
			fclose(file);
			delete[] tmp;
			DEBUG_END;
			return;
		}
		plane = Plane(Vector3d(a, b, c), d);
		std::vector<int> index(3 * nv + 1);
		for (j = 0; j < nv; ++j)
		{
			if (fscanf(file, "%d", &index[j]) != 1)
			{
				ERROR_PRINT("Error. Invalid file\n");
				fclose(file);
				delete[] tmp;
				DEBUG_END;
				return;
			}
		}
		facets.emplace_back(ii, nv, plane, index, false);
	}
	fclose(file);
	delete[] tmp;
	DEBUG_END;
}

/////////////////////////////////////
// Format:
//
// # WPolyhedron::Dump
// # num_vertices   num_facets   num_edgeData
//    446  225 1338
// # vertices:  ind  x y z
// ...
// i x_i y_i z_i
// ...
// # facets:  num_of_sides   plane_coeff
// #          vertex_inds
// ...
// j nv_j a_j b_j c_j d_j   k_j_1 k_j_2 ... k_j_(nv_j) (k_j_1)
// ...
// # edgeData:  vertex1 vetrex2 facet ind_in_list
// ...
// v1_k v2_k f_k f_ind_k
// ...
//
/////////////////////////////////////
// Example: MSS_MARQ_G_VS2_0.54_2012_09_18/polyhedron.dat
/////////////////////////////////////

bool Polyhedron::fscan_default_1_2(const char *path)
{
	DefaultScaner scaner(path);
	if (!scaner.open())
	{
		ERROR_PRINT("Failed to open file %s", path);
		return false;
	}

	char *line = scaner.getline();
	numVertices = 0;
	numFacets = 0;
	int numEdges = 0;
	if (!line || sscanf(line, "%d %d %d", &numVertices, &numFacets, &numEdges) != 3)
	{
		ERROR_PRINT("Failed to read header");
		return false;
	}

	vertices.reserve(numVertices);
	facets.reserve(numFacets);

	for (int iVertex = 0; iVertex < numVertices; ++iVertex)
	{
		int iVertexScanned = 0;
		Vector3d *v = &vertices[iVertex];
		char *line = scaner.getline();
		if (!line || sscanf(line, "%d %lf %lf %lf", &iVertexScanned, &v->x, &v->y, &v->z) != 4 ||
			iVertexScanned != iVertex)
		{
			ERROR_PRINT("Wrong format, in vertex #%d", iVertex);
			return false;
		}
	}

	for (int iFacet = 0; iFacet < numFacets; ++iFacet)
	{
		char *line = scaner.getline();
		int iFacetScanned = 0;
		int numIncVertices = 0;
		Plane plane;
		if (!line ||
			sscanf(line, "%d %d %lf %lf %lf %lf", &iFacetScanned, &numIncVertices, &plane.norm.x, &plane.norm.y,
				   &plane.norm.z, &plane.dist) != 6 ||
			iFacetScanned != iFacet)
		{
			ERROR_PRINT("Wrong format, in facet #%d", iFacet);
			return false;
		}
		std::vector<int> index(3 * (numIncVertices + 1));
		line = scaner.getline();
		for (int i = 0; i < numIncVertices; ++i)
		{
			char *prevLine = line;
			long iVertex = strtol(line, &line, 10);
			if (prevLine == line)
			{
				ERROR_PRINT("Wrong format, in facet #%d, "
							"index #%d",
							iFacet, i);
				return false;
			}
			index[i] = int(iVertex);
		}
		facets.emplace_back(iFacet, numIncVertices, plane, index, false);
	}
	return true;
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
	DEBUG_START;
	int i, j, nv, nf;
	double xx, yy, zz, a, b, c, d;
	Plane plane;

	FILE *file;
	file = (FILE *)fopen(filename, "rt");
	if (!file)
	{
		ERROR_PRINT("Error. Cannot open file %s\n", filename);
		DEBUG_END;
		return;
	}

	if (fscanf(file, "%d", &i) != 1 || fscanf(file, "%d", &j) != 1)
	{
		ERROR_PRINT("Error. Invalid file\n");
		fclose(file);
		DEBUG_END;
		return;
	}

	numVertices = i;
	numFacets = j;
	vertices.reserve(2 * numFacets); // MORE MEMORY!!!
	facets.reserve(numFacets);

	for (i = 0; i < numVertices; ++i)
	{
		if (fscanf(file, "%lf", &xx) != 1 || fscanf(file, "%lf", &yy) != 1 || fscanf(file, "%lf", &zz) != 1)
		{
			ERROR_PRINT("Error. Invalid file\n");
			fclose(file);
			DEBUG_END;
			return;
		}
		vertices.emplace_back(xx, yy, zz);
	}
	for (i = 0; i < numFacets; ++i)
	{
		if (fscanf(file, "%d", &nv) != 1 || nv < 3 || fscanf(file, "%lf", &a) != 1 || fscanf(file, "%lf", &b) != 1 ||
			fscanf(file, "%lf", &c) != 1 || fscanf(file, "%lf", &d) != 1)
		{
			ERROR_PRINT("Error. Invalid file\n");
			fclose(file);
			DEBUG_END;
			return;
		}
		plane = Plane(Vector3d(a, b, c), d);
		std::vector<int> index(3 * nv + 1);
		for (j = 0; j < 3 * nv + 1; ++j)
		{
			if (fscanf(file, "%d", &index[j]) != 1)
			{
				ERROR_PRINT("Error. Invalid file\n");
				fclose(file);
				DEBUG_END;
				return;
			}
		}
		facets.emplace_back(i, nv, plane, index, false);
	}
	for (i = 0; i < numVertices; ++i)
	{
		if (fscanf(file, "%d", &nf) != 1 || nf < 3)
		{
			ERROR_PRINT("Error. Invalid file\n");
			fclose(file);
			DEBUG_END;
			return;
		}
		std::vector<int> index(2 * nf + 1);
		for (j = 0; j < 2 * nf + 1; ++j)
		{
			if (fscanf(file, "%d", &index[j]) != 1)
			{
				ERROR_PRINT("Error. Invalid file\n");
				fclose(file);
				DEBUG_END;
				return;
			}
		}
		vertexInfos.emplace_back(i, nf, vertices[i], index);
	}
	fclose(file);
	DEBUG_END;
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
	DEBUG_START;
	std::cout << "Reading PLY file " << filename << std::endl;
	std::ifstream infile(filename);

	std::string line;
	std::getline(infile, line);
	ASSERT(line == "ply" && "PLY file should start from \"ply\" line");

	std::getline(infile, line);
	ASSERT(line == "format ascii 1.0" && "Second line should be \"format ascii "
										 "1.0\", we support only this format");

	while (std::getline(infile, line))
	{
		std::string keyword;
		std::istringstream stream(line);
		stream >> keyword;
		if (keyword == "comment")
			continue;

		if (keyword == "element")
		{
			std::string item;
			int number = 0;
			stream >> item >> number;
			ASSERT(number > 0);
			if (item == "vertex")
				numVertices = number;
			else if (item == "face")
				numFacets = number;
			else
				ASSERT(false && "unknown item");
			continue;
		}

		// TODO: Support variative properties
		if (keyword == "property")
			continue;

		if (keyword == "end_header")
			break;
	}
	vertices.reserve(numVertices);
	facets.reserve(numFacets);

	for (int i = 0; i < numVertices; ++i)
	{
		double xx, yy, zz;
		std::getline(infile, line);
		std::istringstream stream(line);
		stream >> xx >> yy >> zz;
		vertices.emplace_back(xx, yy, zz);
	}

	for (int i = 0; i < numFacets; ++i)
	{
		std::getline(infile, line);
		int number;
		std::istringstream stream(line);
		stream >> number;
		ASSERT(number >= 3);

		std::vector<int> index(3 * number + 1);
		int current = 0;
		for (int j = 0; j < number; ++j)
		{
			stream >> current;
			index[j] = current;
		}
		index.emplace_back(index[0]);
		// TODO: Read color if any
		Plane plane = Plane(vertices[index[0]], vertices[index[1]], vertices[index[2]]);
		facets.emplace_back(i, number, plane, index, false);
		ASSERT(!facets[i].indVertices.empty());
	}
	ASSERT(vertices.size() == size_t(numVertices));
	ASSERT(facets.size() == size_t(numFacets));
	std::cout << "Completed reading PLY file " << filename << std::endl;
	DEBUG_END;
}

void Polyhedron::fprint_default_0(const char *filename)
{
	DEBUG_START;
	FILE *file = (FILE *)fopen(filename, "w");
	if (!file)
	{
		ERROR_PRINT("Error. Cannot open file %s\n", filename);
		DEBUG_END;
		return;
	}

	ALWAYS_PRINT(file, "%d %d\n", numVertices, numFacets);
	for (int i = 0; i < numVertices; ++i)
		ALWAYS_PRINT(file, "%.8f %.8f %.8f\n", vertices[i].x, vertices[i].y, vertices[i].z);

	for (int i = 0; i < numFacets; ++i)
		facets[i].fprint_default_0(file);

	fclose(file);
	DEBUG_END;
}

void Polyhedron::fprint_default_1(const char *filename)
{
	DEBUG_START;
	FILE *file = (FILE *)fopen(filename, "w");
	if (!file)
	{
		ERROR_PRINT("Error. Cannot open file %s\n", filename);
		fclose(file);
		DEBUG_END;
		return;
	}

	ALWAYS_PRINT(file, "%d %d\n", numVertices, numFacets);
	for (int i = 0; i < numVertices; ++i)
		ALWAYS_PRINT(file, "%d %.8f %.8f %.8f\n", i, vertices[i].x, vertices[i].y, vertices[i].z);

	for (int i = 0; i < numFacets; ++i)
		facets[i].fprint_default_1(file);
	fclose(file);
	DEBUG_END;
}

void Polyhedron::fprint_default_1_2(const char *filename)
{
	DEBUG_START;
	FILE *file = (FILE *)fopen(filename, "w");
	if (!file)
	{
		ERROR_PRINT("Error. Cannot open file %s\n", filename);
		fclose(file);
		DEBUG_END;
		return;
	}

	ALWAYS_PRINT(file, "# Produced by Polyhedra Correction Library, "
					   "written by Ilya Palachev <iliyapalachev@gmail.com>\n");
	ALWAYS_PRINT(file, "# num_vertices   num_facets\n");
	ALWAYS_PRINT(file, "%d %d\n", numVertices, numFacets);

	ALWAYS_PRINT(file, "# vertices:\n");
	ALWAYS_PRINT(file, "#   id   x y z\n");
	for (int i = 0; i < numVertices; ++i)
		ALWAYS_PRINT(file, "\t%d\t%.12f\t%.12f\t%.12f\n", i, vertices[i].x, vertices[i].y, vertices[i].z);

	ALWAYS_PRINT(file, "# facets:\n");
	ALWAYS_PRINT(file, "#   id   num_of_sides   plane_coeff  (ax + by + cz + d = 0)\n");
	ALWAYS_PRINT(file, "#   indices_of_vertices\n");
	for (int i = 0; i < numFacets; ++i)
		facets[i].fprint_default_1_2(file);
	ALWAYS_PRINT(file, "# edges are not printed (they're not used by PCL)\n");
	fclose(file);
	DEBUG_END;
}

void Polyhedron::fprint_my_format(const char *filename)
{
	DEBUG_START;
	FILE *file = (FILE *)fopen(filename, "w");
	if (!file)
	{
		ERROR_PRINT("Error. Cannot open file %s\n", filename);
		fclose(file);
		DEBUG_END;
		return;
	}

	ALWAYS_PRINT(file, "%d %d\n", numVertices, numFacets);
	for (int i = 0; i < numVertices; ++i)
		ALWAYS_PRINT(file, "%.8f %.8f %.8f\n", vertices[i].x, vertices[i].y, vertices[i].z);

	for (int i = 0; i < numFacets; ++i)
		facets[i].fprint_my_format(file);
	ALWAYS_PRINT(file, "\n\n\n");
	for (int i = 0; i < numVertices; ++i)
		vertexInfos[i].fprint_my_format(file);

	fclose(file);
	DEBUG_END;
}

void Polyhedron::fprint_ply_scale(double scale, const char *filename, const char *comment)
// comment must have 1 word
{
	DEBUG_START;
	long int vx, vy, vz;
	char *comment_w;

	//	int ncolored = 0;

	FILE *file = (FILE *)fopen(filename, "w");
	if (!file)
	{
		ERROR_PRINT("Error. Cannot open file %s\n", filename);
		DEBUG_END;
		return;
	}

	comment_w = new char[255];
	if (!comment)
		sprintf(comment_w, "unknown");
	else
		sprintf(comment_w, "%s", comment);

	//	numv_new = 0;
	//	for(i = 0; i < numf; ++i)
	//		numv_new += facet[i].nv;

	ALWAYS_PRINT(file, "ply\n");
	ALWAYS_PRINT(file, "format ascii 1.0\n");
	ALWAYS_PRINT(file, "comment converted from %s\n", comment_w);
	ALWAYS_PRINT(file, "element vertex %d\n", numVertices);
	ALWAYS_PRINT(file, "property float x\n");
	ALWAYS_PRINT(file, "property float y\n");
	ALWAYS_PRINT(file, "property float z\n");
	ALWAYS_PRINT(file, "element face %d\n", numFacets);
	ALWAYS_PRINT(file, "property list uchar uint vertex_indices\n");
	ALWAYS_PRINT(file, "property uchar red\n");
	ALWAYS_PRINT(file, "property uchar green\n");
	ALWAYS_PRINT(file, "property uchar blue\n");
	ALWAYS_PRINT(file, "end_header\n");

	for (int i = 0; i < numVertices; ++i)
	{
		vx = (long int)scale * vertices[i].x;
		vy = (long int)scale * vertices[i].y;
		vz = (long int)scale * vertices[i].z;
		ALWAYS_PRINT(file, "%ld %ld %ld\n", vx, vy, vz);
	}

	for (int i = 0; i < numFacets; ++i)
		facets[i].fprint_ply_scale(file);
	fclose(file);

	if (comment_w != NULL)
	{
		delete[] comment_w;
		comment_w = NULL;
	}

	DEBUG_END;
}

void Polyhedron::fprint_ply_autoscale(double maxSize, const char *filename, const char *comment)
{
	DEBUG_START;
	double xmin = 0.;
	double xmax = 0.;
	double ymin = 0.;
	double ymax = 0.;
	double zmin = 0.;
	double zmax = 0.;

	get_boundary(xmin, xmax, ymin, ymax, zmin, zmax);

	/* Calculate absolutely maximal coordinate of the polyhedron. */
	double absMax = 0.;
	absMax = fabs(xmin) > absMax ? fabs(xmin) : absMax;
	absMax = fabs(xmax) > absMax ? fabs(xmax) : absMax;
	absMax = fabs(ymin) > absMax ? fabs(ymin) : absMax;
	absMax = fabs(ymax) > absMax ? fabs(ymax) : absMax;
	absMax = fabs(zmin) > absMax ? fabs(zmin) : absMax;
	absMax = fabs(zmax) > absMax ? fabs(zmax) : absMax;

	double scaleFactor = maxSize / absMax;

	fprint_ply_scale(scaleFactor, filename, comment);

	DEBUG_END;
}

/** Default maximum coordinate of printed polyhedrons. */
const double DEFAULT_MAX_COORDINATE = 1e+6;

std::ostream &operator<<(std::ostream &stream, Polyhedron &p)
{
	DEBUG_START;
	char *name = tmpnam(NULL);
	p.fprint_ply_scale(1e6, name, "generated-from-polyhedron");
	std::ifstream tmpstream;
	tmpstream.open(name, std::ifstream::in);
	stream << tmpstream.rdbuf();
	tmpstream.close();
	DEBUG_END;
	return stream;
}
