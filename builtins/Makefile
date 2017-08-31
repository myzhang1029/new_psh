COMPILE_BUILTINS=echo.c exec.c cd.c

all: $(COMPILE_BUILTINS) builtin.h ../pshell.h
	$(CROSS_PREFIX)$(CC) $(CFLAGS) -c $(COMPILE_BUILTINS)
	$(CROSS_PREFIX)$(AR) rcs builtins.a $(COMPILE_BUILTINS:.c=.o)
