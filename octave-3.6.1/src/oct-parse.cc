/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         octave_parse
#define yylex           octave_lex
#define yyerror         octave_error
#define yylval          octave_lval
#define yychar          octave_char
#define yydebug         octave_debug
#define yynerrs         octave_nerrs


/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 29 "oct-parse.yy"

#define YYDEBUG 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <map>
#include <sstream>

#include "Cell.h"
#include "Matrix.h"
#include "cmd-edit.h"
#include "cmd-hist.h"
#include "file-ops.h"
#include "file-stat.h"
#include "oct-env.h"
#include "oct-time.h"
#include "quit.h"

#include "comment-list.h"
#include "defaults.h"
#include "defun.h"
#include "dirfns.h"
#include "dynamic-ld.h"
#include "error.h"
#include "input.h"
#include "lex.h"
#include "load-path.h"
#include "oct-hist.h"
#include "oct-map.h"
#include "ov-fcn-handle.h"
#include "ov-usr-fcn.h"
#include "ov-null-mat.h"
#include "toplev.h"
#include "pager.h"
#include "parse.h"
#include "pt-all.h"
#include "pt-eval.h"
#include "symtab.h"
#include "token.h"
#include "unwind-prot.h"
#include "utils.h"
#include "variables.h"

#if defined (GNULIB_NAMESPACE)
// Calls to the following functions appear in the generated output from
// Bison without the namespace tag.  Redefine them so we will use them
// via the gnulib namespace.
#define fclose GNULIB_NAMESPACE::fclose
#define fprintf GNULIB_NAMESPACE::fprintf
#define malloc GNULIB_NAMESPACE::malloc
#endif

// The current input line number.
int input_line_number = 1;

// The column of the current token.
int current_input_column = 1;

// Buffer for help text snagged from function files.
std::stack<std::string> help_buf;

// Buffer for comments appearing before a function statement.
static std::string fcn_comment_header;

// TRUE means we are using readline.
// (--no-line-editing)
bool line_editing = true;

// TRUE means we printed messages about reading startup files.
bool reading_startup_message_printed = false;

// TRUE means input is coming from startup file.
bool input_from_startup_file = false;

// = 0 currently outside any function.
// = 1 inside the primary function or a subfunction.
// > 1 means we are looking at a function definition that seems to be
//     inside a function. Note that the function still might not be a
//     nested function.
static int current_function_depth = 0;

// Maximum function depth detected. Just here to determine whether
// we have nested functions or just implicitly ended subfunctions.
static int max_function_depth = 0;

// FALSE if we are still at the primary function. Subfunctions can
// only be declared inside function files.
static int parsing_subfunctions = false;

// Have we found an explicit end to a function?
static bool endfunction_found = false;

// Keep track of symbol table information when parsing functions.
std::stack<symbol_table::scope_id> symtab_context;

// Name of the current class when we are parsing class methods or
// constructors.
std::string current_class_name;

// TRUE means we are in the process of autoloading a function.
static bool autoloading = false;

// TRUE means the current function file was found in a relative path
// element.
static bool fcn_file_from_relative_lookup = false;

// Pointer to the primary user function or user script function.
static octave_function *primary_fcn_ptr = 0;

// Scope where we install all subfunctions and nested functions. Only
// used while reading function files.
static symbol_table::scope_id primary_fcn_scope;

// List of autoloads (function -> file mapping).
static std::map<std::string, std::string> autoload_map;

// Forward declarations for some functions defined at the bottom of
// the file.

// Generic error messages.
static void
yyerror (const char *s);

// Error mesages for mismatched end tokens.
static void
end_error (const char *type, token::end_tok_type ettype, int l, int c);

// Check to see that end tokens are properly matched.
static bool
end_token_ok (token *tok, token::end_tok_type expected);

// Maybe print a warning if an assignment expression is used as the
// test in a logical expression.
static void
maybe_warn_assign_as_truth_value (tree_expression *expr);

// Maybe print a warning about switch labels that aren't constants.
static void
maybe_warn_variable_switch_label (tree_expression *expr);

// Finish building a range.
static tree_expression *
finish_colon_expression (tree_colon_expression *e);

// Build a constant.
static tree_constant *
make_constant (int op, token *tok_val);

// Build a function handle.
static tree_fcn_handle *
make_fcn_handle (token *tok_val);

// Build an anonymous function handle.
static tree_anon_fcn_handle *
make_anon_fcn_handle (tree_parameter_list *param_list, tree_statement *stmt);

// Build a binary expression.
static tree_expression *
make_binary_op (int op, tree_expression *op1, token *tok_val,
                tree_expression *op2);

// Build a boolean expression.
static tree_expression *
make_boolean_op (int op, tree_expression *op1, token *tok_val,
                 tree_expression *op2);

// Build a prefix expression.
static tree_expression *
make_prefix_op (int op, tree_expression *op1, token *tok_val);

// Build a postfix expression.
static tree_expression *
make_postfix_op (int op, tree_expression *op1, token *tok_val);

// Build an unwind-protect command.
static tree_command *
make_unwind_command (token *unwind_tok, tree_statement_list *body,
                     tree_statement_list *cleanup, token *end_tok,
                     octave_comment_list *lc, octave_comment_list *mc);

// Build a try-catch command.
static tree_command *
make_try_command (token *try_tok, tree_statement_list *body,
                  tree_statement_list *cleanup, token *end_tok,
                  octave_comment_list *lc, octave_comment_list *mc);

// Build a while command.
static tree_command *
make_while_command (token *while_tok, tree_expression *expr,
                    tree_statement_list *body, token *end_tok,
                    octave_comment_list *lc);

// Build a do-until command.
static tree_command *
make_do_until_command (token *until_tok, tree_statement_list *body,
                       tree_expression *expr, octave_comment_list *lc);

// Build a for command.
static tree_command *
make_for_command (int tok_id, token *for_tok, tree_argument_list *lhs,
                  tree_expression *expr, tree_expression *maxproc,
                  tree_statement_list *body, token *end_tok,
                  octave_comment_list *lc);

// Build a break command.
static tree_command *
make_break_command (token *break_tok);

// Build a continue command.
static tree_command *
make_continue_command (token *continue_tok);

// Build a return command.
static tree_command *
make_return_command (token *return_tok);

// Start an if command.
static tree_if_command_list *
start_if_command (tree_expression *expr, tree_statement_list *list);

// Finish an if command.
static tree_if_command *
finish_if_command (token *if_tok, tree_if_command_list *list,
                   token *end_tok, octave_comment_list *lc);

// Build an elseif clause.
static tree_if_clause *
make_elseif_clause (token *elseif_tok, tree_expression *expr,
                    tree_statement_list *list, octave_comment_list *lc);

// Finish a switch command.
static tree_switch_command *
finish_switch_command (token *switch_tok, tree_expression *expr,
                       tree_switch_case_list *list, token *end_tok,
                       octave_comment_list *lc);

// Build a switch case.
static tree_switch_case *
make_switch_case (token *case_tok, tree_expression *expr,
                  tree_statement_list *list, octave_comment_list *lc);

// Build an assignment to a variable.
static tree_expression *
make_assign_op (int op, tree_argument_list *lhs, token *eq_tok,
                tree_expression *rhs);

// Define a script.
static void
make_script (tree_statement_list *cmds, tree_statement *end_script);

// Begin defining a function.
static octave_user_function *
start_function (tree_parameter_list *param_list, tree_statement_list *body,
                tree_statement *end_function);

// Create a no-op statement for end_function.
static tree_statement *
make_end (const std::string& type, int l, int c);

// Do most of the work for defining a function.
static octave_user_function *
frob_function (const std::string& fname, octave_user_function *fcn);

// Finish defining a function.
static tree_function_def *
finish_function (tree_parameter_list *ret_list,
                 octave_user_function *fcn, octave_comment_list *lc);

// Reset state after parsing function.
static void
recover_from_parsing_function (void);

// Make an index expression.
static tree_index_expression *
make_index_expression (tree_expression *expr,
                       tree_argument_list *args, char type);

// Make an indirect reference expression.
static tree_index_expression *
make_indirect_ref (tree_expression *expr, const std::string&);

// Make an indirect reference expression with dynamic field name.
static tree_index_expression *
make_indirect_ref (tree_expression *expr, tree_expression *field);

// Make a declaration command.
static tree_decl_command *
make_decl_command (int tok, token *tok_val, tree_decl_init_list *lst);

// Validate argument list forming a matrix or cell row.
static tree_argument_list *
validate_matrix_row (tree_argument_list *row);

// Finish building a matrix list.
static tree_expression *
finish_matrix (tree_matrix *m);

// Finish building a cell list.
static tree_expression *
finish_cell (tree_cell *c);

// Maybe print a warning.  Duh.
static void
maybe_warn_missing_semi (tree_statement_list *);

// Set the print flag for a statement based on the separator type.
static tree_statement_list *
set_stmt_print_flag (tree_statement_list *, char, bool);

// Create a statement list.
static tree_statement_list *make_statement_list (tree_statement *stmt);

// Append a statement to an existing statement list.
static tree_statement_list *
append_statement_list (tree_statement_list *list, char sep,
                       tree_statement *stmt, bool warn_missing_semi);

// Finish building a statement.
template <class T>
static tree_statement *
make_statement (T *arg)
{
  octave_comment_list *comment = octave_comment_buffer::get_comment ();

  return new tree_statement (arg, comment);
}

#define ABORT_PARSE \
  do \
    { \
      global_command = 0; \
      yyerrok; \
      if (! symtab_context.empty ()) \
        { \
          symbol_table::set_scope (symtab_context.top ()); \
          symtab_context.pop (); \
        } \
      if (interactive || forced_interactive) \
        YYACCEPT; \
      else \
        YYABORT; \
    } \
  while (0)



/* Line 268 of yacc.c  */
#line 433 "oct-parse.cc"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ADD_EQ = 258,
     SUB_EQ = 259,
     MUL_EQ = 260,
     DIV_EQ = 261,
     LEFTDIV_EQ = 262,
     POW_EQ = 263,
     EMUL_EQ = 264,
     EDIV_EQ = 265,
     ELEFTDIV_EQ = 266,
     EPOW_EQ = 267,
     AND_EQ = 268,
     OR_EQ = 269,
     LSHIFT_EQ = 270,
     RSHIFT_EQ = 271,
     LSHIFT = 272,
     RSHIFT = 273,
     EXPR_AND_AND = 274,
     EXPR_OR_OR = 275,
     EXPR_AND = 276,
     EXPR_OR = 277,
     EXPR_NOT = 278,
     EXPR_LT = 279,
     EXPR_LE = 280,
     EXPR_EQ = 281,
     EXPR_NE = 282,
     EXPR_GE = 283,
     EXPR_GT = 284,
     LEFTDIV = 285,
     EMUL = 286,
     EDIV = 287,
     ELEFTDIV = 288,
     EPLUS = 289,
     EMINUS = 290,
     QUOTE = 291,
     TRANSPOSE = 292,
     PLUS_PLUS = 293,
     MINUS_MINUS = 294,
     POW = 295,
     EPOW = 296,
     NUM = 297,
     IMAG_NUM = 298,
     STRUCT_ELT = 299,
     NAME = 300,
     END = 301,
     DQ_STRING = 302,
     SQ_STRING = 303,
     FOR = 304,
     PARFOR = 305,
     WHILE = 306,
     DO = 307,
     UNTIL = 308,
     IF = 309,
     ELSEIF = 310,
     ELSE = 311,
     SWITCH = 312,
     CASE = 313,
     OTHERWISE = 314,
     BREAK = 315,
     CONTINUE = 316,
     FUNC_RET = 317,
     UNWIND = 318,
     CLEANUP = 319,
     TRY = 320,
     CATCH = 321,
     GLOBAL = 322,
     STATIC = 323,
     FCN_HANDLE = 324,
     PROPERTIES = 325,
     METHODS = 326,
     EVENTS = 327,
     ENUMERATION = 328,
     METAQUERY = 329,
     SUPERCLASSREF = 330,
     GET = 331,
     SET = 332,
     END_OF_INPUT = 333,
     LEXICAL_ERROR = 334,
     FCN = 335,
     SCRIPT_FILE = 336,
     FUNCTION_FILE = 337,
     CLASSDEF = 338,
     CLOSE_BRACE = 339,
     UNARY = 340
   };
#endif
/* Tokens.  */
#define ADD_EQ 258
#define SUB_EQ 259
#define MUL_EQ 260
#define DIV_EQ 261
#define LEFTDIV_EQ 262
#define POW_EQ 263
#define EMUL_EQ 264
#define EDIV_EQ 265
#define ELEFTDIV_EQ 266
#define EPOW_EQ 267
#define AND_EQ 268
#define OR_EQ 269
#define LSHIFT_EQ 270
#define RSHIFT_EQ 271
#define LSHIFT 272
#define RSHIFT 273
#define EXPR_AND_AND 274
#define EXPR_OR_OR 275
#define EXPR_AND 276
#define EXPR_OR 277
#define EXPR_NOT 278
#define EXPR_LT 279
#define EXPR_LE 280
#define EXPR_EQ 281
#define EXPR_NE 282
#define EXPR_GE 283
#define EXPR_GT 284
#define LEFTDIV 285
#define EMUL 286
#define EDIV 287
#define ELEFTDIV 288
#define EPLUS 289
#define EMINUS 290
#define QUOTE 291
#define TRANSPOSE 292
#define PLUS_PLUS 293
#define MINUS_MINUS 294
#define POW 295
#define EPOW 296
#define NUM 297
#define IMAG_NUM 298
#define STRUCT_ELT 299
#define NAME 300
#define END 301
#define DQ_STRING 302
#define SQ_STRING 303
#define FOR 304
#define PARFOR 305
#define WHILE 306
#define DO 307
#define UNTIL 308
#define IF 309
#define ELSEIF 310
#define ELSE 311
#define SWITCH 312
#define CASE 313
#define OTHERWISE 314
#define BREAK 315
#define CONTINUE 316
#define FUNC_RET 317
#define UNWIND 318
#define CLEANUP 319
#define TRY 320
#define CATCH 321
#define GLOBAL 322
#define STATIC 323
#define FCN_HANDLE 324
#define PROPERTIES 325
#define METHODS 326
#define EVENTS 327
#define ENUMERATION 328
#define METAQUERY 329
#define SUPERCLASSREF 330
#define GET 331
#define SET 332
#define END_OF_INPUT 333
#define LEXICAL_ERROR 334
#define FCN 335
#define SCRIPT_FILE 336
#define FUNCTION_FILE 337
#define CLASSDEF 338
#define CLOSE_BRACE 339
#define UNARY 340




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 293 of yacc.c  */
#line 390 "oct-parse.yy"

  // The type of the basic tokens returned by the lexer.
  token *tok_val;

  // Comment strings that we need to deal with mid-rule.
  octave_comment_list *comment_type;

  // Types for the nonterminals we generate.
  char sep_type;
  tree *tree_type;
  tree_matrix *tree_matrix_type;
  tree_cell *tree_cell_type;
  tree_expression *tree_expression_type;
  tree_constant *tree_constant_type;
  tree_fcn_handle *tree_fcn_handle_type;
  tree_anon_fcn_handle *tree_anon_fcn_handle_type;
  tree_identifier *tree_identifier_type;
  tree_index_expression *tree_index_expression_type;
  tree_colon_expression *tree_colon_expression_type;
  tree_argument_list *tree_argument_list_type;
  tree_parameter_list *tree_parameter_list_type;
  tree_command *tree_command_type;
  tree_if_command *tree_if_command_type;
  tree_if_clause *tree_if_clause_type;
  tree_if_command_list *tree_if_command_list_type;
  tree_switch_command *tree_switch_command_type;
  tree_switch_case *tree_switch_case_type;
  tree_switch_case_list *tree_switch_case_list_type;
  tree_decl_elt *tree_decl_elt_type;
  tree_decl_init_list *tree_decl_init_list_type;
  tree_decl_command *tree_decl_command_type;
  tree_statement *tree_statement_type;
  tree_statement_list *tree_statement_list_type;
  octave_user_function *octave_user_function_type;
  void *dummy_type;



