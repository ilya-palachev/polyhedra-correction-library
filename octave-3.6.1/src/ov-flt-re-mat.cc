/*

Copyright (C) 1996-2012 John W. Eaton
Copyright (C) 2009-2010 VZLU Prague

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

#include <climits>

#include <iostream>
#include <vector>

#include "data-conv.h"
#include "lo-ieee.h"
#include "lo-utils.h"
#include "lo-specfun.h"
#include "lo-mappers.h"
#include "mach-info.h"
#include "mx-base.h"
#include "quit.h"
#include "oct-locbuf.h"

#include "defun.h"
#include "gripes.h"
#include "oct-obj.h"
#include "oct-lvalue.h"
#include "oct-stream.h"
#include "ops.h"
#include "ov-base.h"
#include "ov-base-mat.h"
#include "ov-base-mat.cc"
#include "ov-scalar.h"
#include "ov-float.h"
#include "ov-flt-complex.h"
#include "ov-re-mat.h"
#include "ov-flt-re-mat.h"
#include "ov-flt-cx-mat.h"
#include "ov-re-sparse.h"
#include "ov-flt-re-diag.h"
#include "ov-flt-cx-diag.h"
#include "ov-type-conv.h"
#include "pr-output.h"
#include "variables.h"
#include "ops.h"

#include "byte-swap.h"
#include "ls-oct-ascii.h"
#include "ls-utils.h"
#include "ls-hdf5.h"

#if ! defined (UCHAR_MAX)
#define UCHAR_MAX 255
#endif

template class octave_base_matrix<FloatNDArray>;

DEFINE_OCTAVE_ALLOCATOR (octave_float_matrix);

DEFINE_OV_TYPEID_FUNCTIONS_AND_DATA (octave_float_matrix, "float matrix", "single");

octave_base_value *
octave_float_matrix::try_narrowing_conversion (void)
{
  octave_base_value *retval = 0;

  if (matrix.nelem () == 1)
    retval = new octave_float_scalar (matrix (0));

  return retval;
}

double
octave_float_matrix::double_value (bool) const
{
  double retval = lo_ieee_nan_value ();

  if (numel () > 0)
    {
      gripe_implicit_conversion ("Octave:array-as-scalar",
                                 "real matrix", "real scalar");

      retval = matrix (0, 0);
    }
  else
    gripe_invalid_conversion ("real matrix", "real scalar");

  return retval;
}

float
octave_float_matrix::float_value (bool) const
{
  float retval = lo_ieee_float_nan_value ();

  if (numel () > 0)
    {
      gripe_implicit_conversion ("Octave:array-as-scalar",
                                 "real matrix", "real scalar");

      retval = matrix (0, 0);
    }
  else
    gripe_invalid_conversion ("real matrix", "real scalar");

  return retval;
}

// FIXME

Matrix
octave_float_matrix::matrix_value (bool) const
{
  return Matrix (matrix.matrix_value ());
}

FloatMatrix
octave_float_matrix::float_matrix_value (bool) const
{
  return matrix.matrix_value ();
}

Complex
octave_float_matrix::complex_value (bool) const
{
  double tmp = lo_ieee_nan_value ();

  Complex retval (tmp, tmp);

  if (rows () > 0 && columns () > 0)
    {
      gripe_implicit_conversion ("Octave:array-as-scalar",
                                 "real matrix", "complex scalar");

      retval = matrix (0, 0);
    }
  else
    gripe_invalid_conversion ("real matrix", "complex scalar");

  return retval;
}

FloatComplex
octave_float_matrix::float_complex_value (bool) const
{
  double tmp = lo_ieee_float_nan_value ();

  FloatComplex retval (tmp, tmp);

  if (rows () > 0 && columns () > 0)
    {
      gripe_implicit_conversion ("Octave:array-as-scalar",
                                 "real matrix", "complex scalar");

      retval = matrix (0, 0);
    }
  else
    gripe_invalid_conversion ("real matrix", "complex scalar");

  return retval;
}

// FIXME

ComplexMatrix
octave_float_matrix::complex_matrix_value (bool) const
{
  return ComplexMatrix (matrix.matrix_value ());
}

FloatComplexMatrix
octave_float_matrix::float_complex_matrix_value (bool) const
{
  return FloatComplexMatrix (matrix.matrix_value ());
}

ComplexNDArray
octave_float_matrix::complex_array_value (bool) const
{
  return ComplexNDArray (matrix);
}

FloatComplexNDArray
octave_float_matrix::float_complex_array_value (bool) const
{
  return FloatComplexNDArray (matrix);
}

NDArray
octave_float_matrix::array_value (bool) const
{
  return NDArray (matrix);
}

boolNDArray
octave_float_matrix::bool_array_value (bool warn) const
{
  if (matrix.any_element_is_nan ())
    gripe_nan_to_logical_conversion ();
  else if (warn && matrix.any_element_not_one_or_zero ())
    gripe_logical_conversion ();

  return boolNDArray (matrix);
}

charNDArray
octave_float_matrix::char_array_value (bool) const
{
  charNDArray retval (dims ());

  octave_idx_type nel = numel ();

  for (octave_idx_type i = 0; i < nel; i++)
    retval.elem (i) = static_cast<char>(matrix.elem (i));

  return retval;
}

SparseMatrix
octave_float_matrix::sparse_matrix_value (bool) const
{
  return SparseMatrix (matrix_value ());
}

SparseComplexMatrix
octave_float_matrix::sparse_complex_matrix_value (bool) const
{
  // FIXME Need a SparseComplexMatrix (Matrix) constructor to make
  // this function more efficient. Then this should become
  // return SparseComplexMatrix (matrix.matrix_value ());
  return SparseComplexMatrix (sparse_matrix_value ());
}

octave_value
octave_float_matrix::diag (octave_idx_type k) const
{
  octave_value retval;
  if (k == 0 && matrix.ndims () == 2
      && (matrix.rows () == 1 || matrix.columns () == 1))
    retval = FloatDiagMatrix (DiagArray2<float> (matrix));
  else
    retval = octave_base_matrix<FloatNDArray>::diag (k);

  return retval;
}

octave_value
octave_float_matrix::convert_to_str_internal (bool, bool, char type) const
{
  octave_value retval;
  dim_vector dv = dims ();
  octave_idx_type nel = dv.numel ();

  charNDArray chm (dv);

  bool warned = false;

  for (octave_idx_type i = 0; i < nel; i++)
    {
      octave_quit ();

      float d = matrix (i);

      if (xisnan (d))
        {
          gripe_nan_to_character_conversion ();
          return retval;
        }
      else
        {
          int ival = NINT (d);

          if (ival < 0 || ival > UCHAR_MAX)
            {
              // FIXME -- is there something
              // better we could do?

              ival = 0;

              if (! warned)
                {
                  ::warning ("range error for conversion to character value");
                  warned = true;
                }
            }

          chm (i) = static_cast<char> (ival);
        }
    }

  retval = octave_value (chm, type);

  return retval;
}

bool
octave_float_matrix::save_ascii (std::ostream& os)
{
  dim_vector d = dims ();

  if (d.length () > 2)
    {
      FloatNDArray tmp = float_array_value ();

      os << "# ndims: " << d.length () << "\n";

      for (int i=0; i < d.length (); i++)
        os << " " << d (i);

      os << "\n" << tmp;
    }
  else
    {
      // Keep this case, rather than use generic code above for backward
      // compatiability. Makes load_ascii much more complex!!
      os << "# rows: " << rows () << "\n"
         << "# columns: " << columns () << "\n";

      os << float_matrix_value ();
    }

  return true;
}

bool
octave_float_matrix::load_ascii (std::istream& is)
{
  bool success = true;

  string_vector keywords(2);

  keywords[0] = "ndims";
  keywords[1] = "rows";

  std::string kw;
  octave_idx_type val = 0;

  if (extract_keyword (is, keywords, kw, val, true))
    {
      if (kw == "ndims")
        {
          int mdims = static_cast<int> (val);

          if (mdims >= 0)
            {
              dim_vector dv;
              dv.resize (mdims);

              for (int i = 0; i < mdims; i++)
                is >> dv(i);

              if (is)
                {
                  FloatNDArray tmp(dv);

                  is >> tmp;

                  if (is)
                    matrix = tmp;
                  else
                    {
                      error ("load: failed to load matrix constant");
                      success = false;
                    }
                }
              else
                {
                  error ("load: failed to read dimensions");
                  success = false;
                }
            }
          else
            {
              error ("load: failed to extract number of dimensions");
              success = false;
            }
        }
      else if (kw == "rows")
        {
          octave_idx_type nr = val;
          octave_idx_type nc = 0;

          if (nr >= 0 && extract_keyword (is, "columns", nc) && nc >= 0)
            {
              if (nr > 0 && nc > 0)
                {
                  FloatMatrix tmp (nr, nc);
                  is >> tmp;
                  if (is)
                    matrix = tmp;
                  else
                    {
                      error ("load: failed to load matrix constant");
                      success = false;
                    }
                }
              else if (nr == 0 || nc == 0)
                matrix = FloatMatrix (nr, nc);
              else
                panic_impossible ();
            }
          else
            {
              error ("load: failed to extract number of rows and columns");
              success = false;
            }
        }
      else
        panic_impossible ();
    }
  else
    {
      error ("load: failed to extract number of rows and columns");
      success = false;
    }

  return success;
}

bool
octave_float_matrix::save_binary (std::ostream& os, bool&)
{

  dim_vector d = dims ();
  if (d.length() < 1)
    return false;

  // Use negative value for ndims to differentiate with old format!!
  int32_t tmp = - d.length();
  os.write (reinterpret_cast<char *> (&tmp), 4);
  for (int i = 0; i < d.length (); i++)
    {
      tmp = d(i);
      os.write (reinterpret_cast<char *> (&tmp), 4);
    }

  FloatNDArray m = float_array_value ();
  save_type st = LS_FLOAT;
  if (d.numel () > 8192) // FIXME -- make this configurable.
    {
      float max_val, min_val;
      if (m.all_integers (max_val, min_val))
        st = get_save_type (max_val, min_val);
    }

  const float *mtmp = m.data ();
  write_floats (os, mtmp, st, d.numel ());

  return true;
}

bool
octave_float_matrix::load_binary (std::istream& is, bool swap,
                                 oct_mach_info::float_format fmt)
{
  char tmp;
  int32_t mdims;
  if (! is.read (reinterpret_cast<char *> (&mdims), 4))
    return false;
  if (swap)
    swap_bytes<4> (&mdims);
  if (mdims < 0)
    {
      mdims = - mdims;
      int32_t di;
      dim_vector dv;
      dv.resize (mdims);

      for (int i = 0; i < mdims; i++)
        {
          if (! is.read (reinterpret_cast<char *> (&di), 4))
            return false;
          if (swap)
            swap_bytes<4> (&di);
          dv(i) = di;
        }

      // Convert an array with a single dimension to be a row vector.
      // Octave should never write files like this, other software
      // might.

      if (mdims == 1)
        {
          mdims = 2;
          dv.resize (mdims);
          dv(1) = dv(0);
          dv(0) = 1;
        }

      if (! is.read (reinterpret_cast<char *> (&tmp), 1))
        return false;

      FloatNDArray m(dv);
      float *re = m.fortran_vec ();
      read_floats (is, re, static_cast<save_type> (tmp), dv.numel (), swap, fmt);
      if (error_state || ! is)
        return false;
      matrix = m;
    }
  else
    {
      int32_t nr, nc;
      nr = mdims;
      if (! is.read (reinterpret_cast<char *> (&nc), 4))
        return false;
      if (swap)
        swap_bytes<4> (&nc);
      if (! is.read (reinterpret_cast<char *> (&tmp), 1))
        return false;
      FloatMatrix m (nr, nc);
      float *re = m.fortran_vec ();
      octave_idx_type len = nr * nc;
      read_floats (is, re, static_cast<save_type> (tmp), len, swap, fmt);
      if (error_state || ! is)
        return false;
      matrix = m;
    }
  return true;
}

#if defined (HAVE_HDF5)

bool
octave_float_matrix::save_hdf5 (hid_t loc_id, const char *name, bool)
{
  dim_vector dv = dims ();
  int empty = save_hdf5_empty (loc_id, name, dv);
  if (empty)
    return (empty > 0);

  int rank = dv.length ();
  hid_t space_hid = -1, data_hid = -1;
  bool retval = true;
  FloatNDArray m = array_value ();

  OCTAVE_LOCAL_BUFFER (hsize_t, hdims, rank);

  // Octave uses column-major, while HDF5 uses row-major ordering
  for (int i = 0; i < rank; i++)
    hdims[i] = dv (rank-i-1);

  space_hid = H5Screate_simple (rank, hdims, 0);

  if (space_hid < 0) return false;

  hid_t save_type_hid = H5T_NATIVE_FLOAT;

#if HAVE_HDF5_INT2FLOAT_CONVERSIONS
  // hdf5 currently doesn't support float/integer conversions
  else
    {
      float max_val, min_val;

      if (m.all_integers (max_val, min_val))
        save_type_hid
          = save_type_to_hdf5 (get_save_type (max_val, min_val));
    }
#endif /* HAVE_HDF5_INT2FLOAT_CONVERSIONS */
#if HAVE_HDF5_18
  data_hid = H5Dcreate (loc_id, name, save_type_hid, space_hid,
                        H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
#else
  data_hid = H5Dcreate (loc_id, name, save_type_hid, space_hid,
                        H5P_DEFAULT);
#endif
  if (data_hid < 0)
    {
      H5Sclose (space_hid);
      return false;
    }

  float *mtmp = m.fortran_vec ();
  retval = H5Dwrite (data_hid, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL,
                     H5P_DEFAULT, mtmp) >= 0;

  H5Dclose (data_hid);
  H5Sclose (space_hid);

  return retval;
}

