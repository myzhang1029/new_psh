#include "builtin.h"

int builtin_exec(char *command, char **parameters)
{
	if(parameters[1]==NULL)
		return 1;/* Do nothing */
	if(execv(parameters[1], &parameters[1])==-1)
		OUT2E("exec: %s: %s\n", parameters[1], strerror(errno));
	return 2;
}
