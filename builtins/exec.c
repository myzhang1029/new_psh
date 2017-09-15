#include "builtin.h"

int builtin_exec(char *command, char **parameters)
{
	if(parameters[1]==NULL)
		return 1;/* Do nothing */
	char **execargv = (char**) malloc(sizeof(char)*MAXLINE);
	if(execargv == NULL)
	{
		OUT2E("exec: malloc failed: %s\n", strerror(errno));
		return 2;
	}
	else
	{
		int i=0;
		memmove(execargv, parameters[1], MAXLINE);
		execargv[i]=NULL;
	}
	if(execv(parameters[1],execargv)==-1)
		OUT2E("exec: %s: %s\n", parameters[1], strerror(errno));
	return 2;
}
