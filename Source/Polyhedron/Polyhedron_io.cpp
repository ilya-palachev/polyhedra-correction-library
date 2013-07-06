#include "PolyhedraCorrectionLibrary.h"

void Polyhedron::my_fprint(const char* filename)
{

	FILE* file;
	file = (FILE*) fopen(filename, "w");
	if (!file)
	{
		fprintf(stdout,
				"Polyhedron::fprint_default_0. Error. Cannot open file %s\n",
				filename);
		return;
	}
	this->my_fprint(file);
	fclose(file);
}

void Polyhedron::my_fprint(FILE* file)
{
#ifndef NDEBUG
	int i;
	fprintf(file, "Polyhedron:\n");
	fprintf(file, "numv = %d\nnumf = %d\n", numVertices, numFacets);
	for (i = 0; i < numVertices; ++i)
		fprintf(file, "   vertex[%d] = (%f, %f, %f)\n", i, vertices[i].x,
				vertices[i].y, vertices[i].z);
	for (i = 0; i < numFacets; ++i)
		facets[i].my_fprint_all(file);
	if (vertexInfos != NULL)
	{
		for (i = 0; i < numVertices; ++i)
			vertexInfos[i].my_fprint_all(file);
	}
#endif
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
	file = (FILE*) fopen(filename, "rt");
	if (!file)
	{
		fprintf(stdout,
				"Polyhedron::fscan_default_0. Error. Cannot open file %s\n",
				filename);
		return;
	}

	if (fscanf(file, "%d", &i) != 1 || fscanf(file, "%d", &j) != 1)
	{
		fprintf(stdout, "Polyhedron::fscan_default_0. Error. Invalid file\n");
		fclose(file);
		return;
	}

	numVertices = i;
	numFacets = j;
	vertices = new Vector3d[10 * numFacets];   //MORE MEMORY!!!
	facets = new Facet[10 * numFacets];

	for (i = 0; i < numVertices; ++i)
	{
		if ((fscanf(file, "%lf", &xx) != 1) || (fscanf(file, "%lf", &yy) != 1)
				|| (fscanf(file, "%lf", &zz) != 1))
		{
			fprintf(stdout,
					"Polyhedron::fscan_default_0. Error. Invalid file. v %d\n",
					i);
			fprintf(stdout, "xx = %f, yy = %f, zz = %f\n", xx, yy, zz);
			fclose(file);
			return;
		}
		vertices[i].x = xx;
		vertices[i].y = yy;
		vertices[i].z = zz;
	}
	for (i = 0; i < numFacets; ++i)
	{
		if (fscanf(file, "%d", &nv) != 1 || nv < 3
				|| fscanf(file, "%lf", &a) != 1 || fscanf(file, "%lf", &b) != 1
				|| fscanf(file, "%lf", &c) != 1 || fscanf(file, "%lf", &d) != 1)
		{
			fprintf(stdout,
					"Polyhedron::fscan_default_0. Error. Invalid file. f %d\n",
					i);
			fclose(file);
			return;
		}
		plane = Plane(Vector3d(a, b, c), d);
		index = new int[3 * nv + 1];
		for (j = 0; j < nv; ++j)
		{
			if (fscanf(file, "%d", index + j) != 1)
			{
				fprintf(stdout,
						"Polyhedron::fscan_default_0. Error. Invalid file. f%d v%d\n",
						i, j);
				fclose(file);
				return;
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
	file = (FILE*) fopen(filename, "rt");
	if (!file)
	{
		fprintf(stdout,
				"Polyhedron::fscan_default_1. Error. Cannot open file %s\n",
				filename);
		return;
	}

	if (fscanf(file, "%d", &i) != 1 || fscanf(file, "%d", &j) != 1)
	{
		fprintf(stdout, "Polyhedron::fscan_default_1. Error. Invalid file\n");
		fclose(file);
		return;
	}

	numVertices = i;
	numFacets = j;
	vertices = new Vector3d[2 * numFacets];   //MORE MEMORY!!!
	facets = new Facet[numFacets];

	for (i = 0; i < numVertices; ++i)
	{
		if (fscanf(file, "%d", &ii) != 1 || fscanf(file, "%lf", &xx) != 1
				|| fscanf(file, "%lf", &yy) != 1
				|| fscanf(file, "%lf", &zz) != 1)
		{
			fprintf(stdout,
					"Polyhedron::fscan_default_1. Error. Invalid file\n");
			fclose(file);
			return;
		}
		vertices[ii].x = xx;
		vertices[ii].y = yy;
		vertices[ii].z = zz;
	}
	for (i = 0; i < numFacets; ++i)
	{
		if (fscanf(file, "%d", &ii) != 1 || fscanf(file, "%d", &nv) != 1
				|| nv < 3 || fscanf(file, "%lf", &a) != 1
				|| fscanf(file, "%lf", &b) != 1 || fscanf(file, "%lf", &c) != 1
				|| fscanf(file, "%lf", &d) != 1)
		{
			fprintf(stdout,
					"Polyhedron::fscan_default_1. Error. Invalid file\n");
			fclose(file);
			return;
		}
		plane = Plane(Vector3d(a, b, c), d);
		index = new int[3 * nv + 1];
		for (j = 0; j < nv; ++j)
		{
			if (fscanf(file, "%d", index + j) != 1)
			{
				fprintf(stdout,
						"Polyhedron::fscan_default_1. Error. Invalid file\n");
				fclose(file);
				return;
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

	file = (FILE*) fopen(filename, "rt");

	if (!file)
	{
		fprintf(stdout,
				"Polyhedron::fscan_default_1_1. Error. Cannot open file %s\n",
				filename);
		return;
	}
	tmp = new char[255];
	for (i = 0; i < 4; ++i)
		if (fscanf(file, "%s", tmp) != 1)
		{
			fprintf(stdout,
					"Polyhedron::fscan_defualt_1_1. Error. Invalid file\n");
			fclose(file);
			return;
		}

	if (!file)
	{
		fprintf(stdout,
				"Polyhedron::fscan_default_1_1. Error. Cannot open file %s\n",
				filename);
		return;
	}

	if (fscanf(file, "%d", &i) != 1 || fscanf(file, "%d", &j) != 1)
	{
		fprintf(stdout, "Polyhedron::fscan_default_1_1. Error. Invalid file\n");
		fclose(file);
		return;
	}

	if (fscanf(file, "%s", tmp) != 1)
	{
		fprintf(stdout, "Polyhedron::fscan_defualt_1_1. Error. Invalid file\n");
		fclose(file);
		return;
	}

	numVertices = i;
	numFacets = j;
	vertices = new Vector3d[10 * numFacets];   //MORE MEMORY!!!
	facets = new Facet[10 * numFacets];

	for (i = 0; i < numVertices; ++i)
	{
		if (fscanf(file, "%d", &ii) != 1 || fscanf(file, "%lf", &xx) != 1
				|| fscanf(file, "%lf", &yy) != 1
				|| fscanf(file, "%lf", &zz) != 1)
		{
			fprintf(stdout,
					"Polyhedron::fscan_default_1_1. Error. Invalid file\n");
			fclose(file);
			return;
		}
		vertices[ii].x = xx;
		vertices[ii].y = yy;
		vertices[ii].z = zz;
	}

	if (fscanf(file, "%s", tmp) != 1)
	{
		fprintf(stdout, "Polyhedron::fscan_defualt_1_1. Error. Invalid file\n");
		fclose(file);
		return;
	}

	for (i = 0; i < numFacets; ++i)
	{
		if (fscanf(file, "%d", &ii) != 1 || fscanf(file, "%d", &nv) != 1
				|| nv < 3 || fscanf(file, "%lf", &a) != 1
				|| fscanf(file, "%lf", &b) != 1 || fscanf(file, "%lf", &c) != 1
				|| fscanf(file, "%lf", &d) != 1)
		{
			fprintf(stdout,
					"Polyhedron::fscan_default_1_1. Error. Invalid file\n");
			fclose(file);
			return;
		}
		plane = Plane(Vector3d(a, b, c), d);
		index = new int[3 * nv + 1];
		for (j = 0; j < nv; ++j)
		{
			if (fscanf(file, "%d", index + j) != 1)
			{
				fprintf(stdout,
						"Polyhedron::fscan_default_1_1. Error. Invalid file\n");
				fclose(file);
				return;
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
//Example: MSS_MARQ_G_VS2_0.54_2012_09_18/polyhedron.dat
/////////////////////////////////////

#define STD_POLYHEDRON_FORMAT_HEADER_1 6
#define STD_POLYHEDRON_FORMAT_HEADER_2 6
#define STD_POLYHEDRON_FORMAT_HEADER_3 6
#define STD_POLYHEDRON_FORMAT_HEADER_4 6
static void _fini_fscan_default_1_2(FILE* fd, char* scannedString,
		EdgeData* edgeData)
{
	if (fd)
	{
		fclose(fd);
	}

	if (scannedString)
	{
		delete[] scannedString;
	}

	if (edgeData)
	{
		delete edgeData;
	}
}

bool Polyhedron::fscan_default_1_2(const char *filename)
{
	FILE* fd = (FILE*) fopen(filename, "r");
	if (!fd)
	{
		ERROR_PRINT("Failed to open file %s", filename);
		return false;
	}

	char* scannedString = new char[255];
	EdgeData* edgeData = NULL;
	for (int i = 0; i < STD_POLYHEDRON_FORMAT_HEADER_1; ++i)
	{
		if (fscanf(fd, "%s", scannedString) != 1)
		{
			ERROR_PRINT("Wrong format, in header #1");
			_fini_fscan_default_1_2(fd, scannedString, edgeData);
			return false;
		}
		DEBUG_PRINT("scanned word == \"%s\"", scannedString);
	}

	int numEdges;
	if (fscanf(fd, "%d", &numVertices) != 1
			|| fscanf(fd, "%d", &numFacets) != 1
			|| fscanf(fd, "%d", &numEdges) != 1)
	{
		ERROR_PRINT("Wrong format, in num_vertices, num_facets, num_edgeData");
		_fini_fscan_default_1_2(fd, scannedString, edgeData);
		return false;
	}

	vertices = new Vector3d[numVertices];
	facets = new Facet[numFacets];
	edgeData = new EdgeData(numEdges);

	for (int i = 0; i < STD_POLYHEDRON_FORMAT_HEADER_2; ++i)
	{
		if (fscanf(fd, "%s", scannedString) != 1)
		{
			ERROR_PRINT("Wrong format, in header #2");
			_fini_fscan_default_1_2(fd, scannedString, edgeData);
			return false;
		}
		DEBUG_PRINT("scanned word == \"%s\"", scannedString);
	}

	for (int iVertex = 0; iVertex < numVertices; ++iVertex)
	{
		int iVertexScanned;
		Vector3d* currVertex = &vertices[iVertex];
		if (fscanf(fd, "%d", &iVertexScanned) != 1
				|| iVertexScanned != iVertex
				|| fscanf(fd, "%lf", &currVertex->x) != 1
				|| fscanf(fd, "%lf", &currVertex->y) != 1
				|| fscanf(fd, "%lf", &currVertex->z) != 1
)
		{
			ERROR_PRINT("Wrong format, in vertex #%d", iVertex);
			_fini_fscan_default_1_2(fd, scannedString, edgeData);
			return false;
		}
	}

	for (int i = 0; i < STD_POLYHEDRON_FORMAT_HEADER_3; ++i)
	{
		if (fscanf(fd, "%s", scannedString) != 1)
		{
			ERROR_PRINT("Wrong format, in header #3");
			_fini_fscan_default_1_2(fd, scannedString, edgeData);
			return false;
		}
		DEBUG_PRINT("scanned word == \"%s\"", scannedString);
	}

	for (int iFacet = 0; iFacet < numFacets; ++iFacet)
	{
		Facet* currFacet = &facets[iFacet];
		if (fscanf(fd, "%d", &currFacet->numVertices) != 1
			|| fscanf(fd, "%lf", &currFacet->plane.norm.x) != 1
			|| fscanf(fd, "%lf", &currFacet->plane.norm.y) != 1
			|| fscanf(fd, "%lf", &currFacet->plane.norm.z) != 1
			|| fscanf(fd, "%lf", &currFacet->plane.dist) != 1)
		{
			ERROR_PRINT("Wrong format, in description of facet #%d", iFacet);
			_fini_fscan_default_1_2(fd, scannedString, edgeData);
			return false;
		}

		*currFacet = Facet(iFacet, currFacet->numVertices, currFacet->plane,
				this);

		for (int iVertex = 0; iVertex < currFacet->numVertices; ++iVertex)
		{
			int* currVertex = &currFacet->indVertices[iVertex];
			if (fscanf(fd, "%d", currVertex) != 1)
			{
				ERROR_PRINT("Wrong format, in vertex #%d of facet #%d",
						iVertex, iFacet);
				_fini_fscan_default_1_2(fd, scannedString, edgeData);
				return false;
			}
		}

		int iVertexCycling;
		if (fscanf(fd, "%s", scannedString) != 1)
		{
			ERROR_PRINT("Wrong format, in cycling vertex of facet #%d",
					iFacet);
			_fini_fscan_default_1_2(fd, scannedString, edgeData);
			return false;
		}
	}

	for (int i = 0; i < STD_POLYHEDRON_FORMAT_HEADER_4; ++i)
	{
		if (fscanf(fd, "%s", scannedString) != 1)
		{
			ERROR_PRINT("Wrong format, in header #4");
			_fini_fscan_default_1_2(fd, scannedString, edgeData);
			return false;
		}
		DEBUG_PRINT("scanned word == \"%s\"", scannedString);
	}

	for (int iEdge = 0; iEdge < numEdges; ++iEdge)
	{
		int iVertex0, iVertex1, iFacet, iIndInFacet;
		if (fscanf(fd, "%d", &iVertex0) != 1
				|| fscanf(fd, "%d", &iVertex1) != 1
				|| fscanf(fd, "%d", &iFacet) != 1
				|| fscanf(fd, "%d", &iIndInFacet) != 1
				|| iVertex0 != facets[iFacet].indVertices[iIndInFacet])
		{
			ERROR_PRINT("Wrong format, in edge #%d",
					iEdge);
			_fini_fscan_default_1_2(fd, scannedString, edgeData);
			return false;
		}
		edgeData->addEdge(numEdges, iVertex0, iVertex1, iFacet);
	}

	EdgeData* edgeData2 = new EdgeData;
	EdgeConstructor* edgeConstructor = new EdgeConstructor(this, edgeData2);
	edgeConstructor->run(edgeData2);
	if (*edgeData != *edgeData2)
	{
		ERROR_PRINT("Scanned edge data is not equal to obtained by standard"
				" edge constructor");
		_fini_fscan_default_1_2(fd, scannedString, edgeData);
		return false;
	}

	_fini_fscan_default_1_2(fd, scannedString, edgeData);
	return true;
}
#undef STD_POLYHEDRON_FORMAT_HEADER_1
#undef STD_POLYHEDRON_FORMAT_HEADER_2
#undef STD_POLYHEDRON_FORMAT_HEADER_3
#undef STD_POLYHEDRON_FORMAT_HEADER_4

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
	file = (FILE*) fopen(filename, "rt");
	if (!file)
	{
		fprintf(stdout,
				"Polyhedron::fscan_my_format. Error. Cannot open file %s\n",
				filename);
		return;
	}

	if (fscanf(file, "%d", &i) != 1 || fscanf(file, "%d", &j) != 1)
	{
		fprintf(stdout, "Polyhedron::fscan_my_format. Error. Invalid file\n");
		fclose(file);
		return;
	}

	numVertices = i;
	numFacets = j;
	vertices = new Vector3d[2 * numFacets];   //MORE MEMORY!!!
	facets = new Facet[numFacets];

	for (i = 0; i < numVertices; ++i)
	{
		if (fscanf(file, "%lf", &xx) != 1 || fscanf(file, "%lf", &yy) != 1
				|| fscanf(file, "%lf", &zz) != 1)
		{
			fprintf(stdout,
					"Polyhedron::fscan_my_format. Error. Invalid file\n");
			fclose(file);
			return;
		}
		vertices[i].x = xx;
		vertices[i].y = yy;
		vertices[i].z = zz;
	}
	for (i = 0; i < numFacets; ++i)
	{
		if (fscanf(file, "%d", &nv) != 1 || nv < 3
				|| fscanf(file, "%lf", &a) != 1 || fscanf(file, "%lf", &b) != 1
				|| fscanf(file, "%lf", &c) != 1 || fscanf(file, "%lf", &d) != 1)
		{
			fprintf(stdout,
					"Polyhedron::fscan_my_format. Error. Invalid file\n");
			fclose(file);
			return;
		}
		plane = Plane(Vector3d(a, b, c), d);
		index = new int[3 * nv + 1];
		for (j = 0; j < 3 * nv + 1; ++j)
		{
			if (fscanf(file, "%d", index + j) != 1)
			{
				fprintf(stdout,
						"Polyhedron::fscan_my_format. Error. Invalid file\n");
				fclose(file);
				return;
			}
		}
		facets[i] = Facet(i, nv, plane, index, this, false);
		delete[] index;
	}
	for (i = 0; i < numVertices; ++i)
	{
		if (fscanf(file, "%d", &nf) != 1 || nf < 3)
		{
			fprintf(stdout,
					"Polyhedron::fscan_my_format. Error. Invalid file\n");
			fclose(file);
			return;
		}
		index = new int[2 * nf + 1];
		for (j = 0; j < 2 * nf + 1; ++j)
		{
			if (fscanf(file, "%d", index + j) != 1)
			{
				fprintf(stdout,
						"Polyhedron::fscan_my_format. Error. Invalid file\n");
				fclose(file);
				return;
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
	file = (FILE*) fopen(filename, "rt");
	if (!file)
	{
		fprintf(stdout, "Polyhedron::fscan_ply. Error. Cannot open file %s\n",
				filename);
		return;
	}

	tmp = new char[255];
	for (i = 0; i < 11; ++i)
		if (fscanf(file, "%s", tmp) != 1)
		{
			fprintf(stdout, "Polyhedron::fscan_ply. Error. Invalid file\n");
			fclose(file);
			return;
		}

	if (fscanf(file, "%d", &nv) != 1)
	{
		fprintf(stdout, "Polyhedron::fscan_ply. Error. Invalid file\n");
		fclose(file);
		return;
	}
	numv = nv;

	for (i = 0; i < 11; ++i)
		if (fscanf(file, "%s", tmp) != 1)
		{
			fprintf(stdout, "Polyhedron::fscan_ply. Error. Invalid file\n");
			fclose(file);
			return;
		}

	if (fscanf(file, "%d", &nv) != 1)
	{
		fprintf(stdout, "Polyhedron::fscan_ply. Error. Invalid file\n");
		fclose(file);
		return;
	}
	numf = nv;

	vertices = new Vector3d[2 * numf];   //MORE MEMORY!!!
	facets = new Facet[numf];

	for (i = 0; i < numv; ++i)
	{
		if (fscanf(file, "%lf", &xx) != 1 || fscanf(file, "%lf", &yy) != 1
				|| fscanf(file, "%lf", &zz) != 1)
		{
			fprintf(stdout, "Polyhedron::fscan_ply. Error. Invalid file\n");
			fclose(file);
			return;
		}
		vertices[i].x = xx;
		vertices[i].y = yy;
		vertices[i].z = zz;
	}
	for (i = 0; i < numf; ++i)
	{
		if (fscanf(file, "%d", &nv) != 1 || nv < 3)
		{
			fprintf(stdout, "Polyhedron::fscan_ply. Error. Invalid file\n");
			fclose(file);
			return;
		}
		index = new int[3 * nv + 1];
		for (j = 0; j < nv; ++j)
		{
			if (fscanf(file, "%d", index + j) != 1)
			{
				fprintf(stdout, "Polyhedron::fscan_ply. Error. Invalid file\n");
				fclose(file);
				return;
			}
		}
		plane = Plane(vertices[index[0]], vertices[index[1]],
				vertices[index[2]]);
		facets[i] = Facet(i, nv, plane, index, this, false);
		delete[] index;
	}
	fclose(file);
	delete[] tmp;
}

void Polyhedron::fprint_default_0(const char *filename)
{
	FILE* file = (FILE*) fopen(filename, "w");
	if (!file)
	{
		fprintf(stdout,
				"Polyhedron::fprint_default_0. Error. Cannot open file %s\n",
				filename);
		return;
	}

	fprintf(file, "%d %d\n", numVertices, numFacets);
	for (int i = 0; i < numVertices; ++i)
		fprintf(file, "%.8f %.8f %.8f\n", vertices[i].x, vertices[i].y,
				vertices[i].z);

	for (int i = 0; i < numFacets; ++i)
		facets[i].fprint_default_0(file);

	fclose(file);
}

void Polyhedron::fprint_default_1(const char *filename)
{
	FILE* file = (FILE*) fopen(filename, "w");
	if (!file)
	{
		fprintf(stdout,
				"Polyhedron::fprint_default_1. Error. Cannot open file %s\n",
				filename);
		fclose(file);
		return;
	}

	fprintf(file, "%d %d\n", numVertices, numFacets);
	for (int i = 0; i < numVertices; ++i)
		fprintf(file, "%d %.8f %.8f %.8f\n", i, vertices[i].x, vertices[i].y,
				vertices[i].z);

	for (int i = 0; i < numFacets; ++i)
		facets[i].fprint_default_1(file);
	fclose(file);
}

void Polyhedron::fprint_my_format(const char *filename)
{
	FILE* file = (FILE*) fopen(filename, "w");
	if (!file)
	{
		fprintf(stdout,
				"Polyhedron::fprint_my_format. Error. Cannot open file %s\n",
				filename);
		fclose(file);
		return;
	}

	fprintf(file, "%d %d\n", numVertices, numFacets);
	for (int i = 0; i < numVertices; ++i)
		fprintf(file, "%.8f %.8f %.8f\n", vertices[i].x, vertices[i].y,
				vertices[i].z);

	for (int i = 0; i < numFacets; ++i)
		facets[i].fprint_my_format(file);
	fprintf(file, "\n\n\n");
	for (int i = 0; i < numVertices; ++i)
		vertexInfos[i].fprint_my_format(file);

	fclose(file);
}

void Polyhedron::fprint_ply(const char *filename, const char *comment = NULL)
//comment must have 1 word
{
	char* comment_w;

	FILE* file = (FILE*) fopen(filename, "w");
	if (!file)
	{
		fprintf(stdout, "Polyhedron::fscan_ply. Error. Cannot open file %s\n",
				filename);
		return;
	}

	comment_w = new char[255];
	if (!comment)
		sprintf(comment_w, "unknown");
	else
		sprintf(comment_w, "%s", comment);

	int numv_new = 0;
	for (int i = 0; i < numFacets; ++i)
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

	for (int i = 0; i < numFacets; ++i)
		facets[i].fprint_ply_vertex(file);

	for (int i = 0; i < numFacets; ++i)
		facets[i].fprint_ply_vertex(file);
	fclose(file);
}

void Polyhedron::fprint_ply_scale(double scale, const char *filename,
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

	FILE* file = (FILE*) fopen(filename, "w");
	if (!file)
	{
		fprintf(stdout,
				"Polyhedron::fprint_ply_scale. Error. Cannot open file %s\n",
				filename);
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

	for (int i = 0; i < numVertices; ++i)
	{
		vx = (long int) scale * vertices[i].x;
		vy = (long int) scale * vertices[i].y;
		vz = (long int) scale * vertices[i].z;
		fprintf(file, "%ld %ld %ldb\n", vx, vy, vz);
	}

	for (int i = 0; i < numFacets; ++i)
		facets[i].fprint_ply_scale(file);
	fclose(file);
}

