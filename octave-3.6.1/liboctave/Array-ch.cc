/*

Copyright (C) 1995-2012 John W. Eaton

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

// Instantiate Arrays of char values.

#include "Array.h"
#include "Array.cc"
#define INLINE_ASCENDING_SORT
#define INLINE_DESCENDING_SORT
#include "oct-sort.cc"

INSTANTIATE_ARRAY_SORT (char);

INSTANTIATE_ARRAY (char, OCTAVE_API);

template OCTAVE_API std::ostream& operator << (std::ostream&, const Array<char>&);

#include "DiagArray2.h"
#include "DiagArray2.cc"

template class OCTAVE_API DiagArray2<char>;
