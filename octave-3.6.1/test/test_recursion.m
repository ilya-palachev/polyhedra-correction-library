## Copyright (C) 2006-2012 John W. Eaton
##
## This file is part of Octave.
##
## Octave is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or (at
## your option) any later version.
##
## Octave is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Octave; see the file COPYING.  If not, see
## <http://www.gnu.org/licenses/>.

%% test/octave.test/recursion/recursion-1.m
%!function y = f (x)
%!  if (x == 1)
%!    y = x;
%!    return;
%!  else
%!    y = x * f (x-1);
%!  endif
%!endfunction
%!
%!assert(f (5), 120);

%% test/octave.test/recursion/recursion-2.m
%!function y = f (x)
%!  if (x == 1)
%!    y = x;
%!    return;
%!  else
%!    y = f (x-1) * x;
%!  endif
%!endfunction
%!
%!assert(f (5), 120);

%%FIXME: Need test for maximum recursion depth
