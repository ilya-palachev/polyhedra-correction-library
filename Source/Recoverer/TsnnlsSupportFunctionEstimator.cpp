/*
 * Copyright (c) 2009-2014 Ilya Palachev <iliyapalachev@gmail.com>
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
#include "Constants.h"
#include "TsnnlsSupportFunctionEstimator.h"

TsnnlsSupportFunctionEstimator::TsnnlsSupportFunctionEstimator(
		SupportFunctionEstimationData& data)
{
	DEBUG_START;
	DEBUG_END;
}

TsnnlsSupportFunctionEstimator::~TsnnlsSupportFunctionEstimator()
{
	DEBUG_START;
	DEBUG_END;
}

static taucs_ccs_matrix* convertEigenToTaucs(SparseMatrix matrix)
{
	DEBUG_START;
	auto matrixTaucs = taucs_ccs_new(matrix.rows(), matrix.cols(),
			matrix.nonZeros());

	ASSERT(0 && "Not implemented yet.");
	/*
	 * TODO: Implement conversion here.
	 */
	DEBUG_END;
	return matrixTaucs;
}

static void analyzeTaucsMatrix(taucs_ccs_matrix* Q, bool ifAnalyzeExpect)
{
	DEBUG_START;
#ifndef NDEBUG

	int* numElemRow = (int*) calloc (Q->m, sizeof(int));

	for (int iCol = 0; iCol < Q->n + 1; ++iCol)
	{
		DEBUG_PRINT("Q->colptr[%d] = %d", iCol, Q->colptr[iCol]);
		if (iCol < Q->n)
		{
			for (int iRow = Q->colptr[iCol]; iRow < Q->colptr[iCol + 1]; ++iRow)
			{
				DEBUG_PRINT("Q[%d][%d] = %.16lf", Q->rowind[iRow], iCol,
						Q->values.d[iRow]);
				numElemRow[Q->rowind[iRow]]++;
			}
		}
	}
	DEBUG_PRINT("Q->colptr[%d] - Q->colptr[%d] = %d", Q->n, 0,
			Q->colptr[Q->n] - Q->colptr[0]);

	if (ifAnalyzeExpect)
	{
		int numUnexcpectedNonzeros = 0;
		for (int iRow = 0; iRow < Q->m; ++iRow)
		{
			DEBUG_PRINT("%d-th row of Q has %d elements.", iRow,
					numElemRow[iRow]);
			if (numElemRow[iRow] != NUM_NONZERO_COEFFICIENTS_IN_CONDITION)
			{
				DEBUG_PRINT("Warning: unexpected number of nonzero elements in "
						"row");
				++numUnexcpectedNonzeros;
			}
		}
		DEBUG_PRINT("Number of rows with unexpected number of nonzero elements "
				"is %d", numUnexcpectedNonzeros);
	}
	free(numElemRow);
#endif
	DEBUG_END;
}

static double l1_distance(int n, double* x, double* y)
{
	DEBUG_START;
	double result = 0.;
	for (int i = 0; i < n; ++i)
	{
		result += fabs(x[i] - y[i]);
	}
	DEBUG_END;
	return result;
}

#if 0
static double l1_norm(int n, double* x)
{
	DEBUG_START;
	double result = 0.;
	for (int i = 0; i < n; ++i)
	{
		result += fabs(x[i]);
	}
	DEBUG_END;
	return result;
}
#endif

static double l2_distance(int n, double* x, double* y)
{
	DEBUG_START;
	double result = 0.;
	for (int i = 0; i < n; ++i)
	{
		result += (x[i] - y[i]) * (x[i] - y[i]);
	}
	DEBUG_END;
	return result;
}

#if 0
static double l2_norm(int n, double* x)
{
	DEBUG_START;
	double result = 0.;
	for (int i = 0; i < n; ++i)
	{
		result += (x[i]) * (x[i]);
	}
	DEBUG_END;
	return result;
}
#endif

