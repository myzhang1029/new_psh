/*
   pwd.c - builtin pwd

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

#include "backends/backend.h"
#include "builtin.h"

int builtin_pwd(ARGS)
{
	int flag = 0;
	char *path = NULL;
	/* Ignore any args after [1]
	unless it is started by '-' as in bash */
	{
		int argc;
		for (argc = 1; b_parameters[argc] && b_parameters[argc][0] == '-'; ++argc)
		{
			switch (b_parameters[argc][1])
			{
				case 'P':
					flag = 1;
					break;
				case 'L':
					break;
				default: /* Invalid option */
					OUT2E("%s: %s: -%c: invalid option\n", argv0, b_command, b_parameters[argc][1]);
					return 2;
			}
		}
	}
	if (!flag) /* No -P */
	{
		char *wd = getenv("PWD");
		char *p;
		int use_logical = 1;

		if (!wd || wd[0] != '/')
			use_logical = 0;
		p = wd;
		while ((p = strstr(p, "/.")))
		{
			if (!p[2] || p[2] == '/' || (p[2] == '.' && (!p[3] || p[3] == '/')))
				use_logical = 0;
			p++;
		}
		if (use_logical)
			path = strdup(wd);
		else
			path = pshgetcwd();
	}
	else
		path = pshgetcwd();

	puts(path);
	free(path);
	return 1;
}
