CC=gcc
PREFIX=/usr/local
FILES=main.c show.c input.c preprocess.c parser.c builtins.c builtins/exec.c builtins/echo.c builtins/cd.c

psh: pshell.h builtins/builtin.h $(FILES) 
	$(CROSS_PREFIX)$(CC) $(CFLAGS) $(FILES) -o $@ -lreadline $(LDFLAGS)

install:
	install -p -m 755 psh $(PREFIX)/bin

clean:
	-rm -f psh */*.o *.o
