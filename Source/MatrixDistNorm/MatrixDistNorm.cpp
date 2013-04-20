/* 
 * File:   MatrixDistNorm.cpp
 * Author: nk
 * 
 * Created on 1 Май 2012 г., 12:44
 */
#include "PolyhedraCorrectionLibrary.h"

MatrixDistNorm::MatrixDistNorm() :
				n(0),
				nMax(0),
				matrix(NULL),
				ifStay(NULL) {
}

MatrixDistNorm::MatrixDistNorm(
		int nMax_orig) :
				n(0),
				nMax(nMax_orig),
				matrix(new double[nMax_orig * nMax_orig]),
				ifStay(new bool[nMax_orig]) {
	for (int i = 0; i < nMax_orig; ++i)
		ifStay[i] = true;
}

MatrixDistNorm::MatrixDistNorm(
		const MatrixDistNorm& orig) :
				n(orig.n),
				nMax(orig.nMax),
				matrix(new double[orig.nMax * orig.nMax]),
				ifStay(new bool[orig.nMax]) {
	for (int i = 0; i < nMax * nMax; ++i) {
		matrix[i] = orig.matrix[i];
	}
	for (int i = 0; i < nMax; ++i) {
		ifStay[i] = orig.ifStay[i];
	}
}

MatrixDistNorm::~MatrixDistNorm() {
	if (matrix != NULL) {
		delete[] matrix;
		matrix = NULL;
	}
	if (ifStay != NULL) {
		delete[] ifStay;
		ifStay = NULL;
	}
}

void MatrixDistNorm::build(
		int m,
		TreeClusterNormNode* nodeArray) {

	if (m > nMax) {
		printf("Error. m > nMax\n");
		return;
	}

	n = m;
	double distance;
	for (int i = 0; i < n; ++i) {
		for (int j = i + 1; j < n; ++j) {
			distance = distCluster(*(nodeArray[i].cluster), *(nodeArray[j].cluster));
			//printf("distance (%d, %d) = %lf\n", i, j, distance);
			matrix[i * m + j] = distance;
			matrix[j * m + i] = distance;
		}
	}
	for (int i = 0; i < n; ++i) {
		ifStay[i] = true;
	}
}

double MatrixDistNorm::findMin(
		int& imin,
		int& jmin) {
	double min, distance;
	bool ifAnyMinFound = false;
	for (int i = 0; i < n; ++i) {
		for (int j = i + 1; j < n; ++j) {
			if (!ifStay[i] || !ifStay[j])
				continue;
			distance = matrix[i * n + j];
			//printf("\t\tdistance(%d, %d) = %lf\n", i, j, distance);
			if (!ifAnyMinFound || distance < min) {
				ifAnyMinFound = true;
				imin = i;
				jmin = j;
				min = distance;
			}
		}
	}
	return min;
}

void MatrixDistNorm::rebuild(
		int imin,
		int jmin,
		TreeClusterNormNode* nodeArray) {

	double distance;

	//  printf("rebuild : imin = %d, jmin = %d\n", imin, jmin);
	ifStay[jmin] = false;
	//  printf("ifStay[%d] = 0\n", jmin);

	for (int i = 0; i < n; ++i) {
		if (!ifStay[i])
			continue;
		distance = distCluster(*nodeArray[imin].cluster, *nodeArray[i].cluster);
		matrix[i * n + imin] = distance;
		matrix[imin * n + i] = distance;
	}

}

void MatrixDistNorm::fprint_clusters(
		FILE* file,
		TreeClusterNormNode* nodeArray) {

	for (int i = 0; i < n; ++i) {
		if (ifStay[i] == true) {
			printf("cluster%d = {", i);
			nodeArray[i].cluster->fprint(file);
			printf("}\n");
		}
	}
}

void MatrixDistNorm::fprint_clusters2(
		FILE* file,
		TreeClusterNormNode* nodeArray) {

	int numcl = 0;
	double allarea = 0;
	for (int i = 0; i < n; ++i) {
		if (ifStay[i] == true) {
			numcl++;
		}
	}

	for (int i = 0; i < numcl; ++i) {
		//        if(ifStay[i] == true)
		//        {
		printf("cluster%d = {", i);
		nodeArray[i].cluster->fprint(file);
		allarea += nodeArray[i].cluster->area();
		printf("}\n");
		//        }
	}
	printf("allarea = %lf\n", allarea);
}

double MatrixDistNorm::sqNorm(
		TreeClusterNormNode* nodeArray1,
		TreeClusterNormNode* nodeArray2) {

	int numcl = 0;
	double area1 = 0;
	double area2 = 0;
	double norm = 0;
	for (int i = 0; i < n; ++i) {
		if (ifStay[i] == true) {
			numcl++;
		}
	}

	for (int i = 0; i < numcl; ++i) {
		//        if(ifStay[i] == true)
		//        {
		//printf("cluster%d = {",i);
		//nodeArray[i].cluster->fprint(file);
		area1 = nodeArray1[i].cluster->area();
		area2 = nodeArray2[i].cluster->area();
		area1 -= area2;
		area1 *= area1;
		norm += area1;
		//printf("}\n");
		//        }
	}
	return norm / numcl;
}

void MatrixDistNorm::setColors(
		TreeClusterNormNode* nodeArray) {

	int numcl = 0;
	for (int i = 0; i < n; ++i) {
		if (ifStay[i] == true) {
			numcl++;
		}
	}

	int j;
	char red, green, blue;

	for (int i = 0; i < numcl; ++i) {

		j = i % 24;
		switch (j) {
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

		if (ifStay[i]) {
			nodeArray[i].cluster->setColor(red, green, blue);
		}
	}
}

void MatrixDistNorm::setColors2(
		TreeClusterNormNode* nodeArray) {

	int numcl = 0;
	for (int i = 0; i < n; ++i) {
		if (ifStay[i] == true) {
			numcl++;
		}
	}

	int j;
	char red, green, blue;

	for (int i = 0; i < numcl; ++i) {

		j = i % 24;
		switch (j) {
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

//        if (ifStay[i]) {
		nodeArray[i].cluster->setColor(red, green, blue);
//        }
	}
}

void MatrixDistNorm::fprint(
		FILE* file) {

	printf("MatrixDistNorm : \n\n");

	for (int i = 0; i < n; ++i) {
		printf("ifStay[%d] = %d\n", i, ifStay[i]);
	}
	for (int i = 0; i < n; ++i) {
		for (int j = i + 1; j < n; ++j) {
			printf("matrix[%d][%d] = %lf\n", i, j, matrix[i * n + j]);
		}
	}
}

void MatrixDistNorm::fprint_ifStay(
		FILE* file) {

	//printf("MatrixDistNorm : ifStay : \n\n");

	for (int i = 0; i < n; ++i) {
		if (i < 100)
			printf("\\;");
		if (i < 10)
			printf("\\;");
		printf("\\;%d", i);
	}
	printf("\n");
	for (int i = 0; i < n; ++i) {
		if (i > 10 && i < 20)
			printf("\\;");
		if (i > 21)
			printf("\\;");
		printf("\\;\\;\\;%d", ifStay[i]);
	}
	printf("\n");
}

