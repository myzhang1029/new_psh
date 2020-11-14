#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([psh], [0.18.0], [myzhang1029@hotmail.com])
AC_CONFIG_SRCDIR([config.h.in], [src], [lib])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([subdir-objects])

# Code to select a backend
AC_CANONICAL_HOST

posix_backend=false
generic_backend=false
case "${host_os}" in
    cygwin*|darwin*|linux*|bsd*)
        posix_backend=true
        ;;
    *)
        AC_MSG_ERROR(["Currently unsupported host system: $host_os"])
        #generic_backend=true
        ;;
esac
AM_CONDITIONAL([POSIX], [test "$posix_backend" = "true"])
AM_CONDITIONAL([GENERIC], [test "$generic_backend" = "true"])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_PROG_RANLIB

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stddef.h stdint.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC

AC_OUTPUT([Makefile lib/Makefile src/Makefile])
