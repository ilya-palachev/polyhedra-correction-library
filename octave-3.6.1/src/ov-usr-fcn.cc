/*

Copyright (C) 1996-2012 John W. Eaton

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

#include <sstream>

#include "str-vec.h"

#include <defaults.h>
#include "Cell.h"
#include "defun.h"
#include "error.h"
#include "gripes.h"
#include "input.h"
#include "oct-obj.h"
#include "ov-usr-fcn.h"
#include "ov.h"
#include "pager.h"
#include "pt-eval.h"
#include "pt-jump.h"
#include "pt-misc.h"
#include "pt-pr-code.h"
#include "pt-stmt.h"
#include "pt-walk.h"
#include "symtab.h"
#include "toplev.h"
#include "unwind-prot.h"
#include "utils.h"
#include "parse.h"
#include "profiler.h"
#include "variables.h"

// Whether to optimize subsasgn method calls.
static bool Voptimize_subsasgn_calls = true;

// User defined scripts.

DEFINE_OCTAVE_ALLOCATOR (octave_user_script);

DEFINE_OV_TYPEID_FUNCTIONS_AND_DATA (octave_user_script,
                                     "user-defined script",
                                     "user-defined script");

octave_user_script::octave_user_script (void)
  : octave_user_code (), cmd_list (0), file_name (),
    t_parsed (static_cast<time_t> (0)),
    t_checked (static_cast<time_t> (0)),
    call_depth (-1)
{ }

octave_user_script::octave_user_script (const std::string& fnm,
                                        const std::string& nm,
                                        tree_statement_list *cmds,
                                        const std::string& ds)
  : octave_user_code (nm, ds), cmd_list (cmds), file_name (fnm),
    t_parsed (static_cast<time_t> (0)),
    t_checked (static_cast<time_t> (0)),
    call_depth (-1)
{
  if (cmd_list)
    cmd_list->mark_as_script_body ();
}

octave_user_script::octave_user_script (const std::string& fnm,
                                        const std::string& nm,
                                        const std::string& ds)
  : octave_user_code (nm, ds), cmd_list (0), file_name (fnm),
    t_parsed (static_cast<time_t> (0)),
    t_checked (static_cast<time_t> (0)),
    call_depth (-1)
{ }

octave_user_script::~octave_user_script (void)
{
  delete cmd_list;
}

octave_value_list
octave_user_script::subsref (const std::string&,
                             const std::list<octave_value_list>&, int)
{
  octave_value_list retval;

  ::error ("invalid use of script %s in index expression", file_name.c_str ());

  return retval;
}

octave_value_list
octave_user_script::do_multi_index_op (int nargout,
                                       const octave_value_list& args)
{
  octave_value_list retval;

  unwind_protect frame;

  if (! error_state)
    {
      if (args.length () == 0 && nargout == 0)
        {
          if (cmd_list)
            {
              frame.protect_var (call_depth);
              call_depth++;

              if (call_depth < Vmax_recursion_depth)
                {
                  octave_call_stack::push (this);

                  frame.add_fcn (octave_call_stack::pop);

                  frame.protect_var (tree_evaluator::statement_context);
                  tree_evaluator::statement_context = tree_evaluator::script;

                  BEGIN_PROFILER_BLOCK (profiler_name ())
                  cmd_list->accept (*current_evaluator);
                  END_PROFILER_BLOCK

                  if (tree_return_command::returning)
                    tree_return_command::returning = 0;

                  if (tree_break_command::breaking)
                    tree_break_command::breaking--;

                  if (error_state)
                    octave_call_stack::backtrace_error_message ();
                }
              else
                ::error ("max_recursion_depth exceeded");
            }
        }
      else
        error ("invalid call to script %s", file_name.c_str ());
    }

  return retval;
}

void
octave_user_script::accept (tree_walker& tw)
{
  tw.visit_octave_user_script (*this);
}

// User defined functions.

DEFINE_OCTAVE_ALLOCATOR (octave_user_function);

DEFINE_OV_TYPEID_FUNCTIONS_AND_DATA (octave_user_function,
                                     "user-defined function",
                                     "user-defined function");

// Ugh.  This really needs to be simplified (code/data?
// extrinsic/intrinsic state?).

octave_user_function::octave_user_function
  (symbol_table::scope_id sid, tree_parameter_list *pl,
   tree_parameter_list *rl, tree_statement_list *cl)
  : octave_user_code (std::string (), std::string ()),
    param_list (pl), ret_list (rl), cmd_list (cl),
    lead_comm (), trail_comm (), file_name (),
    location_line (0), location_column (0),
    parent_name (), t_parsed (static_cast<time_t> (0)),
    t_checked (static_cast<time_t> (0)),
    system_fcn_file (false), call_depth (-1),
    num_named_args (param_list ? param_list->length () : 0),
    subfunction (false), inline_function (false),
    anonymous_function (false), class_constructor (false),
    class_method (false), parent_scope (-1), local_scope (sid),
    curr_unwind_protect_frame (0)
{
  if (cmd_list)
    cmd_list->mark_as_function_body ();

  if (local_scope >= 0)
    symbol_table::set_curr_fcn (this, local_scope);
}

octave_user_function::~octave_user_function (void)
{
  delete param_list;
  delete ret_list;
  delete cmd_list;
  delete lead_comm;
  delete trail_comm;

  symbol_table::erase_scope (local_scope);
}

octave_user_function *
octave_user_function::define_ret_list (tree_parameter_list *t)
{
  ret_list = t;

  return this;
}

void
octave_user_function::stash_fcn_file_name (const std::string& nm)
{
  file_name = nm;
}

std::string
octave_user_function::profiler_name (void) const
{
  std::ostringstream result;

  if (is_inline_function ())
    result << "inline@" << fcn_file_name ()
           << ":" << location_line << ":" << location_column;
  else if (is_anonymous_function ())
    result << "anonymous@" << fcn_file_name ()
           << ":" << location_line << ":" << location_column;
  else if (is_subfunction ())
    result << parent_fcn_name () << ">" << name ();
  else
    result << name ();

  return result.str ();
}

void
octave_user_function::mark_as_system_fcn_file (void)
{
  if (! file_name.empty ())
    {
      // We really should stash the whole path to the file we found,
      // when we looked it up, to avoid possible race conditions...
      // FIXME
      //
      // We probably also don't need to get the library directory
      // every time, but since this function is only called when the
      // function file is parsed, it probably doesn't matter that
      // much.

      std::string ff_name = fcn_file_in_path (file_name);

      if (Vfcn_file_dir == ff_name.substr (0, Vfcn_file_dir.length ()))
        system_fcn_file = 1;
    }
  else
    system_fcn_file = 0;
}

bool
octave_user_function::takes_varargs (void) const
{
  return (param_list && param_list->takes_varargs ());
}

bool
octave_user_function::takes_var_return (void) const
{
  return (ret_list && ret_list->takes_varargs ());
}

void
octave_user_function::lock_subfunctions (void)
{
  symbol_table::lock_subfunctions (local_scope);
}

void
octave_user_function::unlock_subfunctions (void)
{
  symbol_table::unlock_subfunctions (local_scope);
}

octave_value_list
octave_user_function::all_va_args (const octave_value_list& args)
{
  octave_value_list retval;

  octave_idx_type n = args.length () - num_named_args;

  if (n > 0)
    retval = args.slice (num_named_args, n);

  return retval;
}

octave_value_list
octave_user_function::subsref (const std::string& type,
                               const std::list<octave_value_list>& idx,
                               int nargout)
{
  return octave_user_function::subsref (type, idx, nargout, 0);
}

octave_value_list
octave_user_function::subsref (const std::string& type,
                               const std::list<octave_value_list>& idx,
                               int nargout, const std::list<octave_lvalue>* lvalue_list)
{
  octave_value_list retval;

  switch (type[0])
    {
    case '(':
      {
        int tmp_nargout = (type.length () > 1 && nargout == 0) ? 1 : nargout;

        retval = do_multi_index_op (tmp_nargout, idx.front (),
                                    idx.size () == 1 ? lvalue_list : 0);
      }
      break;

    case '{':
    case '.':
      {
        std::string nm = type_name ();
        error ("%s cannot be indexed with %c", nm.c_str (), type[0]);
      }
      break;

    default:
      panic_impossible ();
    }

  // FIXME -- perhaps there should be an
  // octave_value_list::next_subsref member function?  See also
  // octave_builtin::subsref.

  if (idx.size () > 1)
    retval = retval(0).next_subsref (nargout, type, idx);

  return retval;
}

octave_value_list
octave_user_function::do_multi_index_op (int nargout,
                                         const octave_value_list& args)
{
  return do_multi_index_op (nargout, args, 0);
}

octave_value_list
octave_user_function::do_multi_index_op (int nargout,
                                         const octave_value_list& args,
                                         const std::list<octave_lvalue>* lvalue_list)
{
  octave_value_list retval;

  if (error_state)
    return retval;

  if (! cmd_list)
    return retval;

  int nargin = args.length ();

  unwind_protect frame;

  frame.protect_var (call_depth);
  call_depth++;

  if (call_depth >= Vmax_recursion_depth)
    {
      ::error ("max_recursion_depth exceeded");
      return retval;
    }

  // Save old and set current symbol table context, for
  // eval_undefined_error().

  octave_call_stack::push (this, local_scope, call_depth);
  frame.add_fcn (octave_call_stack::pop);

  if (call_depth > 0)
    {
      symbol_table::push_context ();

      frame.add_fcn (symbol_table::pop_context);
    }

  string_vector arg_names = args.name_tags ();

  if (param_list && ! param_list->varargs_only ())
    {
      param_list->define_from_arg_vector (args);
      if (error_state)
        return retval;
    }

  // Force parameter list to be undefined when this function exits.
  // Doing so decrements the reference counts on the values of local
  // variables that are also named function parameters.

  if (param_list)
    frame.add_method (param_list, &tree_parameter_list::undefine);

  // Force return list to be undefined when this function exits.
  // Doing so decrements the reference counts on the values of local
  // variables that are also named values returned by this function.

  if (ret_list)
    frame.add_method (ret_list, &tree_parameter_list::undefine);

  if (call_depth == 0)
    {
      // Force symbols to be undefined again when this function
      // exits.
      //
      // This cleanup function is added to the unwind_protect stack
      // after the calls to clear the parameter lists so that local
      // variables will be cleared before the parameter lists are
      // cleared.  That way, any function parameters that have been
      // declared global will be unmarked as global before they are
      // undefined by the clear_param_list cleanup function.

      frame.add_fcn (symbol_table::clear_variables);
    }

  bind_automatic_vars (arg_names, nargin, nargout, all_va_args (args),
                       lvalue_list);

  bool echo_commands = (Vecho_executing_commands & ECHO_FUNCTIONS);

  if (echo_commands)
    print_code_function_header ();

  // Set pointer to the current unwind_protect frame to allow
  // certain builtins register simple cleanup in a very optimized manner.
  // This is *not* intended as a general-purpose on-cleanup mechanism,
  frame.protect_var (curr_unwind_protect_frame);
  curr_unwind_protect_frame = &frame;

  // Evaluate the commands that make up the function.

  frame.protect_var (tree_evaluator::statement_context);
  tree_evaluator::statement_context = tree_evaluator::function;

  bool special_expr = (is_inline_function () || is_anonymous_function ());

  BEGIN_PROFILER_BLOCK (profiler_name ())

  if (special_expr)
    {
      assert (cmd_list->length () == 1);

      tree_statement *stmt = 0;

      if ((stmt = cmd_list->front ())
          && stmt->is_expression ())
        {
          tree_expression *expr = stmt->expression ();

          retval = expr->rvalue (nargout);
        }
    }
  else
    cmd_list->accept (*current_evaluator);

  END_PROFILER_BLOCK

  if (echo_commands)
    print_code_function_trailer ();

  if (tree_return_command::returning)
    tree_return_command::returning = 0;

  if (tree_break_command::breaking)
    tree_break_command::breaking--;

  if (error_state)
    {
      octave_call_stack::backtrace_error_message ();
      return retval;
    }

  // Copy return values out.

  if (ret_list && ! special_expr)
    {
      ret_list->initialize_undefined_elements (my_name, nargout, Matrix ());

      Cell varargout;

      if (ret_list->takes_varargs ())
        {
          octave_value varargout_varval = symbol_table::varval ("varargout");

          if (varargout_varval.is_defined ())
            {
              varargout = varargout_varval.cell_value ();

              if (error_state)
                error ("expecting varargout to be a cell array object");
            }
        }

      if (! error_state)
        retval = ret_list->convert_to_const_vector (nargout, varargout);
    }

  return retval;
}

void
octave_user_function::accept (tree_walker& tw)
{
  tw.visit_octave_user_function (*this);
}

bool
octave_user_function::subsasgn_optimization_ok (void)
{
  bool retval = false;
  if (Voptimize_subsasgn_calls
      && param_list->length () > 0 && ! param_list->varargs_only ()
      && ret_list->length () == 1 && ! ret_list->takes_varargs ())
    {
      tree_identifier *par1 = param_list->front ()->ident ();
      tree_identifier *ret1 = ret_list->front ()->ident ();
      retval = par1->name () == ret1->name ();
    }

  return retval;
}

#if 0
void
octave_user_function::print_symtab_info (std::ostream& os) const
{
  symbol_table::print_info (os, local_scope);
}
#endif

void
octave_user_function::print_code_function_header (void)
{
  tree_print_code tpc (octave_stdout, VPS4);

  tpc.visit_octave_user_function_header (*this);
}

void
octave_user_function::print_code_function_trailer (void)
{
  tree_print_code tpc (octave_stdout, VPS4);

  tpc.visit_octave_user_function_trailer (*this);
}

void
octave_user_function::bind_automatic_vars
  (const string_vector& arg_names, int nargin, int nargout,
   const octave_value_list& va_args, const std::list<octave_lvalue> *lvalue_list)
{
  if (! arg_names.empty ())
    {
      // It is better to save this in the hidden variable .argn. and
      // then use that in the inputname function instead of using argn,
      // which might be redefined in a function.  Keep the old argn name
      // for backward compatibility of functions that use it directly.

      symbol_table::varref ("argn") = arg_names;
      symbol_table::varref (".argn.") = Cell (arg_names);

      symbol_table::mark_hidden (".argn.");

      symbol_table::mark_automatic ("argn");
      symbol_table::mark_automatic (".argn.");
    }

  symbol_table::varref (".nargin.") = nargin;
  symbol_table::varref (".nargout.") = nargout;

  symbol_table::mark_hidden (".nargin.");
  symbol_table::mark_hidden (".nargout.");

  symbol_table::mark_automatic (".nargin.");
  symbol_table::mark_automatic (".nargout.");

  if (takes_varargs ())
    symbol_table::varref ("varargin") = va_args.cell_value ();

  if (lvalue_list)
    {
      octave_idx_type nbh = 0;
      for (std::list<octave_lvalue>::const_iterator p = lvalue_list->begin ();
           p != lvalue_list->end (); p++)
        nbh += p->is_black_hole ();

      if (nbh > 0)
        {
          // Only assign the hidden variable if black holes actually present.
          Matrix bh (1, nbh);
          octave_idx_type k = 0, l = 0;
          for (std::list<octave_lvalue>::const_iterator p = lvalue_list->begin ();
               p != lvalue_list->end (); p++)
            {
              if (p->is_black_hole ())
                bh(l++) = k+1;
              k += p->numel ();
            }

          symbol_table::varref (".ignored.") = bh;

          symbol_table::mark_hidden (".ignored.");
          symbol_table::mark_automatic (".ignored.");
        }
    }
}

DEFUN (nargin, args, ,
  "-*- texinfo -*-\n\
@deftypefn  {Built-in Function} {} nargin ()\n\
@deftypefnx {Built-in Function} {} nargin (@var{fcn_name})\n\
Within a function, return the number of arguments passed to the function.\n\
At the top level, return the number of command line arguments passed to\n\
Octave.  If called with the optional argument @var{fcn_name}, return the\n\
maximum number of arguments the named function can accept, or -1 if the\n\
function accepts a variable number of arguments.\n\
@seealso{nargout, varargin, isargout, varargout, nthargout}\n\
@end deftypefn")
{
  octave_value retval;

  int nargin = args.length ();

  if (nargin == 1)
    {
      std::string fname = args(0).string_value ();

      if (! error_state)
        {
          octave_value fcn_val = symbol_table::find_function (fname);

          if (fcn_val.is_user_function ())
            {
              octave_user_function *fcn = fcn_val.user_function_value (true);

              if (fcn)
                {
                  if (fcn->takes_varargs ())
                    retval = -1;
                  else
                    {
                      tree_parameter_list *param_list = fcn->parameter_list ();

                      retval = param_list ? param_list->length () : 0;
                    }
                }
              else
                error ("nargin: loading user-defined function failed");
            }
          else
            {
              // FIXME -- what about built-in functions or functions
              // defined in .oct files or .mex files?

              error ("nargin: FCN_NAME must be a user-defined function");
            }
        }
      else
        error ("nargin: FCN_NAME must be a string");
    }
  else if (nargin == 0)
    {
      retval = symbol_table::varval (".nargin.");

      if (retval.is_undefined ())
        retval = 0;
    }
  else
    print_usage ();

  return retval;
}

DEFUN (nargout, args, ,
  "-*- texinfo -*-\n\
@deftypefn  {Built-in Function} {} nargout ()\n\
@deftypefnx {Built-in Function} {} nargout (@var{fcn_name})\n\
Within a function, return the number of values the caller expects to\n\
receive.  If called with the optional argument @var{fcn_name}, return the\n\
maximum number of values the named function can produce, or -1 if the\n\
function can produce a variable number of values.\n\
\n\
For example,\n\
\n\
@example\n\
f ()\n\
@end example\n\
\n\
@noindent\n\
will cause @code{nargout} to return 0 inside the function @code{f} and\n\
\n\
@example\n\
[s, t] = f ()\n\
@end example\n\
\n\
@noindent\n\
will cause @code{nargout} to return 2 inside the function\n\
@code{f}.\n\
\n\
At the top level, @code{nargout} is undefined.\n\
@seealso{nargin, varargin, isargout, varargout, nthargout}\n\
@end deftypefn")
{
  octave_value retval;

  int nargin = args.length ();

  if (nargin == 1)
    {
      std::string fname = args(0).string_value ();

      if (! error_state)
        {
          octave_value fcn_val = symbol_table::find_user_function (fname);

          octave_user_function *fcn = fcn_val.user_function_value (true);

          if (fcn)
            {
              if (fcn->takes_var_return ())
                retval = -1;
              else
                {
                  tree_parameter_list *ret_list = fcn->return_list ();

                  retval = ret_list ? ret_list->length () : 0;
                }
            }
          else
            error ("nargout: invalid function");
        }
      else
        error ("nargout: FCN_NAME must be a string");
    }
  else if (nargin == 0)
    {
      if (! symbol_table::at_top_level ())
        {
          retval = symbol_table::varval (".nargout.");

          if (retval.is_undefined ())
            retval = 0;
        }
      else
        error ("nargout: invalid call at top level");
    }
  else
    print_usage ();

  return retval;
}

DEFUN (optimize_subsasgn_calls, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn  {Built-in Function} {@var{val} =} optimize_subsasgn_calls ()\n\
@deftypefnx {Built-in Function} {@var{old_val} =} optimize_subsasgn_calls (@var{new_val})\n\
@deftypefnx {Built-in Function} {} optimize_subsasgn_calls (@var{new_val}, \"local\")\n\
Query or set the internal flag for subsasgn method call optimizations.\n\
If true, Octave will attempt to eliminate the redundant copying when calling\n\
subsasgn method of a user-defined class.\n\
\n\
When called from inside a function with the \"local\" option, the variable is\n\
changed locally for the function and any subroutines it calls.  The original\n\
variable value is restored when exiting the function.\n\
@end deftypefn")
{
  return SET_INTERNAL_VARIABLE (optimize_subsasgn_calls);
}

static bool val_in_table (const Matrix& table, double val)
{
  if (table.is_empty ())
    return false;

  octave_idx_type i = table.lookup (val, ASCENDING);
  return (i > 0 && table(i-1) == val);
}

static bool isargout1 (int nargout, const Matrix& ignored, double k)
{
  if (k != xround (k) || k <= 0)
    {
      error ("isargout: K must be a positive integer");
      return false;
    }
  else
    return (k == 1 || k <= nargout) && ! val_in_table (ignored, k);
}

DEFUN (isargout, args, ,
  "-*- texinfo -*-\n\
@deftypefn {Built-in Function} {} isargout (@var{k})\n\
Within a function, return a logical value indicating whether the argument\n\
@var{k} will be assigned on output to a variable.  If the result is false,\n\
the argument has been ignored during the function call through the use of\n\
the tilde (~) special output argument.  Functions can use @code{isargout} to\n\
avoid performing unnecessary calculations for outputs which are unwanted.\n\
\n\
If @var{k} is outside the range @code{1:max(nargout)}, the function returns\n\
false.  @var{k} can also be an array, in which case the function works\n\
element-by-element and a logical array is returned.  At the top level,\n\
@code{isargout} returns an error.\n\
@seealso{nargout, nargin, varargin, varargout, nthargout}\n\
@end deftypefn")
{
  octave_value retval;

  int nargin = args.length ();

  if (nargin == 1)
    {
      if (! symbol_table::at_top_level ())
        {
          int nargout1 = symbol_table::varval (".nargout.").int_value ();
          if (error_state)
            {
              error ("isargout: internal error");
              return retval;
            }

          Matrix ignored;
          octave_value tmp = symbol_table::varval (".ignored.");
          if (tmp.is_defined ())
            ignored = tmp.matrix_value ();

          if (args(0).is_scalar_type ())
            {
              double k = args(0).double_value ();
              if (! error_state)
                retval = isargout1 (nargout1, ignored, k);
            }
          else if (args(0).is_numeric_type ())
            {
              const NDArray ka = args(0).array_value ();
              if (! error_state)
                {
                  boolNDArray r (ka.dims ());
                  for (octave_idx_type i = 0; i < ka.numel () && ! error_state; i++)
                    r(i) = isargout1 (nargout1, ignored, ka(i));

                  retval = r;
                }
            }
          else
            gripe_wrong_type_arg ("isargout", args(0));
        }
      else
        error ("isargout: invalid call at top level");
    }
  else
    print_usage ();

  return retval;
}
