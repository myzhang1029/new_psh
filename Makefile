CC=gcc
psh: pshell.h main.c type_prompt.c read_command.c builtin_command.c parsing.c test
	$(CROSS_PREFIX)$(CC) $(CFLAGS) main.c type_prompt.c read_command.c builtin_command.c parsing.c -o $@ -lreadline 

test: test.c
	$(CC) test.c -o test

install:
	install -p -m 755 psh $(PREFIX)/bin

clean:
	-rm -f psh test