bool
octave_float_matrix::load_hdf5 (hid_t loc_id, const char *name)
{
  bool retval = false;

  dim_vector dv;
  int empty = load_hdf5_empty (loc_id, name, dv);
  if (empty > 0)
    matrix.resize(dv);
  if (empty)
      return (empty > 0);

#if HAVE_HDF5_18
  hid_t data_hid = H5Dopen (loc_id, name, H5P_DEFAULT);
#else
  hid_t data_hid = H5Dopen (loc_id, name);
#endif
  hid_t space_id = H5Dget_space (data_hid);

  hsize_t rank = H5Sget_simple_extent_ndims (space_id);

  if (rank < 1)
    {
      H5Sclose (space_id);
      H5Dclose (data_hid);
      return false;
    }

  OCTAVE_LOCAL_BUFFER (hsize_t, hdims, rank);
  OCTAVE_LOCAL_BUFFER (hsize_t, maxdims, rank);

  H5Sget_simple_extent_dims (space_id, hdims, maxdims);

  // Octave uses column-major, while HDF5 uses row-major ordering
  if (rank == 1)
    {
      dv.resize (2);
      dv(0) = 1;
      dv(1) = hdims[0];
    }
  else
    {
      dv.resize (rank);
      for (hsize_t i = 0, j = rank - 1; i < rank; i++, j--)
        dv(j) = hdims[i];
    }

  FloatNDArray m (dv);
  float *re = m.fortran_vec ();
  if (H5Dread (data_hid, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL,
               H5P_DEFAULT, re) >= 0)
    {
      retval = true;
      matrix = m;
    }

  H5Sclose (space_id);
  H5Dclose (data_hid);

  return retval;
}

