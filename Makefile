CC=gcc
PREFIX=/usr/local
FILES=main.o show.o splitbuf.o input.o preprocess.o parser.o builtins.o builtins/exec.o builtins/echo.o builtins/cd.o builtins/pwd.o

psh: pshell.h builtins/builtin.h $(FILES) 
	$(CROSS_PREFIX)$(CC) $(LDFLAGS) $(FILES) -o $@ -lreadline

%.o:%.c
	$(CROSS_PREFIX)$(CC) $(CFLAGS) -c $^ -o $@

install:
	install -p -m 755 psh $(PREFIX)/bin

clean:
	-rm -f psh */*.o *.o
