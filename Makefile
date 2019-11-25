CC=gcc
PREFIX=/usr/local
FILES=run_backend.o main.o show.o input.o filpinfo.o util.o builtins.o builtins/echo.o builtins/cd.o builtins/pwd.o builtins/history.o builtins/builtin.o builtins/true.o builtins/exit.o builtins/misc.o
CFLAGS=-W -Wall -std=c89 -I.

psh: pshell.h builtins/builtin.h Makefile $(FILES)
	$(CROSS_PREFIX)$(CC) $(LDFLAGS) $(FILES) -o $@ -lreadline -lhistory

%.o:%.c pshell.h builtins/builtin.h
	$(CROSS_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

install:
	install -p -m 755 psh $(PREFIX)/bin

clean:
	-rm -f psh */*.o *.o
