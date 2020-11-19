AUTOMAKE_OPTIONS = foreign
bin_PROGRAMS = psh
psh_SOURCES = builtins.c command.c filpinfo.c input.c main.c parser.c args.c \
			  prompts.c util.c variable.c builtins/builtin.c builtins/cd.c \
			  builtins/echo.c builtins/exec.c builtins/exit.c \
			  builtins/history.c builtins/pwd.c builtins/true.c
psh_CFLAGS = -I$(top_srcdir)/include
noinst_HEADERS = $(top_srcdir)/include/backend.h $(top_srcdir)/include/builtin.h \
				 $(top_srcdir)/include/command.h $(top_srcdir)/include/filpinfo.h \
				 $(top_srcdir)/include/input.h $(top_srcdir)/include/prompts.h \
				 $(top_srcdir)/include/psh.h $(top_srcdir)/include/token.h \
				 $(top_srcdir)/include/util.h $(top_srcdir)/include/variable.h \
				 $(top_srcdir)/include/args.h
psh_LDADD = ../lib/libpsh.a

if POSIX
psh_SOURCES += backends/posix/posix.c
else # Unfinished platforms should be captured by configure
psh_SOURCES += backends/generic/generic.c
endif
