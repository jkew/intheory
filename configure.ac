#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([intheory], [0.1], [john.v.kew.ii@gmail.com])
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE([foreign])
LT_INIT

AC_CONFIG_SRCDIR([src/intheory.c])

AM_SILENT_RULES
# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC

AC_OUTPUT
AC_OUTPUT(Makefile src/Makefile test/Makefile examples/Makefile)

