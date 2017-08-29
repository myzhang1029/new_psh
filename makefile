psh: pshell.h pshell.c type_prompt.c read_command.c builtin_command.c parsing.c test
	$(CC) pshell.c type_prompt.c read_command.c builtin_command.c parsing.c -o $@ -D READLINE_ON -I /usr/include -lreadline -ltermcap

test: test.c
	$(CC) test.c -o test

clean:
	-rm -f psh test
