#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

dnl --------------------------------
dnl Initialization macros.
dnl --------------------------------

AC_PREREQ(2.61)
#AC_INIT([$PACKAGE], [$LIBRARY_VERSION], [https://bugs.launchpad.net/libgexf])
AC_INIT(libgexf/libgexf.h)
AC_CONFIG_SRCDIR([libgexf/libgexf.h])
AC_CONFIG_HEADER([config.h])


dnl -----------------------------------------------
dnl Package name and version number (user defined)
dnl -----------------------------------------------

LIBRARY_NAME=libgexf
PACKAGE=$LIBRARY_NAME
AC_SUBST(LIBRARY_NAME)

#API version (often = GENERIC_MAJOR_VERSION.GENERIC_MINOR_VERSION)
#GENERIC_API_VERSION=1.0
#AC_SUBST(GENERIC_API_VERSION)

#shared library versioning
#GENERIC_LIBRARY_VERSION=1:0:0
#                       | | |
#                +------+ | +---+
#                |        |     |
#             current:revision:age
#                |        |     |
#                |        |     +- increment if interfaces have been added
#                |        |        set to zero if interfaces have been removed
#                                  or changed
#                |        +- increment if source code has changed
#                |           set to zero if current is incremented
#                +- increment if interfaces have been added, removed or changed


LIBRARY_VERSION=0.1.1
AC_SUBST(LIBRARY_VERSION)
AM_INIT_AUTOMAKE($PACKAGE, $LIBRARY_VERSION, no-define)


dnl -----------------------------------------------
dnl Checks for programs.
dnl -----------------------------------------------

AC_PROG_CXX
AC_PROG_CC


dnl -----------------------------------------------
dnl Checks for libraries.
dnl -----------------------------------------------

dnl libxml2 support
AC_PATH_PROG(LIBXML2_CONFIG, xml2-config, no)
  if test "x$LIBXML2_CONFIG" = "xno" ; then
    AC_MSG_RESULT([*** libxml2 support is not available ***])
  else
    XML2_CXXFLAGS=`$LIBXML2_CONFIG --cflags`
    XML2_LIBS=`$LIBXML2_CONFIG --libs`
    have_libxml2="yes"
    AC_DEFINE(HAVE_LIBXML2,1,[Define this for libxml2 support])
  fi
AM_CONDITIONAL(HAVE_LIBXML2, test x"$have_libxml2" = "xyes")
AC_SUBST(XML2_CXXFLAGS)
AC_SUBST(XML2_LIBS)


# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST

# Checks for library functions.
AM_PROG_LIBTOOL
AM_SANITY_CHECK

AC_LANG_CPLUSPLUS 


dnl -----------------------------------------------
dnl Generates Makefile's, configuration files and scripts
dnl -----------------------------------------------

AM_INIT_AUTOMAKE(libgexf, 0.1.2)
AC_OUTPUT(Makefile \
          libgexf/Makefile
)
