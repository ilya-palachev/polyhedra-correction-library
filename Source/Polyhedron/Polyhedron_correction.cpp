/* 
 * File:   Polyhedron_correction.cpp
 * Author: ilya
 * 
 * Created on 19 Ноябрь 2012 г., 07:39
 */

#include "PolyhedraCorrectionLibrary.h"

const double EPS_MAX_ERROR = 1e-6;
const double EPS_FOR_PRINT = 1e-15;

void print_matrix2(FILE* file, int nrow, int ncol, double* a);

static FILE* fout;

int Polyhedron::correct_polyhedron()
{
	DEBUG_START;

	fprintf(stdout, COLOUR_MAGENTA);
	my_fprint(stdout);
	fprintf(stdout, COLOUR_WHITE);

	fout = (FILE*) fopen("corpol_matrix_dbg.txt", "w");

	int numEdges;
	Edge* edges = NULL;

	corpol_preprocess();

	list<int>* facetsNotAssociated = corpol_find_not_associated_facets();
#ifndef NDEBUG
	DEBUG_PRINT("The following facets have no associations:");
	for (list<int>::iterator iter = facetsNotAssociated->begin();
			iter != facetsNotAssociated->end(); ++iter)
	{
		DEBUG_PRINT("%d", *iter);
	}
#endif

		int numFacetsNotAssociated = facetsNotAssociated->size();
	int dim = (numFacets - numFacetsNotAssociated) * 5;

	double * matrix = new double[dim * dim];
	double * matrixFactorized = new double[dim * dim];
	double * rightPart = new double[dim];
	double * solution = new double[dim];
	int * indexPivot = new int[dim];

	Plane * prevPlanes = new Plane[numFacets];

	DEBUG_PRINT("memory allocation done");

	for (int i = 0; i < numFacets; ++i)
	{
		prevPlanes[i] = facets[i].plane;
	}
	DEBUG_PRINT("memory initialization done");

	double error = corpol_calculate_functional(prevPlanes);
	corpol_calculate_matrix(dim, prevPlanes, matrix, rightPart, solution,
			facetsNotAssociated);
	print_matrix2(fout, dim, dim, matrix);

	DEBUG_PRINT("first functional calculation done. error = %e", error);

	int numIterations = 0;

	while (error > EPS_MAX_ERROR)
	{
		DEBUG_PRINT(COLOUR_GREEN "Iteration %d : begin\n", numIterations);

		char* fileName = new char[255];
		sprintf(fileName, "./poly-data-out/correction-of-cube/cube%d.txt",
				numIterations);

		this->my_fprint(fileName);
		delete[] fileName;

		int ret = corpol_iteration(dim, prevPlanes, matrix, rightPart, solution,
				matrixFactorized, indexPivot, facetsNotAssociated);
		for (int i = 0; i < numFacets; ++i)
		{
			DEBUG_PRINT(
					"PLane[%d]: (%lf, %lf, %lf, %lf) --> (%lf, %lf, %lf, %lf)",
					i, prevPlanes[i].norm.x, prevPlanes[i].norm.y,
					prevPlanes[i].norm.z, prevPlanes[i].dist,
					facets[i].plane.norm.x, facets[i].plane.norm.y,
					facets[i].plane.norm.z, facets[i].plane.dist);
			prevPlanes[i] = facets[i].plane;
		}
		error = corpol_calculate_functional(prevPlanes);
		DEBUG_PRINT("error = %le", error);
		DEBUG_PRINT(COLOUR_GREEN "Iteration %d : End\n", numIterations);
		++numIterations;

		if (ret != 0)
		{
			break;
		}
	}

	fclose(fout);

	if (matrix != NULL)
	{
		delete[] matrix;
		matrix = NULL;
	}
	if (matrixFactorized != NULL)
	{
		delete[] matrixFactorized;
		matrixFactorized = NULL;
	}
	if (rightPart != NULL)
	{
		delete[] rightPart;
		rightPart = NULL;
	}
	if (solution != NULL)
	{
		delete[] solution;
		solution = NULL;
	}
	DEBUG_END;
	return 0;
}

list<int>* Polyhedron::corpol_find_not_associated_facets()
{
	list<int>* facetsNotAssociated = new list<int>;
	for (int iFacet = 0; iFacet < numFacets; ++iFacet)
	{
		int* indVertices = facets[iFacet].indVertices;
		int numVerticesFacet = facets[iFacet].numVertices;
		int numAssociations = 0;
		for (int iVertex = 0; iVertex < numVerticesFacet; ++iVertex)
		{
			int edgeid = preprocess_edges_find(indVertices[iVertex],
					indVertices[iVertex + 1]);
			numAssociations += edges[edgeid].assocList.size();
		}
		if (numAssociations == 0)
		{
			facetsNotAssociated->push_back(iFacet);
		}
	}
	return facetsNotAssociated;
}