/* Line 293 of yacc.c  */
#line 678 "oct-parse.cc"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 690 "oct-parse.cc"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  125
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1377

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  103
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  105
/* YYNRULES -- Number of rules.  */
#define YYNRULES  270
/* YYNRULES -- Number of states.  */
#define YYNSTATES  499

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   340

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      95,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      92,   102,     7,     6,    99,     5,    93,     8,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     4,    98,
       2,     3,     2,     2,   101,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    96,     2,    97,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    94,     2,   100,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,    10,    12,    14,    16,    18,
      21,    24,    27,    29,    33,    34,    36,    39,    41,    45,
      47,    49,    51,    54,    56,    59,    61,    63,    65,    67,
      69,    71,    73,    75,    78,    82,    86,    90,    92,    95,
      97,   101,   104,   108,   112,   114,   117,   119,   123,   125,
     128,   131,   135,   137,   139,   141,   143,   145,   147,   149,
     153,   155,   157,   159,   161,   163,   167,   171,   175,   177,
     179,   182,   185,   189,   194,   198,   203,   206,   209,   213,
     219,   222,   225,   228,   231,   234,   238,   242,   246,   250,
     254,   258,   262,   266,   270,   274,   278,   282,   284,   286,
     290,   292,   296,   300,   304,   308,   312,   316,   320,   324,
     328,   332,   336,   340,   342,   346,   350,   354,   358,   362,
     366,   370,   374,   378,   382,   386,   390,   394,   398,   402,
     406,   408,   410,   412,   414,   416,   418,   420,   422,   424,
     426,   428,   429,   433,   437,   439,   442,   443,   445,   450,
     452,   454,   456,   461,   463,   466,   470,   473,   480,   485,
     492,   493,   495,   497,   500,   502,   505,   512,   517,   524,
     531,   540,   551,   560,   573,   575,   577,   579,   589,   599,
     605,   606,   608,   610,   614,   617,   618,   620,   622,   626,
     629,   631,   635,   637,   641,   645,   650,   652,   656,   660,
     663,   668,   670,   674,   678,   681,   686,   690,   692,   694,
     697,   699,   704,   711,   712,   716,   718,   722,   724,   729,
     732,   733,   735,   740,   743,   749,   753,   755,   757,   759,
     761,   765,   769,   773,   777,   780,   787,   789,   793,   795,
     801,   804,   811,   813,   817,   820,   827,   829,   833,   835,
     838,   845,   847,   851,   856,   857,   859,   861,   863,   865,
     868,   871,   872,   874,   876,   878,   880,   883,   886,   889,
     890
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     104,     0,    -1,   105,    -1,   169,    -1,   106,   203,    -1,
     203,    -1,    95,    -1,    84,    -1,   106,    -1,   106,    95,
      -1,   106,    84,    -1,   107,   205,    -1,   111,    -1,   107,
     204,   111,    -1,    -1,   109,    -1,   110,   207,    -1,   111,
      -1,   110,   206,   111,    -1,   139,    -1,   140,    -1,   112,
      -1,   114,   113,    -1,   117,    -1,   113,   117,    -1,    51,
      -1,    81,    -1,    80,    -1,    53,    -1,    54,    -1,    48,
      -1,    49,    -1,   117,    -1,    96,    97,    -1,    96,    98,
      97,    -1,    96,    99,    97,    -1,    96,   120,    97,    -1,
     121,    -1,   121,    98,    -1,   125,    -1,   121,    98,   125,
      -1,    94,   100,    -1,    94,    98,   100,    -1,    94,   123,
     100,    -1,   124,    -1,   124,    98,    -1,   125,    -1,   124,
      98,   125,    -1,   131,    -1,   131,    99,    -1,   101,    75,
      -1,   101,   163,   111,    -1,   114,    -1,   118,    -1,   126,
      -1,   119,    -1,   122,    -1,   116,    -1,   115,    -1,    92,
     139,   102,    -1,     4,    -1,    29,    -1,   139,    -1,   129,
      -1,   130,    -1,   131,    99,   129,    -1,   131,    99,   130,
      -1,   131,    99,   139,    -1,    93,    -1,   128,    -1,   133,
      44,    -1,   133,    45,    -1,   133,    92,   102,    -1,   133,
      92,   131,   102,    -1,   133,    94,   100,    -1,   133,    94,
     131,   100,    -1,   133,    42,    -1,   133,    43,    -1,   133,
     132,    50,    -1,   133,   132,    92,   139,   102,    -1,    44,
     133,    -1,    45,   133,    -1,    29,   133,    -1,     6,   133,
      -1,     5,   133,    -1,   133,    46,   133,    -1,   133,    47,
     133,    -1,   133,     6,   133,    -1,   133,     5,   133,    -1,
     133,     7,   133,    -1,   133,     8,   133,    -1,   133,    40,
     133,    -1,   133,    41,   133,    -1,   133,    37,   133,    -1,
     133,    38,   133,    -1,   133,    36,   133,    -1,   133,    39,
     133,    -1,   135,    -1,   133,    -1,   135,     4,   133,    -1,
     134,    -1,   136,    23,   136,    -1,   136,    24,   136,    -1,
     136,    30,   136,    -1,   136,    31,   136,    -1,   136,    32,
     136,    -1,   136,    34,   136,    -1,   136,    35,   136,    -1,
     136,    33,   136,    -1,   136,    27,   136,    -1,   136,    28,
     136,    -1,   136,    25,   136,    -1,   136,    26,   136,    -1,
     136,    -1,    96,   131,    90,    -1,   137,     3,   139,    -1,
     137,     9,   139,    -1,   137,    10,   139,    -1,   137,    11,
     139,    -1,   137,    12,   139,    -1,   137,    13,   139,    -1,
     137,    14,   139,    -1,   137,    21,   139,    -1,   137,    22,
     139,    -1,   137,    15,   139,    -1,   137,    16,   139,    -1,
     137,    17,   139,    -1,   137,    18,   139,    -1,   137,    19,
     139,    -1,   137,    20,   139,    -1,   136,    -1,   138,    -1,
     127,    -1,   142,    -1,   146,    -1,   157,    -1,   158,    -1,
     159,    -1,   172,    -1,   168,    -1,   180,    -1,    -1,    73,
     141,   143,    -1,    74,   141,   143,    -1,   145,    -1,   143,
     145,    -1,    -1,   114,    -1,   114,     3,   144,   139,    -1,
     130,    -1,   147,    -1,   152,    -1,    60,   202,   148,    52,
      -1,   149,    -1,   149,   151,    -1,   139,   207,   108,    -1,
     149,   150,    -1,    61,   202,   207,   139,   207,   108,    -1,
      62,   202,   207,   108,    -1,    63,   202,   139,   207,   153,
      52,    -1,    -1,   156,    -1,   154,    -1,   154,   156,    -1,
     155,    -1,   154,   155,    -1,    64,   202,   207,   139,   207,
     108,    -1,    65,   202,   207,   108,    -1,    57,   202,   139,
     207,   108,    52,    -1,    58,   202,   207,   108,    59,   139,
      -1,    55,   202,   137,     3,   139,   207,   108,    52,    -1,
      55,   202,    92,   137,     3,   139,   102,   207,   108,    52,
      -1,    56,   202,   137,     3,   139,   207,   108,    52,    -1,
      56,   202,    92,   137,     3,   139,    99,   139,   102,   207,
     108,    52,    -1,    66,    -1,    67,    -1,    68,    -1,    69,
     202,   207,   108,    70,   202,   207,   108,    52,    -1,    71,
     202,   207,   108,    72,   202,   207,   108,    52,    -1,    71,
     202,   207,   108,    52,    -1,    -1,    92,    -1,   102,    -1,
     161,   164,   162,    -1,   161,     1,    -1,    -1,   165,    -1,
     145,    -1,   165,    99,   145,    -1,    96,    97,    -1,   167,
      -1,    96,   167,    97,    -1,   114,    -1,   167,    99,   114,
      -1,    87,   108,    84,    -1,    88,   170,   207,    84,    -1,
     172,    -1,   170,   206,   172,    -1,   160,    86,   202,    -1,
     171,   174,    -1,   171,   166,     3,   174,    -1,   114,    -1,
      82,    93,   114,    -1,    83,    93,   114,    -1,   173,   175,
      -1,   163,   207,   108,   176,    -1,   207,   108,   176,    -1,
      52,    -1,    84,    -1,    89,   202,    -1,    52,    -1,   177,
     181,   114,   184,    -1,   179,   207,   186,   207,   202,   178,
      -1,    -1,    92,   182,   102,    -1,   183,    -1,   182,    99,
     183,    -1,   114,    -1,   114,     3,   144,   139,    -1,    29,
     114,    -1,    -1,   185,    -1,    30,   114,    93,   114,    -1,
      30,   114,    -1,   185,    27,   114,    93,   114,    -1,   185,
      27,   114,    -1,   188,    -1,   192,    -1,   195,    -1,   199,
      -1,   186,   207,   188,    -1,   186,   207,   192,    -1,   186,
     207,   195,    -1,   186,   207,   199,    -1,    76,   202,    -1,
     187,   181,   207,   189,   207,    52,    -1,   190,    -1,   189,
     207,   190,    -1,   114,    -1,   114,     3,   144,   139,    98,
      -1,    77,   202,    -1,   191,   181,   207,   193,   207,    52,
      -1,   172,    -1,   193,   207,   172,    -1,    78,   202,    -1,
     194,   181,   207,   196,   207,    52,    -1,   197,    -1,   196,
     207,   197,    -1,   114,    -1,    79,   202,    -1,   198,   181,
     207,   200,   207,    52,    -1,   201,    -1,   200,   207,   201,
      -1,   114,    92,   139,   102,    -1,    -1,    85,    -1,     1,
      -1,    99,    -1,    98,    -1,   204,    99,    -1,   204,    98,
      -1,    -1,   204,    -1,    99,    -1,    98,    -1,    95,    -1,
     206,    99,    -1,   206,    98,    -1,   206,    95,    -1,    -1,
     206,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   543,   543,   549,   551,   553,   557,   559,   564,   566,
     568,   572,   576,   578,   583,   584,   588,   592,   594,   598,
     600,   602,   614,   618,   620,   631,   639,   643,   647,   649,
     653,   655,   657,   661,   667,   673,   679,   687,   689,   693,
     695,   702,   704,   706,   710,   712,   716,   718,   726,   728,
     732,   739,   746,   748,   750,   752,   754,   756,   758,   760,
     764,   771,   777,   779,   781,   783,   788,   793,   800,   804,
     806,   808,   810,   812,   814,   816,   818,   820,   822,   824,
     826,   828,   830,   832,   834,   836,   838,   840,   842,   844,
     846,   848,   850,   852,   854,   856,   858,   862,   866,   868,
     875,   877,   879,   881,   883,   885,   887,   889,   891,   893,
     895,   897,   899,   906,   911,   925,   927,   929,   931,   933,
     935,   937,   939,   941,   943,   945,   947,   949,   951,   953,
     957,   959,   961,   969,   971,   973,   975,   977,   979,   981,
     983,   993,   995,  1000,  1007,  1009,  1017,  1019,  1021,  1026,
    1036,  1038,  1046,  1053,  1055,  1062,  1068,  1075,  1083,  1091,
    1099,  1100,  1102,  1104,  1111,  1113,  1120,  1124,  1134,  1141,
    1146,  1152,  1158,  1164,  1176,  1181,  1186,  1197,  1203,  1209,
    1221,  1243,  1257,  1264,  1269,  1278,  1279,  1289,  1291,  1302,
    1307,  1315,  1325,  1327,  1338,  1354,  1358,  1359,  1366,  1375,
    1380,  1387,  1396,  1401,  1408,  1419,  1421,  1425,  1433,  1472,
    1479,  1490,  1494,  1499,  1500,  1504,  1506,  1510,  1512,  1514,
    1520,  1521,  1525,  1527,  1529,  1531,  1535,  1537,  1539,  1541,
    1543,  1545,  1547,  1549,  1553,  1558,  1563,  1565,  1569,  1571,
    1575,  1579,  1583,  1585,  1589,  1593,  1597,  1599,  1603,  1607,
    1611,  1615,  1617,  1621,  1630,  1633,  1635,  1638,  1640,  1642,
    1644,  1649,  1650,  1654,  1656,  1658,  1660,  1662,  1664,  1669,
    1670
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "'='", "':'", "'-'", "'+'", "'*'", "'/'",
  "ADD_EQ", "SUB_EQ", "MUL_EQ", "DIV_EQ", "LEFTDIV_EQ", "POW_EQ",
  "EMUL_EQ", "EDIV_EQ", "ELEFTDIV_EQ", "EPOW_EQ", "AND_EQ", "OR_EQ",
  "LSHIFT_EQ", "RSHIFT_EQ", "LSHIFT", "RSHIFT", "EXPR_AND_AND",
  "EXPR_OR_OR", "EXPR_AND", "EXPR_OR", "EXPR_NOT", "EXPR_LT", "EXPR_LE",
  "EXPR_EQ", "EXPR_NE", "EXPR_GE", "EXPR_GT", "LEFTDIV", "EMUL", "EDIV",
  "ELEFTDIV", "EPLUS", "EMINUS", "QUOTE", "TRANSPOSE", "PLUS_PLUS",
  "MINUS_MINUS", "POW", "EPOW", "NUM", "IMAG_NUM", "STRUCT_ELT", "NAME",
  "END", "DQ_STRING", "SQ_STRING", "FOR", "PARFOR", "WHILE", "DO", "UNTIL",
  "IF", "ELSEIF", "ELSE", "SWITCH", "CASE", "OTHERWISE", "BREAK",
  "CONTINUE", "FUNC_RET", "UNWIND", "CLEANUP", "TRY", "CATCH", "GLOBAL",
  "STATIC", "FCN_HANDLE", "PROPERTIES", "METHODS", "EVENTS", "ENUMERATION",
  "METAQUERY", "SUPERCLASSREF", "GET", "SET", "END_OF_INPUT",
  "LEXICAL_ERROR", "FCN", "SCRIPT_FILE", "FUNCTION_FILE", "CLASSDEF",
  "CLOSE_BRACE", "UNARY", "'('", "'.'", "'{'", "'\\n'", "'['", "']'",
  "';'", "','", "'}'", "'@'", "')'", "$accept", "input", "input1",
  "simple_list", "simple_list1", "opt_list", "list", "list1", "statement",
  "word_list_cmd", "word_list", "identifier", "superclass_identifier",
  "meta_identifier", "string", "constant", "matrix", "matrix_rows",
  "matrix_rows1", "cell", "cell_rows", "cell_rows1", "cell_or_matrix_row",
  "fcn_handle", "anon_fcn_handle", "primary_expr", "magic_colon",
  "magic_tilde", "arg_list", "indirect_ref_op", "oper_expr", "colon_expr",
  "colon_expr1", "simple_expr", "assign_lhs", "assign_expr", "expression",
  "command", "parsing_decl_list", "declaration", "decl1",
  "decl_param_init", "decl2", "select_command", "if_command",
  "if_cmd_list", "if_cmd_list1", "elseif_clause", "else_clause",
  "switch_command", "case_list", "case_list1", "switch_case",
  "default_case", "loop_command", "jump_command", "except_command",
  "push_fcn_symtab", "param_list_beg", "param_list_end", "param_list",
  "param_list1", "param_list2", "return_list", "return_list1",
  "script_file", "function_file", "function_list", "function_beg",
  "function", "fcn_name", "function1", "function2", "function_end",
  "classdef_beg", "classdef_end", "classdef1", "classdef", "opt_attr_list",
  "attr_list", "attr", "opt_superclasses", "superclasses", "class_body",
  "properties_beg", "properties_block", "properties_list",
  "class_property", "methods_beg", "methods_block", "methods_list",
  "events_beg", "events_block", "events_list", "class_event", "enum_beg",
  "enum_block", "enum_list", "class_enum", "stash_comment", "parse_error",
  "sep_no_nl", "opt_sep_no_nl", "sep", "opt_sep", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,    61,    58,    45,    43,    42,    47,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,   276,   277,   278,
     279,   280,   281,   282,   283,   284,   285,   286,   287,   288,
     289,   290,   291,   292,   293,   294,   295,   296,   297,   298,
     299,   300,   301,   302,   303,   304,   305,   306,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     339,   340,    40,    46,   123,    10,    91,    93,    59,    44,
     125,    64,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   103,   104,   104,   104,   104,   105,   105,   105,   105,
     105,   106,   107,   107,   108,   108,   109,   110,   110,   111,
     111,   111,   112,   113,   113,   114,   115,   116,   117,   117,
     118,   118,   118,   119,   119,   119,   119,   120,   120,   121,
     121,   122,   122,   122,   123,   123,   124,   124,   125,   125,
     126,   127,   128,   128,   128,   128,   128,   128,   128,   128,
     129,   130,   131,   131,   131,   131,   131,   131,   132,   133,
     133,   133,   133,   133,   133,   133,   133,   133,   133,   133,
     133,   133,   133,   133,   133,   133,   133,   133,   133,   133,
     133,   133,   133,   133,   133,   133,   133,   134,   135,   135,
     136,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   137,   137,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   138,
     139,   139,   139,   140,   140,   140,   140,   140,   140,   140,
     140,   141,   142,   142,   143,   143,   144,   145,   145,   145,
     146,   146,   147,   148,   148,   149,   149,   150,   151,   152,
     153,   153,   153,   153,   154,   154,   155,   156,   157,   157,
     157,   157,   157,   157,   158,   158,   158,   159,   159,   159,
     160,   161,   162,   163,   163,   164,   164,   165,   165,   166,
     166,   166,   167,   167,   168,   169,   170,   170,   171,   172,
     172,   173,   173,   173,   174,   175,   175,   176,   176,   177,
     178,   179,   180,   181,   181,   182,   182,   183,   183,   183,
     184,   184,   185,   185,   185,   185,   186,   186,   186,   186,
     186,   186,   186,   186,   187,   188,   189,   189,   190,   190,
     191,   192,   193,   193,   194,   195,   196,   196,   197,   198,
     199,   200,   200,   201,   202,   203,   203,   204,   204,   204,
     204,   205,   205,   206,   206,   206,   206,   206,   206,   207,
     207
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     2,     1,     1,     1,     1,     2,
       2,     2,     1,     3,     0,     1,     2,     1,     3,     1,
       1,     1,     2,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     3,     3,     3,     1,     2,     1,
       3,     2,     3,     3,     1,     2,     1,     3,     1,     2,
       2,     3,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     1,     1,     1,     1,     3,     3,     3,     1,     1,
       2,     2,     3,     4,     3,     4,     2,     2,     3,     5,
       2,     2,     2,     2,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     1,     1,     3,
       1,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     1,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     3,     3,     1,     2,     0,     1,     4,     1,
       1,     1,     4,     1,     2,     3,     2,     6,     4,     6,
       0,     1,     1,     2,     1,     2,     6,     4,     6,     6,
       8,    10,     8,    12,     1,     1,     1,     9,     9,     5,
       0,     1,     1,     3,     2,     0,     1,     1,     3,     2,
       1,     3,     1,     3,     3,     4,     1,     3,     3,     2,
       4,     1,     3,     3,     2,     4,     3,     1,     1,     2,
       1,     4,     6,     0,     3,     1,     3,     1,     4,     2,
       0,     1,     4,     2,     5,     3,     1,     1,     1,     1,
       3,     3,     3,     3,     2,     6,     1,     3,     1,     5,
       2,     6,     1,     3,     2,     6,     1,     3,     1,     2,
       6,     1,     3,     4,     0,     1,     1,     1,     1,     2,
       2,     0,     1,     1,     1,     1,     2,     2,     2,     0,
       1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,   256,     0,     0,     0,     0,     0,    30,    31,    25,
      28,    29,   254,   254,   254,   254,   254,   254,   174,   175,
     176,   254,   254,   141,   141,    27,    26,     7,   255,    14,
     180,   254,     0,     0,     6,     0,     0,     0,     2,     0,
     261,    12,    21,    52,    58,    57,    32,    53,    55,    56,
      54,   132,    69,    98,   100,    97,   130,     0,   131,    19,
      20,   133,   134,   150,   151,   135,   136,   137,     0,   139,
       3,     0,   138,   213,   269,   140,     5,     0,     0,    52,
      84,    83,    82,    80,    81,     0,     0,     0,   269,     0,
       0,   269,   269,     0,     0,     0,    15,   269,    17,   269,
     196,   209,     0,    60,    61,     0,    41,     0,    44,    46,
      63,    64,    48,    62,    33,     0,     0,     0,    37,    39,
      48,    50,   181,     0,   180,     1,    10,     9,     4,   258,
     257,   262,    11,    22,    23,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    76,    77,    70,    71,     0,
       0,     0,    68,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   254,     0,     0,     0,   201,     0,   190,
     269,   199,     0,     0,   265,   264,   263,   270,     0,     0,
     113,     0,     0,     0,   269,    14,   269,     0,   153,   269,
      14,    14,    61,   147,   149,   142,   144,   143,   194,   270,
      16,   180,     0,    59,    42,    43,    45,    49,    34,    35,
      36,    38,   114,   184,   187,     0,   186,    51,   260,   259,
      13,    24,    88,    87,    89,    90,    95,    93,    94,    96,
      91,    92,    85,    86,    72,     0,    74,     0,    78,     0,
      99,   101,   102,   111,   112,   109,   110,   103,   104,   105,
     108,   106,   107,   115,   116,   117,   118,   119,   120,   121,
     124,   125,   126,   127,   128,   129,   122,   123,   198,     0,
       0,   189,   192,     0,     0,     0,   269,   204,    14,     0,
     217,     0,   215,   220,   268,   267,   266,   254,   254,   254,
     254,   269,   213,   226,   213,   227,   213,   228,   213,   229,
       0,     0,     0,     0,    14,     0,    14,   152,   254,   254,
     156,   154,   160,     0,     0,   146,   145,    18,   197,   195,
      47,    65,    66,    67,    40,   182,   183,     0,     0,    73,
      75,     0,   202,   203,   191,   201,   200,   193,    14,     0,
     219,   146,     0,   214,     0,   211,   221,   234,   240,   244,
     249,   254,   269,   269,   269,   269,     0,   269,     0,   269,
       0,     0,   155,   269,   269,   254,   254,     0,   162,   164,
     161,   254,   179,   254,     0,   188,    79,     0,   207,   208,
     206,     0,   216,   223,     0,   230,   231,   232,   233,     0,
       0,   180,     0,     0,     0,    14,   115,    14,   168,   169,
       0,    14,   269,   269,   159,   165,   163,   269,   269,   148,
     205,   218,     0,   225,   210,   212,   238,   269,   236,   242,
     269,   248,   269,   246,     0,   269,   251,   269,     0,     0,
       0,   269,   158,     0,    14,    14,    14,   222,     0,   146,
       0,   180,     0,     0,     0,    14,   170,     0,   172,    14,
     269,   167,     0,     0,   224,     0,   235,   237,   241,   243,
     245,   247,     0,   250,   252,     0,   269,   157,    14,   177,
     178,     0,   253,   171,    14,   166,   239,     0,   173
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    37,    38,    39,    40,    95,    96,    97,    98,    42,
     133,    79,    44,    45,    46,    47,    48,   117,   118,    49,
     107,   108,   119,    50,    51,    52,   110,   111,   112,   154,
      53,    54,    55,    56,    57,    58,    59,    60,    93,    61,
     215,   394,   216,    62,    63,   207,   208,   330,   331,    64,
     387,   388,   389,   390,    65,    66,    67,    68,   123,   346,
     124,   235,   236,   188,   189,    69,    70,    99,    71,    72,
     190,   191,   297,   400,    73,   435,    74,    75,   193,   301,
     302,   365,   366,   311,   312,   313,   437,   438,   314,   315,
     440,   316,   317,   442,   443,   318,   319,   445,   446,    85,
      76,   131,   132,   197,   198
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -355
static const yytype_int16 yypact[] =
{
     538,  -355,  1054,  1054,  1054,  1054,  1054,  -355,  -355,  -355,
    -355,  -355,  -355,  -355,  -355,  -355,  -355,  -355,  -355,  -355,
    -355,  -355,  -355,  -355,  -355,  -355,  -355,  -355,  -355,   936,
    -355,  -355,  1072,   677,  -355,   643,   -31,    10,  -355,    27,
     -60,  -355,  -355,    61,  -355,  -355,  -355,  -355,  -355,  -355,
    -355,  -355,  -355,   235,  -355,    12,  1275,  1238,  -355,  -355,
    -355,  -355,  -355,  -355,  -355,  -355,  -355,  -355,   -43,  -355,
    -355,   -17,  -355,   -35,    18,  -355,  -355,   643,   -26,  -355,
       9,     9,     9,     9,     9,  1127,  1181,  1072,    18,  1072,
    1072,    18,    18,    -6,    -6,    13,  -355,    18,  -355,    18,
    -355,  -355,   -25,  -355,  1054,   -16,  -355,     6,     2,  -355,
    -355,  -355,    -8,  -355,  -355,    11,    24,    31,    43,  -355,
     -40,  -355,  -355,     7,   995,  -355,  -355,  -355,  -355,  -355,
    -355,   877,  -355,    61,  -355,  1054,  1054,  1054,  1054,  1054,
    1054,  1054,  1054,  1054,  1054,  -355,  -355,  -355,  -355,  1054,
    1054,   609,  -355,   741,   -18,  1054,  1054,  1054,  1054,  1054,
    1054,  1054,  1054,  1054,  1054,  1054,  1054,  1054,  1072,  1072,
    1072,  1072,  1072,  1072,  1072,  1072,  1072,  1072,  1072,  1072,
    1072,  1072,  1072,  -355,   -11,    50,   -29,     8,   101,    51,
      -9,  -355,    -5,   111,  -355,  -355,  -355,    59,   105,  1072,
    1329,   149,  1072,   153,    18,   936,    18,   133,    84,    18,
     936,   936,  -355,   193,  -355,    -6,  -355,    -6,  -355,   818,
    -355,    35,   115,  -355,  -355,  -355,   758,   758,  -355,  -355,
    -355,   758,  -355,  -355,  -355,    99,   113,  -355,  -355,  -355,
    -355,  -355,  1152,  1152,     9,     9,     9,     9,     9,     9,
    1152,  1152,   -30,   -30,  -355,   -32,  -355,    60,  -355,  1072,
     235,  -355,  -355,   203,  1342,   517,   498,   141,   141,   141,
     141,   141,   141,  -355,  -355,  -355,  -355,  -355,  -355,  -355,
    -355,  -355,  -355,  -355,  -355,  -355,  -355,  -355,  -355,   111,
     111,  -355,  -355,   -21,   -10,   111,    18,  -355,   936,   111,
     210,    -7,  -355,   186,  -355,  -355,  -355,  -355,  -355,  -355,
    -355,    18,   -35,  -355,   -35,  -355,   -35,  -355,   -35,  -355,
    1308,  1072,  1328,  1072,   936,   159,   936,  -355,  -355,  -355,
    -355,  -355,   124,   151,   -12,  -355,  -355,  -355,  -355,  -355,
    -355,  -355,  -355,  -355,  -355,  -355,  -355,    -6,   758,  -355,
    -355,   120,  -355,  -355,  -355,  -355,  -355,  -355,   936,   -15,
    -355,  -355,    -5,  -355,   111,  -355,   196,  -355,  -355,  -355,
    -355,   105,    18,    18,    18,    18,  1072,    18,  1072,    18,
     180,  1072,  -355,    18,    18,  -355,  -355,   187,   124,  -355,
    -355,  -355,  -355,  -355,  1072,  -355,  -355,   -15,  -355,  -355,
    -355,  1072,  -355,   152,   111,  -355,  -355,  -355,  -355,   192,
     111,  -355,   111,   111,   145,   936,   150,   936,  -355,  -355,
    1072,   936,    18,    18,  -355,  -355,  -355,    18,    18,  -355,
    -355,  -355,   111,   158,  -355,  -355,   245,    18,  -355,  -355,
      18,  -355,    18,  -355,   161,    18,  -355,    18,   198,  1072,
     204,    18,  -355,  1072,   936,   936,   936,  -355,   111,  -355,
     139,   207,   155,  1072,   157,   936,  -355,   162,  -355,   936,
      18,  -355,   209,   211,  -355,  1072,  -355,  -355,  -355,  -355,
    -355,  -355,   164,  -355,  -355,   215,    18,  -355,   936,  -355,
    -355,   170,  -355,  -355,   936,  -355,  -355,   217,  -355
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -355,  -355,  -355,  -355,  -355,  -163,  -355,  -355,     1,  -355,
    -355,     0,  -355,  -355,   -34,  -355,  -355,  -355,  -355,  -355,
    -355,  -355,   -28,  -355,  -355,  -355,  -221,   -13,   -33,  -355,
     420,  -355,  -355,   352,   -73,  -355,    -2,  -355,   231,  -355,
     176,  -354,  -119,  -355,  -355,  -355,  -355,  -355,  -355,  -355,
    -355,  -355,  -105,  -104,  -355,  -355,  -355,  -355,  -355,  -355,
      95,  -355,  -355,  -355,   100,  -355,  -355,  -355,  -355,   -27,
    -355,     3,  -355,  -110,  -355,  -355,  -355,  -355,  -176,  -355,
     -74,  -355,  -355,  -355,  -355,   -75,  -355,  -160,  -355,   -70,
    -355,  -355,   -68,  -355,  -158,  -355,   -66,  -355,  -157,     4,
     269,  -355,  -355,    40,    56
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -271
static const yytype_int16 yytable[] =
{
      43,    41,   120,   100,   234,   109,   341,   401,   233,   134,
     125,  -192,   201,   203,   147,   148,   155,    86,    87,    88,
      89,    90,     9,   212,   299,    91,    92,    -8,     1,    43,
     102,   113,   258,   113,     9,   101,   212,   398,   129,   130,
     392,     9,   325,   183,   121,     9,     9,   333,   334,   121,
     232,   145,   146,   147,   148,   149,   150,   192,     9,   227,
     393,   122,   151,   152,   153,   184,   185,   348,   291,   399,
     349,   187,   184,   185,   259,   113,   354,   223,   295,   186,
     214,   214,   289,   122,   224,   204,   194,   206,   209,   195,
     196,   227,   362,   213,   213,   363,   336,   218,   336,   241,
     226,   151,   152,   153,   294,   475,   225,  -192,   228,  -185,
     214,   126,    28,   194,    10,    11,   195,   196,   255,  -270,
     257,   229,   127,   213,    43,   237,   320,   341,   230,   322,
     304,    43,   240,   305,   306,   359,   372,   219,   373,   221,
     374,   231,   375,   290,   205,   328,   329,   210,   211,   113,
     295,   113,   321,   220,   304,   222,   323,   305,   306,   348,
     350,   380,     9,   382,   156,   157,   273,   274,   275,   276,
     277,   278,   279,   280,   281,   282,   283,   284,   285,   286,
     287,   307,   308,   309,   310,   327,   292,   288,   385,   386,
       9,   476,   300,   303,   338,   397,   335,   102,   340,   339,
     102,   345,   214,   344,   214,    43,     9,   480,     9,   483,
      43,    43,   347,   361,   342,   213,   364,   213,   381,    43,
     337,   391,   396,   404,   113,   343,   156,   157,   395,   113,
     160,   161,   418,   162,   163,   164,   165,   166,   167,   424,
     135,   136,   137,   138,   434,   432,   298,   447,   459,   449,
     466,   458,   448,   463,   450,    94,   468,   351,   452,   478,
     324,   489,   326,   490,   486,   332,   492,   493,   496,   498,
     217,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   425,   426,   296,   293,   430,   402,   352,
     353,   471,   472,   473,   355,   357,   405,   356,    43,   360,
     477,   406,   485,   407,   481,   408,   487,   484,   128,     0,
       0,   367,   368,   369,   370,     0,     0,     0,     0,   377,
       0,   379,     0,     0,    43,   495,    43,   151,   152,   153,
       0,   497,   383,   384,   214,   342,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   343,   213,     0,     0,
       0,     0,   358,     0,     0,     0,     0,     0,    43,     0,
       0,     0,   300,     0,   403,     0,     0,   371,     0,     0,
       0,     0,     0,     0,   414,   409,   416,     0,     0,   419,
       0,     0,     0,     0,   439,     0,     0,     0,     0,   422,
     423,     0,   429,     0,     0,   427,     0,   428,     0,   431,
       0,     0,     0,     0,   433,     0,     0,     0,     0,     0,
     436,     0,   441,   444,     0,    43,     0,    43,   451,     0,
       0,    43,    80,    81,    82,    83,    84,     0,   410,   411,
     412,   413,   457,   415,   479,   417,     0,   200,   200,   420,
     421,     0,     0,     0,     0,     0,     0,   467,     0,     0,
       0,   470,     0,     0,    43,    43,    43,     0,   474,     0,
     436,   482,   441,     0,   444,    43,     0,     0,     0,    43,
       0,     0,     0,   491,     0,     0,     0,     0,   453,   454,
       0,     0,     0,   455,   456,     0,     0,     0,    43,     0,
       0,     0,     0,   460,    43,     0,   461,     0,   462,     0,
       0,   464,     0,   465,     0,     0,     0,   469,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
       0,   156,   157,     0,    82,   160,   488,     0,   162,   163,
     164,   165,   166,   167,     0,     0,     0,     0,     0,     1,
     156,   157,   494,     2,     3,     0,     0,   162,   163,   164,
     165,   166,   167,     0,     0,   242,   243,   244,   245,   246,
     247,   248,   249,   250,   251,     0,     0,     4,     0,   252,
     253,     0,     0,     0,     0,   260,     0,     0,     0,     0,
       0,     0,     5,     6,     0,     0,     7,     8,     0,     9,
       0,    10,    11,    12,    13,    14,    15,     0,    16,     0,
       0,    17,     0,     0,    18,    19,    20,    21,     0,    22,
       0,    23,    24,   103,     2,     3,     0,     0,    25,    26,
       0,     0,    27,    28,  -180,    29,    30,    31,     0,     0,
      32,     0,    33,    34,    35,     0,     0,     0,   104,    36,
       0,     0,     0,     0,     0,     0,     0,   103,     2,     3,
       0,     0,     0,     5,     6,     0,     0,     7,     8,     0,
       9,     0,    10,    11,     0,     0,     0,     0,     0,     0,
       0,     0,   104,     0,     0,     0,     0,     0,     0,     0,
       0,   103,     2,     3,     0,     0,     0,     5,     6,    25,
      26,     7,     8,     0,     9,     0,    10,    11,     0,     0,
       0,    32,     0,    33,     0,    35,   104,     0,     0,     0,
      36,   254,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     5,     6,    25,    26,     7,     8,     0,     9,     0,
      10,    11,     0,     0,     0,    32,     0,    33,     0,    35,
     114,   115,   116,     0,    36,   103,     2,     3,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    25,    26,     0,
       0,     0,   103,     2,     3,     0,     0,     0,     0,    32,
     104,    33,     0,    35,     0,   105,     0,   106,    36,     0,
       0,     0,     0,     0,     0,     5,     6,   104,     0,     7,
       8,     0,     9,     0,    10,    11,     0,     0,     0,     0,
       0,     0,     5,     6,     0,     0,     7,     8,     0,     9,
       0,    10,    11,     0,     0,     0,     0,     0,     0,     0,
       0,    25,    26,     2,     3,     0,     0,     0,     0,     0,
       0,     0,     0,    32,     0,    33,     0,    35,    25,    26,
       0,   256,    36,     0,     0,     0,     0,     4,     0,     0,
      32,     0,    33,     0,    35,     0,     0,     0,     0,    36,
       0,     0,     5,     6,     0,     0,     7,     8,     0,     9,
       0,    10,    11,    12,    13,    14,    15,     0,    16,     0,
       0,    17,     2,     3,    18,    19,    20,    21,     0,    22,
       0,    23,    24,     0,     0,     0,     0,     0,    25,    26,
       0,     0,     0,     0,  -180,    29,     4,    31,     0,     0,
      32,     0,    33,   304,    35,     0,   305,   306,     0,    36,
       0,     5,     6,     0,     0,     7,     8,     0,     9,     0,
      10,    11,    12,    13,    14,    15,     0,    16,     0,     0,
      17,     2,     3,    18,    19,    20,    21,     0,    22,     0,
      23,    24,     0,     0,     0,     0,     0,    25,    26,     0,
       0,     0,     0,  -180,    29,     4,    31,     0,     0,    32,
       0,    33,     0,    35,     0,   238,   239,     0,    36,     0,
       5,     6,     0,     0,     7,     8,     0,     9,     0,    10,
      11,    12,    13,    14,    15,     0,    16,     0,     0,    17,
       2,     3,    18,    19,    20,    21,     0,    22,     0,    23,
      24,     0,     0,     0,     0,     0,    25,    26,     0,     0,
       0,     0,  -180,    29,     4,    31,     0,     0,    32,     0,
      33,     0,    35,     0,     0,     0,     0,    36,     0,     5,
       6,     0,     0,     7,     8,     0,     9,     0,    10,    11,
      12,    13,    14,    15,     0,    16,     0,     0,    17,     2,
       3,    18,    19,    20,    21,     0,    22,     0,    23,    24,
       0,     0,     0,     0,     0,    25,    26,     2,     3,     0,
       0,     0,    29,     4,    31,     0,     0,    32,     0,    33,
       0,    35,     0,     0,     0,     0,    36,     0,     5,     6,
       0,     4,     7,     8,     0,     9,     0,    10,    11,     0,
       0,     0,     0,     0,     0,     0,     5,     6,     0,     0,
       7,     8,     0,     9,     0,    10,    11,     0,     0,     0,
       0,     0,     2,     3,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    32,     0,    33,     0,
      77,     0,    25,    26,     0,    78,     4,     0,     0,   137,
     138,     0,     0,     0,    32,     0,    33,     0,    35,     0,
       0,     5,     6,    36,     0,     7,     8,     0,     9,     0,
      10,    11,     0,     0,     0,     0,     2,     3,   139,   140,
     141,   142,     0,     0,   145,   146,   147,   148,   149,   150,
       0,     0,     0,     0,     0,     0,     0,    25,    26,     0,
       4,     0,     0,     0,     0,     0,     0,     0,     0,   199,
       0,    33,     0,    35,     0,     5,     6,     0,    78,     7,
       8,     0,     9,     0,    10,    11,     0,     0,     0,     0,
       0,   168,     0,     0,   151,   152,   153,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,    25,    26,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   202,     0,    33,     0,    35,  -113,     0,
       0,     0,    78,     0,  -113,  -113,  -113,  -113,  -113,  -113,
    -113,  -113,  -113,  -113,  -113,  -113,  -113,  -113,   156,   157,
     158,   159,   160,   161,     0,   162,   163,   164,   165,   166,
     167,   376,     0,     0,     0,     0,     0,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,   378,     0,     0,     0,     0,     0,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,     0,   156,   157,   158,   159,   160,   161,     0,   162,
     163,   164,   165,   166,   167,   156,   157,   158,     0,   160,
     161,     0,   162,   163,   164,   165,   166,   167
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-355))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
       0,     0,    35,    30,   123,    33,   227,   361,     1,    43,
       0,     3,    85,    86,    44,    45,     4,    13,    14,    15,
      16,    17,    51,    29,    29,    21,    22,     0,     1,    29,
      32,    33,    50,    35,    51,    31,    29,    52,    98,    99,
      52,    51,   205,    86,    75,    51,    51,   210,   211,    75,
      90,    42,    43,    44,    45,    46,    47,    92,    51,    99,
      72,    92,    92,    93,    94,    82,    83,    99,    97,    84,
     102,    71,    82,    83,    92,    77,    97,   102,    99,    96,
      93,    94,    93,    92,   100,    87,    95,    89,    90,    98,
      99,    99,    99,    93,    94,   102,   215,    84,   217,   133,
      98,    92,    93,    94,     3,   459,   100,    99,    97,   102,
     123,    84,    85,    95,    53,    54,    98,    99,   151,    84,
     153,    97,    95,   123,   124,   124,   199,   348,    97,   202,
      95,   131,   131,    98,    99,   298,   312,    97,   314,    99,
     316,    98,   318,    93,    88,    61,    62,    91,    92,   151,
      99,   153,     3,    97,    95,    99,     3,    98,    99,    99,
     100,   324,    51,   326,    23,    24,   168,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,    76,    77,    78,    79,    52,   186,   183,    64,    65,
      51,    52,   192,   193,   221,   358,     3,   199,   226,    84,
     202,   102,   215,   231,   217,   205,    51,    52,    51,    52,
     210,   211,    99,     3,   227,   215,    30,   217,    59,   219,
     219,    70,   102,    27,   226,   227,    23,    24,   347,   231,
      27,    28,    52,    30,    31,    32,    33,    34,    35,    52,
       5,     6,     7,     8,    52,    93,   190,   102,     3,    99,
      52,    93,   415,    92,   417,    24,    52,   259,   421,    52,
     204,    52,   206,    52,   102,   209,   102,    52,    98,    52,
      94,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,   388,   388,   190,   186,   397,   362,   289,
     290,   454,   455,   456,   294,   295,   371,   294,   298,   299,
     460,   371,   465,   371,   462,   371,   469,   464,    39,    -1,
      -1,   307,   308,   309,   310,    -1,    -1,    -1,    -1,   321,
      -1,   323,    -1,    -1,   324,   488,   326,    92,    93,    94,
      -1,   494,   328,   329,   347,   348,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   348,   347,    -1,    -1,
      -1,    -1,   296,    -1,    -1,    -1,    -1,    -1,   358,    -1,
      -1,    -1,   362,    -1,   364,    -1,    -1,   311,    -1,    -1,
      -1,    -1,    -1,    -1,   376,   371,   378,    -1,    -1,   381,
      -1,    -1,    -1,    -1,   411,    -1,    -1,    -1,    -1,   385,
     386,    -1,   394,    -1,    -1,   391,    -1,   393,    -1,   401,
      -1,    -1,    -1,    -1,   404,    -1,    -1,    -1,    -1,    -1,
     410,    -1,   412,   413,    -1,   415,    -1,   417,   420,    -1,
      -1,   421,     2,     3,     4,     5,     6,    -1,   372,   373,
     374,   375,   432,   377,   461,   379,    -1,    85,    86,   383,
     384,    -1,    -1,    -1,    -1,    -1,    -1,   449,    -1,    -1,
      -1,   453,    -1,    -1,   454,   455,   456,    -1,   458,    -1,
     460,   463,   462,    -1,   464,   465,    -1,    -1,    -1,   469,
      -1,    -1,    -1,   475,    -1,    -1,    -1,    -1,   422,   423,
      -1,    -1,    -1,   427,   428,    -1,    -1,    -1,   488,    -1,
      -1,    -1,    -1,   437,   494,    -1,   440,    -1,   442,    -1,
      -1,   445,    -1,   447,    -1,    -1,    -1,   451,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,   167,
      -1,    23,    24,    -1,   104,    27,   470,    -1,    30,    31,
      32,    33,    34,    35,    -1,    -1,    -1,    -1,    -1,     1,
      23,    24,   486,     5,     6,    -1,    -1,    30,    31,    32,
      33,    34,    35,    -1,    -1,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,    -1,    -1,    29,    -1,   149,
     150,    -1,    -1,    -1,    -1,   155,    -1,    -1,    -1,    -1,
      -1,    -1,    44,    45,    -1,    -1,    48,    49,    -1,    51,
      -1,    53,    54,    55,    56,    57,    58,    -1,    60,    -1,
      -1,    63,    -1,    -1,    66,    67,    68,    69,    -1,    71,
      -1,    73,    74,     4,     5,     6,    -1,    -1,    80,    81,
      -1,    -1,    84,    85,    86,    87,    88,    89,    -1,    -1,
      92,    -1,    94,    95,    96,    -1,    -1,    -1,    29,   101,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     4,     5,     6,
      -1,    -1,    -1,    44,    45,    -1,    -1,    48,    49,    -1,
      51,    -1,    53,    54,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     4,     5,     6,    -1,    -1,    -1,    44,    45,    80,
      81,    48,    49,    -1,    51,    -1,    53,    54,    -1,    -1,
      -1,    92,    -1,    94,    -1,    96,    29,    -1,    -1,    -1,
     101,   102,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    44,    45,    80,    81,    48,    49,    -1,    51,    -1,
      53,    54,    -1,    -1,    -1,    92,    -1,    94,    -1,    96,
      97,    98,    99,    -1,   101,     4,     5,     6,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    80,    81,    -1,
      -1,    -1,     4,     5,     6,    -1,    -1,    -1,    -1,    92,
      29,    94,    -1,    96,    -1,    98,    -1,   100,   101,    -1,
      -1,    -1,    -1,    -1,    -1,    44,    45,    29,    -1,    48,
      49,    -1,    51,    -1,    53,    54,    -1,    -1,    -1,    -1,
      -1,    -1,    44,    45,    -1,    -1,    48,    49,    -1,    51,
      -1,    53,    54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    80,    81,     5,     6,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    92,    -1,    94,    -1,    96,    80,    81,
      -1,   100,   101,    -1,    -1,    -1,    -1,    29,    -1,    -1,
      92,    -1,    94,    -1,    96,    -1,    -1,    -1,    -1,   101,
      -1,    -1,    44,    45,    -1,    -1,    48,    49,    -1,    51,
      -1,    53,    54,    55,    56,    57,    58,    -1,    60,    -1,
      -1,    63,     5,     6,    66,    67,    68,    69,    -1,    71,
      -1,    73,    74,    -1,    -1,    -1,    -1,    -1,    80,    81,
      -1,    -1,    -1,    -1,    86,    87,    29,    89,    -1,    -1,
      92,    -1,    94,    95,    96,    -1,    98,    99,    -1,   101,
      -1,    44,    45,    -1,    -1,    48,    49,    -1,    51,    -1,
      53,    54,    55,    56,    57,    58,    -1,    60,    -1,    -1,
      63,     5,     6,    66,    67,    68,    69,    -1,    71,    -1,
      73,    74,    -1,    -1,    -1,    -1,    -1,    80,    81,    -1,
      -1,    -1,    -1,    86,    87,    29,    89,    -1,    -1,    92,
      -1,    94,    -1,    96,    -1,    98,    99,    -1,   101,    -1,
      44,    45,    -1,    -1,    48,    49,    -1,    51,    -1,    53,
      54,    55,    56,    57,    58,    -1,    60,    -1,    -1,    63,
       5,     6,    66,    67,    68,    69,    -1,    71,    -1,    73,
      74,    -1,    -1,    -1,    -1,    -1,    80,    81,    -1,    -1,
      -1,    -1,    86,    87,    29,    89,    -1,    -1,    92,    -1,
      94,    -1,    96,    -1,    -1,    -1,    -1,   101,    -1,    44,
      45,    -1,    -1,    48,    49,    -1,    51,    -1,    53,    54,
      55,    56,    57,    58,    -1,    60,    -1,    -1,    63,     5,
       6,    66,    67,    68,    69,    -1,    71,    -1,    73,    74,
      -1,    -1,    -1,    -1,    -1,    80,    81,     5,     6,    -1,
      -1,    -1,    87,    29,    89,    -1,    -1,    92,    -1,    94,
      -1,    96,    -1,    -1,    -1,    -1,   101,    -1,    44,    45,
      -1,    29,    48,    49,    -1,    51,    -1,    53,    54,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    44,    45,    -1,    -1,
      48,    49,    -1,    51,    -1,    53,    54,    -1,    -1,    -1,
      -1,    -1,     5,     6,    80,    81,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    92,    -1,    94,    -1,
      96,    -1,    80,    81,    -1,   101,    29,    -1,    -1,     7,
       8,    -1,    -1,    -1,    92,    -1,    94,    -1,    96,    -1,
      -1,    44,    45,   101,    -1,    48,    49,    -1,    51,    -1,
      53,    54,    -1,    -1,    -1,    -1,     5,     6,    36,    37,
      38,    39,    -1,    -1,    42,    43,    44,    45,    46,    47,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    80,    81,    -1,
      29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    92,
      -1,    94,    -1,    96,    -1,    44,    45,    -1,   101,    48,
      49,    -1,    51,    -1,    53,    54,    -1,    -1,    -1,    -1,
      -1,     3,    -1,    -1,    92,    93,    94,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    80,    81,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    92,    -1,    94,    -1,    96,     3,    -1,
      -1,    -1,   101,    -1,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    30,    31,    32,    33,    34,
      35,     3,    -1,    -1,    -1,    -1,    -1,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,     3,    -1,    -1,    -1,    -1,    -1,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    -1,    23,    24,    25,    26,    27,    28,    -1,    30,
      31,    32,    33,    34,    35,    23,    24,    25,    -1,    27,
      28,    -1,    30,    31,    32,    33,    34,    35
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     5,     6,    29,    44,    45,    48,    49,    51,
      53,    54,    55,    56,    57,    58,    60,    63,    66,    67,
      68,    69,    71,    73,    74,    80,    81,    84,    85,    87,
      88,    89,    92,    94,    95,    96,   101,   104,   105,   106,
     107,   111,   112,   114,   115,   116,   117,   118,   119,   122,
     126,   127,   128,   133,   134,   135,   136,   137,   138,   139,
     140,   142,   146,   147,   152,   157,   158,   159,   160,   168,
     169,   171,   172,   177,   179,   180,   203,    96,   101,   114,
     133,   133,   133,   133,   133,   202,   202,   202,   202,   202,
     202,   202,   202,   141,   141,   108,   109,   110,   111,   170,
     172,   202,   139,     4,    29,    98,   100,   123,   124,   125,
     129,   130,   131,   139,    97,    98,    99,   120,   121,   125,
     131,    75,    92,   161,   163,     0,    84,    95,   203,    98,
      99,   204,   205,   113,   117,     5,     6,     7,     8,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    92,    93,    94,   132,     4,    23,    24,    25,    26,
      27,    28,    30,    31,    32,    33,    34,    35,     3,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    86,    82,    83,    96,   114,   166,   167,
     173,   174,    92,   181,    95,    98,    99,   206,   207,    92,
     136,   137,    92,   137,   139,   207,   139,   148,   149,   139,
     207,   207,    29,   114,   130,   143,   145,   143,    84,   206,
     207,   206,   207,   102,   100,   100,    98,    99,    97,    97,
      97,    98,    90,     1,   145,   164,   165,   111,    98,    99,
     111,   117,   133,   133,   133,   133,   133,   133,   133,   133,
     133,   133,   133,   133,   102,   131,   100,   131,    50,    92,
     133,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   139,   139,   139,   139,   139,   139,   139,
     139,   139,   139,   139,   139,   139,   139,   139,   202,    93,
      93,    97,   114,   167,     3,    99,   163,   175,   207,    29,
     114,   182,   183,   114,    95,    98,    99,    76,    77,    78,
      79,   186,   187,   188,   191,   192,   194,   195,   198,   199,
     137,     3,   137,     3,   207,   108,   207,    52,    61,    62,
     150,   151,   207,   108,   108,     3,   145,   111,   172,    84,
     125,   129,   130,   139,   125,   102,   162,    99,    99,   102,
     100,   139,   114,   114,    97,   114,   174,   114,   207,   108,
     114,     3,    99,   102,    30,   184,   185,   202,   202,   202,
     202,   207,   181,   181,   181,   181,     3,   139,     3,   139,
     108,    59,   108,   202,   202,    64,    65,   153,   154,   155,
     156,    70,    52,    72,   144,   145,   102,   108,    52,    84,
     176,   144,   183,   114,    27,   188,   192,   195,   199,   202,
     207,   207,   207,   207,   139,   207,   139,   207,    52,   139,
     207,   207,   202,   202,    52,   155,   156,   202,   202,   139,
     176,   139,    93,   114,    52,   178,   114,   189,   190,   172,
     193,   114,   196,   197,   114,   200,   201,   102,   108,    99,
     108,   139,   108,   207,   207,   207,   207,   114,    93,     3,
     207,   207,   207,    92,   207,   207,    52,   139,    52,   207,
     139,   108,   108,   108,   114,   144,    52,   190,    52,   172,
      52,   197,   139,    52,   201,   108,   102,   108,   207,    52,
      52,   139,   102,    52,   207,   108,    98,   108,    52
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* This macro is provided for backward compatibility. */

