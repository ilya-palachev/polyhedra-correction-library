/*

Copyright (C) 2000-2012 John W. Eaton

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

/* This gives us a better chance of finding a prototype for strptime
   on some systems.  */

#if ! defined (_XOPEN_SOURCE)
#define _XOPEN_SOURCE
#endif

#include <sys/types.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lo-cutils.h"
#include "syswait.h"

OCTAVE_API void
octave_qsort (void *base, size_t n, size_t size,
              int (*cmp) (const void *, const void *))
{
  qsort (base, n, size, cmp);
}

OCTAVE_API int
octave_strcasecmp (const char *s1, const char *s2)
{
  return strcasecmp (s1, s2);
}

OCTAVE_API int
octave_strncasecmp (const char *s1, const char *s2, size_t n)
{
  return strncasecmp (s1, s2, n);
}

#ifdef HAVE_LOADLIBRARY_API
#include <windows.h>

/* Need this since in C++ can't cast from int(*)() to void* */
OCTAVE_API void *
octave_w32_library_search (HINSTANCE handle, const char * name)
{
  return (GetProcAddress (handle, name));
}
#endif

OCTAVE_API pid_t
octave_waitpid (pid_t pid, int *status, int options)
{
  return WAITPID (pid, status, options);
}