double Polyhedron::corpol_calculate_functional(Plane* prevPlanes)
{
	double sum = 0;

	for (int iEdge = 0; iEdge < numEdges; ++iEdge)
	{
//    	DEBUG_PRINT("%s: processing edge #%d\n", __func__, i);
		int f0 = edges[iEdge].f0;
		int f1 = edges[iEdge].f1;
		Plane plane0 = facets[f0].plane;
		Plane plane1 = facets[f1].plane;
		Plane planePrev0 = prevPlanes[f0];
		Plane planePrev1 = prevPlanes[f1];

		int iAssoc = 0;
		for (list<EdgeContourAssociation>::iterator itCont =
				edges[iEdge].assocList.begin();
				itCont != edges[iEdge].assocList.end(); ++itCont)
		{
//        	DEBUG_PRINT("\t%s: processing contour #%d\n", __func__, j);
			int curContour = itCont->indContour;
			int curNearestSide = itCont->indNearestSide;
			SideOfContour * sides = contours[curContour].sides;
			Plane planeOfProjection = contours[curContour].plane;
			double weight = itCont->weight;

			double enumerator = -planePrev1.norm * planeOfProjection.norm;
			double denominator = (planePrev0.norm - planePrev1.norm)
					* planeOfProjection.norm;
//			DEBUG_PRINT("iEdge = %d (%d, %d), iContour = %d | enu = %lf, den = %lf",
//					iEdge, edges[iEdge].v0, edges[iEdge].v1, itCont->indContour, enumerator, denominator);

//			ASSERT(fabs(denominator) > 1e-10);
			double gamma_ij = enumerator / denominator;

			double a_ij = gamma_ij * plane0.norm.x
					+ (1 - gamma_ij) * plane1.norm.x;
			double b_ij = gamma_ij * plane0.norm.y
					+ (1 - gamma_ij) * plane1.norm.y;
			double c_ij = gamma_ij * plane0.norm.z
					+ (1 - gamma_ij) * plane1.norm.z;
			double d_ij = gamma_ij * plane0.dist + (1 - gamma_ij) * plane1.dist;

			Vector3d A_ij0 = sides[curNearestSide].A1;
			Vector3d A_ij1 = sides[curNearestSide].A2;

			double summand0 = a_ij * A_ij0.x + b_ij * A_ij0.y + c_ij * A_ij0.z
					+ d_ij;

			summand0 *= summand0;
			summand0 *= weight;

			double summand1 = a_ij * A_ij1.x + b_ij * A_ij1.y + c_ij * A_ij1.z
					+ d_ij;

			summand1 *= summand1;
			summand1 *= weight;

			sum += summand0 + summand1;
		}
	}
	return sum;
}

int Polyhedron::corpol_iteration(int dim, Plane* prevPlanes, double* matrix,
		double* rightPart, double* solution, double* matrixFactorized,
		int* indexPivot, list<int>* facetsNotAssociated)
{
	DEBUG_START;
	corpol_calculate_matrix(dim, prevPlanes, matrix, rightPart, solution,
			facetsNotAssociated);
	print_matrix2(fout, dim, dim, matrix);

#ifdef GLOBAL_CORRECTION_DERIVATIVE_TESTING
	// Test the calculated matrix by the method of
	// numerical calculation of first derivatives
	// of the functional
	corpol_derivativeTest_all(prevPlanes, matrix, facetsNotAssociated);
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
			lda, matrixFactorized, ldaf, indexPivot, rightPart, ldb, solution,
			ldx, &reciprocalToConditionalNumber, &forwardErrorBound,
			&relativeBackwardError);

	if (info == 0)
	{
		DEBUG_PRINT("LAPACKE_dsysvx: successful exit");
	}
	else if (info < 0)
	{
		DEBUG_PRINT("LAPACKE_dsysvx: the %d-th argument had an illegal value",
				-info);
	}
	else if (info <= dim)
	{
		DEBUG_PRINT("LAPACKE_dsysvx: D(%d,%d) is exactly zero.", info, info);
		DEBUG_PRINT(
				"The factorization has been completed but the factor D is ");
		DEBUG_PRINT("exactly singular, so the solution and error bounds could");
		DEBUG_PRINT("not be computed. RCOND = 0 is returned.");
	}
	else if (info == dim + 1)
	{
		DEBUG_PRINT("LAPACKE_dsysvx: D is non-singular, but RCOND is ");
		DEBUG_PRINT("less than machine");
		DEBUG_PRINT("precision, meaning that the matrix is singular");
		DEBUG_PRINT("to working precision.  Nevertheless, the");
		DEBUG_PRINT("solution and error bounds are computed because");
		DEBUG_PRINT("there are a number of situations where the");
		DEBUG_PRINT("computed solution can be more accurate than the");
		DEBUG_PRINT("value of RCOND would suggest.");

	}
	else
	{
		DEBUG_PRINT("LAPACKE_dsysvx: FATAL. Unknown output value");
	}

	list<int>::iterator iterNotAssicated = facetsNotAssociated->begin();
	int countNotAssociated = 0;
	for (int iFacet = 0; iFacet < numFacets; ++iFacet)
	{
		if (*iterNotAssicated == iFacet)
		{
			facets[iFacet].plane = prevPlanes[iFacet];
			++iterNotAssicated;
			++countNotAssociated;
			continue;
		}
		int iFacetShifted = iFacet - countNotAssociated;
		facets[iFacet].plane.norm.x = solution[5 * iFacetShifted];
		facets[iFacet].plane.norm.y = solution[5 * iFacetShifted + 1];
		facets[iFacet].plane.norm.z = solution[5 * iFacetShifted + 2];
		facets[iFacet].plane.dist = solution[5 * iFacetShifted + 3];
	}

	DEBUG_END;
	return info;
}

