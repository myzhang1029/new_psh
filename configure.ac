#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([psh], [0.18.0], [myzhang1029@hotmail.com])
AC_CONFIG_SRCDIR([src/main.c], [lib/util.c])
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
AC_PROG_RANLIB

# Checks for libraries.
AC_SEARCH_LIBS([readline],
               [readline],
               [
                AC_DEFINE_UNQUOTED([HAVE_READLINE_GNU], [1], [Define to 1 if GNU libreadline is found])
                AC_DEFINE_UNQUOTED([HAVE_READLINE], [1], [Define to 1 if libreadline is found])
               ],
               [AC_SEARCH_LIBS([readline],
                               [edit],
                               [
                                AC_DEFINE_UNQUOTED([HAVE_READLINE_ALTS], [1], [Define to 1 if alternative readline implementations are found])
                                AC_DEFINE_UNQUOTED([HAVE_READLINE], [1], [Define to 1 if libreadline is found])
                               ],
                               [AC_MSG_WARN(
                                [No libreadline or libedit, disabling readline. If yout think this is a mistake, try adding its path to CFLAGS and LDFLAGS.])
                               ])
               ])

AC_SEARCH_LIBS([history_list],
               [history],
               [AC_DEFINE_UNQUOTED([HAVE_WORKING_HISTORY], [1],
                                   [Define to 1 if working libhistory if found])
               ],
               [AC_MSG_WARN([No libhistory, disabling history. If you think this is a mistake, try adding its path to CFLAGS and LDFLAGS.])])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stddef.h stdint.h stdlib.h string.h unistd.h readline/readline.h readline/history.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC

AC_OUTPUT([Makefile lib/Makefile src/Makefile])
