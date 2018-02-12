/*
   cd.c - builtin cd

   Copyright 2013 wuyve.
   Copyright 2017 Zhang Maiyun.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "builtin.h"
#include "backends/backend.h"
#include <string.h>
#include <stdlib.h>

static int create_new_pwd(char **cd_dir)
{
	char *nowpwd=getenv("PWD");
	if(!cd_dir || !(*cd_dir))
		return 1;
	if((*cd_dir)[0]=='/')/* cd_dir is an abs path */
	{
		if(strstr((*cd_dir), "..") == NULL)
		{
			if(strstr((*cd_dir), ".") == NULL)
				/* cd_dir did not contain a .. or . */
				return 0;
			else
				/* have . but no .. */
			{
				char *delim;
				while(1)
				{
					delim=strstr((*cd_dir), ".");
					if(!delim)
						return 0; /* All set */
					/* NOTE: Must reach here. */
					if(!(delim+1))/* after '.' is EOL */
						*delim = 0;/* Just remove '.' */
					else if(*(delim+1) != '/')/* Not the thing we're looking for */
						continue;/* ignore */
					else
						memmove(delim, delim+2, strlen(delim+2)+1/*\0*/);
					/* Move forward the string to remove ./ */
				}
			}
		}
	}
	else
	{
		char *oldpwd=getenv("PWD");
		*cd_dir=realloc(*cd_dir, strlen(*cd_dir)+1/*\0*/+strlen(oldpwd)+1/*'/'*/);
		if(!cd_dir)
		{
			OUT2E("%s: create_new_pwd: realloc failed\n", argv0);
			return 1;
		}
		{
			int count=strlen(*cd_dir)+1/*\0*/;
			char *d = (*cd_dir)+strlen(oldpwd)+1/*'/'*/+count-1;
			char *s = (*cd_dir)+count-1;
			while (count--)
				*d-- = *s--;
		}
		memmove(*cd_dir, oldpwd, strlen(oldpwd));
		(*cd_dir)[strlen(oldpwd)]='/';
	}
	return 0;
}

int builtin_cd(ARGS)
{
	char *cd_path = NULL;
	char *oldpwd = getenv("PWD");
	if(b_parameters[1] == NULL)/* 'cd', the same as cd $HOME */
	{
		char *homedir=getenv("HOME");
		if(!homedir)
		{
			OUT2E("%s: %s: HOME not set\n", argv0, b_command);
			return 2;
		}
		cd_path=malloc(strlen(homedir)+1);
		if(cd_path == NULL)
		{
			OUT2E("%s: malloc failed: %s\n", b_command, strerror(errno));
			return 2;
		}
		strcpy(cd_path, homedir);
	}
	else if(strcmp(b_parameters[1], "-") == 0)/* 'cd -', the same as cd $OLDPWD*/
	{
		char *oldpwd=getenv("OLDPWD");
		if(!oldpwd)
		{
			OUT2E("%s: %s: OLDPWD not set\n", argv0, b_command);
			return 2;
		}
		cd_path=malloc(strlen(oldpwd)+1);
		if(cd_path == NULL)
		{
			OUT2E("%s: malloc failed: %s\n", b_command, strerror(errno));
			return 2;
		}
		strcpy(cd_path, oldpwd);
	}
	else
	{
		cd_path = malloc(strlen(b_parameters[1]+1));
		if(cd_path == NULL)
		{
			OUT2E("%s: malloc failed: %s\n", b_command, strerror(errno));
			return 2;
		}
		strcpy(cd_path, b_parameters[1]);

	}

	create_new_pwd(&cd_path);

	if(chdir(cd_path)!= 0)
		OUT2E("%s: %s: %s\n", b_command, strerror(errno), cd_path);
	else
	{
		setenv("OLDPWD", getenv("PWD"), 1);
		setenv("PWD", cd_path, 1);
	}
	free(cd_path);
	return 1;
}