#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1806 of yacc.c  */
#line 544 "oct-parse.yy"
    {
                    global_command = (yyvsp[(1) - (1)].tree_statement_list_type);
                    promptflag = 1;
                    YYACCEPT;
                  }
    break;

  case 3:

/* Line 1806 of yacc.c  */
#line 550 "oct-parse.yy"
    { YYACCEPT; }
    break;

  case 4:

/* Line 1806 of yacc.c  */
#line 552 "oct-parse.yy"
    { ABORT_PARSE; }
    break;

  case 5:

/* Line 1806 of yacc.c  */
#line 554 "oct-parse.yy"
    { ABORT_PARSE; }
    break;

  case 6:

/* Line 1806 of yacc.c  */
#line 558 "oct-parse.yy"
    { (yyval.tree_statement_list_type) = 0; }
    break;

  case 7:

/* Line 1806 of yacc.c  */
#line 560 "oct-parse.yy"
    {
                    parser_end_of_input = 1;
                    (yyval.tree_statement_list_type) = 0;
                  }
    break;

  case 8:

/* Line 1806 of yacc.c  */
#line 565 "oct-parse.yy"
    { (yyval.tree_statement_list_type) = (yyvsp[(1) - (1)].tree_statement_list_type); }
    break;

  case 9:

/* Line 1806 of yacc.c  */
#line 567 "oct-parse.yy"
    { (yyval.tree_statement_list_type) = (yyvsp[(1) - (2)].tree_statement_list_type); }
    break;

  case 10:

/* Line 1806 of yacc.c  */
#line 569 "oct-parse.yy"
    { (yyval.tree_statement_list_type) = (yyvsp[(1) - (2)].tree_statement_list_type); }
    break;

  case 11:

/* Line 1806 of yacc.c  */
#line 573 "oct-parse.yy"
    { (yyval.tree_statement_list_type) = set_stmt_print_flag ((yyvsp[(1) - (2)].tree_statement_list_type), (yyvsp[(2) - (2)].sep_type), false); }
    break;

  case 12:

/* Line 1806 of yacc.c  */
#line 577 "oct-parse.yy"
    { (yyval.tree_statement_list_type) = make_statement_list ((yyvsp[(1) - (1)].tree_statement_type)); }
    break;

  case 13:

/* Line 1806 of yacc.c  */
#line 579 "oct-parse.yy"
    { (yyval.tree_statement_list_type) = append_statement_list ((yyvsp[(1) - (3)].tree_statement_list_type), (yyvsp[(2) - (3)].sep_type), (yyvsp[(3) - (3)].tree_statement_type), false); }
    break;

  case 14:

/* Line 1806 of yacc.c  */
#line 583 "oct-parse.yy"
    { (yyval.tree_statement_list_type) = new tree_statement_list (); }
    break;

  case 15:

/* Line 1806 of yacc.c  */
#line 585 "oct-parse.yy"
    { (yyval.tree_statement_list_type) = (yyvsp[(1) - (1)].tree_statement_list_type); }
    break;

  case 16:

/* Line 1806 of yacc.c  */
#line 589 "oct-parse.yy"
    { (yyval.tree_statement_list_type) = set_stmt_print_flag ((yyvsp[(1) - (2)].tree_statement_list_type), (yyvsp[(2) - (2)].sep_type), true); }
    break;

  case 17:

/* Line 1806 of yacc.c  */
#line 593 "oct-parse.yy"
    { (yyval.tree_statement_list_type) = make_statement_list ((yyvsp[(1) - (1)].tree_statement_type)); }
    break;

  case 18:

/* Line 1806 of yacc.c  */
#line 595 "oct-parse.yy"
    { (yyval.tree_statement_list_type) = append_statement_list ((yyvsp[(1) - (3)].tree_statement_list_type), (yyvsp[(2) - (3)].sep_type), (yyvsp[(3) - (3)].tree_statement_type), true); }
    break;

  case 19:

/* Line 1806 of yacc.c  */
#line 599 "oct-parse.yy"
    { (yyval.tree_statement_type) = make_statement ((yyvsp[(1) - (1)].tree_expression_type)); }
    break;

  case 20:

/* Line 1806 of yacc.c  */
#line 601 "oct-parse.yy"
    { (yyval.tree_statement_type) = make_statement ((yyvsp[(1) - (1)].tree_command_type)); }
    break;

  case 21:

/* Line 1806 of yacc.c  */
#line 603 "oct-parse.yy"
    { (yyval.tree_statement_type) = make_statement ((yyvsp[(1) - (1)].tree_index_expression_type)); }
    break;

  case 22:

/* Line 1806 of yacc.c  */
#line 615 "oct-parse.yy"
    { (yyval.tree_index_expression_type) = make_index_expression ((yyvsp[(1) - (2)].tree_identifier_type), (yyvsp[(2) - (2)].tree_argument_list_type), '('); }
    break;

  case 23:

/* Line 1806 of yacc.c  */
#line 619 "oct-parse.yy"
    { (yyval.tree_argument_list_type) = new tree_argument_list ((yyvsp[(1) - (1)].tree_constant_type)); }
    break;

  case 24:

/* Line 1806 of yacc.c  */
#line 621 "oct-parse.yy"
    {
                    (yyvsp[(1) - (2)].tree_argument_list_type)->append ((yyvsp[(2) - (2)].tree_constant_type));
                    (yyval.tree_argument_list_type) = (yyvsp[(1) - (2)].tree_argument_list_type);
                  }
    break;

  case 25:

/* Line 1806 of yacc.c  */
#line 632 "oct-parse.yy"
    {
                    symbol_table::symbol_record *sr = (yyvsp[(1) - (1)].tok_val)->sym_rec ();
                    (yyval.tree_identifier_type) = new tree_identifier (*sr, (yyvsp[(1) - (1)].tok_val)->line (), (yyvsp[(1) - (1)].tok_val)->column ());
                  }
    break;

  case 26:

/* Line 1806 of yacc.c  */
#line 640 "oct-parse.yy"
    { (yyval.tree_identifier_type) = new tree_identifier ((yyvsp[(1) - (1)].tok_val)->line (), (yyvsp[(1) - (1)].tok_val)->column ()); }
    break;

  case 27:

/* Line 1806 of yacc.c  */
#line 644 "oct-parse.yy"
    { (yyval.tree_identifier_type) = new tree_identifier ((yyvsp[(1) - (1)].tok_val)->line (), (yyvsp[(1) - (1)].tok_val)->column ()); }
    break;

  case 28:

/* Line 1806 of yacc.c  */
#line 648 "oct-parse.yy"
    { (yyval.tree_constant_type) = make_constant (DQ_STRING, (yyvsp[(1) - (1)].tok_val)); }
    break;

  case 29:

/* Line 1806 of yacc.c  */
#line 650 "oct-parse.yy"
    { (yyval.tree_constant_type) = make_constant (SQ_STRING, (yyvsp[(1) - (1)].tok_val)); }
    break;

  case 30:

/* Line 1806 of yacc.c  */
#line 654 "oct-parse.yy"
    { (yyval.tree_constant_type) = make_constant (NUM, (yyvsp[(1) - (1)].tok_val)); }
    break;

  case 31:

/* Line 1806 of yacc.c  */
#line 656 "oct-parse.yy"
    { (yyval.tree_constant_type) = make_constant (IMAG_NUM, (yyvsp[(1) - (1)].tok_val)); }
    break;

  case 32:

/* Line 1806 of yacc.c  */
#line 658 "oct-parse.yy"
    { (yyval.tree_constant_type) = (yyvsp[(1) - (1)].tree_constant_type); }
    break;

  case 33:

/* Line 1806 of yacc.c  */
#line 662 "oct-parse.yy"
    {
                    (yyval.tree_expression_type) = new tree_constant (octave_null_matrix::instance);
                    lexer_flags.looking_at_matrix_or_assign_lhs = false;
                    lexer_flags.pending_local_variables.clear ();
                  }
    break;

  case 34:

/* Line 1806 of yacc.c  */
#line 668 "oct-parse.yy"
    {
                    (yyval.tree_expression_type) = new tree_constant (octave_null_matrix::instance);
                    lexer_flags.looking_at_matrix_or_assign_lhs = false;
                    lexer_flags.pending_local_variables.clear ();
                  }
    break;

  case 35:

/* Line 1806 of yacc.c  */
#line 674 "oct-parse.yy"
    {
                    (yyval.tree_expression_type) = new tree_constant (octave_null_matrix::instance);
                    lexer_flags.looking_at_matrix_or_assign_lhs = false;
                    lexer_flags.pending_local_variables.clear ();
                  }
    break;

  case 36:

/* Line 1806 of yacc.c  */
#line 680 "oct-parse.yy"
    {
                    (yyval.tree_expression_type) = finish_matrix ((yyvsp[(2) - (3)].tree_matrix_type));
                    lexer_flags.looking_at_matrix_or_assign_lhs = false;
                    lexer_flags.pending_local_variables.clear ();
                  }
    break;

  case 37:

/* Line 1806 of yacc.c  */
#line 688 "oct-parse.yy"
    { (yyval.tree_matrix_type) = (yyvsp[(1) - (1)].tree_matrix_type); }
    break;

  case 38:

/* Line 1806 of yacc.c  */
#line 690 "oct-parse.yy"
    { (yyval.tree_matrix_type) = (yyvsp[(1) - (2)].tree_matrix_type); }
    break;

  case 39:

/* Line 1806 of yacc.c  */
#line 694 "oct-parse.yy"
    { (yyval.tree_matrix_type) = new tree_matrix ((yyvsp[(1) - (1)].tree_argument_list_type)); }
    break;

  case 40:

/* Line 1806 of yacc.c  */
#line 696 "oct-parse.yy"
    {
                    (yyvsp[(1) - (3)].tree_matrix_type)->append ((yyvsp[(3) - (3)].tree_argument_list_type));
                    (yyval.tree_matrix_type) = (yyvsp[(1) - (3)].tree_matrix_type);
                  }
    break;

  case 41:

/* Line 1806 of yacc.c  */
#line 703 "oct-parse.yy"
    { (yyval.tree_expression_type) = new tree_constant (octave_value (Cell ())); }
    break;

  case 42:

/* Line 1806 of yacc.c  */
#line 705 "oct-parse.yy"
    { (yyval.tree_expression_type) = new tree_constant (octave_value (Cell ())); }
    break;

  case 43:

/* Line 1806 of yacc.c  */
#line 707 "oct-parse.yy"
    { (yyval.tree_expression_type) = finish_cell ((yyvsp[(2) - (3)].tree_cell_type)); }
    break;

  case 44:

/* Line 1806 of yacc.c  */
#line 711 "oct-parse.yy"
    { (yyval.tree_cell_type) = (yyvsp[(1) - (1)].tree_cell_type); }
    break;

  case 45:

/* Line 1806 of yacc.c  */
#line 713 "oct-parse.yy"
    { (yyval.tree_cell_type) = (yyvsp[(1) - (2)].tree_cell_type); }
    break;

  case 46:

/* Line 1806 of yacc.c  */
#line 717 "oct-parse.yy"
    { (yyval.tree_cell_type) = new tree_cell ((yyvsp[(1) - (1)].tree_argument_list_type)); }
    break;

  case 47:

/* Line 1806 of yacc.c  */
#line 719 "oct-parse.yy"
    {
                    (yyvsp[(1) - (3)].tree_cell_type)->append ((yyvsp[(3) - (3)].tree_argument_list_type));
                    (yyval.tree_cell_type) = (yyvsp[(1) - (3)].tree_cell_type);
                  }
    break;

  case 48:

/* Line 1806 of yacc.c  */
#line 727 "oct-parse.yy"
    { (yyval.tree_argument_list_type) = validate_matrix_row ((yyvsp[(1) - (1)].tree_argument_list_type)); }
    break;

  case 49:

/* Line 1806 of yacc.c  */
#line 729 "oct-parse.yy"
    { (yyval.tree_argument_list_type) = validate_matrix_row ((yyvsp[(1) - (2)].tree_argument_list_type)); }
    break;

  case 50:

/* Line 1806 of yacc.c  */
#line 733 "oct-parse.yy"
    {
                    (yyval.tree_fcn_handle_type) = make_fcn_handle ((yyvsp[(2) - (2)].tok_val));
                    lexer_flags.looking_at_function_handle--;
                  }
    break;

  case 51:

/* Line 1806 of yacc.c  */
#line 740 "oct-parse.yy"
    {
                    lexer_flags.quote_is_transpose = false;
                    (yyval.tree_anon_fcn_handle_type) = make_anon_fcn_handle ((yyvsp[(2) - (3)].tree_parameter_list_type), (yyvsp[(3) - (3)].tree_statement_type));
                  }
    break;

  case 52:

/* Line 1806 of yacc.c  */
#line 747 "oct-parse.yy"
    { (yyval.tree_expression_type) = (yyvsp[(1) - (1)].tree_identifier_type); }
    break;

  case 53:

/* Line 1806 of yacc.c  */
#line 749 "oct-parse.yy"
    { (yyval.tree_expression_type) = (yyvsp[(1) - (1)].tree_constant_type); }
    break;

  case 54:

/* Line 1806 of yacc.c  */
#line 751 "oct-parse.yy"
    { (yyval.tree_expression_type) = (yyvsp[(1) - (1)].tree_fcn_handle_type); }
    break;

  case 55:

/* Line 1806 of yacc.c  */
#line 753 "oct-parse.yy"
    { (yyval.tree_expression_type) = (yyvsp[(1) - (1)].tree_expression_type); }
    break;

  case 56:

/* Line 1806 of yacc.c  */
#line 755 "oct-parse.yy"
    { (yyval.tree_expression_type) = (yyvsp[(1) - (1)].tree_expression_type); }
    break;

  case 57:

/* Line 1806 of yacc.c  */
#line 757 "oct-parse.yy"
    { (yyval.tree_expression_type) = (yyvsp[(1) - (1)].tree_identifier_type); }
    break;

  case 58:

/* Line 1806 of yacc.c  */
#line 759 "oct-parse.yy"
    { (yyval.tree_expression_type) = (yyvsp[(1) - (1)].tree_identifier_type); }
    break;

  case 59:

/* Line 1806 of yacc.c  */
#line 761 "oct-parse.yy"
    { (yyval.tree_expression_type) = (yyvsp[(2) - (3)].tree_expression_type)->mark_in_parens (); }
    break;

  case 60:

/* Line 1806 of yacc.c  */
#line 765 "oct-parse.yy"
    {
                    octave_value tmp (octave_value::magic_colon_t);
                    (yyval.tree_constant_type) = new tree_constant (tmp);
                  }
    break;

  case 61:

/* Line 1806 of yacc.c  */
#line 772 "oct-parse.yy"
    {
                    (yyval.tree_identifier_type) = new tree_black_hole ();
                  }
    break;

  case 62:

/* Line 1806 of yacc.c  */
#line 778 "oct-parse.yy"
    { (yyval.tree_argument_list_type) = new tree_argument_list ((yyvsp[(1) - (1)].tree_expression_type)); }
    break;

  case 63:

/* Line 1806 of yacc.c  */
#line 780 "oct-parse.yy"
    { (yyval.tree_argument_list_type) = new tree_argument_list ((yyvsp[(1) - (1)].tree_constant_type)); }
    break;

  case 64:

/* Line 1806 of yacc.c  */
#line 782 "oct-parse.yy"
    { (yyval.tree_argument_list_type) = new tree_argument_list ((yyvsp[(1) - (1)].tree_identifier_type)); }
    break;

  case 65:

/* Line 1806 of yacc.c  */
#line 784 "oct-parse.yy"
    {
                    (yyvsp[(1) - (3)].tree_argument_list_type)->append ((yyvsp[(3) - (3)].tree_constant_type));
                    (yyval.tree_argument_list_type) = (yyvsp[(1) - (3)].tree_argument_list_type);
                  }
    break;

  case 66:

/* Line 1806 of yacc.c  */
#line 789 "oct-parse.yy"
    {
                    (yyvsp[(1) - (3)].tree_argument_list_type)->append ((yyvsp[(3) - (3)].tree_identifier_type));
                    (yyval.tree_argument_list_type) = (yyvsp[(1) - (3)].tree_argument_list_type);
                  }
    break;

  case 67:

/* Line 1806 of yacc.c  */
#line 794 "oct-parse.yy"
    {
                    (yyvsp[(1) - (3)].tree_argument_list_type)->append ((yyvsp[(3) - (3)].tree_expression_type));
                    (yyval.tree_argument_list_type) = (yyvsp[(1) - (3)].tree_argument_list_type);
                  }
    break;

  case 68:

/* Line 1806 of yacc.c  */
#line 801 "oct-parse.yy"
    { lexer_flags.looking_at_indirect_ref = true; }
    break;

  case 69:

/* Line 1806 of yacc.c  */
#line 805 "oct-parse.yy"
    { (yyval.tree_expression_type) = (yyvsp[(1) - (1)].tree_expression_type); }
    break;

  case 70:

/* Line 1806 of yacc.c  */
#line 807 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_postfix_op (PLUS_PLUS, (yyvsp[(1) - (2)].tree_expression_type), (yyvsp[(2) - (2)].tok_val)); }
    break;

  case 71:

/* Line 1806 of yacc.c  */
#line 809 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_postfix_op (MINUS_MINUS, (yyvsp[(1) - (2)].tree_expression_type), (yyvsp[(2) - (2)].tok_val)); }
    break;

  case 72:

/* Line 1806 of yacc.c  */
#line 811 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_index_expression ((yyvsp[(1) - (3)].tree_expression_type), 0, '('); }
    break;

  case 73:

/* Line 1806 of yacc.c  */
#line 813 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_index_expression ((yyvsp[(1) - (4)].tree_expression_type), (yyvsp[(3) - (4)].tree_argument_list_type), '('); }
    break;

  case 74:

/* Line 1806 of yacc.c  */
#line 815 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_index_expression ((yyvsp[(1) - (3)].tree_expression_type), 0, '{'); }
    break;

  case 75:

/* Line 1806 of yacc.c  */
#line 817 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_index_expression ((yyvsp[(1) - (4)].tree_expression_type), (yyvsp[(3) - (4)].tree_argument_list_type), '{'); }
    break;

  case 76:

/* Line 1806 of yacc.c  */
#line 819 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_postfix_op (QUOTE, (yyvsp[(1) - (2)].tree_expression_type), (yyvsp[(2) - (2)].tok_val)); }
    break;

  case 77:

/* Line 1806 of yacc.c  */
#line 821 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_postfix_op (TRANSPOSE, (yyvsp[(1) - (2)].tree_expression_type), (yyvsp[(2) - (2)].tok_val)); }
    break;

  case 78:

/* Line 1806 of yacc.c  */
#line 823 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_indirect_ref ((yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(3) - (3)].tok_val)->text ()); }
    break;

  case 79:

/* Line 1806 of yacc.c  */
#line 825 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_indirect_ref ((yyvsp[(1) - (5)].tree_expression_type), (yyvsp[(4) - (5)].tree_expression_type)); }
    break;

  case 80:

/* Line 1806 of yacc.c  */
#line 827 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_prefix_op (PLUS_PLUS, (yyvsp[(2) - (2)].tree_expression_type), (yyvsp[(1) - (2)].tok_val)); }
    break;

  case 81:

/* Line 1806 of yacc.c  */
#line 829 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_prefix_op (MINUS_MINUS, (yyvsp[(2) - (2)].tree_expression_type), (yyvsp[(1) - (2)].tok_val)); }
    break;

  case 82:

/* Line 1806 of yacc.c  */
#line 831 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_prefix_op (EXPR_NOT, (yyvsp[(2) - (2)].tree_expression_type), (yyvsp[(1) - (2)].tok_val)); }
    break;

  case 83:

/* Line 1806 of yacc.c  */
#line 833 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_prefix_op ('+', (yyvsp[(2) - (2)].tree_expression_type), (yyvsp[(1) - (2)].tok_val)); }
    break;

  case 84:

/* Line 1806 of yacc.c  */
#line 835 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_prefix_op ('-', (yyvsp[(2) - (2)].tree_expression_type), (yyvsp[(1) - (2)].tok_val)); }
    break;

  case 85:

/* Line 1806 of yacc.c  */
#line 837 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_binary_op (POW, (yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 86:

/* Line 1806 of yacc.c  */
#line 839 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_binary_op (EPOW, (yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 87:

/* Line 1806 of yacc.c  */
#line 841 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_binary_op ('+', (yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 88:

/* Line 1806 of yacc.c  */
#line 843 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_binary_op ('-', (yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 89:

/* Line 1806 of yacc.c  */
#line 845 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_binary_op ('*', (yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 90:

/* Line 1806 of yacc.c  */
#line 847 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_binary_op ('/', (yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 91:

/* Line 1806 of yacc.c  */
#line 849 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_binary_op ('+', (yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 92:

/* Line 1806 of yacc.c  */
#line 851 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_binary_op ('-', (yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 93:

/* Line 1806 of yacc.c  */
#line 853 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_binary_op (EMUL, (yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 94:

/* Line 1806 of yacc.c  */
#line 855 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_binary_op (EDIV, (yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 95:

/* Line 1806 of yacc.c  */
#line 857 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_binary_op (LEFTDIV, (yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 96:

/* Line 1806 of yacc.c  */
#line 859 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_binary_op (ELEFTDIV, (yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 97:

/* Line 1806 of yacc.c  */
#line 863 "oct-parse.yy"
    { (yyval.tree_expression_type) = finish_colon_expression ((yyvsp[(1) - (1)].tree_colon_expression_type)); }
    break;

  case 98:

/* Line 1806 of yacc.c  */
#line 867 "oct-parse.yy"
    { (yyval.tree_colon_expression_type) = new tree_colon_expression ((yyvsp[(1) - (1)].tree_expression_type)); }
    break;

  case 99:

/* Line 1806 of yacc.c  */
#line 869 "oct-parse.yy"
    {
                    if (! ((yyval.tree_colon_expression_type) = (yyvsp[(1) - (3)].tree_colon_expression_type)->append ((yyvsp[(3) - (3)].tree_expression_type))))
                      ABORT_PARSE;
                  }
    break;

  case 100:

/* Line 1806 of yacc.c  */
#line 876 "oct-parse.yy"
    { (yyval.tree_expression_type) = (yyvsp[(1) - (1)].tree_expression_type); }
    break;

  case 101:

/* Line 1806 of yacc.c  */
#line 878 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_binary_op (LSHIFT, (yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 102:

/* Line 1806 of yacc.c  */
#line 880 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_binary_op (RSHIFT, (yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 103:

/* Line 1806 of yacc.c  */
#line 882 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_binary_op (EXPR_LT, (yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 104:

/* Line 1806 of yacc.c  */
#line 884 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_binary_op (EXPR_LE, (yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 105:

/* Line 1806 of yacc.c  */
#line 886 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_binary_op (EXPR_EQ, (yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 106:

/* Line 1806 of yacc.c  */
#line 888 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_binary_op (EXPR_GE, (yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 107:

/* Line 1806 of yacc.c  */
#line 890 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_binary_op (EXPR_GT, (yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 108:

/* Line 1806 of yacc.c  */
#line 892 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_binary_op (EXPR_NE, (yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 109:

/* Line 1806 of yacc.c  */
#line 894 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_binary_op (EXPR_AND, (yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 110:

/* Line 1806 of yacc.c  */
#line 896 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_binary_op (EXPR_OR, (yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 111:

/* Line 1806 of yacc.c  */
#line 898 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_boolean_op (EXPR_AND_AND, (yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 112:

/* Line 1806 of yacc.c  */
#line 900 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_boolean_op (EXPR_OR_OR, (yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 113:

/* Line 1806 of yacc.c  */
#line 907 "oct-parse.yy"
    {
                    (yyval.tree_argument_list_type) = new tree_argument_list ((yyvsp[(1) - (1)].tree_expression_type));
                    (yyval.tree_argument_list_type)->mark_as_simple_assign_lhs ();
                  }
    break;

  case 114:

/* Line 1806 of yacc.c  */
#line 912 "oct-parse.yy"
    {
                    (yyval.tree_argument_list_type) = (yyvsp[(2) - (3)].tree_argument_list_type);
                    lexer_flags.looking_at_matrix_or_assign_lhs = false;
                    for (std::set<std::string>::const_iterator p = lexer_flags.pending_local_variables.begin ();
                         p != lexer_flags.pending_local_variables.end ();
                         p++)
                      {
                        symbol_table::force_variable (*p);
                      }
                    lexer_flags.pending_local_variables.clear ();
                  }
    break;

  case 115:

/* Line 1806 of yacc.c  */
#line 926 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_assign_op ('=', (yyvsp[(1) - (3)].tree_argument_list_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 116:

/* Line 1806 of yacc.c  */
#line 928 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_assign_op (ADD_EQ, (yyvsp[(1) - (3)].tree_argument_list_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 117:

/* Line 1806 of yacc.c  */
#line 930 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_assign_op (SUB_EQ, (yyvsp[(1) - (3)].tree_argument_list_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 118:

/* Line 1806 of yacc.c  */
#line 932 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_assign_op (MUL_EQ, (yyvsp[(1) - (3)].tree_argument_list_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 119:

/* Line 1806 of yacc.c  */
#line 934 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_assign_op (DIV_EQ, (yyvsp[(1) - (3)].tree_argument_list_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 120:

/* Line 1806 of yacc.c  */
#line 936 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_assign_op (LEFTDIV_EQ, (yyvsp[(1) - (3)].tree_argument_list_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 121:

/* Line 1806 of yacc.c  */
#line 938 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_assign_op (POW_EQ, (yyvsp[(1) - (3)].tree_argument_list_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 122:

/* Line 1806 of yacc.c  */
#line 940 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_assign_op (LSHIFT_EQ, (yyvsp[(1) - (3)].tree_argument_list_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 123:

/* Line 1806 of yacc.c  */
#line 942 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_assign_op (RSHIFT_EQ, (yyvsp[(1) - (3)].tree_argument_list_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 124:

/* Line 1806 of yacc.c  */
#line 944 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_assign_op (EMUL_EQ, (yyvsp[(1) - (3)].tree_argument_list_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 125:

/* Line 1806 of yacc.c  */
#line 946 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_assign_op (EDIV_EQ, (yyvsp[(1) - (3)].tree_argument_list_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 126:

/* Line 1806 of yacc.c  */
#line 948 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_assign_op (ELEFTDIV_EQ, (yyvsp[(1) - (3)].tree_argument_list_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 127:

/* Line 1806 of yacc.c  */
#line 950 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_assign_op (EPOW_EQ, (yyvsp[(1) - (3)].tree_argument_list_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 128:

/* Line 1806 of yacc.c  */
#line 952 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_assign_op (AND_EQ, (yyvsp[(1) - (3)].tree_argument_list_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 129:

/* Line 1806 of yacc.c  */
#line 954 "oct-parse.yy"
    { (yyval.tree_expression_type) = make_assign_op (OR_EQ, (yyvsp[(1) - (3)].tree_argument_list_type), (yyvsp[(2) - (3)].tok_val), (yyvsp[(3) - (3)].tree_expression_type)); }
    break;

  case 130:

/* Line 1806 of yacc.c  */
#line 958 "oct-parse.yy"
    { (yyval.tree_expression_type) = (yyvsp[(1) - (1)].tree_expression_type); }
    break;

  case 131:

/* Line 1806 of yacc.c  */
#line 960 "oct-parse.yy"
    { (yyval.tree_expression_type) = (yyvsp[(1) - (1)].tree_expression_type); }
    break;

  case 132:

/* Line 1806 of yacc.c  */
#line 962 "oct-parse.yy"
    { (yyval.tree_expression_type) = (yyvsp[(1) - (1)].tree_anon_fcn_handle_type); }
    break;

  case 133:

/* Line 1806 of yacc.c  */
#line 970 "oct-parse.yy"
    { (yyval.tree_command_type) = (yyvsp[(1) - (1)].tree_decl_command_type); }
    break;

  case 134:

/* Line 1806 of yacc.c  */
#line 972 "oct-parse.yy"
    { (yyval.tree_command_type) = (yyvsp[(1) - (1)].tree_command_type); }
    break;

  case 135:

/* Line 1806 of yacc.c  */
#line 974 "oct-parse.yy"
    { (yyval.tree_command_type) = (yyvsp[(1) - (1)].tree_command_type); }
    break;

  case 136:

/* Line 1806 of yacc.c  */
#line 976 "oct-parse.yy"
    { (yyval.tree_command_type) = (yyvsp[(1) - (1)].tree_command_type); }
    break;

  case 137:

/* Line 1806 of yacc.c  */
#line 978 "oct-parse.yy"
    { (yyval.tree_command_type) = (yyvsp[(1) - (1)].tree_command_type); }
    break;

  case 138:

/* Line 1806 of yacc.c  */
#line 980 "oct-parse.yy"
    { (yyval.tree_command_type) = (yyvsp[(1) - (1)].tree_command_type); }
    break;

  case 139:

/* Line 1806 of yacc.c  */
#line 982 "oct-parse.yy"
    { (yyval.tree_command_type) = (yyvsp[(1) - (1)].tree_command_type); }
    break;

  case 140:

/* Line 1806 of yacc.c  */
#line 984 "oct-parse.yy"
    { (yyval.tree_command_type) = (yyvsp[(1) - (1)].tree_command_type); }
    break;

  case 141:

/* Line 1806 of yacc.c  */
#line 993 "oct-parse.yy"
    { lexer_flags.looking_at_decl_list = true; }
    break;

  case 142:

/* Line 1806 of yacc.c  */
#line 996 "oct-parse.yy"
    {
                    (yyval.tree_decl_command_type) = make_decl_command (GLOBAL, (yyvsp[(1) - (3)].tok_val), (yyvsp[(3) - (3)].tree_decl_init_list_type));
                    lexer_flags.looking_at_decl_list = false;
                  }
    break;

  case 143:

/* Line 1806 of yacc.c  */
#line 1001 "oct-parse.yy"
    {
                    (yyval.tree_decl_command_type) = make_decl_command (STATIC, (yyvsp[(1) - (3)].tok_val), (yyvsp[(3) - (3)].tree_decl_init_list_type));
                    lexer_flags.looking_at_decl_list = false;
                  }
    break;

  case 144:

/* Line 1806 of yacc.c  */
#line 1008 "oct-parse.yy"
    { (yyval.tree_decl_init_list_type) = new tree_decl_init_list ((yyvsp[(1) - (1)].tree_decl_elt_type)); }
    break;

  case 145:

/* Line 1806 of yacc.c  */
#line 1010 "oct-parse.yy"
    {
                    (yyvsp[(1) - (2)].tree_decl_init_list_type)->append ((yyvsp[(2) - (2)].tree_decl_elt_type));
                    (yyval.tree_decl_init_list_type) = (yyvsp[(1) - (2)].tree_decl_init_list_type);
                  }
    break;

  case 146:

/* Line 1806 of yacc.c  */
#line 1017 "oct-parse.yy"
    { lexer_flags.looking_at_initializer_expression = true; }
    break;

  case 147:

/* Line 1806 of yacc.c  */
#line 1020 "oct-parse.yy"
    { (yyval.tree_decl_elt_type) = new tree_decl_elt ((yyvsp[(1) - (1)].tree_identifier_type)); }
    break;

  case 148:

/* Line 1806 of yacc.c  */
#line 1022 "oct-parse.yy"
    {
                    lexer_flags.looking_at_initializer_expression = false;
                    (yyval.tree_decl_elt_type) = new tree_decl_elt ((yyvsp[(1) - (4)].tree_identifier_type), (yyvsp[(4) - (4)].tree_expression_type));
                  }
    break;

  case 149:

/* Line 1806 of yacc.c  */
#line 1027 "oct-parse.yy"
    {
                    (yyval.tree_decl_elt_type) = new tree_decl_elt ((yyvsp[(1) - (1)].tree_identifier_type));
                  }
    break;

  case 150:

/* Line 1806 of yacc.c  */
#line 1037 "oct-parse.yy"
    { (yyval.tree_command_type) = (yyvsp[(1) - (1)].tree_if_command_type); }
    break;

  case 151:

/* Line 1806 of yacc.c  */
#line 1039 "oct-parse.yy"
    { (yyval.tree_command_type) = (yyvsp[(1) - (1)].tree_switch_command_type); }
    break;

  case 152:

/* Line 1806 of yacc.c  */
#line 1047 "oct-parse.yy"
    {
                    if (! ((yyval.tree_if_command_type) = finish_if_command ((yyvsp[(1) - (4)].tok_val), (yyvsp[(3) - (4)].tree_if_command_list_type), (yyvsp[(4) - (4)].tok_val), (yyvsp[(2) - (4)].comment_type))))
                      ABORT_PARSE;
                  }
    break;

  case 153:

/* Line 1806 of yacc.c  */
#line 1054 "oct-parse.yy"
    { (yyval.tree_if_command_list_type) = (yyvsp[(1) - (1)].tree_if_command_list_type); }
    break;

  case 154:

/* Line 1806 of yacc.c  */
#line 1056 "oct-parse.yy"
    {
                    (yyvsp[(1) - (2)].tree_if_command_list_type)->append ((yyvsp[(2) - (2)].tree_if_clause_type));
                    (yyval.tree_if_command_list_type) = (yyvsp[(1) - (2)].tree_if_command_list_type);
                  }
    break;

  case 155:

/* Line 1806 of yacc.c  */
#line 1063 "oct-parse.yy"
    {
                    (yyvsp[(1) - (3)].tree_expression_type)->mark_braindead_shortcircuit (curr_fcn_file_full_name);

                    (yyval.tree_if_command_list_type) = start_if_command ((yyvsp[(1) - (3)].tree_expression_type), (yyvsp[(3) - (3)].tree_statement_list_type));
                  }
    break;

  case 156:

/* Line 1806 of yacc.c  */
#line 1069 "oct-parse.yy"
    {
                    (yyvsp[(1) - (2)].tree_if_command_list_type)->append ((yyvsp[(2) - (2)].tree_if_clause_type));
                    (yyval.tree_if_command_list_type) = (yyvsp[(1) - (2)].tree_if_command_list_type);
                  }
    break;

  case 157:

/* Line 1806 of yacc.c  */
#line 1076 "oct-parse.yy"
    {
                    (yyvsp[(4) - (6)].tree_expression_type)->mark_braindead_shortcircuit (curr_fcn_file_full_name);

                    (yyval.tree_if_clause_type) = make_elseif_clause ((yyvsp[(1) - (6)].tok_val), (yyvsp[(4) - (6)].tree_expression_type), (yyvsp[(6) - (6)].tree_statement_list_type), (yyvsp[(2) - (6)].comment_type));
                  }
    break;

  case 158:

/* Line 1806 of yacc.c  */
#line 1084 "oct-parse.yy"
    { (yyval.tree_if_clause_type) = new tree_if_clause ((yyvsp[(4) - (4)].tree_statement_list_type), (yyvsp[(2) - (4)].comment_type)); }
    break;

  case 159:

/* Line 1806 of yacc.c  */
#line 1092 "oct-parse.yy"
    {
                    if (! ((yyval.tree_switch_command_type) = finish_switch_command ((yyvsp[(1) - (6)].tok_val), (yyvsp[(3) - (6)].tree_expression_type), (yyvsp[(5) - (6)].tree_switch_case_list_type), (yyvsp[(6) - (6)].tok_val), (yyvsp[(2) - (6)].comment_type))))
                      ABORT_PARSE;
                  }
    break;

  case 160:

/* Line 1806 of yacc.c  */
#line 1099 "oct-parse.yy"
    { (yyval.tree_switch_case_list_type) = new tree_switch_case_list (); }
    break;

  case 161:

/* Line 1806 of yacc.c  */
#line 1101 "oct-parse.yy"
    { (yyval.tree_switch_case_list_type) = new tree_switch_case_list ((yyvsp[(1) - (1)].tree_switch_case_type)); }
    break;

  case 162:

/* Line 1806 of yacc.c  */
#line 1103 "oct-parse.yy"
    { (yyval.tree_switch_case_list_type) = (yyvsp[(1) - (1)].tree_switch_case_list_type); }
    break;

  case 163:

/* Line 1806 of yacc.c  */
#line 1105 "oct-parse.yy"
    {
                    (yyvsp[(1) - (2)].tree_switch_case_list_type)->append ((yyvsp[(2) - (2)].tree_switch_case_type));
                    (yyval.tree_switch_case_list_type) = (yyvsp[(1) - (2)].tree_switch_case_list_type);
                  }
    break;

  case 164:

/* Line 1806 of yacc.c  */
#line 1112 "oct-parse.yy"
    { (yyval.tree_switch_case_list_type) = new tree_switch_case_list ((yyvsp[(1) - (1)].tree_switch_case_type)); }
    break;

  case 165:

/* Line 1806 of yacc.c  */
#line 1114 "oct-parse.yy"
    {
                    (yyvsp[(1) - (2)].tree_switch_case_list_type)->append ((yyvsp[(2) - (2)].tree_switch_case_type));
                    (yyval.tree_switch_case_list_type) = (yyvsp[(1) - (2)].tree_switch_case_list_type);
                  }
    break;

  case 166:

/* Line 1806 of yacc.c  */
#line 1121 "oct-parse.yy"
    { (yyval.tree_switch_case_type) = make_switch_case ((yyvsp[(1) - (6)].tok_val), (yyvsp[(4) - (6)].tree_expression_type), (yyvsp[(6) - (6)].tree_statement_list_type), (yyvsp[(2) - (6)].comment_type)); }
    break;

  case 167:

/* Line 1806 of yacc.c  */
#line 1125 "oct-parse.yy"
    {
                    (yyval.tree_switch_case_type) = new tree_switch_case ((yyvsp[(4) - (4)].tree_statement_list_type), (yyvsp[(2) - (4)].comment_type));
                  }
    break;

  case 168:

/* Line 1806 of yacc.c  */
#line 1135 "oct-parse.yy"
    {
                    (yyvsp[(3) - (6)].tree_expression_type)->mark_braindead_shortcircuit (curr_fcn_file_full_name);

                    if (! ((yyval.tree_command_type) = make_while_command ((yyvsp[(1) - (6)].tok_val), (yyvsp[(3) - (6)].tree_expression_type), (yyvsp[(5) - (6)].tree_statement_list_type), (yyvsp[(6) - (6)].tok_val), (yyvsp[(2) - (6)].comment_type))))
                      ABORT_PARSE;
                  }
    break;

  case 169:

/* Line 1806 of yacc.c  */
#line 1142 "oct-parse.yy"
    {
                    if (! ((yyval.tree_command_type) = make_do_until_command ((yyvsp[(5) - (6)].tok_val), (yyvsp[(4) - (6)].tree_statement_list_type), (yyvsp[(6) - (6)].tree_expression_type), (yyvsp[(2) - (6)].comment_type))))
                      ABORT_PARSE;
                  }
    break;

  case 170:

/* Line 1806 of yacc.c  */
#line 1147 "oct-parse.yy"
    {
                    if (! ((yyval.tree_command_type) = make_for_command (FOR, (yyvsp[(1) - (8)].tok_val), (yyvsp[(3) - (8)].tree_argument_list_type), (yyvsp[(5) - (8)].tree_expression_type), 0,
                                                  (yyvsp[(7) - (8)].tree_statement_list_type), (yyvsp[(8) - (8)].tok_val), (yyvsp[(2) - (8)].comment_type))))
                      ABORT_PARSE;
                  }
    break;

  case 171:

/* Line 1806 of yacc.c  */
#line 1153 "oct-parse.yy"
    {
                    if (! ((yyval.tree_command_type) = make_for_command (FOR, (yyvsp[(1) - (10)].tok_val), (yyvsp[(4) - (10)].tree_argument_list_type), (yyvsp[(6) - (10)].tree_expression_type), 0,
                                                  (yyvsp[(9) - (10)].tree_statement_list_type), (yyvsp[(10) - (10)].tok_val), (yyvsp[(2) - (10)].comment_type))))
                      ABORT_PARSE;
                  }
    break;

  case 172:

/* Line 1806 of yacc.c  */
#line 1159 "oct-parse.yy"
    {
                    if (! ((yyval.tree_command_type) = make_for_command (PARFOR, (yyvsp[(1) - (8)].tok_val), (yyvsp[(3) - (8)].tree_argument_list_type), (yyvsp[(5) - (8)].tree_expression_type),
                                                  0, (yyvsp[(7) - (8)].tree_statement_list_type), (yyvsp[(8) - (8)].tok_val), (yyvsp[(2) - (8)].comment_type))))
                      ABORT_PARSE;
                  }
    break;

  case 173:

/* Line 1806 of yacc.c  */
#line 1165 "oct-parse.yy"
    {
                    if (! ((yyval.tree_command_type) = make_for_command (PARFOR, (yyvsp[(1) - (12)].tok_val), (yyvsp[(4) - (12)].tree_argument_list_type), (yyvsp[(6) - (12)].tree_expression_type),
                                                  (yyvsp[(8) - (12)].tree_expression_type), (yyvsp[(11) - (12)].tree_statement_list_type), (yyvsp[(12) - (12)].tok_val), (yyvsp[(2) - (12)].comment_type))))
                      ABORT_PARSE;
                  }
    break;

  case 174:

/* Line 1806 of yacc.c  */
#line 1177 "oct-parse.yy"
    {
                    if (! ((yyval.tree_command_type) = make_break_command ((yyvsp[(1) - (1)].tok_val))))
                      ABORT_PARSE;
                  }
    break;

  case 175:

/* Line 1806 of yacc.c  */
#line 1182 "oct-parse.yy"
    {
                    if (! ((yyval.tree_command_type) = make_continue_command ((yyvsp[(1) - (1)].tok_val))))
                      ABORT_PARSE;
                  }
    break;

  case 176:

/* Line 1806 of yacc.c  */
#line 1187 "oct-parse.yy"
    {
                    if (! ((yyval.tree_command_type) = make_return_command ((yyvsp[(1) - (1)].tok_val))))
                      ABORT_PARSE;
                  }
    break;

  case 177:

/* Line 1806 of yacc.c  */
#line 1199 "oct-parse.yy"
    {
                    if (! ((yyval.tree_command_type) = make_unwind_command ((yyvsp[(1) - (9)].tok_val), (yyvsp[(4) - (9)].tree_statement_list_type), (yyvsp[(8) - (9)].tree_statement_list_type), (yyvsp[(9) - (9)].tok_val), (yyvsp[(2) - (9)].comment_type), (yyvsp[(6) - (9)].comment_type))))
                      ABORT_PARSE;
                  }
    break;

  case 178:

/* Line 1806 of yacc.c  */
#line 1205 "oct-parse.yy"
    {
                    if (! ((yyval.tree_command_type) = make_try_command ((yyvsp[(1) - (9)].tok_val), (yyvsp[(4) - (9)].tree_statement_list_type), (yyvsp[(8) - (9)].tree_statement_list_type), (yyvsp[(9) - (9)].tok_val), (yyvsp[(2) - (9)].comment_type), (yyvsp[(6) - (9)].comment_type))))
                      ABORT_PARSE;
                  }
    break;

  case 179:

/* Line 1806 of yacc.c  */
#line 1210 "oct-parse.yy"
    {
                    if (! ((yyval.tree_command_type) = make_try_command ((yyvsp[(1) - (5)].tok_val), (yyvsp[(4) - (5)].tree_statement_list_type), 0, (yyvsp[(5) - (5)].tok_val), (yyvsp[(2) - (5)].comment_type), 0)))
                      ABORT_PARSE;
                  }
    break;

  case 180:

/* Line 1806 of yacc.c  */
#line 1221 "oct-parse.yy"
    {
                    current_function_depth++;

                    if (max_function_depth < current_function_depth)
                      max_function_depth = current_function_depth;

                    symtab_context.push (symbol_table::current_scope ());
                    symbol_table::set_scope (symbol_table::alloc_scope ());

                    if (! reading_script_file && current_function_depth == 1
                        && ! parsing_subfunctions)
                      primary_fcn_scope = symbol_table::current_scope ();

                    if (reading_script_file && current_function_depth > 1)
                      yyerror ("nested functions not implemented in this context");
                  }
    break;

  case 181:

/* Line 1806 of yacc.c  */
#line 1244 "oct-parse.yy"
    {
                    lexer_flags.looking_at_parameter_list = true;

                    if (lexer_flags.looking_at_function_handle)
                      {
                        symtab_context.push (symbol_table::current_scope ());
                        symbol_table::set_scope (symbol_table::alloc_scope ());
                        lexer_flags.looking_at_function_handle--;
                        lexer_flags.looking_at_anon_fcn_args = true;
                      }
                  }
    break;

  case 182:

/* Line 1806 of yacc.c  */
#line 1258 "oct-parse.yy"
    {
                    lexer_flags.looking_at_parameter_list = false;
                    lexer_flags.looking_for_object_index = false;
                  }
    break;

  case 183:

/* Line 1806 of yacc.c  */
#line 1265 "oct-parse.yy"
    {
                    lexer_flags.quote_is_transpose = false;
                    (yyval.tree_parameter_list_type) = (yyvsp[(2) - (3)].tree_parameter_list_type);
                  }
    break;

  case 184:

/* Line 1806 of yacc.c  */
#line 1270 "oct-parse.yy"
    {
                    yyerror ("invalid parameter list");
                    (yyval.tree_parameter_list_type) = 0;
                    ABORT_PARSE;
                  }
    break;

  case 185:

/* Line 1806 of yacc.c  */
#line 1278 "oct-parse.yy"
    { (yyval.tree_parameter_list_type) = 0; }
    break;

  case 186:

/* Line 1806 of yacc.c  */
#line 1280 "oct-parse.yy"
    {
                    (yyvsp[(1) - (1)].tree_parameter_list_type)->mark_as_formal_parameters ();
                    if ((yyvsp[(1) - (1)].tree_parameter_list_type)->validate (tree_parameter_list::in))
                      (yyval.tree_parameter_list_type) = (yyvsp[(1) - (1)].tree_parameter_list_type);
                    else
                      ABORT_PARSE;
                  }
    break;

  case 187:

/* Line 1806 of yacc.c  */
#line 1290 "oct-parse.yy"
    { (yyval.tree_parameter_list_type) = new tree_parameter_list ((yyvsp[(1) - (1)].tree_decl_elt_type)); }
    break;

  case 188:

/* Line 1806 of yacc.c  */
#line 1292 "oct-parse.yy"
    {
                    (yyvsp[(1) - (3)].tree_parameter_list_type)->append ((yyvsp[(3) - (3)].tree_decl_elt_type));
                    (yyval.tree_parameter_list_type) = (yyvsp[(1) - (3)].tree_parameter_list_type);
                  }
    break;

  case 189:

/* Line 1806 of yacc.c  */
#line 1303 "oct-parse.yy"
    {
                    lexer_flags.looking_at_return_list = false;
                    (yyval.tree_parameter_list_type) = new tree_parameter_list ();
                  }
    break;

  case 190:

/* Line 1806 of yacc.c  */
#line 1308 "oct-parse.yy"
    {
                    lexer_flags.looking_at_return_list = false;
                    if ((yyvsp[(1) - (1)].tree_parameter_list_type)->validate (tree_parameter_list::out))
                      (yyval.tree_parameter_list_type) = (yyvsp[(1) - (1)].tree_parameter_list_type);
                    else
                      ABORT_PARSE;
                  }
    break;

  case 191:

/* Line 1806 of yacc.c  */
#line 1316 "oct-parse.yy"
    {
                    lexer_flags.looking_at_return_list = false;
                    if ((yyvsp[(2) - (3)].tree_parameter_list_type)->validate (tree_parameter_list::out))
                      (yyval.tree_parameter_list_type) = (yyvsp[(2) - (3)].tree_parameter_list_type);
                    else
                      ABORT_PARSE;
                  }
    break;

  case 192:

/* Line 1806 of yacc.c  */
#line 1326 "oct-parse.yy"
    { (yyval.tree_parameter_list_type) = new tree_parameter_list (new tree_decl_elt ((yyvsp[(1) - (1)].tree_identifier_type))); }
    break;

  case 193:

/* Line 1806 of yacc.c  */
#line 1328 "oct-parse.yy"
    {
                    (yyvsp[(1) - (3)].tree_parameter_list_type)->append (new tree_decl_elt ((yyvsp[(3) - (3)].tree_identifier_type)));
                    (yyval.tree_parameter_list_type) = (yyvsp[(1) - (3)].tree_parameter_list_type);
                  }
    break;

  case 194:

/* Line 1806 of yacc.c  */
#line 1339 "oct-parse.yy"
    {
                    tree_statement *end_of_script
                      = make_end ("endscript", input_line_number,
                                  current_input_column);

                    make_script ((yyvsp[(2) - (3)].tree_statement_list_type), end_of_script);

                    (yyval.tree_command_type) = 0;
                  }
    break;

  case 195:

/* Line 1806 of yacc.c  */
#line 1355 "oct-parse.yy"
    { (yyval.tree_command_type) = 0; }
    break;

  case 198:

/* Line 1806 of yacc.c  */
#line 1367 "oct-parse.yy"
    {
                    (yyval.comment_type) = (yyvsp[(3) - (3)].comment_type);

                    if (reading_classdef_file || lexer_flags.parsing_classdef)
                      lexer_flags.maybe_classdef_get_set_method = true;
                  }
    break;

  case 199:

/* Line 1806 of yacc.c  */
#line 1376 "oct-parse.yy"
    {
                    (yyval.tree_command_type) = finish_function (0, (yyvsp[(2) - (2)].octave_user_function_type), (yyvsp[(1) - (2)].comment_type));
                    recover_from_parsing_function ();
                  }
    break;

  case 200:

/* Line 1806 of yacc.c  */
#line 1381 "oct-parse.yy"
    {
                    (yyval.tree_command_type) = finish_function ((yyvsp[(2) - (4)].tree_parameter_list_type), (yyvsp[(4) - (4)].octave_user_function_type), (yyvsp[(1) - (4)].comment_type));
                    recover_from_parsing_function ();
                  }
    break;

  case 201:

/* Line 1806 of yacc.c  */
#line 1388 "oct-parse.yy"
    {
                    std::string id_name = (yyvsp[(1) - (1)].tree_identifier_type)->name ();

                    lexer_flags.parsed_function_name.top () = true;
                    lexer_flags.maybe_classdef_get_set_method = false;

                    (yyval.tree_identifier_type) = (yyvsp[(1) - (1)].tree_identifier_type);
                  }
    break;

  case 202:

/* Line 1806 of yacc.c  */
#line 1397 "oct-parse.yy"
    {
                    lexer_flags.maybe_classdef_get_set_method = false;
                    (yyval.tree_identifier_type) = (yyvsp[(3) - (3)].tree_identifier_type);
                  }
    break;

  case 203:

/* Line 1806 of yacc.c  */
#line 1402 "oct-parse.yy"
    {
                    lexer_flags.maybe_classdef_get_set_method = false;
                    (yyval.tree_identifier_type) = (yyvsp[(3) - (3)].tree_identifier_type);
                  }
    break;

  case 204:

/* Line 1806 of yacc.c  */
#line 1409 "oct-parse.yy"
    {
                    std::string fname = (yyvsp[(1) - (2)].tree_identifier_type)->name ();

                    delete (yyvsp[(1) - (2)].tree_identifier_type);

                    if (! ((yyval.octave_user_function_type) = frob_function (fname, (yyvsp[(2) - (2)].octave_user_function_type))))
                      ABORT_PARSE;
                  }
    break;

  case 205:

/* Line 1806 of yacc.c  */
#line 1420 "oct-parse.yy"
    { (yyval.octave_user_function_type) = start_function ((yyvsp[(1) - (4)].tree_parameter_list_type), (yyvsp[(3) - (4)].tree_statement_list_type), (yyvsp[(4) - (4)].tree_statement_type)); }
    break;

  case 206:

/* Line 1806 of yacc.c  */
#line 1422 "oct-parse.yy"
    { (yyval.octave_user_function_type) = start_function (0, (yyvsp[(2) - (3)].tree_statement_list_type), (yyvsp[(3) - (3)].tree_statement_type)); }
    break;

  case 207:

/* Line 1806 of yacc.c  */
#line 1426 "oct-parse.yy"
    {
                    endfunction_found = true;
                    if (end_token_ok ((yyvsp[(1) - (1)].tok_val), token::function_end))
                      (yyval.tree_statement_type) = make_end ("endfunction", (yyvsp[(1) - (1)].tok_val)->line (), (yyvsp[(1) - (1)].tok_val)->column ());
                    else
                      ABORT_PARSE;
                  }
    break;

  case 208:

/* Line 1806 of yacc.c  */
#line 1434 "oct-parse.yy"
    {
// A lot of tests are based on the assumption that this is OK
//                  if (reading_script_file)
//                    {
//                      yyerror ("function body open at end of script");
//                      YYABORT;
//                    }

                    if (endfunction_found)
                      {
                        yyerror ("inconsistent function endings -- "
                                 "if one function is explicitly ended, "
                                 "so must all the others");
                        YYABORT;
                      }

                    if (! (reading_fcn_file || reading_script_file
                           || get_input_from_eval_string))
                      {
                        yyerror ("function body open at end of input");
                        YYABORT;
                      }

                    if (reading_classdef_file)
                      {
                        yyerror ("classdef body open at end of input");
                        YYABORT;
                      }

                    (yyval.tree_statement_type) = make_end ("endfunction", input_line_number,
                                   current_input_column);
                  }
    break;

  case 209:

/* Line 1806 of yacc.c  */
#line 1473 "oct-parse.yy"
    {
                    (yyval.comment_type) = 0;
                    lexer_flags.parsing_classdef = true;
                  }
    break;

  case 210:

/* Line 1806 of yacc.c  */
#line 1480 "oct-parse.yy"
    {
                    lexer_flags.parsing_classdef = false;

                    if (end_token_ok ((yyvsp[(1) - (1)].tok_val), token::classdef_end))
                      (yyval.tree_statement_type) = make_end ("endclassdef", (yyvsp[(1) - (1)].tok_val)->line (), (yyvsp[(1) - (1)].tok_val)->column ());
                    else
                      ABORT_PARSE;
                  }
    break;

  case 211:

/* Line 1806 of yacc.c  */
#line 1491 "oct-parse.yy"
    { (yyval.octave_user_function_type) = 0; }
    break;

  case 212:

/* Line 1806 of yacc.c  */
#line 1495 "oct-parse.yy"
    { (yyval.tree_command_type) = 0; }
    break;

  case 213:

/* Line 1806 of yacc.c  */
#line 1499 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 214:

/* Line 1806 of yacc.c  */
#line 1501 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 215:

/* Line 1806 of yacc.c  */
#line 1505 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 216:

/* Line 1806 of yacc.c  */
#line 1507 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 217:

/* Line 1806 of yacc.c  */
#line 1511 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 218:

/* Line 1806 of yacc.c  */
#line 1513 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 219:

/* Line 1806 of yacc.c  */
#line 1515 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 220:

/* Line 1806 of yacc.c  */
#line 1520 "oct-parse.yy"
    { (yyval.tree_parameter_list_type) = 0; }
    break;

  case 221:

/* Line 1806 of yacc.c  */
#line 1522 "oct-parse.yy"
    { (yyval.tree_parameter_list_type) = 0; }
    break;

  case 222:

