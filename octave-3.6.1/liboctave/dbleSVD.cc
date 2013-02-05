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

#include <iostream>

#include "dbleSVD.h"
#include "f77-fcn.h"
#include "oct-locbuf.h"

extern "C"
{
  F77_RET_T
  F77_FUNC (dgesvd, DGESVD) (F77_CONST_CHAR_ARG_DECL,
                             F77_CONST_CHAR_ARG_DECL,
                             const octave_idx_type&, const octave_idx_type&,
                             double*, const octave_idx_type&, double*,
                             double*, const octave_idx_type&, double*,
                             const octave_idx_type&, double*,
                             const octave_idx_type&, octave_idx_type&
                             F77_CHAR_ARG_LEN_DECL
                             F77_CHAR_ARG_LEN_DECL);

  F77_RET_T
  F77_FUNC (dgesdd, DGESDD) (F77_CONST_CHAR_ARG_DECL,
                             const octave_idx_type&, const octave_idx_type&,
                             double*, const octave_idx_type&, double*,
                             double*, const octave_idx_type&, double*,
                             const octave_idx_type&, double*,
                             const octave_idx_type&, octave_idx_type *,
                             octave_idx_type&
                             F77_CHAR_ARG_LEN_DECL);
}

Matrix
SVD::left_singular_matrix (void) const
{
  if (type_computed == SVD::sigma_only)
    {
      (*current_liboctave_error_handler)
        ("SVD: U not computed because type == SVD::sigma_only");
      return Matrix ();
    }
  else
    return left_sm;
}

Matrix
SVD::right_singular_matrix (void) const
{
  if (type_computed == SVD::sigma_only)
    {
      (*current_liboctave_error_handler)
        ("SVD: V not computed because type == SVD::sigma_only");
      return Matrix ();
    }
  else
    return right_sm;
}

octave_idx_type
SVD::init (const Matrix& a, SVD::type svd_type, SVD::driver svd_driver)
{
  octave_idx_type info;

  octave_idx_type m = a.rows ();
  octave_idx_type n = a.cols ();

  Matrix atmp = a;
  double *tmp_data = atmp.fortran_vec ();

  octave_idx_type min_mn = m < n ? m : n;

  char jobu = 'A';
  char jobv = 'A';

  octave_idx_type ncol_u = m;
  octave_idx_type nrow_vt = n;
  octave_idx_type nrow_s = m;
  octave_idx_type ncol_s = n;

  switch (svd_type)
    {
    case SVD::economy:
      jobu = jobv = 'S';
      ncol_u = nrow_vt = nrow_s = ncol_s = min_mn;
      break;

    case SVD::sigma_only:

      // Note:  for this case, both jobu and jobv should be 'N', but
      // there seems to be a bug in dgesvd from Lapack V2.0.  To
      // demonstrate the bug, set both jobu and jobv to 'N' and find
      // the singular values of [eye(3), eye(3)].  The result is
      // [-sqrt(2), -sqrt(2), -sqrt(2)].
      //
      // For Lapack 3.0, this problem seems to be fixed.

      jobu = 'N';
      jobv = 'N';
      ncol_u = nrow_vt = 1;
      break;

    default:
      break;
    }

  type_computed = svd_type;

  if (! (jobu == 'N' || jobu == 'O'))
    left_sm.resize (m, ncol_u);

  double *u = left_sm.fortran_vec ();

  sigma.resize (nrow_s, ncol_s);
  double *s_vec  = sigma.fortran_vec ();

  if (! (jobv == 'N' || jobv == 'O'))
    right_sm.resize (nrow_vt, n);

  double *vt = right_sm.fortran_vec ();

  // Ask DGESVD what the dimension of WORK should be.

  octave_idx_type lwork = -1;

  Array<double> work (dim_vector (1, 1));

  octave_idx_type one = 1;
  octave_idx_type m1 = std::max (m, one), nrow_vt1 = std::max (nrow_vt, one);

  if (svd_driver == SVD::GESVD)
    {
      F77_XFCN (dgesvd, DGESVD, (F77_CONST_CHAR_ARG2 (&jobu, 1),
                                 F77_CONST_CHAR_ARG2 (&jobv, 1),
                                 m, n, tmp_data, m1, s_vec, u, m1, vt,
                                 nrow_vt1, work.fortran_vec (), lwork, info
                                 F77_CHAR_ARG_LEN (1)
                                 F77_CHAR_ARG_LEN (1)));

      lwork = static_cast<octave_idx_type> (work(0));
      work.resize (dim_vector (lwork, 1));

      F77_XFCN (dgesvd, DGESVD, (F77_CONST_CHAR_ARG2 (&jobu, 1),
                                 F77_CONST_CHAR_ARG2 (&jobv, 1),
                                 m, n, tmp_data, m1, s_vec, u, m1, vt,
                                 nrow_vt1, work.fortran_vec (), lwork, info
                                 F77_CHAR_ARG_LEN (1)
                                 F77_CHAR_ARG_LEN (1)));

    }
  else if (svd_driver == SVD::GESDD)
    {
      assert (jobu == jobv);
      char jobz = jobu;
      OCTAVE_LOCAL_BUFFER (octave_idx_type, iwork, 8*min_mn);

      F77_XFCN (dgesdd, DGESDD, (F77_CONST_CHAR_ARG2 (&jobz, 1),
                                 m, n, tmp_data, m1, s_vec, u, m1, vt,
                                 nrow_vt1, work.fortran_vec (), lwork, iwork, info
                                 F77_CHAR_ARG_LEN (1)));

      lwork = static_cast<octave_idx_type> (work(0));
      work.resize (dim_vector (lwork, 1));

      F77_XFCN (dgesdd, DGESDD, (F77_CONST_CHAR_ARG2 (&jobz, 1),
                                 m, n, tmp_data, m1, s_vec, u, m1, vt,
                                 nrow_vt1, work.fortran_vec (), lwork, iwork, info
                                 F77_CHAR_ARG_LEN (1)));

    }
  else
    assert (0); // impossible

  if (! (jobv == 'N' || jobv == 'O'))
    right_sm = right_sm.transpose ();

  return info;
}

std::ostream&
operator << (std::ostream& os, const SVD& a)
{
  os << a.left_singular_matrix () << "\n";
  os << a.singular_values () << "\n";
  os << a.right_singular_matrix () << "\n";

  return os;
}