#endif

void
octave_float_matrix::print_raw (std::ostream& os,
                          bool pr_as_read_syntax) const
{
  octave_print_internal (os, matrix, pr_as_read_syntax,
                         current_print_indent_level ());
}

mxArray *
octave_float_matrix::as_mxArray (void) const
{
  mxArray *retval = new mxArray (mxSINGLE_CLASS, dims (), mxREAL);

  float *pr = static_cast<float *> (retval->get_data ());

  mwSize nel = numel ();

  const float *p = matrix.data ();

  for (mwIndex i = 0; i < nel; i++)
    pr[i] = p[i];

  return retval;
}

// This uses a smarter strategy for doing the complex->real mappers.  We
// allocate an array for a real result and keep filling it until a complex
// result is produced.
static octave_value
do_rc_map (const FloatNDArray& a, FloatComplex (&fcn) (float))
{
  octave_idx_type n = a.numel ();
  NoAlias<FloatNDArray> rr (a.dims ());

  for (octave_idx_type i = 0; i < n; i++)
    {
      octave_quit ();

      FloatComplex tmp = fcn (a(i));
      if (tmp.imag () == 0.0)
        rr(i) = tmp.real ();
      else
        {
          NoAlias<FloatComplexNDArray> rc (a.dims ());

          for (octave_idx_type j = 0; j < i; j++)
            rc(j) = rr(j);

          rc(i) = tmp;

          for (octave_idx_type j = i+1; j < n; j++)
            {
              octave_quit ();

              rc(j) = fcn (a(j));
            }

          return new octave_float_complex_matrix (rc);
        }
    }

  return rr;
}

