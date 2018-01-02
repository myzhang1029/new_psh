#include "builtin.h"

int builtin_builtin(char *command, char **parameters)
{
	if(run_builtin(parameters[1], &parameters[1])==0)
	{
		OUT2E("psh: no such builtin: %s\n", parameters[1]);
		return 2;
	}
	return 1;
}

