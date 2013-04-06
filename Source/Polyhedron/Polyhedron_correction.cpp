/* 
 * File:   Polyhedron_correction.cpp
 * Author: ilya
 * 
 * Created on 19 Ноябрь 2012 г., 07:39
 */

#include "PolyhedraCorrectionLibrary.h"
#include <dlfcn.h>

const double EPS_MAX_ERROR = 1e-6;
const double EPS_FOR_PRINT = 1e-15;

void print_matrix2(
		FILE* file,
		int nrow,
		int ncol,
		double* a) {
	fprintf(file, "=========begin=of=matrix=================\n");
	fprintf(file, "number of rows: %d\n", nrow);
	fprintf(file, "number of columns: %d\n", ncol);
	for (int irow = 0; irow < nrow; ++irow) {
		for (int icol = 0; icol < ncol; ++icol) {
			double valuePrinted = a[ncol * irow + icol];
			if (fabs(valuePrinted) < EPS_FOR_PRINT) {
				fprintf(file, "\t |\t");
			} else {
				fprintf(file, "%lf |\t", valuePrinted);
			}
		}
		fprintf(file, "\n");
		if (irow % 5 == 4) {
			for (int icol = 0; icol < ncol; ++icol) {
				fprintf(file, "----------------");
			}
			fprintf(file, "\n");
		}
	}
	fprintf(file, "=========end=of=matrix===================\n");
}

static FILE* fout;

int Polyhedron::correct_polyhedron() {
	DBG_START
	;

	fout = (FILE*) fopen("corpol_matrix_dbg.txt", "w");

	int dim = numFacets * 5;

	int numEdges;
	Edge* edges = NULL;

	corpol_preprocess();

	double * matrix = new double[dim * dim];
	double * matrixFactorized = new double[dim * dim];
	double * rightPart = new double[dim];
	double * solution = new double[dim];
	int * indexPivot = new int[dim];

	Plane * prevPlanes = new Plane[numFacets];

	DBGPRINT("memory allocation done");

	for (int i = 0; i < numFacets; ++i) {
		prevPlanes[i] = facets[i].plane;
	}
	DBGPRINT("memory initialization done");

	double error = corpol_calculate_functional(prevPlanes);
	corpol_calculate_matrix(prevPlanes, matrix, rightPart, solution);
	print_matrix2(fout, dim, dim, matrix);

	DBGPRINT("first functional calculation done. error = %e", error);

	int numIterations = 0;

	while (error > EPS_MAX_ERROR) {
		DBGPRINT("Iteration %d\n", numIterations);

		char* fileName = new char[255];
		sprintf(fileName, "./poly-data-out/correction-of-cube/cube%d.txt",
				numIterations);

		this->my_fprint(fileName);
		delete[] fileName;

		int ret = corpol_iteration(prevPlanes, matrix, rightPart, solution,
				matrixFactorized, indexPivot);
		for (int i = 0; i < numFacets; ++i) {
			DBGPRINT("PLane[%d]: (%lf, %lf, %lf, %lf) --> (%lf, %lf, %lf, %lf)",
					i,
					prevPlanes[i].norm.x, prevPlanes[i].norm.y, prevPlanes[i].norm.z,
					prevPlanes[i].dist, facets[i].plane.norm.x,
					facets[i].plane.norm.y, facets[i].plane.norm.z,
					facets[i].plane.dist);
			prevPlanes[i] = facets[i].plane;
		}
		error = corpol_calculate_functional(prevPlanes);
		DBGPRINT("error = %le", error);
		++numIterations;
		if (ret != 0) {
			break;
		}
	}

	fclose(fout);

	if (matrix != NULL) {
		delete[] matrix;
		matrix = NULL;
	}
	if (matrixFactorized != NULL) {
		delete[] matrixFactorized;
		matrixFactorized = NULL;
	}
	if (rightPart != NULL) {
		delete[] rightPart;
		rightPart = NULL;
	}
	if (solution != NULL) {
		delete[] solution;
		solution = NULL;
	}
	DBG_END
	;
	return 0;
}

