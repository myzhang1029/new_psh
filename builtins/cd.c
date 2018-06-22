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

#include <stdlib.h>
#include <string.h>
#include "backends/backend.h"
#include "builtin.h"

static int create_new_pwd(char **cd_dir)
{
	if (!cd_dir || !(*cd_dir))
		return 1;
	if ((*cd_dir)[0] == '/') /* cd_dir is an abs path */
	{
		char *delim;
		while ((delim = strstr((*cd_dir), "/.")))
		{
			if (*(delim + 2) == 0) /* /. */
			{
				*delim = 0;
			}
			else if (*(delim + 2) == '/') /* /./ */
			{
				memmove(delim, delim + 2, strlen(delim + 2) + 1);
			}
			else if (delim == *cd_dir) /* root/.. */
			{
				if (*(delim + 3) == 0)
					*(delim + 1) = 0;
				else
					memmove(delim, delim + 3, strlen(delim + 3) + 1);
			}
			else if (*(delim + 3) == 0) /* /..\0 */
			{
				char *lastnode;
				*delim = 0; /* Terminate the string at the
					       current / */

				lastnode = strrchr(*cd_dir, '/');
				if (lastnode == *cd_dir) /* Root node reached */
					*(lastnode + 1) = 0;
				else
					*lastnode = 0; /* Then terminate the
							  string at the last /
							*/
			}
			else /* /../ */
			{
				char oldval = *delim;
				char *lastnode;
				*delim = 0;
				lastnode = strrchr(*cd_dir, '/');
				*delim = oldval;
				memmove(lastnode, delim + 3, strlen(delim + 3) + 1);
			}
		} /* while */
	}
	else
	{
		char *oldpwd = getenv("PWD");
		*cd_dir = realloc(*cd_dir, P_CS * (strlen(*cd_dir) + 1 /*\0*/ + strlen(oldpwd) + 1 /*'/'*/));
		if (!cd_dir)
		{
			OUT2E("%s: create_new_pwd: realloc failed\n", argv0);
			return 1;
		}
		{
			int count = strlen(*cd_dir) + 1 /*\0*/;
			char *d = (*cd_dir) + strlen(oldpwd) + 1 /*'/'*/ + count - 1;
			char *s = (*cd_dir) + count - 1;
			while (count--)
				*d-- = *s--;
		}
		memmove(*cd_dir, oldpwd, strlen(oldpwd));
		(*cd_dir)[strlen(oldpwd)] = '/';
	}
	return 0;
}

int builtin_cd(ARGS)
{
	char *cd_path = NULL;
	if (b_parameters[1] == NULL) /* 'cd', the same as cd $HOME */
	{
		char *homedir = getenv("HOME");
		if (!homedir)
		{
			OUT2E("%s: %s: HOME not set\n", argv0, b_command);
			return 2;
		}
		cd_path = malloc(P_CS * (strlen(homedir) + 1));
		if (cd_path == NULL)
		{
			OUT2E("%s: malloc failed: %s\n", b_command, strerror(errno));
			return 2;
		}
		strcpy(cd_path, homedir);
	}
	else if (strcmp(b_parameters[1], "-") == 0) /* 'cd -', the same as cd $OLDPWD*/
	{
		char *oldpwd = getenv("OLDPWD");
		if (!oldpwd)
		{
			OUT2E("%s: %s: OLDPWD not set\n", argv0, b_command);
			return 2;
		}
		cd_path = malloc(P_CS * (strlen(oldpwd) + 1));
		if (cd_path == NULL)
		{
			OUT2E("%s: malloc failed: %s\n", b_command, strerror(errno));
			return 2;
		}
		puts(oldpwd);
		strcpy(cd_path, oldpwd);
	}
	else
	{
		cd_path = malloc(P_CS * (strlen(b_parameters[1]) + 1));
		if (cd_path == NULL)
		{
			OUT2E("%s: malloc failed: %s\n", b_command, strerror(errno));
			return 2;
		}
		strncpy(cd_path, b_parameters[1], strlen(b_parameters[1]));
	}

	create_new_pwd(&cd_path);

	if (pshchdir(cd_path) != 0)
		OUT2E("%s: %s: %s\n", b_command, strerror(errno), cd_path);
	else
	{
		pshsetenv("OLDPWD", getenv("PWD"), 1);
		pshsetenv("PWD", cd_path, 1);
	}
	free(cd_path);
	return 1;
}
