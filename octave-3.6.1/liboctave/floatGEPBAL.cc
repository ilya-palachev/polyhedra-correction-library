/*

Copyright (C) 1994-2012 John W. Eaton

This file is part of Octave.

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

Octave is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Octave; see the file COPYING.  If not, see
<http://www.gnu.org/licenses/>.

*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <vector>

#include "floatGEPBAL.h"
#include "Array-util.h"
#include "f77-fcn.h"
#include "oct-locbuf.h"

extern "C"
{
  F77_RET_T
  F77_FUNC (sggbal, SGGBAL) (F77_CONST_CHAR_ARG_DECL,
                             const octave_idx_type& N, float* A,
                             const octave_idx_type& LDA, float* B,
                             const octave_idx_type& LDB,
                             octave_idx_type& ILO, octave_idx_type& IHI,
                             float* LSCALE, float* RSCALE,
                             float* WORK, octave_idx_type& INFO
                             F77_CHAR_ARG_LEN_DECL);

  F77_RET_T
  F77_FUNC (sggbak, SGGBAK) (F77_CONST_CHAR_ARG_DECL,
                             F77_CONST_CHAR_ARG_DECL,
                             const octave_idx_type& N,
                             const octave_idx_type& ILO,
                             const octave_idx_type& IHI,
                             const float* LSCALE, const float* RSCALE,
                             octave_idx_type& M, float* V,
                             const octave_idx_type& LDV, octave_idx_type& INFO
                             F77_CHAR_ARG_LEN_DECL
                             F77_CHAR_ARG_LEN_DECL);

}

octave_idx_type
FloatGEPBALANCE::init (const FloatMatrix& a, const FloatMatrix& b,
                  const std::string& balance_job)
{
  octave_idx_type n = a.cols ();

  if (a.rows () != n)
    {
      (*current_liboctave_error_handler) ("FloatGEPBALANCE requires square matrix");
      return -1;
    }

  if (a.dims() != b.dims ())
    {
      gripe_nonconformant ("FloatGEPBALANCE", n, n, b.rows(), b.cols());
      return -1;
    }

  octave_idx_type info;
  octave_idx_type ilo;
  octave_idx_type ihi;

  OCTAVE_LOCAL_BUFFER (float, plscale, n);
  OCTAVE_LOCAL_BUFFER (float, prscale, n);
  OCTAVE_LOCAL_BUFFER (float, pwork, 6 * n);

  balanced_mat = a;
  float *p_balanced_mat = balanced_mat.fortran_vec ();
  balanced_mat2 = b;
  float *p_balanced_mat2 = balanced_mat2.fortran_vec ();

  char job = balance_job[0];

  F77_XFCN (sggbal, SGGBAL, (F77_CONST_CHAR_ARG2 (&job, 1),
                             n, p_balanced_mat, n, p_balanced_mat2,
                             n, ilo, ihi, plscale, prscale, pwork, info
                             F77_CHAR_ARG_LEN  (1)));

  balancing_mat = FloatMatrix (n, n, 0.0);
  balancing_mat2 = FloatMatrix (n, n, 0.0);
  for (octave_idx_type i = 0; i < n; i++)
    {
      octave_quit ();
      balancing_mat.elem (i ,i) = 1.0;
      balancing_mat2.elem (i ,i) = 1.0;
    }

  float *p_balancing_mat = balancing_mat.fortran_vec ();
  float *p_balancing_mat2 = balancing_mat2.fortran_vec ();

  // first left
  F77_XFCN (sggbak, SGGBAK, (F77_CONST_CHAR_ARG2 (&job, 1),
                             F77_CONST_CHAR_ARG2 ("L", 1),
                             n, ilo, ihi, plscale, prscale,
                             n, p_balancing_mat, n, info
                             F77_CHAR_ARG_LEN (1)
                             F77_CHAR_ARG_LEN (1)));

  // then right
  F77_XFCN (sggbak, SGGBAK, (F77_CONST_CHAR_ARG2 (&job, 1),
                             F77_CONST_CHAR_ARG2 ("R", 1),
                             n, ilo, ihi, plscale, prscale,
                             n, p_balancing_mat2, n, info
                             F77_CHAR_ARG_LEN (1)
                             F77_CHAR_ARG_LEN (1)));

  return info;
}
