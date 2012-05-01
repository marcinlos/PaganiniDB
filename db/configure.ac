#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([PaganiniDB], [1.0],, [paganini-1.0])
#AC_CONFIG_SRCDIR([src/memory.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CC
LT_INIT

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stddef.h stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
#AC_CHECK_FUNCS([atexit])

AC_DEFINE([_PAGANINI],,[To distinguish between library and user])

AC_CONFIG_FILES([Makefile 
                 src/Makefile
                 src/error_msg/Makefile
                 src/paging/Makefile])
                 
AC_OUTPUT