static double linf_distance(int n, double* x, double* y)
{
	DEBUG_START;
	double result = 0.;
	for (int i = 0; i < n; ++i)
	{
		result = fabs(x[i] - y[i]) > result ? fabs(x[i] - y[i]) : result;
	}
	DEBUG_END;
	return result;
}

#if 0
static double linf_norm(int n, double* x)
{
	DEBUG_START;
	double result = 0.;
	for (int i = 0; i < n; ++i)
	{
		result = fabs(x[i]) > result ? fabs(x[i]) : result;
	}
	DEBUG_END;
	return result;
}
#endif

void TsnnlsSupportFunctionEstimator::run()
{
	DEBUG_START;
	int numHvalues = 0;
	double *hvalues = NULL;

	/* 1. Build the transpose of support matrix. */
	taucs_ccs_matrix* Qt = convertEigenToTaucs(supportMatrix());
	analyzeTaucsMatrix(Qt, false);
	DEBUG_PRINT("Matrix has been built.");
	DEBUG_PRINT("Qt has %d rows and %d columns", Qt->m, Qt->n);

	/* 1. Enable highest level of verbosity in TSNNLS package. */
	tsnnls_verbosity(10);
	char* strStderr = strdup("stderr");
	taucs_logfile(strStderr);

	/*
	 * 2.Calculate the reciprocal of the condition number.
	 *
	 * In order to be able to do this, we need first to transpose Qt to obtain
	 * the support matrix Q, since LAPACK's routine DGECON usually fails for
	 * matrices where the number of rows is less than the number of columns.
	 *
	 * You can see this at this page:
	 * http://www.netlib.org/lapack/explore-html/db/de4/dgecon_8f_source.html
	 *
	 * Here is the error:
	 *  ** On entry to DGECON parameter number  4 had an illegal value
	 *
	 * Here is the source code that generates the error:
	 *
	 * ELSE IF( lda.LT.max( 1, n ) ) THEN
	 * 		info = -4
	 * ...
	 * IF( info.NE.0 ) THEN
	 * 		CALL xerbla( 'DGECON', -info )
	 * 		RETURN
	 */

	double* Qtvals = taucs_convert_ccs_to_doubles(Qt);
	taucs_ccs_matrix* fixed = taucs_construct_sorted_ccs_matrix(Qtvals, Qt->n,
			Qt->m);
	free(Qtvals);
	taucs_ccs_free(Qt);
	Qt = fixed;

	taucs_ccs_matrix* Q = taucs_ccs_transpose(Qt);
	double conditionNumberQ = taucs_rcond(Q);
	DEBUG_PRINT("rcond(Q) = %.16lf",
			conditionNumberQ);

	double inRelErrTolerance = conditionNumberQ * conditionNumberQ *
			EPS_MIN_DOUBLE;
	DEBUG_PRINT("inRelErrTolerance = %.16lf", inRelErrTolerance);

	double outResidualNorm;

	/* 3. Run the main TSNNLS algorithm of minimization. */
	double* h = t_snnls(Qt, hvalues, &outResidualNorm, inRelErrTolerance + 100.,
			1);
	DEBUG_PRINT("Function t_snnls has returned pointer %p.", (void*) h);

	char* errorTsnnls;
	tsnnls_error(&errorTsnnls);
	ALWAYS_PRINT(stdout, "Error from tsnnls: %s", errorTsnnls);

	DEBUG_PRINT("outResidualNorm = %.16lf", outResidualNorm);

	ALWAYS_PRINT(stdout, "||h - h0||_{1} = %.16lf",
			l1_distance(numHvalues, hvalues, h));
	ALWAYS_PRINT(stdout, "||h - h0||_{2} = %.16lf",
			l2_distance(numHvalues, hvalues, h));
	ALWAYS_PRINT(stdout, "||h - h0||_{inf} = %.16lf",
			linf_distance(numHvalues, hvalues, h));

	free(h);
	taucs_ccs_free(Q);
	taucs_ccs_free(Qt);
}

