CC=gcc
PREFIX=/usr/local
PSH_FILES=main.o prompts.o input.o filpinfo.o builtins.o builtins/echo.o builtins/cd.o builtins/pwd.o builtins/history.o builtins/builtin.o builtins/true.o builtins/exit.o builtins/misc.o
CFLAGS=-W -Wall -std=c89 -I.

libpsh.a: $(LIBPSH_FILES)

psh: pshell.h builtins/builtin.h $(PSH_FILES)
	$(CROSS_PREFIX)$(CC) $(LDFLAGS) $(PSH_FILES) -o $@ -lreadline -lhistory

%.o:%.c pshell.h builtins/builtin.h Makefile
	$(CROSS_PREFIX)$(CC) $(CFLAGS) -c $< -o $@

install: psh
	install -p -m 755 psh $(PREFIX)/bin

clean:
	-rm -f psh */*.o *.o