octave_value
octave_float_matrix::map (unary_mapper_t umap) const
{
  switch (umap)
    {
    case umap_imag:
      return FloatNDArray (matrix.dims (), 0.0);

    case umap_real:
    case umap_conj:
      return matrix;

    // Mappers handled specially.
#define ARRAY_METHOD_MAPPER(UMAP, FCN) \
    case umap_ ## UMAP: \
      return octave_value (matrix.FCN ())

      ARRAY_METHOD_MAPPER (abs, abs);
      ARRAY_METHOD_MAPPER (isnan, isnan);
      ARRAY_METHOD_MAPPER (isinf, isinf);
      ARRAY_METHOD_MAPPER (finite, isfinite);

#define ARRAY_MAPPER(UMAP, TYPE, FCN) \
    case umap_ ## UMAP: \
      return octave_value (matrix.map<TYPE> (FCN))

#define RC_ARRAY_MAPPER(UMAP, TYPE, FCN) \
    case umap_ ## UMAP: \
      return do_rc_map (matrix, FCN)

      RC_ARRAY_MAPPER (acos, FloatComplex, rc_acos);
      RC_ARRAY_MAPPER (acosh, FloatComplex, rc_acosh);
      ARRAY_MAPPER (angle, float, ::arg);
      ARRAY_MAPPER (arg, float, ::arg);
      RC_ARRAY_MAPPER (asin, FloatComplex, rc_asin);
      ARRAY_MAPPER (asinh, float, ::asinhf);
      ARRAY_MAPPER (atan, float, ::atanf);
      RC_ARRAY_MAPPER (atanh, FloatComplex, rc_atanh);
      ARRAY_MAPPER (erf, float, ::erff);
      ARRAY_MAPPER (erfinv, float, ::erfinv);
      ARRAY_MAPPER (erfc, float, ::erfcf);
      ARRAY_MAPPER (erfcx, float, ::erfcx);
      ARRAY_MAPPER (gamma, float, xgamma);
      RC_ARRAY_MAPPER (lgamma, FloatComplex, rc_lgamma);
      ARRAY_MAPPER (cbrt, float, ::cbrtf);
      ARRAY_MAPPER (ceil, float, ::ceilf);
      ARRAY_MAPPER (cos, float, ::cosf);
      ARRAY_MAPPER (cosh, float, ::coshf);
      ARRAY_MAPPER (exp, float, ::expf);
      ARRAY_MAPPER (expm1, float, ::expm1f);
      ARRAY_MAPPER (fix, float, ::fix);
      ARRAY_MAPPER (floor, float, ::floorf);
      RC_ARRAY_MAPPER (log, FloatComplex, rc_log);
      RC_ARRAY_MAPPER (log2, FloatComplex, rc_log2);
      RC_ARRAY_MAPPER (log10, FloatComplex, rc_log10);
      RC_ARRAY_MAPPER (log1p, FloatComplex, rc_log1p);
      ARRAY_MAPPER (round, float, xround);
      ARRAY_MAPPER (roundb, float, xroundb);
      ARRAY_MAPPER (signum, float, ::signum);
      ARRAY_MAPPER (sin, float, ::sinf);
      ARRAY_MAPPER (sinh, float, ::sinhf);
      RC_ARRAY_MAPPER (sqrt, FloatComplex, rc_sqrt);
      ARRAY_MAPPER (tan, float, ::tanf);
      ARRAY_MAPPER (tanh, float, ::tanhf);
      ARRAY_MAPPER (isna, bool, octave_is_NA);

    default:
      return octave_base_value::map (umap);
    }
}

