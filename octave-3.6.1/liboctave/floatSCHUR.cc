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

#include "floatSCHUR.h"
#include "f77-fcn.h"
#include "lo-error.h"

extern "C"
{
  F77_RET_T
  F77_FUNC (sgeesx, SGEESX) (F77_CONST_CHAR_ARG_DECL,
                             F77_CONST_CHAR_ARG_DECL,
                             FloatSCHUR::select_function,
                             F77_CONST_CHAR_ARG_DECL,
                             const octave_idx_type&, float*,
                             const octave_idx_type&, octave_idx_type&,
                             float*, float*, float*, const octave_idx_type&,
                             float&, float&, float*, const octave_idx_type&,
                             octave_idx_type*, const octave_idx_type&,
                             octave_idx_type*, octave_idx_type&
                             F77_CHAR_ARG_LEN_DECL
                             F77_CHAR_ARG_LEN_DECL
                             F77_CHAR_ARG_LEN_DECL);
}

static octave_idx_type
select_ana (const float& a, const float&)
{
   return (a < 0.0);
}

static octave_idx_type
select_dig (const float& a, const float& b)
{
  return (hypot (a, b) < 1.0);
}

octave_idx_type
FloatSCHUR::init (const FloatMatrix& a, const std::string& ord, bool calc_unitary)
{
  octave_idx_type a_nr = a.rows ();
  octave_idx_type a_nc = a.cols ();

  if (a_nr != a_nc)
    {
      (*current_liboctave_error_handler) ("FloatSCHUR requires square matrix");
      return -1;
    }
  else if (a_nr == 0)
    {
      schur_mat.clear ();
      unitary_mat.clear ();
      return 0;
    }

  // Workspace requirements may need to be fixed if any of the
  // following change.

  char jobvs;
  char sense = 'N';
  char sort = 'N';

  if (calc_unitary)
    jobvs = 'V';
  else
    jobvs = 'N';

  char ord_char = ord.empty () ? 'U' : ord[0];

  if (ord_char == 'A' || ord_char == 'D' || ord_char == 'a' || ord_char == 'd')
    sort = 'S';

  if (ord_char == 'A' || ord_char == 'a')
    selector = select_ana;
  else if (ord_char == 'D' || ord_char == 'd')
    selector = select_dig;
  else
    selector = 0;

  octave_idx_type n = a_nc;
  octave_idx_type lwork = 8 * n;
  octave_idx_type liwork = 1;
  octave_idx_type info;
  octave_idx_type sdim;
  float rconde;
  float rcondv;

  schur_mat = a;

  if (calc_unitary)
    unitary_mat.clear (n, n);

  float *s = schur_mat.fortran_vec ();
  float *q = unitary_mat.fortran_vec ();

  Array<float> wr (dim_vector (n, 1));
  float *pwr = wr.fortran_vec ();

  Array<float> wi (dim_vector (n, 1));
  float *pwi = wi.fortran_vec ();

  Array<float> work (dim_vector (lwork, 1));
  float *pwork = work.fortran_vec ();

  // BWORK is not referenced for the non-ordered Schur routine.
  octave_idx_type ntmp = (ord_char == 'N' || ord_char == 'n') ? 0 : n;
  Array<octave_idx_type> bwork (dim_vector (ntmp, 1));
  octave_idx_type *pbwork = bwork.fortran_vec ();

  Array<octave_idx_type> iwork (dim_vector (liwork, 1));
  octave_idx_type *piwork = iwork.fortran_vec ();

  F77_XFCN (sgeesx, SGEESX, (F77_CONST_CHAR_ARG2 (&jobvs, 1),
                             F77_CONST_CHAR_ARG2 (&sort, 1),
                             selector,
                             F77_CONST_CHAR_ARG2 (&sense, 1),
                             n, s, n, sdim, pwr, pwi, q, n, rconde, rcondv,
                             pwork, lwork, piwork, liwork, pbwork, info
                             F77_CHAR_ARG_LEN (1)
                             F77_CHAR_ARG_LEN (1)
                             F77_CHAR_ARG_LEN (1)));

  return info;
}

FloatSCHUR::FloatSCHUR (const FloatMatrix& s, const FloatMatrix& u)
  : schur_mat (s), unitary_mat (u), selector (0)
{
  octave_idx_type n = s.rows ();
  if (s.columns () != n || u.rows () != n || u.columns () != n)
    (*current_liboctave_error_handler)
      ("schur: inconsistent matrix dimensions");
}

std::ostream&
operator << (std::ostream& os, const FloatSCHUR& a)
{
  os << a.schur_matrix () << "\n";
  os << a.unitary_matrix () << "\n";

  return os;
}
