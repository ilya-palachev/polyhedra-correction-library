/* 
 * Copyright (c) 2009-2013 Ilya Palachev <iliyapalachev@gmail.com>
 * Copyright (c) 2009-2012 Nikolai Kaligin <nkaligin@yandex.ru>
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

#include "DebugPrint.h"
#include "DebugAssert.h"
#include "Analyzers/Clusterizer/MatrixDistNorm/MatrixDistNorm.h"
#include "Analyzers/Clusterizer/ClusterNorm/ClusterNorm.h"

MatrixDistNorm::MatrixDistNorm() :
		nMax(0), n(0), matrix(NULL), ifStay(NULL)
{
	DEBUG_START;
	DEBUG_END;
}

MatrixDistNorm::MatrixDistNorm(int nMax_orig) :
		nMax(nMax_orig), n(0), matrix(new double[nMax_orig * nMax_orig]),
		ifStay(new bool[nMax_orig])
{
	DEBUG_START;
	for (int i = 0; i < nMax_orig; ++i)
		ifStay[i] = true;
	DEBUG_END;
}

MatrixDistNorm::MatrixDistNorm(const MatrixDistNorm& orig) :
		nMax(orig.nMax), n(orig.n), matrix(new double[orig.nMax * orig.nMax]),
		ifStay(new bool[orig.nMax])
{
	DEBUG_START;
	for (int i = 0; i < nMax * nMax; ++i)
	{
		matrix[i] = orig.matrix[i];
	}
	for (int i = 0; i < nMax; ++i)
	{
		ifStay[i] = orig.ifStay[i];
	}
	DEBUG_END;
}

MatrixDistNorm::~MatrixDistNorm()
{
	DEBUG_START;
	if (matrix != NULL)
	{
		delete[] matrix;
		matrix = NULL;
	}
	if (ifStay != NULL)
	{
		delete[] ifStay;
		ifStay = NULL;
	}
	DEBUG_END;
}

void MatrixDistNorm::build(int m, TreeClusterNormNode* nodeArray)
{
	DEBUG_START;
	if (m > nMax)
	{
		ERROR_PRINT("Error. m > nMax\n");
		return;
	}

	n = m;
	double distance;
	for (int i = 0; i < n; ++i)
	{
		for (int j = i + 1; j < n; ++j)
		{
			distance = distCluster(*(nodeArray[i].cluster),
					*(nodeArray[j].cluster));
			DEBUG_PRINT("distance (%d, %d) = %lf\n", i, j, distance);
			matrix[i * m + j] = distance;
			matrix[j * m + i] = distance;
		}
	}
	for (int i = 0; i < n; ++i)
	{
		ifStay[i] = true;
	}
	DEBUG_END;
}

double MatrixDistNorm::findMin(int& imin, int& jmin)
{
	DEBUG_START;
	double min = 0., distance = 0.;
	bool ifAnyMinFound = false;
	for (int i = 0; i < n; ++i)
	{
		for (int j = i + 1; j < n; ++j)
		{
			if (!ifStay[i] || !ifStay[j])
				continue;
			distance = matrix[i * n + j];
			DEBUG_PRINT("\t\tdistance(%d, %d) = %lf\n", i, j, distance);
			if (!ifAnyMinFound || distance < min)
			{
				ifAnyMinFound = true;
				imin = i;
				jmin = j;
				min = distance;
			}
		}
	}
	DEBUG_END;
	return min;
}

void MatrixDistNorm::rebuild(int imin, int jmin, TreeClusterNormNode* nodeArray)
{
	DEBUG_START;
	double distance;

	DEBUG_PRINT("rebuild : imin = %d, jmin = %d\n", imin, jmin);
	ifStay[jmin] = false;
	DEBUG_PRINT("ifStay[%d] = 0\n", jmin);

	for (int i = 0; i < n; ++i)
	{
		if (!ifStay[i])
			continue;
		distance = distCluster(*nodeArray[imin].cluster, *nodeArray[i].cluster);
		matrix[i * n + imin] = distance;
		matrix[imin * n + i] = distance;
	}
	DEBUG_END;
}

void MatrixDistNorm::fprint_clusters(FILE* file, TreeClusterNormNode* nodeArray)
{
	DEBUG_START;
	for (int i = 0; i < n; ++i)
	{
		if (ifStay[i] == true)
		{
			REGULAR_PRINT(file, "cluster%d = {", i);
			nodeArray[i].cluster->fprint(file);
			REGULAR_PRINT(file, "}\n");
		}
	}
	DEBUG_END;
}

void MatrixDistNorm::fprint_clusters2(FILE* file,
		TreeClusterNormNode* nodeArray)
{
	DEBUG_START;
	int numcl = 0;
	double allarea = 0;
	for (int i = 0; i < n; ++i)
	{
		if (ifStay[i] == true)
		{
			numcl++;
		}
	}

	for (int i = 0; i < numcl; ++i)
	{
		//        if(ifStay[i] == true)
		//        {
		REGULAR_PRINT(file, "cluster%d = {", i);
		nodeArray[i].cluster->fprint(file);
		allarea += nodeArray[i].cluster->area();
		REGULAR_PRINT(file, "}\n");
		//        }
	}
	REGULAR_PRINT(file, "all area = %lf\n", allarea);
	DEBUG_END;
}

double MatrixDistNorm::sqNorm(TreeClusterNormNode* nodeArray1,
		TreeClusterNormNode* nodeArray2)
{
	DEBUG_START;
	int numcl = 0;
	double area1 = 0;
	double area2 = 0;
	double norm = 0;
	for (int i = 0; i < n; ++i)
	{
		if (ifStay[i] == true)
		{
			numcl++;
		}
	}

	for (int i = 0; i < numcl; ++i)
	{
		REGULAR_PRINT(stdout, "cluster%d = {", i);
		area1 = nodeArray1[i].cluster->area();
		area2 = nodeArray2[i].cluster->area();
		area1 -= area2;
		area1 *= area1;
		norm += area1;
		REGULAR_PRINT(stdout, "}\n");
	}
	DEBUG_END;
	return norm / numcl;
}

void MatrixDistNorm::setColors(TreeClusterNormNode* nodeArray)
{
	DEBUG_START;
	int numcl = 0;
	for (int i = 0; i < n; ++i)
	{
		if (ifStay[i] == true)
		{
			numcl++;
		}
	}

	int j;
	unsigned char red, green, blue;

	for (int i = 0; i < numcl; ++i)
	{

		j = i % 24;
		switch (j)
		{
		case 0:
			red = 255;
			green = 0;
			blue = 0;
			break;
		case 1:
			red = 0;
			green = 255;
			blue = 0;
			break;
		case 2:
			red = 0;
			green = 0;
			blue = 255;
			break;
		case 3:
			red = 255;
			green = 255;
			blue = 0;
			break;
		case 4:
			red = 255;
			green = 0;
			blue = 255;
			break;
		case 5:
			red = 0;
			green = 255;
			blue = 255;
			break;
		case 6:
			red = 128;
			green = 0;
			blue = 0;
			break;
		case 7:
			red = 0;
			green = 128;
			blue = 0;
			break;
		case 8:
			red = 0;
			green = 0;
			blue = 128;
			break;
		case 9:
			red = 128;
			green = 128;
			blue = 0;
			break;
		case 10:
			red = 128;
			green = 0;
			blue = 128;
			break;
		case 11:
			red = 0;
			green = 128;
			blue = 128;
			break;
		case 12:
			red = 255;
			green = 192;
			blue = 192;
			break;
		case 13:
			red = 192;
			green = 255;
			blue = 192;
			break;
		case 14:
			red = 192;
			green = 192;
			blue = 255;
			break;
		case 15:
			red = 255;
			green = 255;
			blue = 192;
			break;
		case 16:
			red = 255;
			green = 192;
			blue = 255;
			break;
		case 17:
			red = 192;
			green = 255;
			blue = 255;
			break;
		case 18:
			red = 128;
			green = 192;
			blue = 192;
			break;
		case 19:
			red = 192;
			green = 128;
			blue = 192;
			break;
		case 20:
			red = 192;
			green = 192;
			blue = 128;
			break;
		case 21:
			red = 128;
			green = 128;
			blue = 192;
			break;
		case 22:
			red = 128;
			green = 192;
			blue = 128;
			break;
		case 23:
			red = 192;
			green = 128;
			blue = 128;
			break;
		default:
			red = 0;
			green = 0;
			blue = 0;
			break;
		}

		if (ifStay[i])
		{
			nodeArray[i].cluster->setColor(red, green, blue);
		}
	}
	DEBUG_END;
}

void MatrixDistNorm::setColors2(TreeClusterNormNode* nodeArray)
{
	DEBUG_START;
	int numcl = 0;
	for (int i = 0; i < n; ++i)
	{
		if (ifStay[i] == true)
		{
			numcl++;
		}
	}

	int j;
	unsigned char red, green, blue;

	for (int i = 0; i < numcl; ++i)
	{

		j = i % 24;
		switch (j)
		{
		case 0:
			red = 255;
			green = 0;
			blue = 0;
			break;
		case 1:
			red = 0;
			green = 255;
			blue = 0;
			break;
		case 2:
			red = 0;
			green = 0;
			blue = 255;
			break;
		case 3:
			red = 255;
			green = 255;
			blue = 0;
			break;
		case 4:
			red = 255;
			green = 0;
			blue = 255;
			break;
		case 5:
			red = 0;
			green = 255;
			blue = 255;
			break;
		case 6:
			red = 128;
			green = 0;
			blue = 0;
			break;
		case 7:
			red = 0;
			green = 128;
			blue = 0;
			break;
		case 8:
			red = 0;
			green = 0;
			blue = 128;
			break;
		case 9:
			red = 128;
			green = 128;
			blue = 0;
			break;
		case 10:
			red = 128;
			green = 0;
			blue = 128;
			break;
		case 11:
			red = 0;
			green = 128;
			blue = 128;
			break;
		case 12:
			red = 255;
			green = 192;
			blue = 192;
			break;
		case 13:
			red = 192;
			green = 255;
			blue = 192;
			break;
		case 14:
			red = 192;
			green = 192;
			blue = 255;
			break;
		case 15:
			red = 255;
			green = 255;
			blue = 192;
			break;
		case 16:
			red = 255;
			green = 192;
			blue = 255;
			break;
		case 17:
			red = 192;
			green = 255;
			blue = 255;
			break;
		case 18:
			red = 128;
			green = 192;
			blue = 192;
			break;
		case 19:
			red = 192;
			green = 128;
			blue = 192;
			break;
		case 20:
			red = 192;
			green = 192;
			blue = 128;
			break;
		case 21:
			red = 128;
			green = 128;
			blue = 192;
			break;
		case 22:
			red = 128;
			green = 192;
			blue = 128;
			break;
		case 23:
			red = 192;
			green = 128;
			blue = 128;
			break;
		default:
			red = 0;
			green = 0;
			blue = 0;
			break;
		}

		nodeArray[i].cluster->setColor(red, green, blue);
	}
	DEBUG_END;
}

void MatrixDistNorm::fprint(FILE* file)
{
	DEBUG_START;
	REGULAR_PRINT(file, "MatrixDistNorm : \n\n");

	for (int i = 0; i < n; ++i)
	{
		REGULAR_PRINT(file, "ifStay[%d] = %d\n", i, ifStay[i]);
	}
	for (int i = 0; i < n; ++i)
	{
		for (int j = i + 1; j < n; ++j)
		{
			REGULAR_PRINT(file, "matrix[%d][%d] = %lf\n",
					i, j, matrix[i * n + j]);
		}
	}
	DEBUG_END;
}

void MatrixDistNorm::fprint_ifStay(FILE* file)
{
	DEBUG_START;
	for (int i = 0; i < n; ++i)
	{
		if (i < 100)
			REGULAR_PRINT(file, "\\;");
		if (i < 10)
			REGULAR_PRINT(file, "\\;");
		REGULAR_PRINT(file, "\\;%d", i);
	}
	REGULAR_PRINT(file, "\n");
	for (int i = 0; i < n; ++i)
	{
		if (i > 10 && i < 20)
			REGULAR_PRINT(file, "\\;");
		if (i > 21)
			REGULAR_PRINT(file, "\\;");
		REGULAR_PRINT(file, "\\;\\;\\;%d", ifStay[i]);
	}
	REGULAR_PRINT(file, "\n");
	DEBUG_END;
}