DEFUN (single, args, ,
  "-*- texinfo -*-\n\
@deftypefn {Built-in Function} {} single (@var{x})\n\
Convert @var{x} to single precision type.\n\
@seealso{double}\n\
@end deftypefn")
{
  // The OCTAVE_TYPE_CONV_BODY3 macro declares retval, so they go
  // inside their own scopes, and we don't declare retval here to
  // avoid a shadowed declaration warning.

  if (args.length () == 1)
    {
      if (args(0).is_diag_matrix ())
        {
          if (args(0).is_complex_type ())
            {
              OCTAVE_TYPE_CONV_BODY3 (single, octave_float_complex_diag_matrix, octave_float_complex);
            }
          else
            {
              OCTAVE_TYPE_CONV_BODY3 (single, octave_float_diag_matrix, octave_float_scalar);
            }
        }
      else if (args(0).is_sparse_type ())
        {
          error ("single: sparse type does not support single precision");
        }
      else if (args(0).is_complex_type ())
        {
          OCTAVE_TYPE_CONV_BODY3 (single, octave_float_complex_matrix, octave_float_complex);
        }
      else
        {
          OCTAVE_TYPE_CONV_BODY3 (single, octave_float_matrix, octave_float_scalar);
        }
    }
  else
    print_usage ();

  return octave_value ();
}

/*

%!assert (class (single(1)), "single")
%!assert (class (single(1 + i)), "single")
%!assert (class (single (int8 (1))), "single")
%!assert (class (single (uint8 (1))), "single")
%!assert (class (single (int16 (1))), "single")
%!assert (class (single (uint16 (1))), "single")
%!assert (class (single (int32 (1))), "single")
%!assert (class (single (uint32 (1))), "single")
%!assert (class (single (int64 (1))), "single")
%!assert (class (single (uint64 (1))), "single")
%!assert (class (single (true)), "single")
%!assert (class (single ("A")), "single")
%!error (single (sparse (1)))
%!test
%! x = diag ([1 3 2]);
%! y = single (x);
%! assert (class (x), "double");
%! assert (class (y), "single");
%!test
%! x = diag ([i 3 2]);
%! y = single (x);
%! assert (class (x), "double");
%! assert (class (y), "single");

*/

