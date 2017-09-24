#include "../pshell.h"
#define ARGS char *command, char **parameters
int builtin_exec(ARGS);
int builtin_echo(ARGS);
int builtin_cd(ARGS);
int builtin_pwd(ARGS);

