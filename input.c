/*
   input.c - input reader

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

#include "pshell.h"
#include <setjmp.h>

extern jmp_buf reset_point;

/*return value: number of parameters
  0 represents only command without any parameters
  -1 represents wrong input*/

int read_command(char *prompt, struct parse_info *info)
{
	int count;
	char *buffer;
#ifndef NO_READLINE
	buffer = readline(prompt);
#else
	buffer=malloc(sizeof(char)*MAXLINE);
	memset(buffer, 0, sizeof(char)*MAXLINE);
	printf(prompt);
	fgets(buffer, MAXLINE, stdin);
#endif
	if(feof(stdin))
	{
		printf("\n");
		exit(0);
	}
#ifndef NO_HISTORY
	if(buffer && *buffer)
	{
		char *expans;
		int res;
		res=history_expand(buffer,&expans);
		if(res<0)
		{
			OUT2E("%s: Error on history expansion\n", argv0);
			free(expans);
			longjmp(reset_point, 1);
		}
		if(res==1||res==2)
			printf("%s\n",expans);
		if(res==2)
		{
			free(expans);
			return -2;
		}
		strncpy(buffer,expans,MAXLINE);
		free(expans);
		add_history(buffer);
	}
#endif
	if(buffer[0] == '\0')
		return -1;
	count=filpinfo(buffer, info);
	free(buffer);
	return count;
}
