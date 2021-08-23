#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([sovol], [2.0], [touuki@protonmail.com])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_SRCDIR([src/Particle.hh])
#AC_CONFIG_HEADERS([config.h])
AM_PROG_AR
# Checks for programs.
AC_PROG_CXX
AC_PROG_RANLIB

# Checks for libraries.
AS_IF([test `basename $CXX` != "em++"], 
  [AX_LIB_HDF5([serial]) 
  AS_IF([test "$with_hdf5" = "no"], [AC_MSG_ERROR([Unable to find hdf5.])])]
)

# Checks for header files.
AC_CHECK_HEADER_STDBOOL

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT32_T

# Checks for library functions.
AC_CHECK_FUNCS([pow sqrt setenv])

AC_CONFIG_FILES([Makefile])
AM_CONDITIONAL([WASM], [test `basename $CXX` = "em++"])
AC_OUTPUT
