/*
   builtins.c - builtins caller file of the psh

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
#include "builtins/builtin.h"
#define cmdis(cmd) (strcmp(command,cmd) == 0)

int run_builtin(char *command, char **parameters)
{
	if(cmdis("exit") || cmdis("quit"))
	{
		if(parameters[1]==NULL)
		{
			free(parameters);
			exit(0);
		}
		else
		{
			int i=atoi(parameters[1]);
			free(parameters);
			exit(i);
		}
	}
	else if(cmdis("about"))
	{
		printf("psh is a not fully implemented shell in UNIX.\n");
		return 1;
	}
	else if(cmdis("cd")||cmdis("chdir"))
		return builtin_cd(command, parameters);
	else if(cmdis("echo"))
		return builtin_echo(command, parameters);
	else if(cmdis("exec"))
		return builtin_exec(command, parameters);
	else if(cmdis("pwd"))
		return builtin_pwd(command, parameters);
	else if(cmdis("history"))
		return builtin_history(command, parameters);
	else if(cmdis(":"))
		return 1; 
	else if(cmdis("export")||cmdis("alias"))
	{
		OUT2E("psh: %s: Not supported\n", command);
		return 1;
	}
	return 0;
}