/* Line 1806 of yacc.c  */
#line 1526 "oct-parse.yy"
    { (yyval.tree_parameter_list_type) = 0; }
    break;

  case 223:

/* Line 1806 of yacc.c  */
#line 1528 "oct-parse.yy"
    { (yyval.tree_parameter_list_type) = 0; }
    break;

  case 224:

/* Line 1806 of yacc.c  */
#line 1530 "oct-parse.yy"
    { (yyval.tree_parameter_list_type) = 0; }
    break;

  case 225:

/* Line 1806 of yacc.c  */
#line 1532 "oct-parse.yy"
    { (yyval.tree_parameter_list_type) = 0; }
    break;

  case 226:

/* Line 1806 of yacc.c  */
#line 1536 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 227:

/* Line 1806 of yacc.c  */
#line 1538 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 228:

/* Line 1806 of yacc.c  */
#line 1540 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 229:

/* Line 1806 of yacc.c  */
#line 1542 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 230:

/* Line 1806 of yacc.c  */
#line 1544 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 231:

/* Line 1806 of yacc.c  */
#line 1546 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 232:

/* Line 1806 of yacc.c  */
#line 1548 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 233:

/* Line 1806 of yacc.c  */
#line 1550 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 234:

/* Line 1806 of yacc.c  */
#line 1554 "oct-parse.yy"
    { (yyval.comment_type) = 0; }
    break;

  case 235:

/* Line 1806 of yacc.c  */
#line 1559 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 236:

/* Line 1806 of yacc.c  */
#line 1564 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 237:

/* Line 1806 of yacc.c  */
#line 1566 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 238:

/* Line 1806 of yacc.c  */
#line 1570 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 239:

/* Line 1806 of yacc.c  */
#line 1572 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 240:

/* Line 1806 of yacc.c  */
#line 1576 "oct-parse.yy"
    { (yyval.comment_type) = 0; }
    break;

  case 241:

/* Line 1806 of yacc.c  */
#line 1580 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 242:

/* Line 1806 of yacc.c  */
#line 1584 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 243:

/* Line 1806 of yacc.c  */
#line 1586 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 244:

/* Line 1806 of yacc.c  */
#line 1590 "oct-parse.yy"
    { (yyval.comment_type) = 0; }
    break;

  case 245:

/* Line 1806 of yacc.c  */
#line 1594 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 246:

/* Line 1806 of yacc.c  */
#line 1598 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 247:

/* Line 1806 of yacc.c  */
#line 1600 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 248:

/* Line 1806 of yacc.c  */
#line 1604 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 249:

/* Line 1806 of yacc.c  */
#line 1608 "oct-parse.yy"
    { (yyval.comment_type) = 0; }
    break;

  case 250:

/* Line 1806 of yacc.c  */
#line 1612 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 251:

/* Line 1806 of yacc.c  */
#line 1616 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 252:

/* Line 1806 of yacc.c  */
#line 1618 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 253:

/* Line 1806 of yacc.c  */
#line 1622 "oct-parse.yy"
    { (yyval.dummy_type) = 0; }
    break;

  case 254:

/* Line 1806 of yacc.c  */
#line 1630 "oct-parse.yy"
    { (yyval.comment_type) = octave_comment_buffer::get_comment (); }
    break;

  case 255:

/* Line 1806 of yacc.c  */
#line 1634 "oct-parse.yy"
    { yyerror ("parse error"); }
    break;

  case 257:

/* Line 1806 of yacc.c  */
#line 1639 "oct-parse.yy"
    { (yyval.sep_type) = ','; }
    break;

  case 258:

/* Line 1806 of yacc.c  */
#line 1641 "oct-parse.yy"
    { (yyval.sep_type) = ';'; }
    break;

  case 259:

/* Line 1806 of yacc.c  */
#line 1643 "oct-parse.yy"
    { (yyval.sep_type) = (yyvsp[(1) - (2)].sep_type); }
    break;

  case 260:

/* Line 1806 of yacc.c  */
#line 1645 "oct-parse.yy"
    { (yyval.sep_type) = (yyvsp[(1) - (2)].sep_type); }
    break;

  case 261:

/* Line 1806 of yacc.c  */
#line 1649 "oct-parse.yy"
    { (yyval.sep_type) = 0; }
    break;

  case 262:

/* Line 1806 of yacc.c  */
#line 1651 "oct-parse.yy"
    { (yyval.sep_type) = (yyvsp[(1) - (1)].sep_type); }
    break;

  case 263:

/* Line 1806 of yacc.c  */
#line 1655 "oct-parse.yy"
    { (yyval.sep_type) = ','; }
    break;

  case 264:

/* Line 1806 of yacc.c  */
#line 1657 "oct-parse.yy"
    { (yyval.sep_type) = ';'; }
    break;

  case 265:

/* Line 1806 of yacc.c  */
#line 1659 "oct-parse.yy"
    { (yyval.sep_type) = '\n'; }
    break;

  case 266:

/* Line 1806 of yacc.c  */
#line 1661 "oct-parse.yy"
    { (yyval.sep_type) = (yyvsp[(1) - (2)].sep_type); }
    break;

  case 267:

/* Line 1806 of yacc.c  */
#line 1663 "oct-parse.yy"
    { (yyval.sep_type) = (yyvsp[(1) - (2)].sep_type); }
    break;

  case 268:

/* Line 1806 of yacc.c  */
#line 1665 "oct-parse.yy"
    { (yyval.sep_type) = (yyvsp[(1) - (2)].sep_type); }
    break;

  case 269:

/* Line 1806 of yacc.c  */
#line 1669 "oct-parse.yy"
    { (yyval.sep_type) = 0; }
    break;

  case 270:

/* Line 1806 of yacc.c  */
#line 1671 "oct-parse.yy"
    { (yyval.sep_type) = (yyvsp[(1) - (1)].sep_type); }
    break;



/* Line 1806 of yacc.c  */
#line 4758 "oct-parse.cc"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 2067 of yacc.c  */
#line 1674 "oct-parse.yy"


// Generic error messages.

static void
yyerror (const char *s)
{
  int err_col = current_input_column - 1;

  std::ostringstream output_buf;

  if (reading_fcn_file || reading_script_file || reading_classdef_file)
    output_buf << "parse error near line " << input_line_number
               << " of file " << curr_fcn_file_full_name;
  else
    output_buf << "parse error:";

  if (s && strcmp (s, "parse error") != 0)
    output_buf << "\n\n  " << s;

  output_buf << "\n\n";

  if (! current_input_line.empty ())
    {
      size_t len = current_input_line.length ();

      if (current_input_line[len-1] == '\n')
        current_input_line.resize (len-1);

      // Print the line, maybe with a pointer near the error token.

      output_buf << ">>> " << current_input_line << "\n";

      if (err_col == 0)
        err_col = len;

      for (int i = 0; i < err_col + 3; i++)
        output_buf << " ";

      output_buf << "^";
    }

  output_buf << "\n";

  std::string msg = output_buf.str ();

  parse_error ("%s", msg.c_str ());
}

// Error mesages for mismatched end tokens.

static void
end_error (const char *type, token::end_tok_type ettype, int l, int c)
{
  static const char *fmt
    = "`%s' command matched by `%s' near line %d column %d";

  switch (ettype)
    {
    case token::simple_end:
      error (fmt, type, "end", l, c);
      break;

    case token::for_end:
      error (fmt, type, "endfor", l, c);
      break;

    case token::function_end:
      error (fmt, type, "endfunction", l, c);
      break;

    case token::classdef_end:
      error (fmt, type, "endclassdef", l, c);
      break;

    case token::if_end:
      error (fmt, type, "endif", l, c);
      break;

    case token::switch_end:
      error (fmt, type, "endswitch", l, c);
      break;

    case token::while_end:
      error (fmt, type, "endwhile", l, c);
      break;

    case token::try_catch_end:
      error (fmt, type, "end_try_catch", l, c);
      break;

    case token::unwind_protect_end:
      error (fmt, type, "end_unwind_protect", l, c);
      break;

    default:
      panic_impossible ();
      break;
    }
}

// Check to see that end tokens are properly matched.

static bool
end_token_ok (token *tok, token::end_tok_type expected)
{
  bool retval = true;

  token::end_tok_type ettype = tok->ettype ();

  if (ettype != expected && ettype != token::simple_end)
    {
      retval = false;

      yyerror ("parse error");

      int l = tok->line ();
      int c = tok->column ();

      switch (expected)
        {
        case token::classdef_end:
          end_error ("classdef", ettype, l, c);
          break;

        case token::for_end:
          end_error ("for", ettype, l, c);
          break;

        case token::enumeration_end:
          end_error ("enumeration", ettype, l, c);
          break;

        case token::function_end:
          end_error ("function", ettype, l, c);
          break;

        case token::if_end:
          end_error ("if", ettype, l, c);
          break;

        case token::parfor_end:
          end_error ("parfor", ettype, l, c);
          break;

        case token::try_catch_end:
          end_error ("try", ettype, l, c);
          break;

        case token::switch_end:
          end_error ("switch", ettype, l, c);
          break;

        case token::unwind_protect_end:
          end_error ("unwind_protect", ettype, l, c);
          break;

        case token::while_end:
          end_error ("while", ettype, l, c);
          break;

        default:
          panic_impossible ();
          break;
        }
    }

  return retval;
}

// Maybe print a warning if an assignment expression is used as the
// test in a logical expression.

static void
maybe_warn_assign_as_truth_value (tree_expression *expr)
{
  if (expr->is_assignment_expression ()
      && expr->paren_count () < 2)
    {
      if (curr_fcn_file_full_name.empty ())
        warning_with_id
          ("Octave:assign-as-truth-value",
           "suggest parenthesis around assignment used as truth value");
      else
        warning_with_id
          ("Octave:assign-as-truth-value",
           "suggest parenthesis around assignment used as truth value near line %d, column %d in file `%s'",
           expr->line (), expr->column (), curr_fcn_file_full_name.c_str ());
    }
}

// Maybe print a warning about switch labels that aren't constants.

static void
maybe_warn_variable_switch_label (tree_expression *expr)
{
  if (! expr->is_constant ())
    {
      if (curr_fcn_file_full_name.empty ())
        warning_with_id ("Octave:variable-switch-label",
                         "variable switch label");
      else
        warning_with_id
          ("Octave:variable-switch-label",
           "variable switch label near line %d, column %d in file `%s'",
           expr->line (), expr->column (), curr_fcn_file_full_name.c_str ());
    }
}

static tree_expression *
fold (tree_binary_expression *e)
{
  tree_expression *retval = e;

  unwind_protect frame;

  frame.protect_var (error_state);
  frame.protect_var (warning_state);

  frame.protect_var (discard_error_messages);
  frame.protect_var (discard_warning_messages);

  discard_error_messages = true;
  discard_warning_messages = true;

  tree_expression *op1 = e->lhs ();
  tree_expression *op2 = e->rhs ();

  if (op1->is_constant () && op2->is_constant ())
    {
      octave_value tmp = e->rvalue1 ();

      if (! (error_state || warning_state))
        {
          tree_constant *tc_retval
            = new tree_constant (tmp, op1->line (), op1->column ());

          std::ostringstream buf;

          tree_print_code tpc (buf);

          e->accept (tpc);

          tc_retval->stash_original_text (buf.str ());

          delete e;

          retval = tc_retval;
        }
    }

  return retval;
}

static tree_expression *
fold (tree_unary_expression *e)
{
  tree_expression *retval = e;

  unwind_protect frame;

  frame.protect_var (error_state);
  frame.protect_var (warning_state);

  frame.protect_var (discard_error_messages);
  frame.protect_var (discard_warning_messages);

  discard_error_messages = true;
  discard_warning_messages = true;

  tree_expression *op = e->operand ();

  if (op->is_constant ())
    {
      octave_value tmp = e->rvalue1 ();

      if (! (error_state || warning_state))
        {
          tree_constant *tc_retval
            = new tree_constant (tmp, op->line (), op->column ());

          std::ostringstream buf;

          tree_print_code tpc (buf);

          e->accept (tpc);

          tc_retval->stash_original_text (buf.str ());

          delete e;

          retval = tc_retval;
        }
    }

  return retval;
}

// Finish building a range.

static tree_expression *
finish_colon_expression (tree_colon_expression *e)
{
  tree_expression *retval = e;

  unwind_protect frame;

  frame.protect_var (error_state);
  frame.protect_var (warning_state);

  frame.protect_var (discard_error_messages);
  frame.protect_var (discard_warning_messages);

  discard_error_messages = true;
  discard_warning_messages = true;

  tree_expression *base = e->base ();
  tree_expression *limit = e->limit ();
  tree_expression *incr = e->increment ();

  if (base)
    {
      if (limit)
        {
          if (base->is_constant () && limit->is_constant ()
              && (! incr || (incr && incr->is_constant ())))
            {
              octave_value tmp = e->rvalue1 ();

              if (! (error_state || warning_state))
                {
                  tree_constant *tc_retval
                    = new tree_constant (tmp, base->line (), base->column ());

                  std::ostringstream buf;

                  tree_print_code tpc (buf);

                  e->accept (tpc);

                  tc_retval->stash_original_text (buf.str ());

                  delete e;

                  retval = tc_retval;
                }
            }
        }
      else
        {
          e->preserve_base ();
          delete e;

          // FIXME -- need to attempt constant folding here
          // too (we need a generic way to do that).
          retval = base;
        }
    }

  return retval;
}

// Make a constant.

static tree_constant *
make_constant (int op, token *tok_val)
{
  int l = tok_val->line ();
  int c = tok_val->column ();

  tree_constant *retval = 0;

  switch (op)
    {
    case NUM:
      {
        octave_value tmp (tok_val->number ());
        retval = new tree_constant (tmp, l, c);
        retval->stash_original_text (tok_val->text_rep ());
      }
      break;

    case IMAG_NUM:
      {
        octave_value tmp (Complex (0.0, tok_val->number ()));
        retval = new tree_constant (tmp, l, c);
        retval->stash_original_text (tok_val->text_rep ());
      }
      break;

    case DQ_STRING:
    case SQ_STRING:
      {
        std::string txt = tok_val->text ();

        char delim = op == DQ_STRING ? '"' : '\'';
        octave_value tmp (txt, delim);

        if (txt.empty ())
          {
            if (op == DQ_STRING)
              tmp = octave_null_str::instance;
            else
              tmp = octave_null_sq_str::instance;
          }

        retval = new tree_constant (tmp, l, c);

        if (op == DQ_STRING)
          txt = undo_string_escapes (txt);

        // FIXME -- maybe this should also be handled by
        // tok_val->text_rep () for character strings?
        retval->stash_original_text (delim + txt + delim);
      }
      break;

    default:
      panic_impossible ();
      break;
    }

  return retval;
}

// Make a function handle.

static tree_fcn_handle *
make_fcn_handle (token *tok_val)
{
  int l = tok_val->line ();
  int c = tok_val->column ();

  tree_fcn_handle *retval = new tree_fcn_handle (tok_val->text (), l, c);

  return retval;
}

// Make an anonymous function handle.

static tree_anon_fcn_handle *
make_anon_fcn_handle (tree_parameter_list *param_list, tree_statement *stmt)
{
  // FIXME -- need to get these from the location of the @ symbol.
  int l = input_line_number;
  int c = current_input_column;

  tree_parameter_list *ret_list = 0;

  symbol_table::scope_id fcn_scope = symbol_table::current_scope ();

  if (symtab_context.empty ())
    panic_impossible ();

  symbol_table::set_scope (symtab_context.top ());

  symtab_context.pop ();

  stmt->set_print_flag (false);

  tree_statement_list *body = new tree_statement_list (stmt);

  body->mark_as_anon_function_body ();

  tree_anon_fcn_handle *retval
    = new tree_anon_fcn_handle (param_list, ret_list, body, fcn_scope, l, c);
  // FIXME: Stash the filename.  This does not work and produces
  // errors when executed.
  //retval->stash_file_name (curr_fcn_file_name);

  return retval;
}

// Build a binary expression.

static tree_expression *
make_binary_op (int op, tree_expression *op1, token *tok_val,
                tree_expression *op2)
{
  octave_value::binary_op t = octave_value::unknown_binary_op;

  switch (op)
    {
    case POW:
      t = octave_value::op_pow;
      break;

    case EPOW:
      t = octave_value::op_el_pow;
      break;

    case '+':
      t = octave_value::op_add;
      break;

    case '-':
      t = octave_value::op_sub;
      break;

    case '*':
      t = octave_value::op_mul;
      break;

    case '/':
      t = octave_value::op_div;
      break;

    case EMUL:
      t = octave_value::op_el_mul;
      break;

    case EDIV:
      t = octave_value::op_el_div;
      break;

    case LEFTDIV:
      t = octave_value::op_ldiv;
      break;

    case ELEFTDIV:
      t = octave_value::op_el_ldiv;
      break;

    case LSHIFT:
      t = octave_value::op_lshift;
      break;

    case RSHIFT:
      t = octave_value::op_rshift;
      break;

    case EXPR_LT:
      t = octave_value::op_lt;
      break;

    case EXPR_LE:
      t = octave_value::op_le;
      break;

    case EXPR_EQ:
      t = octave_value::op_eq;
      break;

    case EXPR_GE:
      t = octave_value::op_ge;
      break;

    case EXPR_GT:
      t = octave_value::op_gt;
      break;

    case EXPR_NE:
      t = octave_value::op_ne;
      break;

    case EXPR_AND:
      t = octave_value::op_el_and;
      break;

    case EXPR_OR:
      t = octave_value::op_el_or;
      break;

    default:
      panic_impossible ();
      break;
    }

  int l = tok_val->line ();
  int c = tok_val->column ();

  tree_binary_expression *e
    = maybe_compound_binary_expression (op1, op2, l, c, t);

  return fold (e);
}

// Build a boolean expression.

static tree_expression *
make_boolean_op (int op, tree_expression *op1, token *tok_val,
                 tree_expression *op2)
{
  tree_boolean_expression::type t;

  switch (op)
    {
    case EXPR_AND_AND:
      t = tree_boolean_expression::bool_and;
      break;

    case EXPR_OR_OR:
      t = tree_boolean_expression::bool_or;
      break;

    default:
      panic_impossible ();
      break;
    }

  int l = tok_val->line ();
  int c = tok_val->column ();

  tree_boolean_expression *e
    = new tree_boolean_expression (op1, op2, l, c, t);

  return fold (e);
}

// Build a prefix expression.

static tree_expression *
make_prefix_op (int op, tree_expression *op1, token *tok_val)
{
  octave_value::unary_op t = octave_value::unknown_unary_op;

  switch (op)
    {
    case EXPR_NOT:
      t = octave_value::op_not;
      break;

    case '+':
      t = octave_value::op_uplus;
      break;

    case '-':
      t = octave_value::op_uminus;
      break;

    case PLUS_PLUS:
      t = octave_value::op_incr;
      break;

    case MINUS_MINUS:
      t = octave_value::op_decr;
      break;

    default:
      panic_impossible ();
      break;
    }

  int l = tok_val->line ();
  int c = tok_val->column ();

  tree_prefix_expression *e
    = new tree_prefix_expression (op1, l, c, t);

  return fold (e);
}

// Build a postfix expression.

static tree_expression *
make_postfix_op (int op, tree_expression *op1, token *tok_val)
{
  octave_value::unary_op t = octave_value::unknown_unary_op;

  switch (op)
    {
    case QUOTE:
      t = octave_value::op_hermitian;
      break;

    case TRANSPOSE:
      t = octave_value::op_transpose;
      break;

    case PLUS_PLUS:
      t = octave_value::op_incr;
      break;

    case MINUS_MINUS:
      t = octave_value::op_decr;
      break;

    default:
      panic_impossible ();
      break;
    }

  int l = tok_val->line ();
  int c = tok_val->column ();

  tree_postfix_expression *e
    = new tree_postfix_expression (op1, l, c, t);

  return fold (e);
}

// Build an unwind-protect command.

static tree_command *
make_unwind_command (token *unwind_tok, tree_statement_list *body,
                     tree_statement_list *cleanup, token *end_tok,
                     octave_comment_list *lc, octave_comment_list *mc)
{
  tree_command *retval = 0;

  if (end_token_ok (end_tok, token::unwind_protect_end))
    {
      octave_comment_list *tc = octave_comment_buffer::get_comment ();

      int l = unwind_tok->line ();
      int c = unwind_tok->column ();

      retval = new tree_unwind_protect_command (body, cleanup,
                                                lc, mc, tc, l, c);
    }

  return retval;
}

// Build a try-catch command.

static tree_command *
make_try_command (token *try_tok, tree_statement_list *body,
                  tree_statement_list *cleanup, token *end_tok,
                  octave_comment_list *lc, octave_comment_list *mc)
{
  tree_command *retval = 0;

  if (end_token_ok (end_tok, token::try_catch_end))
    {
      octave_comment_list *tc = octave_comment_buffer::get_comment ();

      int l = try_tok->line ();
      int c = try_tok->column ();

      retval = new tree_try_catch_command (body, cleanup,
                                           lc, mc, tc, l, c);
    }

  return retval;
}

// Build a while command.

static tree_command *
make_while_command (token *while_tok, tree_expression *expr,
                    tree_statement_list *body, token *end_tok,
                    octave_comment_list *lc)
{
  tree_command *retval = 0;

  maybe_warn_assign_as_truth_value (expr);

  if (end_token_ok (end_tok, token::while_end))
    {
      octave_comment_list *tc = octave_comment_buffer::get_comment ();

      lexer_flags.looping--;

      int l = while_tok->line ();
      int c = while_tok->column ();

      retval = new tree_while_command (expr, body, lc, tc, l, c);
    }

  return retval;
}

// Build a do-until command.

static tree_command *
make_do_until_command (token *until_tok, tree_statement_list *body,
                       tree_expression *expr, octave_comment_list *lc)
{
  tree_command *retval = 0;

  maybe_warn_assign_as_truth_value (expr);

  octave_comment_list *tc = octave_comment_buffer::get_comment ();

  lexer_flags.looping--;

  int l = until_tok->line ();
  int c = until_tok->column ();

  retval = new tree_do_until_command (expr, body, lc, tc, l, c);

  return retval;
}

// Build a for command.

static tree_command *
make_for_command (int tok_id, token *for_tok, tree_argument_list *lhs,
                  tree_expression *expr, tree_expression *maxproc,
                  tree_statement_list *body, token *end_tok,
                  octave_comment_list *lc)
{
  tree_command *retval = 0;

  bool parfor = tok_id == PARFOR;

  if (end_token_ok (end_tok, parfor ? token::parfor_end : token::for_end))
    {
      octave_comment_list *tc = octave_comment_buffer::get_comment ();

      lexer_flags.looping--;

      int l = for_tok->line ();
      int c = for_tok->column ();

      if (lhs->length () == 1)
        {
          tree_expression *tmp = lhs->remove_front ();

          retval = new tree_simple_for_command (parfor, tmp, expr, maxproc,
                                                body, lc, tc, l, c);

          delete lhs;
        }
      else
        {
          if (parfor)
            yyerror ("invalid syntax for parfor statement");
          else
            retval = new tree_complex_for_command (lhs, expr, body,
                                                   lc, tc, l, c);
        }
    }

  return retval;
}

// Build a break command.

static tree_command *
make_break_command (token *break_tok)
{
  tree_command *retval = 0;

  int l = break_tok->line ();
  int c = break_tok->column ();

  retval = new tree_break_command (l, c);

  return retval;
}

// Build a continue command.

static tree_command *
make_continue_command (token *continue_tok)
{
  tree_command *retval = 0;

  int l = continue_tok->line ();
  int c = continue_tok->column ();

  retval = new tree_continue_command (l, c);

  return retval;
}

// Build a return command.

static tree_command *
make_return_command (token *return_tok)
{
  tree_command *retval = 0;

  int l = return_tok->line ();
  int c = return_tok->column ();

  retval = new tree_return_command (l, c);

  return retval;
}

// Start an if command.

static tree_if_command_list *
start_if_command (tree_expression *expr, tree_statement_list *list)
{
  maybe_warn_assign_as_truth_value (expr);

  tree_if_clause *t = new tree_if_clause (expr, list);

  return new tree_if_command_list (t);
}

// Finish an if command.

static tree_if_command *
finish_if_command (token *if_tok, tree_if_command_list *list,
                   token *end_tok, octave_comment_list *lc)
{
  tree_if_command *retval = 0;

  if (end_token_ok (end_tok, token::if_end))
    {
      octave_comment_list *tc = octave_comment_buffer::get_comment ();

      int l = if_tok->line ();
      int c = if_tok->column ();

      if (list && ! list->empty ())
        {
          tree_if_clause *elt = list->front ();

          if (elt)
            {
              elt->line (l);
              elt->column (c);
            }
        }

      retval = new tree_if_command (list, lc, tc, l, c);
    }

  return retval;
}

