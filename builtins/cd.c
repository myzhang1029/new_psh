#include "builtin.h"

int builtin_cd(char *command, char **parameters)
{
	char *cd_path = NULL, hdir=gethd();

	if(parameters[1] == NULL/* 'cd' */)
	{
		cd_path=malloc(strlen(hdir)+1);
		if(cd_path == NULL)
		{
			OUT2E("%s: malloc failed: %s\n", command, strerror(errno));
			return 2;
		}
		strcpy(cd_path, hdir);
	}
	else if(parameters[1][0] == '~')
	{
		cd_path = malloc(strlen(hdir)+strlen(parameters[1])+1);
		if(cd_path == NULL)
		{
			OUT2E("%s: malloc failed: %s\n", command, strerror(errno));
			return 2;
		}
		strcpy(cd_path, hdir);
		strncpy(cd_path+strlen(hdir),parameters[1]+1,strlen(parameters[1]));
	}
	else
	{
		cd_path = malloc(strlen(parameters[1]+1));
		if(cd_path == NULL)
		{
			OUT2E("%s: malloc failed: %s\n", command, strerror(errno));
			return 2;
		}
		strcpy(cd_path,parameters[1]);
	}
	if(chdir(cd_path)!= 0)
		OUT2E("%s: %s: %s\n", command, strerror(errno), cd_path);
	free(cd_path);
	return 1;
}
