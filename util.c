/*
   util.c - utilities for everyone

   Copyright 2018 Zhang Maiyun.

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

#include "pshell.h"

void *freeptrs[16] = {NULL};

int add_atexit_free(void *ptr)
{
	if ((freeptrs[0] = ((int)(freeptrs[0]) + 1)) == (void *)16)
		return -1;
	freeptrs[(int)(freeptrs[0])] = ptr;
	return (int)freeptrs[0];
}

char *p_fgets(char *prompt, FILE *fp)
{
	char *result;
	if (fp == NULL)
		return NULL;
#ifndef NO_READLINE
	if (fp == stdin)
	{
		result = readline(prompt);
		return result;
	}
#endif
	result = malloc(sizeof(char) * MAXLINE);
	if (result == NULL)
		return NULL;
	if (fp == stdin)
		printf("%s", prompt);
	{
		char *ptr = result;
		size_t charcount = 0, totalsize = MAXLINE;
		while (1)
		{
			*ptr = fgetc(fp);
			if (*ptr == EOF)
			{
				if (ptr == result) /* nothing read */
				{
					free(result);
					return NULL;
				}
				*ptr = 0;				      /* Terminate here */
				result = realloc(result, strlen(result) - 1); /* Resize the array to minimum */
				return result;
			}
			if (*ptr == '\n')
			{
				*ptr = 0;
				result = realloc(result, strlen(result) - 1); /* Resize the array to minimum */
				return result;
			}
			++ptr;
			if ((++charcount) == totalsize)
				if ((result = realloc(result, (totalsize = totalsize << 1))) ==
				    NULL) /* malloc more mem */
					return NULL;
		}
	}
	return result;
}

char *p_gets(char *prompt) { return p_fgets(prompt, stdin); }

__attribute__((noreturn)) void code_fault(char *file, int line)
{
	OUT2E("%s: Programming error at %s: %d\n", argv0, file, line);
	OUT2E("Shell version: %s", PSH_VERSION);
	OUT2E("Please create a GitHub Issue with above info\n");
	exit_psh(1);
}

void exit_psh(int status)
{
	int count;
	for (count = 1; count < (int)freeptrs[0]; ++count)
		free(freeptrs[count]);
	exit(status);
}

size_t p_strncpy(char *dst, const char *src, size_t size)
{
	register char *d = dst;
	register const char *s = src;
	register size_t n = ++size;

	if (n != 0 && --n != 0)
	{
		do
		{
			if ((*d++ = *s++) == 0)
				break;
		} while (--n != 0);
	}

	if (n == 0)
	{
		if (size != 0)
			*d = '\0';
		while (*s++)
			;
	}

	return (s - src - 1);
}
