/*
   input.c - input reader

   Copyright 2013 wuyue.
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

/*return value: number of parameters
  0 represents only command without any parameters
  -1 represents wrong input*/

int read_command(char **command,char **parameters,char *prompt)
{
	char *buffer=malloc(sizeof(char)*MAXLINE);
	memset(buffer, 0, sizeof(char)*MAXLINE);
#ifndef NO_READLINE
	buffer = readline(prompt);
#else
	printf(prompt);
	fgets(buffer, MAXLINE, stdin);
#endif
	if(feof(stdin))
	{
		printf("\n");
		exit(0);
	}
	
	buffer=preprocess_cmdline(buffer);
#ifndef NO_HISTORY
	if(buffer && *buffer)
	{
		char *expans;
		int res;
		buffer=preprocess_cmdline(buffer);
		res=history_expand(buffer,&expans);
		if(res<0)
		{
			OUT2E("%s: Error on history expansion\n", argv0);
			free(expans);
			exit(1);
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
	int count=split_buffer(command, parameters, buffer);
#ifdef DEBUG
	/*input analysis*/
	printf("input analysis:\n");
	printf("pathname:[%s]\ncommand:[%s]\nparameters:\n",*command,parameters[0]);
	int i;
	for(i=0; i<count-1; i++)
		printf("[%s]\n",parameters[i]);
#endif
	free(buffer);
	return count;
}
