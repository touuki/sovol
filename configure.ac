#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([sovol], [1.2.0], [touuki@protonmail.com])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AM_CONDITIONAL([WASM], [test `basename $CXX` = "em++"])
AC_CONFIG_SRCDIR([lib/Field.hh])
#AC_CONFIG_HEADERS([config.h])
AM_PROG_AR
# Checks for programs.
AC_PROG_CXX
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADER_STDBOOL

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT32_T

# Checks for library functions.
AC_CHECK_FUNCS([pow sqrt setenv])

AC_CONFIG_FILES([Makefile lib/Makefile cli/Makefile wasm/Makefile])
AC_OUTPUT
