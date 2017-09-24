#include "builtin.h"

int builtin_pwd(char *command, char **parameters)
{
	char *path=getcwd(NULL, 0);
	puts(path);
	free(path);
	return 1;
}