double Polyhedron::corpol_calculate_functional(
		Plane* prevPlanes) {
	DBG_START
	;
	double sum = 0;

	for (int iEdge = 0; iEdge < numEdges; ++iEdge) {
//    	DBGPRINT("%s: processing edge #%d\n", __func__, i);
		int f0 = edges[iEdge].f0;
		int f1 = edges[iEdge].f1;
		Plane plane0 = facets[f0].plane;
		Plane plane1 = facets[f1].plane;
		Plane planePrev0 = prevPlanes[f0];
		Plane planePrev1 = prevPlanes[f1];

		int iAssoc = 0;
		for (list<EdgeContourAssociation>::iterator itCont =
				edges[iEdge].assocList.begin(); itCont != edges[iEdge].assocList.end();
				++itCont) {
//        	DBGPRINT("\t%s: processing contour #%d\n", __func__, j);
			int curContour = itCont->indContour;
			int curNearestSide = itCont->indNearestSide;
			SideOfContour * sides = contours[curContour].sides;
			Plane planeOfProjection = contours[curContour].plane;
			double weight = itCont->weight;


			double enumerator =  -planePrev1.norm * planeOfProjection.norm;
			double denominator = (planePrev0.norm - planePrev1.norm) *
					planeOfProjection.norm;
//			DBGPRINT("iEdge = %d (%d, %d), iContour = %d | enu = %lf, den = %lf",
//					iEdge, edges[iEdge].v0, edges[iEdge].v1, itCont->indContour, enumerator, denominator);

//			ASSERT(fabs(denominator) > 1e-10);
			double gamma_ij = enumerator / denominator;

			double a_ij = gamma_ij * plane0.norm.x + (1 - gamma_ij) * plane1.norm.x;
			double b_ij = gamma_ij * plane0.norm.y + (1 - gamma_ij) * plane1.norm.y;
			double c_ij = gamma_ij * plane0.norm.z + (1 - gamma_ij) * plane1.norm.z;
			double d_ij = gamma_ij * plane0.dist + (1 - gamma_ij) * plane1.dist;

			Vector3d A_ij0 = sides[curNearestSide].A1;
			Vector3d A_ij1 = sides[curNearestSide].A2;

			double summand0 = a_ij * A_ij0.x + b_ij * A_ij0.y + c_ij * A_ij0.z + d_ij;

			summand0 *= summand0;
			summand0 *= weight;

			double summand1 = a_ij * A_ij1.x + b_ij * A_ij1.y + c_ij * A_ij1.z + d_ij;

			summand1 *= summand1;
			summand1 *= weight;

			ASSERT(summand0 < 1.);
			ASSERT(summand1 < 1.);

			sum += summand0 + summand1;
		}
	}
	DBG_END
	;
	return sum;
}

