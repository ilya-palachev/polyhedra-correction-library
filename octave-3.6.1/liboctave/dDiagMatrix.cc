// DiagMatrix manipulations.
/*

Copyright (C) 1994-2012 John W. Eaton
Copyright (C) 2009 VZLU Prague

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

#include "Array-util.h"
#include "lo-error.h"
#include "mx-base.h"
#include "mx-inlines.cc"
#include "oct-cmplx.h"

// Diagonal Matrix class.

bool
DiagMatrix::operator == (const DiagMatrix& a) const
{
  if (rows () != a.rows () || cols () != a.cols ())
    return 0;

  return mx_inline_equal (length (), data (), a.data ());
}

bool
DiagMatrix::operator != (const DiagMatrix& a) const
{
  return !(*this == a);
}

DiagMatrix&
DiagMatrix::fill (double val)
{
  for (octave_idx_type i = 0; i < length (); i++)
    elem (i, i) = val;
  return *this;
}

DiagMatrix&
DiagMatrix::fill (double val, octave_idx_type beg, octave_idx_type end)
{
  if (beg < 0 || end >= length () || end < beg)
    {
      (*current_liboctave_error_handler) ("range error for fill");
      return *this;
    }

  for (octave_idx_type i = beg; i <= end; i++)
    elem (i, i) = val;

  return *this;
}

DiagMatrix&
DiagMatrix::fill (const ColumnVector& a)
{
  octave_idx_type len = length ();
  if (a.length () != len)
    {
      (*current_liboctave_error_handler) ("range error for fill");
      return *this;
    }

  for (octave_idx_type i = 0; i < len; i++)
    elem (i, i) = a.elem (i);

  return *this;
}

DiagMatrix&
DiagMatrix::fill (const RowVector& a)
{
  octave_idx_type len = length ();
  if (a.length () != len)
    {
      (*current_liboctave_error_handler) ("range error for fill");
      return *this;
    }

  for (octave_idx_type i = 0; i < len; i++)
    elem (i, i) = a.elem (i);

  return *this;
}

DiagMatrix&
DiagMatrix::fill (const ColumnVector& a, octave_idx_type beg)
{
  octave_idx_type a_len = a.length ();
  if (beg < 0 || beg + a_len >= length ())
    {
      (*current_liboctave_error_handler) ("range error for fill");
      return *this;
    }

  for (octave_idx_type i = 0; i < a_len; i++)
    elem (i+beg, i+beg) = a.elem (i);

  return *this;
}

DiagMatrix&
DiagMatrix::fill (const RowVector& a, octave_idx_type beg)
{
  octave_idx_type a_len = a.length ();
  if (beg < 0 || beg + a_len >= length ())
    {
      (*current_liboctave_error_handler) ("range error for fill");
      return *this;
    }

  for (octave_idx_type i = 0; i < a_len; i++)
    elem (i+beg, i+beg) = a.elem (i);

  return *this;
}

DiagMatrix
DiagMatrix::abs (void) const
{
  return DiagMatrix (diag ().abs (), rows (), columns ());
}

DiagMatrix
real (const ComplexDiagMatrix& a)
{
  return DiagMatrix (real (a.diag ()), a.rows (), a.cols ());
}

DiagMatrix
imag (const ComplexDiagMatrix& a)
{
  return DiagMatrix (imag (a.diag ()), a.rows (), a.cols ());
}

Matrix
DiagMatrix::extract (octave_idx_type r1, octave_idx_type c1, octave_idx_type r2, octave_idx_type c2) const
{
  if (r1 > r2) { octave_idx_type tmp = r1; r1 = r2; r2 = tmp; }
  if (c1 > c2) { octave_idx_type tmp = c1; c1 = c2; c2 = tmp; }

  octave_idx_type new_r = r2 - r1 + 1;
  octave_idx_type new_c = c2 - c1 + 1;

  Matrix result (new_r, new_c);

  for (octave_idx_type j = 0; j < new_c; j++)
    for (octave_idx_type i = 0; i < new_r; i++)
      result.elem (i, j) = elem (r1+i, c1+j);

  return result;
}

// extract row or column i.

RowVector
DiagMatrix::row (octave_idx_type i) const
{
  octave_idx_type r = rows ();
  octave_idx_type c = cols ();
  if (i < 0 || i >= r)
    {
      (*current_liboctave_error_handler) ("invalid row selection");
      return RowVector ();
    }

  RowVector retval (c, 0.0);
  if (r <= c || (r > c && i < c))
    retval.elem (i) = elem (i, i);

  return retval;
}

RowVector
DiagMatrix::row (char *s) const
{
  if (! s)
    {
      (*current_liboctave_error_handler) ("invalid row selection");
      return RowVector ();
    }

  char c = *s;
  if (c == 'f' || c == 'F')
    return row (static_cast<octave_idx_type>(0));
  else if (c == 'l' || c == 'L')
    return row (rows () - 1);
  else
    {
      (*current_liboctave_error_handler) ("invalid row selection");
      return RowVector ();
    }
}

ColumnVector
DiagMatrix::column (octave_idx_type i) const
{
  octave_idx_type r = rows ();
  octave_idx_type c = cols ();
  if (i < 0 || i >= c)
    {
      (*current_liboctave_error_handler) ("invalid column selection");
      return ColumnVector ();
    }

  ColumnVector retval (r, 0.0);
  if (r >= c || (r < c && i < r))
    retval.elem (i) = elem (i, i);

  return retval;
}

ColumnVector
DiagMatrix::column (char *s) const
{
  if (! s)
    {
      (*current_liboctave_error_handler) ("invalid column selection");
      return ColumnVector ();
    }

  char c = *s;
  if (c == 'f' || c == 'F')
    return column (static_cast<octave_idx_type>(0));
  else if (c == 'l' || c == 'L')
    return column (cols () - 1);
  else
    {
      (*current_liboctave_error_handler) ("invalid column selection");
      return ColumnVector ();
    }
}

DiagMatrix
DiagMatrix::inverse (void) const
{
  octave_idx_type info;
  return inverse (info);
}

DiagMatrix
DiagMatrix::inverse (octave_idx_type &info) const
{
  octave_idx_type r = rows ();
  octave_idx_type c = cols ();
  octave_idx_type len = length ();
  if (r != c)
    {
      (*current_liboctave_error_handler) ("inverse requires square matrix");
      return DiagMatrix ();
    }

  DiagMatrix retval (r, c);

  info = 0;
  for (octave_idx_type i = 0; i < len; i++)
    {
      if (elem (i, i) == 0.0)
        {
          info = -1;
          return *this;
        }
      else
        retval.elem (i, i) = 1.0 / elem (i, i);
    }

  return retval;
}

DiagMatrix
DiagMatrix::pseudo_inverse (void) const
{
  octave_idx_type r = rows ();
  octave_idx_type c = cols ();
  octave_idx_type len = length ();

  DiagMatrix retval (c, r);

  for (octave_idx_type i = 0; i < len; i++)
    {
      if (elem (i, i) != 0.0)
        retval.elem (i, i) = 1.0 / elem (i, i);
      else
        retval.elem (i, i) = 0.0;
    }

  return retval;
}

// diagonal matrix by diagonal matrix -> diagonal matrix operations

// diagonal matrix by diagonal matrix -> diagonal matrix operations

DiagMatrix
operator * (const DiagMatrix& a, const DiagMatrix& b)
{
  octave_idx_type a_nr = a.rows ();
  octave_idx_type a_nc = a.cols ();

  octave_idx_type b_nr = b.rows ();
  octave_idx_type b_nc = b.cols ();

  if (a_nc != b_nr)
    gripe_nonconformant ("operator *", a_nr, a_nc, b_nr, b_nc);

  DiagMatrix c (a_nr, b_nc);

  octave_idx_type len = c.length (), lenm = len < a_nc ? len : a_nc;

  for (octave_idx_type i = 0; i < lenm; i++)
    c.dgxelem (i) = a.dgelem (i) * b.dgelem (i);
  for (octave_idx_type i = lenm; i < len; i++)
    c.dgxelem (i) = 0.0;

  return c;
}

// other operations

DET
DiagMatrix::determinant (void) const
{
  DET det (1.0);
  if (rows () != cols ())
    {
      (*current_liboctave_error_handler) ("determinant requires square matrix");
      det = 0.0;
    }
  else
    {
      octave_idx_type len = length ();
      for (octave_idx_type i = 0; i < len; i++)
        det *= elem (i, i);
    }

  return det;
}

double
DiagMatrix::rcond (void) const
{
  ColumnVector av  = diag (0).map<double> (fabs);
  double amx = av.max (), amn = av.min ();
  return amx == 0 ? 0.0 : amn / amx;
}

std::ostream&
operator << (std::ostream& os, const DiagMatrix& a)
{
//  int field_width = os.precision () + 7;

  for (octave_idx_type i = 0; i < a.rows (); i++)
    {
      for (octave_idx_type j = 0; j < a.cols (); j++)
        {
          if (i == j)
            os << " " /* setw (field_width) */ << a.elem (i, i);
          else
            os << " " /* setw (field_width) */ << 0.0;
        }
      os << "\n";
    }
  return os;
}
