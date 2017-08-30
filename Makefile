CC=gcc
psh: pshell.h main.c show.c input.c builtins.c parser.c
	$(CROSS_PREFIX)$(CC) $(CFLAGS) main.c show.c input.c builtins.c parser.c -o $@ -lreadline 

test: test.c
	$(CC) test.c -o test

install:
	install -p -m 755 psh $(PREFIX)/bin

clean:
	-rm -f psh test