int Polyhedron::corpol_iteration(
		Plane* prevPlanes,
		double* matrix,
		double* rightPart,
		double* solution,
		double* matrixFactorized,
		int* indexPivot) {
	DBG_START
	;
	corpol_calculate_matrix(prevPlanes,
			matrix, rightPart, solution);
	int dim = 5 * numFacets;
	print_matrix2(fout, dim, dim, matrix);

#ifdef GLOBAL_CORRECTION_DERIVATIVE_TESTING
	// Test the calculated matrix by the method of
	// numerical calculation of first derivatives
	// of the functional

	corpol_derivativeTest_1(prevPlanes, matrix);
#endif

	lapack_int dimLapack = dim;
	lapack_int nrhs = 1;
	lapack_int lda = dim;
	lapack_int ldaf = dim;
	lapack_int ldb = 1;
	lapack_int ldx = 1;
	lapack_int info;
	double reciprocalToConditionalNumber;
	double forwardErrorBound;
	double relativeBackwardError;

	info = LAPACKE_dsysvx(LAPACK_ROW_MAJOR, 'N', 'U', dimLapack, nrhs, matrix,
			lda, matrixFactorized, ldaf, indexPivot, rightPart, ldb, solution, ldx,
			&reciprocalToConditionalNumber, &forwardErrorBound,
			&relativeBackwardError);

	if (info == 0) {
		DBGPRINT("LAPACKE_dsysvx: successful exit");
	} else if (info < 0) {
		DBGPRINT("LAPACKE_dsysvx: the %d-th argument had an illegal value", -info);
	} else if (info <= dim) {
		DBGPRINT(
				"LAPACKE_dsysvx: D(%d,%d) is exactly zero.  " "The factorization" "has been completed but the factor D is exactly" "singular, so the solution and error bounds could" "not be computed. RCOND = 0 is returned.",
				info, info);
	} else if (info == dim + 1) {
		DBGPRINT(
				"LAPACKE_dsysvx: D is non-singular, but RCOND is " "less than machine" "precision, meaning that the matrix is singular" "to working precision.  Nevertheless, the" "solution and error bounds are computed because" "there are a number of situations where the" "computed solution can be more accurate than the" "value of RCOND would suggest.");
	} else {
		DBGPRINT("LAPACKE_dsysvx: FATAL. Unknown output value");
	}

	for (int ifacet = 0; ifacet < numFacets; ++ifacet) {
		facets[ifacet].plane.norm.x = solution[5 * ifacet];
		facets[ifacet].plane.norm.y = solution[5 * ifacet + 1];
		facets[ifacet].plane.norm.z = solution[5 * ifacet + 2];
		facets[ifacet].plane.dist = solution[5 * ifacet + 3];
	}

	DBG_END
	;
	return info;
}

