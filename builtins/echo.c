/*
   echo.c - builtin echo

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

int builtin_echo(ARGS)
{
	if (b_parameters[1] == NULL)
	{
		/* A blank line */
		printf("\n");
		return 1;
	}
	else if (b_parameters[1][0] == '-')
	{
		switch (b_parameters[1][1])
		{
			case 0:
				/* Another blank line */
				puts("");
				return 1;
			case 'n':
				if (b_parameters[2] == NULL)
					/* No more blank line */
					return 1;
				else
				{
					int cnt = 2;
					printf("%s", b_parameters[cnt]);
					while (b_parameters[++cnt] != NULL)
					{
						printf(" %s", b_parameters[cnt]);
					}
					return 1;
				}
			default:
			{
				int cnt = 1;
				printf("%s", b_parameters[cnt]);
				while (b_parameters[++cnt] != NULL)
				{
					printf(" %s", b_parameters[cnt]);
					cnt++;
				}
				puts("");
				return 1;
			}
		} /* switch-case */
	}
	else
	{
		int cnt = 1;
		printf("%s", b_parameters[cnt]);
		while (b_parameters[++cnt] != NULL)
		{
			printf(" %s", b_parameters[cnt]);
			cnt++;
		}
		puts("");
		return 1;
	}
}