void Polyhedron::corpol_calculate_matrix(int dim, Plane* prevPlanes,
		double* matrix, double* rightPart, double* solution,
		list<int>* facetsNotAssociated)
{
	DEBUG_START;
	for (int i = 0; i < dim * dim; ++i)
	{
		matrix[i] = 0.;
	}

	for (int i = 0; i < dim; ++i)
	{
		rightPart[i] = 0.;
	}

	list<int>::iterator iterNotAssicated = facetsNotAssociated->begin();
	int countNotAssociated = 0;

	for (int iFacet = 0; iFacet < numFacets; ++iFacet)
	{
		if (*iterNotAssicated == iFacet)
		{
			++iterNotAssicated;
			++countNotAssociated;
			continue;
		}
		int iFacetShifted = iFacet - countNotAssociated;

		int nv = facets[iFacet].numVertices;
		int * index = facets[iFacet].indVertices;

		Plane planePrevThis = prevPlanes[iFacet];

		int i_ak_ak = dim * 5 * iFacetShifted + 5 * iFacetShifted;
		int i_bk_ak = i_ak_ak + dim;
		int i_ck_ak = i_ak_ak + 2 * dim;
		int i_dk_ak = i_ak_ak + 3 * dim;
		int i_lk_ak = i_ak_ak + 4 * dim;

		matrix[i_lk_ak] = 0.5 * planePrevThis.norm.x;
		matrix[i_lk_ak + 1] = 0.5 * planePrevThis.norm.y;
		matrix[i_lk_ak + 2] = 0.5 * planePrevThis.norm.z;
		matrix[i_lk_ak + 3] = matrix[i_lk_ak + 4] = 0.;
		rightPart[5 * iFacetShifted + 4] = 1.;

		matrix[i_ak_ak + 4] = 0.5 * planePrevThis.norm.x;
		matrix[i_bk_ak + 4] = 0.5 * planePrevThis.norm.y;
		matrix[i_ck_ak + 4] = 0.5 * planePrevThis.norm.z;

		for (int iEdge = 0; iEdge < nv; ++iEdge)
		{
			int v0 = index[iEdge];
			int v1 = index[iEdge + 1];
			int iFacetNeighbour = index[nv + 1 + iEdge];
			int iFacetNeighbourShifted = iFacetNeighbour - countNotAssociated;

			int i_ak_an = dim * 5 * iFacetShifted + 5 * iFacetNeighbourShifted;
			int i_bk_an = i_ak_an + dim;
			int i_ck_an = i_ak_an + 2 * dim;
			int i_dk_an = i_ak_an + 3 * dim;
			__attribute__((unused)) int i_lk_an = i_ak_an + 4 * dim;

			Plane planePrevNeighbour = prevPlanes[iFacetNeighbour];

			int edgeid = preprocess_edges_find(v0, v1);

			if (edgeid == -1)
			{
				ERROR_PRINT("Error! edge (%d, %d) cannot be found\n", v0, v1);
				return;
			}
			int iAssociation = 0;
			for (list<EdgeContourAssociation>::iterator itCont =
					edges[edgeid].assocList.begin();
					itCont != edges[edgeid].assocList.end(); ++itCont)
			{
				int curContour = itCont->indContour;
				int curNearestSide = itCont->indNearestSide;
				double weight = itCont->weight;
				SideOfContour * sides = contours[curContour].sides;
				Plane planeOfProjection = contours[curContour].plane;

				double enumerator = -planePrevNeighbour.norm
						* planeOfProjection.norm;
				double denominator = ((planePrevThis.norm
						- planePrevNeighbour.norm) * planeOfProjection.norm);

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
				matrix[i_bk_an + 2] += gamma2 * yz;
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
	DEBUG_END;
}

void print_matrix2(FILE* file, int nrow, int ncol, double* a)
{
	fprintf(file, "=========begin=of=matrix=================\n");
	fprintf(file, "number of rows: %d\n", nrow);
	fprintf(file, "number of columns: %d\n", ncol);
	for (int irow = 0; irow < nrow; ++irow)
	{
		for (int icol = 0; icol < ncol; ++icol)
		{
			double valuePrinted = a[ncol * irow + icol];
			if (fabs(valuePrinted) < EPS_FOR_PRINT)
			{
				fprintf(file, "\t |\t");
			}
			else
			{
				fprintf(file, "%lf |\t", valuePrinted);
			}
		}
		fprintf(file, "\n");
		if (irow % 5 == 4)
		{
			for (int icol = 0; icol < ncol; ++icol)
			{
				fprintf(file, "----------------");
			}
			fprintf(file, "\n");
		}
	}
	fprintf(file, "=========end=of=matrix===================\n");
}