// Build an elseif clause.

static tree_if_clause *
make_elseif_clause (token *elseif_tok, tree_expression *expr,
                    tree_statement_list *list, octave_comment_list *lc)
{
  maybe_warn_assign_as_truth_value (expr);

  int l = elseif_tok->line ();
  int c = elseif_tok->column ();

  return new tree_if_clause (expr, list, lc, l, c);
}

// Finish a switch command.

static tree_switch_command *
finish_switch_command (token *switch_tok, tree_expression *expr,
                       tree_switch_case_list *list, token *end_tok,
                       octave_comment_list *lc)
{
  tree_switch_command *retval = 0;

  if (end_token_ok (end_tok, token::switch_end))
    {
      octave_comment_list *tc = octave_comment_buffer::get_comment ();

      int l = switch_tok->line ();
      int c = switch_tok->column ();

      if (list && ! list->empty ())
        {
          tree_switch_case *elt = list->front ();

          if (elt)
            {
              elt->line (l);
              elt->column (c);
            }
        }

      retval = new tree_switch_command (expr, list, lc, tc, l, c);
    }

  return retval;
}

// Build a switch case.

static tree_switch_case *
make_switch_case (token *case_tok, tree_expression *expr,
                  tree_statement_list *list, octave_comment_list *lc)
{
  maybe_warn_variable_switch_label (expr);

  int l = case_tok->line ();
  int c = case_tok->column ();

  return new tree_switch_case (expr, list, lc, l, c);
}

// Build an assignment to a variable.

static tree_expression *
make_assign_op (int op, tree_argument_list *lhs, token *eq_tok,
                tree_expression *rhs)
{
  tree_expression *retval = 0;

  octave_value::assign_op t = octave_value::unknown_assign_op;

  switch (op)
    {
    case '=':
      t = octave_value::op_asn_eq;
      break;

    case ADD_EQ:
      t = octave_value::op_add_eq;
      break;

    case SUB_EQ:
      t = octave_value::op_sub_eq;
      break;

    case MUL_EQ:
      t = octave_value::op_mul_eq;
      break;

    case DIV_EQ:
      t = octave_value::op_div_eq;
      break;

    case LEFTDIV_EQ:
      t = octave_value::op_ldiv_eq;
      break;

    case POW_EQ:
      t = octave_value::op_pow_eq;
      break;

    case LSHIFT_EQ:
      t = octave_value::op_lshift_eq;
      break;

    case RSHIFT_EQ:
      t = octave_value::op_rshift_eq;
      break;

    case EMUL_EQ:
      t = octave_value::op_el_mul_eq;
      break;

    case EDIV_EQ:
      t = octave_value::op_el_div_eq;
      break;

    case ELEFTDIV_EQ:
      t = octave_value::op_el_ldiv_eq;
      break;

    case EPOW_EQ:
      t = octave_value::op_el_pow_eq;
      break;

    case AND_EQ:
      t = octave_value::op_el_and_eq;
      break;

    case OR_EQ:
      t = octave_value::op_el_or_eq;
      break;

    default:
      panic_impossible ();
      break;
    }

  int l = eq_tok->line ();
  int c = eq_tok->column ();

  if (lhs->is_simple_assign_lhs ())
    {
      tree_expression *tmp = lhs->remove_front ();

      retval = new tree_simple_assignment (tmp, rhs, false, l, c, t);

      delete lhs;
    }
  else if (t == octave_value::op_asn_eq)
    return new tree_multi_assignment (lhs, rhs, false, l, c);
  else
    yyerror ("computed multiple assignment not allowed");

  return retval;
}

// Define a script.

static void
make_script (tree_statement_list *cmds, tree_statement *end_script)
{
  std::string doc_string;

  if (! help_buf.empty ())
    {
      doc_string = help_buf.top ();
      help_buf.pop ();
    }

  if (! cmds)
    cmds = new tree_statement_list ();

  cmds->append (end_script);

  octave_user_script *script
    = new octave_user_script (curr_fcn_file_full_name, curr_fcn_file_name,
                              cmds, doc_string);

  octave_time now;

  script->stash_fcn_file_time (now);

  primary_fcn_ptr = script;

  // Unmark any symbols that may have been tagged as local variables
  // while parsing (for example, by force_local_variable in lex.l).

  symbol_table::unmark_forced_variables ();
}

// Begin defining a function.

static octave_user_function *
start_function (tree_parameter_list *param_list, tree_statement_list *body,
                tree_statement *end_fcn_stmt)
{
  // We'll fill in the return list later.

  if (! body)
    body = new tree_statement_list ();

  body->append (end_fcn_stmt);

  octave_user_function *fcn
    = new octave_user_function (symbol_table::current_scope (),
                                param_list, 0, body);

  if (fcn)
    {
      octave_comment_list *tc = octave_comment_buffer::get_comment ();

      fcn->stash_trailing_comment (tc);
    }

  return fcn;
}

static tree_statement *
make_end (const std::string& type, int l, int c)
{
  return make_statement (new tree_no_op_command (type, l, c));
}

// Do most of the work for defining a function.

static octave_user_function *
frob_function (const std::string& fname, octave_user_function *fcn)
{
  std::string id_name = fname;

  // If input is coming from a file, issue a warning if the name of
  // the file does not match the name of the function stated in the
  // file.  Matlab doesn't provide a diagnostic (it ignores the stated
  // name).
  if (! autoloading && reading_fcn_file
      && (current_function_depth == 1
          && ! (parsing_subfunctions || lexer_flags.parsing_class_method)))
  {
    // FIXME -- should curr_fcn_file_name already be
    // preprocessed when we get here?  It seems to only be a
    // problem with relative file names.

    std::string nm = curr_fcn_file_name;

    size_t pos = nm.find_last_of (file_ops::dir_sep_chars ());

    if (pos != std::string::npos)
      nm = curr_fcn_file_name.substr (pos+1);

    if (nm != id_name)
      {
        warning_with_id
          ("Octave:function-name-clash",
           "function name `%s' does not agree with function file name `%s'",
           id_name.c_str (), curr_fcn_file_full_name.c_str ());

        id_name = nm;
      }
  }

  if (reading_fcn_file || reading_classdef_file || autoloading)
    {
      octave_time now;

      fcn->stash_fcn_file_name (curr_fcn_file_full_name);
      fcn->stash_fcn_file_time (now);
      fcn->mark_as_system_fcn_file ();

      if (fcn_file_from_relative_lookup)
        fcn->mark_relative ();

      if (current_function_depth > 1 || parsing_subfunctions)
        {
          fcn->stash_parent_fcn_name (curr_fcn_file_name);
          fcn->stash_parent_fcn_scope (primary_fcn_scope);
        }

      if (lexer_flags.parsing_class_method)
        {
          if (current_class_name == id_name)
            fcn->mark_as_class_constructor ();
          else
            fcn->mark_as_class_method ();

          fcn->stash_dispatch_class (current_class_name);
        }

      std::string nm = fcn->fcn_file_name ();

      file_stat fs (nm);

      if (fs && fs.is_newer (now))
        warning_with_id ("Octave:future-time-stamp",
                         "time stamp for `%s' is in the future", nm.c_str ());
    }
  else if (! (input_from_tmp_history_file || input_from_startup_file)
           && reading_script_file
           && curr_fcn_file_name == id_name)
    {
      warning ("function `%s' defined within script file `%s'",
               id_name.c_str (), curr_fcn_file_full_name.c_str ());
    }

  fcn->stash_function_name (id_name);
  fcn->stash_fcn_location (input_line_number, current_input_column);

  if (! help_buf.empty () && current_function_depth == 1
      && ! parsing_subfunctions)
    {
      fcn->document (help_buf.top ());

      help_buf.pop ();
    }

  if (reading_fcn_file && current_function_depth == 1
      && ! parsing_subfunctions)
    primary_fcn_ptr = fcn;

  return fcn;
}

static tree_function_def *
finish_function (tree_parameter_list *ret_list,
                 octave_user_function *fcn, octave_comment_list *lc)
{
  tree_function_def *retval = 0;

  if (ret_list)
    ret_list->mark_as_formal_parameters ();

  if (fcn)
    {
      std::string nm = fcn->name ();
      std::string file = fcn->fcn_file_name ();

      std::string tmp = nm;
      if (! file.empty ())
        tmp += ": " + file;

      symbol_table::cache_name (fcn->scope (), tmp);

      if (lc)
        fcn->stash_leading_comment (lc);

      fcn->define_ret_list (ret_list);

      if (current_function_depth > 1 || parsing_subfunctions)
        {
          fcn->mark_as_subfunction ();

          symbol_table::install_subfunction (nm, octave_value (fcn),
                                             primary_fcn_scope);
        }

      if (! reading_fcn_file)
        {
          // We are either reading a script file or defining a function
          // at the command line, so this definition creates a
          // tree_function object that is placed in the parse tree.
          // Otherwise, it is just inserted in the symbol table,
          // either as a subfunction (see above), or as the primary
          // function for the file, via primary_fcn_ptr (see also
          // load_fcn_from_file,, parse_fcn_file, and
          // symbol_table::fcn_info::fcn_info_rep::find_user_function).

          retval = new tree_function_def (fcn);
        }

      // Unmark any symbols that may have been tagged as local
      // variables while parsing (for example, by force_local_variable
      // in lex.l).

      symbol_table::unmark_forced_variables (fcn->scope ());
    }

  return retval;
}

static void
recover_from_parsing_function (void)
{
  if (symtab_context.empty ())
    panic_impossible ();

  symbol_table::set_scope (symtab_context.top ());
  symtab_context.pop ();

  if (reading_fcn_file && current_function_depth == 1
      && ! parsing_subfunctions)
    parsing_subfunctions = true;

  current_function_depth--;

  lexer_flags.defining_func--;
  lexer_flags.parsed_function_name.pop ();
  lexer_flags.looking_at_return_list = false;
  lexer_flags.looking_at_parameter_list = false;
}

// Make an index expression.

static tree_index_expression *
make_index_expression (tree_expression *expr, tree_argument_list *args,
                       char type)
{
  tree_index_expression *retval = 0;

  if (args && args->has_magic_tilde ())
    {
      yyerror ("invalid use of empty argument (~) in index expression");
      return retval;
    }

  int l = expr->line ();
  int c = expr->column ();

  expr->mark_postfix_indexed ();

  if (expr->is_index_expression ())
    {
      tree_index_expression *tmp = static_cast<tree_index_expression *> (expr);

      tmp->append (args, type);

      retval = tmp;
    }
  else
    retval = new tree_index_expression (expr, args, l, c, type);

  return retval;
}

// Make an indirect reference expression.

static tree_index_expression *
make_indirect_ref (tree_expression *expr, const std::string& elt)
{
  tree_index_expression *retval = 0;

  int l = expr->line ();
  int c = expr->column ();

  if (expr->is_index_expression ())
    {
      tree_index_expression *tmp = static_cast<tree_index_expression *> (expr);

      tmp->append (elt);

      retval = tmp;
    }
  else
    retval = new tree_index_expression (expr, elt, l, c);

  lexer_flags.looking_at_indirect_ref = false;

  return retval;
}

// Make an indirect reference expression with dynamic field name.

static tree_index_expression *
make_indirect_ref (tree_expression *expr, tree_expression *elt)
{
  tree_index_expression *retval = 0;

  int l = expr->line ();
  int c = expr->column ();

  if (expr->is_index_expression ())
    {
      tree_index_expression *tmp = static_cast<tree_index_expression *> (expr);

      tmp->append (elt);

      retval = tmp;
    }
  else
    retval = new tree_index_expression (expr, elt, l, c);

  lexer_flags.looking_at_indirect_ref = false;

  return retval;
}

// Make a declaration command.

static tree_decl_command *
make_decl_command (int tok, token *tok_val, tree_decl_init_list *lst)
{
  tree_decl_command *retval = 0;

  int l = tok_val->line ();
  int c = tok_val->column ();

  switch (tok)
    {
    case GLOBAL:
      retval = new tree_global_command (lst, l, c);
      break;

    case STATIC:
      if (current_function_depth > 0)
        retval = new tree_static_command (lst, l, c);
      else
        {
          if (reading_script_file)
            warning ("ignoring persistent declaration near line %d of file `%s'",
                     l, curr_fcn_file_full_name.c_str ());
          else
            warning ("ignoring persistent declaration near line %d", l);
        }
      break;

    default:
      panic_impossible ();
      break;
    }

  return retval;
}

static tree_argument_list *
validate_matrix_row (tree_argument_list *row)
{
  if (row && row->has_magic_tilde ())
    yyerror ("invalid use of tilde (~) in matrix expression");
  return row;
}

// Finish building a matrix list.

static tree_expression *
finish_matrix (tree_matrix *m)
{
  tree_expression *retval = m;

  unwind_protect frame;

  frame.protect_var (error_state);
  frame.protect_var (warning_state);

  frame.protect_var (discard_error_messages);
  frame.protect_var (discard_warning_messages);

  discard_error_messages = true;
  discard_warning_messages = true;

  if (m->all_elements_are_constant ())
    {
      octave_value tmp = m->rvalue1 ();

      if (! (error_state || warning_state))
        {
          tree_constant *tc_retval
            = new tree_constant (tmp, m->line (), m->column ());

          std::ostringstream buf;

          tree_print_code tpc (buf);

          m->accept (tpc);

          tc_retval->stash_original_text (buf.str ());

          delete m;

          retval = tc_retval;
        }
    }

  return retval;
}

// Finish building a cell list.

static tree_expression *
finish_cell (tree_cell *c)
{
  return finish_matrix (c);
}

static void
maybe_warn_missing_semi (tree_statement_list *t)
{
  if (current_function_depth > 0)
    {
      tree_statement *tmp = t->back();

      if (tmp->is_expression ())
        warning_with_id
          ("Octave:missing-semicolon",
           "missing semicolon near line %d, column %d in file `%s'",
            tmp->line (), tmp->column (), curr_fcn_file_full_name.c_str ());
    }
}

static tree_statement_list *
set_stmt_print_flag (tree_statement_list *list, char sep,
                     bool warn_missing_semi)
{
  tree_statement *tmp = list->back ();

  switch (sep)
    {
    case ';':
      tmp->set_print_flag (false);
      break;

    case 0:
    case ',':
    case '\n':
      tmp->set_print_flag (true);
      if (warn_missing_semi)
        maybe_warn_missing_semi (list);
      break;

    default:
      warning ("unrecognized separator type!");
      break;
    }

  // Even if a statement is null, we add it to the list then remove it
  // here so that the print flag is applied to the correct statement.

  if (tmp->is_null_statement ())
    {
      list->pop_back ();
      delete tmp;
    }

  return list;
}

static tree_statement_list *
make_statement_list (tree_statement *stmt)
{
  return new tree_statement_list (stmt);
}

static tree_statement_list *
append_statement_list (tree_statement_list *list, char sep,
                       tree_statement *stmt, bool warn_missing_semi)
{
  set_stmt_print_flag (list, sep, warn_missing_semi);

  list->append (stmt);

  return list;
}

static void
safe_fclose (FILE *f)
{
  // FIXME -- comments at the end of an input file are
  // discarded (otherwise, they would be appended to the next
  // statement, possibly from the command line or another file, which
  // can be quite confusing).

  octave_comment_list *tc = octave_comment_buffer::get_comment ();

  delete tc;

  if (f)
    fclose (static_cast<FILE *> (f));
}

static bool
looks_like_copyright (const std::string& s)
{
  bool retval = false;

  if (! s.empty ())
    {
      size_t offset = s.find_first_not_of (" \t");

      retval = (s.substr (offset, 9) == "Copyright" || s.substr (offset, 6) == "Author");
    }

  return retval;
}

static int
text_getc (FILE *f)
{
  int c = gnulib::getc (f);

  // Convert CRLF into just LF and single CR into LF.

  if (c == '\r')
    {
      c = gnulib::getc (f);

      if (c != '\n')
        {
          ungetc (c, f);
          c = '\n';
        }
    }

  if (c == '\n')
    input_line_number++;

  return c;
}

class
stdio_stream_reader : public stream_reader
{
public:
  stdio_stream_reader (FILE *f_arg) : stream_reader (), f (f_arg) { }

  int getc (void) { return ::text_getc (f); }
  int ungetc (int c)
  {
    if (c == '\n')
      input_line_number--;

    return ::ungetc (c, f);
  }

private:
  FILE *f;

  // No copying!

  stdio_stream_reader (const  stdio_stream_reader&);

  stdio_stream_reader & operator = (const  stdio_stream_reader&);
};

static bool
skip_white_space (stream_reader& reader)
{
  int c = 0;

  while ((c = reader.getc ()) != EOF)
    {
      switch (c)
        {
        case ' ':
        case '\t':
          current_input_column++;
          break;

        case '\n':
          current_input_column = 1;
          break;

        default:
          reader.ungetc (c);
          goto done;
        }
    }

 done:

  return (c == EOF);
}

static bool
looking_at_classdef_keyword (FILE *ffile)
{
  bool status = false;

  long pos = gnulib::ftell (ffile);

  char buf [10];
  gnulib::fgets (buf, 10, ffile);
  size_t len = strlen (buf);
  if (len > 8 && strncmp (buf, "classdef", 8) == 0
      && ! (isalnum (buf[8]) || buf[8] == '_'))
    status = true;

  gnulib::fseek (ffile, pos, SEEK_SET);

  return status;
 }

static std::string
gobble_leading_white_space (FILE *ffile, bool& eof)
{
  std::string help_txt;

  eof = false;

  // TRUE means we have already cached the help text.
  bool have_help_text = false;

  std::string txt;

  stdio_stream_reader stdio_reader (ffile);

  while (true)
    {
      eof = skip_white_space (stdio_reader);

      if (eof)
        break;

      txt = grab_comment_block (stdio_reader, true, eof);

      if (txt.empty ())
        break;

      if (! (have_help_text || looks_like_copyright (txt)))
        {
          help_txt = txt;
          have_help_text = true;
        }

      octave_comment_buffer::append (txt);

      if (eof)
        break;
    }

  return help_txt;
}

static bool
looking_at_function_keyword (FILE *ffile)
{
  bool status = false;

  long pos = gnulib::ftell (ffile);

  char buf [10];
  gnulib::fgets (buf, 10, ffile);
  size_t len = strlen (buf);
  if (len > 8 && strncmp (buf, "function", 8) == 0
      && ! (isalnum (buf[8]) || buf[8] == '_'))
    status = true;

  gnulib::fseek (ffile, pos, SEEK_SET);

  return status;
}

static octave_function *
parse_fcn_file (const std::string& ff, const std::string& dispatch_type,
                bool force_script = false, bool require_file = true,
                const std::string& warn_for = std::string ())
{
  unwind_protect frame;

  octave_function *fcn_ptr = 0;

  // Open function file and parse.

  FILE *in_stream = command_editor::get_input_stream ();

  frame.add_fcn (command_editor::set_input_stream, in_stream);

  frame.protect_var (ff_instream);

  frame.protect_var (input_line_number);
  frame.protect_var (current_input_column);
  frame.protect_var (reading_fcn_file);
  frame.protect_var (line_editing);
  frame.protect_var (current_class_name);
  frame.protect_var (current_function_depth);
  frame.protect_var (max_function_depth);
  frame.protect_var (parsing_subfunctions);
  frame.protect_var (endfunction_found);

  input_line_number = 1;
  current_input_column = 1;
  reading_fcn_file = true;
  line_editing = false;
  current_class_name = dispatch_type;
  current_function_depth = 0;
  max_function_depth = 0;
  parsing_subfunctions = false;
  endfunction_found = false;

  frame.add_fcn (command_history::ignore_entries,
                 command_history::ignoring_entries ());

  command_history::ignore_entries ();

  FILE *ffile = get_input_from_file (ff, 0);

  frame.add_fcn (safe_fclose, ffile);

  if (ffile)
    {
      bool eof;

      std::string help_txt = gobble_leading_white_space (ffile, eof);

      if (! help_txt.empty ())
        help_buf.push (help_txt);

      if (! eof)
        {
          std::string file_type;

          frame.protect_var (get_input_from_eval_string);
          frame.protect_var (parser_end_of_input);
          frame.protect_var (reading_fcn_file);
          frame.protect_var (reading_script_file);
          frame.protect_var (reading_classdef_file);
          frame.protect_var (Vecho_executing_commands);


          get_input_from_eval_string = false;
          parser_end_of_input = false;

          if (! force_script && looking_at_function_keyword (ffile))
            {
              file_type = "function";

              Vecho_executing_commands = ECHO_OFF;

              reading_classdef_file = false;
              reading_fcn_file = true;
              reading_script_file = false;
            }
          else if (! force_script && looking_at_classdef_keyword (ffile))
            {
              file_type = "classdef";

              Vecho_executing_commands = ECHO_OFF;

              reading_classdef_file = true;
              reading_fcn_file = false;
              reading_script_file = false;
            }
          else
            {
              file_type = "script";

              Vecho_executing_commands = ECHO_OFF;

              reading_classdef_file = false;
              reading_fcn_file = false;
              reading_script_file = true;
            }

          YY_BUFFER_STATE old_buf = current_buffer ();
          YY_BUFFER_STATE new_buf = create_buffer (ffile);

          frame.add_fcn (switch_to_buffer, old_buf);
          frame.add_fcn (delete_buffer, new_buf);

          switch_to_buffer (new_buf);

          frame.protect_var (primary_fcn_ptr);
          primary_fcn_ptr = 0;

          reset_parser ();

          // Do this with an unwind-protect cleanup function so that
          // the forced variables will be unmarked in the event of an
          // interrupt.
          symbol_table::scope_id scope = symbol_table::top_scope ();
          frame.add_fcn (symbol_table::unmark_forced_variables, scope);

          if (! help_txt.empty ())
            help_buf.push (help_txt);

          if (reading_script_file)
            prep_lexer_for_script_file ();
          else
            prep_lexer_for_function_file ();

          lexer_flags.parsing_class_method = ! dispatch_type.empty ();

          frame.protect_var (global_command);

          global_command = 0;

          int status = yyparse ();

          // Use an unwind-protect cleanup function so that the
          // global_command list will be deleted in the event of an
          // interrupt.

          frame.add_fcn (cleanup_statement_list, &global_command);

          fcn_ptr = primary_fcn_ptr;

          if (status != 0)
            error ("parse error while reading %s file %s",
                   file_type.c_str(), ff.c_str ());
          else if (reading_fcn_file && endfunction_found
                   && max_function_depth > 1)
            warning_with_id ("Octave:nested-functions-coerced",
                             "nested functions are coerced into subfunctions "
                             "in file %s", ff.c_str ());
        }
      else
        {
          tree_statement *end_of_script
            = make_end ("endscript", input_line_number, current_input_column);

          make_script (0, end_of_script);

          fcn_ptr = primary_fcn_ptr;
        }
    }
  else if (require_file)
    error ("no such file, `%s'", ff.c_str ());
  else if (! warn_for.empty ())
    error ("%s: unable to open file `%s'", warn_for.c_str (), ff.c_str ());

  return fcn_ptr;
}

std::string
get_help_from_file (const std::string& nm, bool& symbol_found,
                    std::string& file)
{
  std::string retval;

  file = fcn_file_in_path (nm);

  if (! file.empty ())
    {
      symbol_found = true;

      FILE *fptr = gnulib::fopen (file.c_str (), "r");

      if (fptr)
        {
          unwind_protect frame;
          frame.add_fcn (safe_fclose, fptr);

          bool eof;
          retval = gobble_leading_white_space (fptr, eof);

          if (retval.empty ())
            {
              octave_function *fcn = parse_fcn_file (file, "");

              if (fcn)
                {
                  retval = fcn->doc_string ();

                  delete fcn;
                }
            }
        }
    }

  return retval;
}

std::string
get_help_from_file (const std::string& nm, bool& symbol_found)
{
  std::string file;
  return get_help_from_file (nm, symbol_found, file);
}

std::string
lookup_autoload (const std::string& nm)
{
  std::string retval;

  typedef std::map<std::string, std::string>::const_iterator am_iter;

  am_iter p = autoload_map.find (nm);

  if (p != autoload_map.end ())
    retval = load_path::find_file (p->second);

  return retval;
}

string_vector
autoloaded_functions (void)
{
  string_vector names (autoload_map.size());

  octave_idx_type i = 0;
  typedef std::map<std::string, std::string>::const_iterator am_iter;
  for (am_iter p = autoload_map.begin (); p != autoload_map.end (); p++)
    names[i++] = p->first;

  return names;
}

string_vector
reverse_lookup_autoload (const std::string& nm)
{
  string_vector names;

  typedef std::map<std::string, std::string>::const_iterator am_iter;
  for (am_iter p = autoload_map.begin (); p != autoload_map.end (); p++)
    if (nm == p->second)
      names.append (p->first);

  return names;
}

