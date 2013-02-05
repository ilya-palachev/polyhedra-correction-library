/*

Copyright (C) 2004-2012 John W. Eaton

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

#include "lo-ieee.h"
#include "lo-utils.h"
#include "mx-base.h"
#include "quit.h"

#include "defun.h"
#include "gripes.h"
#include "oct-obj.h"
#include "oct-lvalue.h"
#include "ops.h"
#include "ov-base.h"

#ifdef HAVE_HDF5
#define HDF5_SAVE_TYPE H5T_NATIVE_INT16
#endif

#include "ov-base-int.h"
#include "ov-base-int.cc"
#include "ov-int16.h"
#include "ov-type-conv.h"
#include "pr-output.h"
#include "variables.h"

#include "byte-swap.h"
#include "ls-oct-ascii.h"
#include "ls-utils.h"
#include "ls-hdf5.h"

template class octave_base_matrix<int16NDArray>;

template class octave_base_int_matrix<int16NDArray>;

DEFINE_OCTAVE_ALLOCATOR (octave_int16_matrix);

DEFINE_OV_TYPEID_FUNCTIONS_AND_DATA (octave_int16_matrix,
                                     "int16 matrix", "int16");

template class octave_base_scalar<octave_int16>;

template class octave_base_int_scalar<octave_int16>;

DEFINE_OCTAVE_ALLOCATOR (octave_int16_scalar);

DEFINE_OV_TYPEID_FUNCTIONS_AND_DATA (octave_int16_scalar,
                                     "int16 scalar", "int16");

DEFUN (int16, args, ,
  "-*- texinfo -*-\n\
@deftypefn {Built-in Function} {} int16 (@var{x})\n\
Convert @var{x} to 16-bit integer type.\n\
@end deftypefn")
{
  OCTAVE_TYPE_CONV_BODY (int16);
}

/*

%!assert (class (int16 (1)), "int16")
%!assert (int16 (1.25), int16 (1))
%!assert (int16 (1.5), int16 (2))
%!assert (int16 (-1.5), int16 (-2))
%!assert (int16 (2^17), int16 (2^16-1))
%!assert (int16 (-2^17), int16 (-2^16))

*/