void Polyhedron::corpol_calculate_matrix(
		Plane* prevPlanes,
		double* matrix,
		double* rightPart,
		double* solution) {
	DBG_START
	;
	for (int i = 0; i < (5 * numFacets) * (5 * numFacets); ++i) {
		matrix[i] = 0.;
	}

	for (int i = 0; i < 5 * numFacets; ++i) {
		rightPart[i] = 0.;
	}

	for (int iplane = 0; iplane < numFacets; ++iplane) {
		int nv = facets[iplane].numVertices;
		int * index = facets[iplane].indVertices;

		Plane planePrevThis = prevPlanes[iplane];

		int i_ak_ak = 5 * numFacets * 5 * iplane + 5 * iplane;
		int i_bk_ak = i_ak_ak + 5 * numFacets;
		int i_ck_ak = i_ak_ak + 2 * 5 * numFacets;
		int i_dk_ak = i_ak_ak + 3 * 5 * numFacets;
		int i_lk_ak = i_ak_ak + 4 * 5 * numFacets;

		matrix[i_lk_ak] = 0.5 * planePrevThis.norm.x;
		matrix[i_lk_ak + 1] = 0.5 * planePrevThis.norm.y;
		matrix[i_lk_ak + 2] = 0.5 * planePrevThis.norm.z;
		matrix[i_lk_ak + 3] = matrix[i_lk_ak + 4] = 0;
		rightPart[5 * iplane + 4] = 1.;

		matrix[i_ak_ak + 4] = 0.5 * planePrevThis.norm.x;
		matrix[i_bk_ak + 4] = 0.5 * planePrevThis.norm.y;
		matrix[i_ck_ak + 4] = 0.5 * planePrevThis.norm.z;

		for (int iedge = 0; iedge < nv; ++iedge) {
			int v0 = index[iedge];
			int v1 = index[iedge + 1];
			int iplaneNeighbour = index[nv + 1 + iedge];

			int i_ak_an = 5 * numFacets * 5 * iplane + 5 * iplaneNeighbour;
			int i_bk_an = i_ak_an + 5 * numFacets;
			int i_ck_an = i_ak_an + 2 * 5 * numFacets;
			int i_dk_an = i_ak_an + 3 * 5 * numFacets;
			__attribute__((unused)) int i_lk_an = i_ak_an + 4 * 5 * numFacets;

			Plane planePrevNeighbour = prevPlanes[iplaneNeighbour];

			int edgeid = preprocess_edges_find(v0, v1);

			if (edgeid == -1) {
				printf("Error! edge (%d, %d) cannot be found\n", v0, v1);
				return;
			}

			for (list<EdgeContourAssociation>::iterator itCont =
					edges[edgeid].assocList.begin();
					itCont != edges[edgeid].assocList.end(); ++itCont) {

				int curContour = itCont->indContour;
				int curNearestSide = itCont->indNearestSide;
				double weight = itCont->weight;
				SideOfContour * sides = contours[curContour].sides;
				Plane planeOfProjection = contours[curContour].plane;

				double enumerator = -planePrevNeighbour.norm * planeOfProjection.norm;
				double denominator = ((planePrevThis.norm - planePrevNeighbour.norm)
						* planeOfProjection.norm);
//				DBGPRINT("iEdge = %d (%d, %d), iContour = %d | enu = %lf, den = %lf",
//						iedge, v0, v1, itCont->indContour, enumerator, denominator);
//				ASSERT(fabs(denominator) > 1e-10);

				double gamma_ij = enumerator / denominator;
				ASSERT(!isnan(gamma_ij));

				Vector3d A_ij1 = sides[curNearestSide].A1;
				Vector3d A_ij2 = sides[curNearestSide].A2;

				double x0 = A_ij1.x;
				double y0 = A_ij1.y;
				double z0 = A_ij1.z;
				double x1 = A_ij2.x;
				double y1 = A_ij2.y;
				double z1 = A_ij2.z;

//                DBGPRINT("\t\t A_ij1 = (%lf, %lf, %lf)", x0, y1, z1);
//                DBGPRINT("\t\t A_ij2 = (%lf, %lf, %lf)", x1, y1, z1);

				double xx = x0 * x0 + x1 * x1;
				double xy = x0 * y0 + x1 * y1;
				double xz = x0 * z0 + x1 * z1;
				double x = x0 + x1;
				double yy = y0 * y0 + y1 * y1;
				double yz = y0 * z0 + y1 * z1;
				double y = y0 + y1;
				double zz = z0 * z0 + z1 * z1;
				double z = z0 + z1;

				double gamma1 = gamma_ij * gamma_ij * weight;
				double gamma2 = gamma_ij * (1 - gamma_ij) * weight;

				matrix[i_ak_ak] += gamma1 * xx;
				matrix[i_ak_ak + 1] += gamma1 * xy;
				matrix[i_ak_ak + 2] += gamma1 * xz;
				matrix[i_ak_ak + 3] += gamma1 * x;

				matrix[i_bk_ak] += gamma1 * xy;
				matrix[i_bk_ak + 1] += gamma1 * yy;
				matrix[i_bk_ak + 2] += gamma1 * yz;
				matrix[i_bk_ak + 3] += gamma1 * y;

				matrix[i_ck_ak] += gamma1 * xz;
				matrix[i_ck_ak + 1] += gamma1 * yz;
				matrix[i_ck_ak + 2] += gamma1 * zz;
				matrix[i_ck_ak + 3] += gamma1 * z;

				matrix[i_dk_ak] += gamma1 * x;
				matrix[i_dk_ak + 1] += gamma1 * y;
				matrix[i_dk_ak + 2] += gamma1 * z;
				matrix[i_dk_ak + 3] += gamma1 * 2;

				matrix[i_ak_an] += gamma2 * xx;
				matrix[i_ak_an + 1] += gamma2 * xy;
				matrix[i_ak_an + 2] += gamma2 * xz;
				matrix[i_ak_an + 3] += gamma2 * x;

				matrix[i_bk_an] += gamma2 * xy;
				matrix[i_bk_an + 1] += gamma2 * yy;
				matrix[i_bk_an + 2] += gamma2 * xz;
				matrix[i_bk_an + 3] += gamma2 * y;

				matrix[i_ck_an] += gamma2 * xz;
				matrix[i_ck_an + 1] += gamma2 * yz;
				matrix[i_ck_an + 2] += gamma2 * zz;
				matrix[i_ck_an + 3] += gamma2 * z;

				matrix[i_dk_an] += gamma2 * x;
				matrix[i_dk_an + 1] += gamma2 * y;
				matrix[i_dk_an + 2] += gamma2 * z;
				matrix[i_dk_an + 3] += gamma2 * 2;
			}
		}
	}
	DBG_END
	;
}