octave_function *
load_fcn_from_file (const std::string& file_name, const std::string& dir_name,
                    const std::string& dispatch_type,
                    const std::string& fcn_name, bool autoload)
{
  octave_function *retval = 0;

  unwind_protect frame;

  std::string nm = file_name;

  size_t nm_len = nm.length ();

  std::string file;

  frame.protect_var (fcn_file_from_relative_lookup);

  fcn_file_from_relative_lookup = false;

  file = nm;

  if ((nm_len > 4 && nm.substr (nm_len-4) == ".oct")
      || (nm_len > 4 && nm.substr (nm_len-4) == ".mex")
      || (nm_len > 2 && nm.substr (nm_len-2) == ".m"))
    {
      nm = octave_env::base_pathname (file);
      nm = nm.substr (0, nm.find_last_of ('.'));

      size_t pos = nm.find_last_of (file_ops::dir_sep_str ());
      if (pos != std::string::npos)
        nm = nm.substr (pos+1);
    }

  if (autoload)
    {
      frame.protect_var (autoloading);
      autoloading = true;
    }

  fcn_file_from_relative_lookup = ! octave_env::absolute_pathname (file);

  file = octave_env::make_absolute (file);

  int len = file.length ();

  if (len > 4 && file.substr (len-4, len-1) == ".oct")
    {
      if (autoload && ! fcn_name.empty ())
        nm = fcn_name;

      retval = octave_dynamic_loader::load_oct (nm, file, fcn_file_from_relative_lookup);
    }
  else if (len > 4 && file.substr (len-4, len-1) == ".mex")
    {
      // Temporarily load m-file version of mex-file, if it exists,
      // to get the help-string to use.
      frame.protect_var (curr_fcn_file_name);
      frame.protect_var (curr_fcn_file_full_name);

      curr_fcn_file_name = nm;
      curr_fcn_file_full_name = file.substr (0, len - 2);

      octave_function *tmpfcn = parse_fcn_file (file.substr (0, len - 2),
                                                dispatch_type, autoloading,
                                                false);

      retval = octave_dynamic_loader::load_mex (nm, file, fcn_file_from_relative_lookup);

      if (tmpfcn)
        retval->document (tmpfcn->doc_string ());
      delete tmpfcn;
    }
  else if (len > 2)
    {
      // These are needed by yyparse.

      frame.protect_var (curr_fcn_file_name);
      frame.protect_var (curr_fcn_file_full_name);

      curr_fcn_file_name = nm;
      curr_fcn_file_full_name = file;

      retval = parse_fcn_file (file, dispatch_type, autoloading);
    }

  if (retval)
    {
      retval->stash_dir_name (dir_name);

      if (retval->is_user_function ())
        {
          symbol_table::scope_id id = retval->scope ();

          symbol_table::stash_dir_name_for_subfunctions (id, dir_name);
        }
    }

  return retval;
}

DEFUN (autoload, args, ,
  "-*- texinfo -*-\n\
@deftypefn {Built-in Function} {} autoload (@var{function}, @var{file})\n\
Define @var{function} to autoload from @var{file}.\n\
\n\
The second argument, @var{file}, should be an absolute file name or\n\
a file name in the same directory as the function or script from which\n\
the autoload command was run.  @var{file} should not depend on the\n\
Octave load path.\n\
\n\
Normally, calls to @code{autoload} appear in PKG_ADD script files that\n\
are evaluated when a directory is added to the Octave's load path.  To\n\
avoid having to hardcode directory names in @var{file}, if @var{file}\n\
is in the same directory as the PKG_ADD script then\n\
\n\
@example\n\
autoload (\"foo\", \"bar.oct\");\n\
@end example\n\
\n\
@noindent\n\
will load the function @code{foo} from the file @code{bar.oct}.  The above\n\
when @code{bar.oct} is not in the same directory or uses like\n\
\n\
@example\n\
autoload (\"foo\", file_in_loadpath (\"bar.oct\"))\n\
@end example\n\
\n\
@noindent\n\
are strongly discouraged, as their behavior might be unpredictable.\n\
\n\
With no arguments, return a structure containing the current autoload map.\n\
@seealso{PKG_ADD}\n\
@end deftypefn")
{
  octave_value retval;

  int nargin = args.length ();

  if (nargin == 0)
    {
      Cell func_names (dim_vector (autoload_map.size (), 1));
      Cell file_names (dim_vector (autoload_map.size (), 1));

      octave_idx_type i = 0;
      typedef std::map<std::string, std::string>::const_iterator am_iter;
      for (am_iter p = autoload_map.begin (); p != autoload_map.end (); p++)
        {
          func_names(i) = p->first;
          file_names(i) = p->second;

          i++;
        }

      octave_map m;

      m.assign ("function", func_names);
      m.assign ("file", file_names);

      retval = m;
    }
  else if (nargin == 2)
    {
      string_vector argv = args.make_argv ("autoload");

      if (! error_state)
        {
          std::string nm = argv[2];

          if (! octave_env::absolute_pathname (nm))
            {
              octave_user_code *fcn = octave_call_stack::caller_user_code ();

              bool found = false;

              if (fcn)
                {
                  std::string fname = fcn->fcn_file_name ();

                  if (! fname.empty ())
                    {
                      fname = octave_env::make_absolute (fname);
                      fname = fname.substr (0, fname.find_last_of (file_ops::dir_sep_str ()) + 1);

                      file_stat fs (fname + nm);

                      if (fs.exists ())
                        {
                          nm = fname + nm;
                          found = true;
                        }
                    }
                }
              if (! found)
                warning_with_id ("Octave:autoload-relative-file-name",
                                 "autoload: `%s' is not an absolute file name",
                                 nm.c_str ());
            }
          autoload_map[argv[1]] = nm;
        }
    }
  else
    print_usage ();

  return retval;
}

void
source_file (const std::string& file_name, const std::string& context,
             bool verbose, bool require_file, const std::string& warn_for)
{
  // Map from absolute name of script file to recursion level.  We
  // use a map instead of simply placing a limit on recursion in the
  // source_file function so that two mutually recursive scripts
  // written as
  //
  //   foo1.m:
  //   ------
  //   foo2
  //
  //   foo2.m:
  //   ------
  //   foo1
  //
  // and called with
  //
  //   foo1
  //
  // (for example) will behave the same if they are written as
  //
  //   foo1.m:
  //   ------
  //   source ("foo2.m")
  //
  //   foo2.m:
  //   ------
  //   source ("foo1.m")
  //
  // and called with
  //
  //   source ("foo1.m")
  //
  // (for example).

  static std::map<std::string, int> source_call_depth;

  std::string file_full_name = file_ops::tilde_expand (file_name);

  file_full_name = octave_env::make_absolute (file_full_name);

  unwind_protect frame;

  frame.protect_var (curr_fcn_file_name);
  frame.protect_var (curr_fcn_file_full_name);

  curr_fcn_file_name = file_name;
  curr_fcn_file_full_name = file_full_name;

  if (source_call_depth.find (file_full_name) == source_call_depth.end ())
    source_call_depth[file_full_name] = -1;

  frame.protect_var (source_call_depth[file_full_name]);

  source_call_depth[file_full_name]++;

  if (source_call_depth[file_full_name] >= Vmax_recursion_depth)
    {
      error ("max_recursion_depth exceeded");
      return;
    }

  if (! context.empty ())
    {
      if (context == "caller")
        octave_call_stack::goto_caller_frame ();
      else if (context == "base")
        octave_call_stack::goto_base_frame ();
      else
        error ("source: context must be \"caller\" or \"base\"");

      if (! error_state)
        frame.add_fcn (octave_call_stack::pop);
    }

  if (! error_state)
    {
      octave_function *fcn = parse_fcn_file (file_full_name, "", true,
                                             require_file, warn_for);

      if (! error_state)
        {
          if (fcn && fcn->is_user_script ())
            {
              octave_value_list args;

              if (verbose)
                {
                  std::cout << "executing commands from " << file_full_name << " ... ";
                  reading_startup_message_printed = true;
                  std::cout.flush ();
                }

              fcn->do_multi_index_op (0, args);

              if (verbose)
                std::cout << "done." << std::endl;

              delete fcn;
            }
        }
      else
        error ("source: error sourcing file `%s'",
               file_full_name.c_str ());
    }
}

DEFUN (mfilename, args, ,
  "-*- texinfo -*-\n\
@deftypefn  {Built-in Function} {} mfilename ()\n\
@deftypefnx {Built-in Function} {} mfilename (\"fullpath\")\n\
@deftypefnx {Built-in Function} {} mfilename (\"fullpathext\")\n\
Return the name of the currently executing file.  At the top-level,\n\
return the empty string.  Given the argument @code{\"fullpath\"},\n\
include the directory part of the file name, but not the extension.\n\
Given the argument @code{\"fullpathext\"}, include the directory part\n\
of the file name and the extension.\n\
@end deftypefn")
{
  octave_value retval;

  int nargin = args.length ();

  if (nargin > 1)
    {
      print_usage ();
      return retval;
    }

  std::string arg;

  if (nargin == 1)
    {
      arg = args(0).string_value ();

      if (error_state)
        {
          error ("mfilename: expecting argument to be a character string");
          return retval;
        }
    }

  std::string fname;

  octave_user_code *fcn = octave_call_stack::caller_user_code ();

  if (fcn)
    {
      fname = fcn->fcn_file_name ();

      if (fname.empty ())
        fname = fcn->name ();
    }

  if (arg == "fullpathext")
    retval = fname;
  else
    {
      size_t dpos = fname.rfind (file_ops::dir_sep_char ());
      size_t epos = fname.rfind ('.');

      if (epos <= dpos)
        epos = std::string::npos;

      fname = (epos != std::string::npos) ? fname.substr (0, epos) : fname;

      if (arg == "fullpath")
        retval = fname;
      else
        retval = (dpos != std::string::npos) ? fname.substr (dpos+1) : fname;
    }

  return retval;
}


DEFUN (source, args, ,
  "-*- texinfo -*-\n\
@deftypefn {Built-in Function} {} source (@var{file})\n\
Parse and execute the contents of @var{file}.  This is equivalent to\n\
executing commands from a script file, but without requiring the file to\n\
be named @file{@var{file}.m}.\n\
@end deftypefn")
{
  octave_value_list retval;

  int nargin = args.length ();

  if (nargin == 1 || nargin == 2)
    {
      std::string file_name = args(0).string_value ();

      if (! error_state)
        {
          std::string context;

          if (nargin == 2)
            context = args(1).string_value ();

          if (! error_state)
            source_file (file_name, context);
          else
            error ("source: expecting context to be character string");
        }
      else
        error ("source: expecting file name as argument");
    }
  else
    print_usage ();

  return retval;
}

// Evaluate an Octave function (built-in or interpreted) and return
// the list of result values.  NAME is the name of the function to
// call.  ARGS are the arguments to the function.  NARGOUT is the
// number of output arguments expected.

octave_value_list
feval (const std::string& name, const octave_value_list& args, int nargout)
{
  octave_value_list retval;

  octave_value fcn = symbol_table::find_function (name, args);

  if (fcn.is_defined ())
    retval = fcn.do_multi_index_op (nargout, args);
  else
    {
      maybe_missing_function_hook (name);
      if (! error_state)
        error ("feval: function `%s' not found", name.c_str ());
    }

  return retval;
}

octave_value_list
feval (octave_function *fcn, const octave_value_list& args, int nargout)
{
  octave_value_list retval;

  if (fcn)
    retval = fcn->do_multi_index_op (nargout, args);

  return retval;
}

static octave_value_list
get_feval_args (const octave_value_list& args)
{
  return args.slice (1, args.length () - 1, true);
}


// Evaluate an Octave function (built-in or interpreted) and return
// the list of result values.  The first element of ARGS should be a
// string containing the name of the function to call, then the rest
// are the actual arguments to the function.  NARGOUT is the number of
// output arguments expected.

octave_value_list
feval (const octave_value_list& args, int nargout)
{
  octave_value_list retval;

  int nargin = args.length ();

  if (nargin > 0)
    {
      octave_value f_arg = args(0);

      if (f_arg.is_string ())
        {
          std::string name = f_arg.string_value ();

          if (! error_state)
            {
              octave_value_list tmp_args = get_feval_args (args);

              retval = feval (name, tmp_args, nargout);
            }
        }
      else if (f_arg.is_function_handle ()
               || f_arg.is_anonymous_function ()
               || f_arg.is_inline_function ())
        {
          const octave_value_list tmp_args = get_feval_args (args);

          retval = f_arg.do_multi_index_op (nargout, tmp_args);
        }
      else
        error ("feval: first argument must be a string, inline function or a function handle");
    }

  return retval;
}

DEFUN (feval, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn {Built-in Function} {} feval (@var{name}, @dots{})\n\
Evaluate the function named @var{name}.  Any arguments after the first\n\
are passed on to the named function.  For example,\n\
\n\
@example\n\
@group\n\
feval (\"acos\", -1)\n\
     @result{} 3.1416\n\
@end group\n\
@end example\n\
\n\
@noindent\n\
calls the function @code{acos} with the argument @samp{-1}.\n\
\n\
The function @code{feval} can also be used with function handles of\n\
any sort (@pxref{Function Handles}).  Historically, @code{feval} was\n\
the only way to call user-supplied functions in strings, but\n\
function handles are now preferred due to the cleaner syntax they\n\
offer.  For example,\n\
\n\
@example\n\
@group\n\
@var{f} = @@exp;\n\
feval (@var{f}, 1)\n\
     @result{} 2.7183\n\
@var{f} (1)\n\
     @result{} 2.7183\n\
@end group\n\
@end example\n\
\n\
@noindent\n\
are equivalent ways to call the function referred to by @var{f}.  If it\n\
cannot be predicted beforehand that @var{f} is a function handle or the\n\
function name in a string, @code{feval} can be used instead.\n\
@end deftypefn")
{
  octave_value_list retval;

  int nargin = args.length ();

  if (nargin > 0)
    retval = feval (args, nargout);
  else
    print_usage ();

  return retval;
}

DEFUN (builtin, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {[@dots{}]} builtin (@var{f}, @dots{})\n\
Call the base function @var{f} even if @var{f} is overloaded to\n\
another function for the given type signature.\n\
@end deftypefn")
{
  octave_value_list retval;

  int nargin = args.length ();

  if (nargin > 0)
    {
      const std::string name (args(0).string_value ());

      if (! error_state)
        {
          octave_value fcn = symbol_table::builtin_find (name);

          if (fcn.is_defined ())
            retval = feval (fcn.function_value (), args.splice (0, 1),
                            nargout);
          else
            error ("builtin: lookup for symbol `%s' failed", name.c_str ());
        }
      else
        error ("builtin: function name (F) must be a string");
    }
  else
    print_usage ();

  return retval;
}

octave_value_list
eval_string (const std::string& s, bool silent, int& parse_status, int nargout)
{
  octave_value_list retval;

  unwind_protect frame;

  frame.protect_var (input_line_number);
  frame.protect_var (current_input_column);
  frame.protect_var (get_input_from_eval_string);
  frame.protect_var (input_from_eval_string_pending);
  frame.protect_var (parser_end_of_input);
  frame.protect_var (line_editing);
  frame.protect_var (current_eval_string);
  frame.protect_var (current_function_depth);
  frame.protect_var (max_function_depth);
  frame.protect_var (parsing_subfunctions);
  frame.protect_var (endfunction_found);
  frame.protect_var (reading_fcn_file);
  frame.protect_var (reading_script_file);
  frame.protect_var (reading_classdef_file);

  input_line_number = 1;
  current_input_column = 1;
  get_input_from_eval_string = true;
  input_from_eval_string_pending = true;
  parser_end_of_input = false;
  line_editing = false;
  current_function_depth = 0;
  max_function_depth = 0;
  parsing_subfunctions = false;
  endfunction_found = false;
  reading_fcn_file = false;
  reading_script_file = false;
  reading_classdef_file = false;

  current_eval_string = s;

  YY_BUFFER_STATE old_buf = current_buffer ();
  YY_BUFFER_STATE new_buf = create_buffer (0);

  frame.add_fcn (switch_to_buffer, old_buf);
  frame.add_fcn (delete_buffer, new_buf);

  switch_to_buffer (new_buf);

  do
    {
      reset_parser ();

      frame.protect_var (global_command);

      global_command = 0;

      // Do this with an unwind-protect cleanup function so that the
      // forced variables will be unmarked in the event of an
      // interrupt.
      symbol_table::scope_id scope = symbol_table::top_scope ();
      frame.add_fcn (symbol_table::unmark_forced_variables, scope);

      parse_status = yyparse ();

      tree_statement_list *command_list = global_command;

      // Unmark forced variables.
      // Restore previous value of global_command.
      frame.run_top (2);

      if (parse_status == 0)
        {
          if (command_list)
            {
              unwind_protect inner_frame;

              // Use an unwind-protect cleanup function so that the
              // global_command list will be deleted in the event of an
              // interrupt.

              inner_frame.add_fcn (cleanup_statement_list, &command_list);

              tree_statement *stmt = 0;

              if (command_list->length () == 1
                  && (stmt = command_list->front ())
                  && stmt->is_expression ())
                {
                  tree_expression *expr = stmt->expression ();

                  if (silent)
                    expr->set_print_flag (false);

                  bool do_bind_ans = false;

                  if (expr->is_identifier ())
                    {
                      tree_identifier *id
                        = dynamic_cast<tree_identifier *> (expr);

                      do_bind_ans = (! id->is_variable ());
                    }
                  else
                    do_bind_ans = (! expr->is_assignment_expression ());

                  retval = expr->rvalue (nargout);

                  if (do_bind_ans && ! (error_state || retval.empty ()))
                    bind_ans (retval(0), expr->print_result ());

                  if (nargout == 0)
                    retval = octave_value_list ();
                }
              else if (nargout == 0)
                command_list->accept (*current_evaluator);
              else
                error ("eval: invalid use of statement list");

              if (error_state
                  || tree_return_command::returning
                  || tree_break_command::breaking
                  || tree_continue_command::continuing)
                break;
            }
          else if (parser_end_of_input)
            break;
        }
    }
  while (parse_status == 0);

  return retval;
}

octave_value
eval_string (const std::string& s, bool silent, int& parse_status)
{
  octave_value retval;

  octave_value_list tmp = eval_string (s, silent, parse_status, 1);

  if (! tmp.empty ())
    retval = tmp(0);

  return retval;
}

static octave_value_list
eval_string (const octave_value& arg, bool silent, int& parse_status,
             int nargout)
{
  std::string s = arg.string_value ();

  if (error_state)
    {
      error ("eval: expecting std::string argument");
      return octave_value (-1);
    }

  return eval_string (s, silent, parse_status, nargout);
}

void
cleanup_statement_list (tree_statement_list **lst)
{
  if (*lst)
    {
      delete *lst;
      *lst = 0;
    }
}

DEFUN (eval, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn  {Built-in Function} {} eval (@var{try})\n\
@deftypefnx {Built-in Function} {} eval (@var{try}, @var{catch})\n\
Parse the string @var{try} and evaluate it as if it were an Octave\n\
program.  If that fails, evaluate the optional string @var{catch}.\n\
The string @var{try} is evaluated in the current context,\n\
so any results remain available after @code{eval} returns.\n\
\n\
The following example makes the variable @var{a} with the approximate\n\
value 3.1416 available.\n\
\n\
@example\n\
eval(\"a = acos(-1);\");\n\
@end example\n\
\n\
If an error occurs during the evaluation of @var{try} the @var{catch}\n\
string is evaluated, as the following example shows:\n\
\n\
@example\n\
@group\n\
eval ('error (\"This is a bad example\");',\n\
      'printf (\"This error occurred:\\n%s\\n\", lasterr ());');\n\
     @print{} This error occurred:\n\
        This is a bad example\n\
@end group\n\
@end example\n\
\n\
Consider using try/catch blocks instead if you are only using @code{eval}\n\
as an error-capturing mechanism rather than for the execution of arbitrary\n\
code strings.\n\
@seealso{evalin}\n\
@end deftypefn")
{
  octave_value_list retval;

  int nargin = args.length ();

  if (nargin > 0)
    {
      unwind_protect frame;

      if (nargin > 1)
        {
          frame.protect_var (buffer_error_messages);
          buffer_error_messages++;
        }

      int parse_status = 0;

      octave_value_list tmp = eval_string (args(0), nargout > 0,
                                           parse_status, nargout);

      if (nargin > 1 && (parse_status != 0 || error_state))
        {
          error_state = 0;

          // Set up for letting the user print any messages from
          // errors that occurred in the first part of this eval().

          buffer_error_messages--;

          tmp = eval_string (args(1), nargout > 0, parse_status, nargout);

          if (nargout > 0)
            retval = tmp;
        }
      else if (nargout > 0)
        retval = tmp;
    }
  else
    print_usage ();

  return retval;
}

/*

%!shared x
%! x = 1;

%!assert (eval ("x"), 1)
%!assert (eval ("x;"))
%!assert (eval ("x;"), 1);

%!test
%! y = eval ("x");
%! assert (y, 1);

%!test
%! y = eval ("x;");
%! assert (y, 1);

%!test
%! eval ("x = 1;")
%! assert (x,1);

%!test
%! eval ("flipud = 2;");
%! assert (flipud, 2);

%!function y = __f ()
%!  eval ("flipud = 2;");
%!  y = flipud;
%!endfunction
%!assert (__f(), 2)

*/

DEFUN (assignin, args, ,
  "-*- texinfo -*-\n\
@deftypefn {Built-in Function} {} assignin (@var{context}, @var{varname}, @var{value})\n\
Assign @var{value} to @var{varname} in context @var{context}, which\n\
may be either @code{\"base\"} or @code{\"caller\"}.\n\
@seealso{evalin}\n\
@end deftypefn")
{
  octave_value_list retval;

  int nargin = args.length ();

  if (nargin == 3)
    {
      std::string context = args(0).string_value ();

      if (! error_state)
        {
          unwind_protect frame;

          if (context == "caller")
            octave_call_stack::goto_caller_frame ();
          else if (context == "base")
            octave_call_stack::goto_base_frame ();
          else
            error ("assignin: CONTEXT must be \"caller\" or \"base\"");

          if (! error_state)
            {
              frame.add_fcn (octave_call_stack::pop);

              std::string nm = args(1).string_value ();

              if (! error_state)
                {
                  if (valid_identifier (nm))
                    symbol_table::varref (nm) = args(2);
                  else
                    error ("assignin: invalid variable name in argument VARNAME");
                }
              else
                error ("assignin: VARNAME must be a string");
            }
        }
      else
        error ("assignin: CONTEXT must be a string");
    }
  else
    print_usage ();

  return retval;
}

DEFUN (evalin, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn  {Built-in Function} {} evalin (@var{context}, @var{try})\n\
@deftypefnx {Built-in Function} {} evalin (@var{context}, @var{try}, @var{catch})\n\
Like @code{eval}, except that the expressions are evaluated in the\n\
context @var{context}, which may be either @code{\"caller\"} or\n\
@code{\"base\"}.\n\
@seealso{eval, assignin}\n\
@end deftypefn")
{
  octave_value_list retval;

  int nargin = args.length ();

  if (nargin > 1)
    {
      std::string context = args(0).string_value ();

      if (! error_state)
        {
          unwind_protect frame;

          if (context == "caller")
            octave_call_stack::goto_caller_frame ();
          else if (context == "base")
            octave_call_stack::goto_base_frame ();
          else
            error ("evalin: CONTEXT must be \"caller\" or \"base\"");

          if (! error_state)
            {
              frame.add_fcn (octave_call_stack::pop);

              if (nargin > 2)
                {
                  frame.protect_var (buffer_error_messages);
                  buffer_error_messages++;
                }

              int parse_status = 0;

              octave_value_list tmp = eval_string (args(1), nargout > 0,
                                                   parse_status, nargout);

              if (nargout > 0)
                retval = tmp;

              if (nargin > 2 && (parse_status != 0 || error_state))
                {
                  error_state = 0;

                  // Set up for letting the user print any messages from
                  // errors that occurred in the first part of this eval().

                  buffer_error_messages--;

                  tmp = eval_string (args(2), nargout > 0,
                                     parse_status, nargout);

                  retval = (nargout > 0) ? tmp : octave_value_list ();
                }
            }
        }
      else
        error ("evalin: CONTEXT must be a string");
    }
  else
    print_usage ();

  return retval;
}

DEFUN (__parser_debug_flag__, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn {Built-in Function} {@var{old_val} =} __parser_debug_flag__ (@var{new_val}))\n\
Undocumented internal function.\n\
@end deftypefn")
{
  octave_value retval;

  bool debug_flag = octave_debug;

  retval = set_internal_variable (debug_flag, args, nargout,
                                  "__parser_debug_flag__");

  octave_debug = debug_flag;

  return retval;
}

