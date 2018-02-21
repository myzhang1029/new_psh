/*
   builtins.c - builtins caller file of the psh

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

#include "builtins/builtin.h"
#include "pshell.h"
#define cmdis(cmd) (strcmp(info->parameters[0], cmd) == 0)

extern int last_command_status;

int run_builtin(struct parse_info *info)
{
	if (cmdis("exit") || cmdis("quit"))
	{
		if (info->parameters[1] == NULL)
		{
			exit_psh(last_command_status);
		}
		else
		{
			int i = atoi(info->parameters[1]);
			exit_psh(i);
		}
	}
	else if (cmdis("getstat"))
		return printf("%d\n", last_command_status), 1;
	else if (cmdis("true") || cmdis(":"))
		return 1;
	else if (cmdis("false"))
		return 2;
	else if (cmdis("about"))
	{
		printf("psh is a not fully implemented shell in UNIX.\n");
		return 1;
	}
	else if (cmdis("cd") || cmdis("chdir"))
		return builtin_cd(info);
	else if (cmdis("echo"))
		return builtin_echo(info);
	else if (cmdis("exec"))
		return builtin_exec(info);
	else if (cmdis("pwd"))
		return builtin_pwd(info);
	else if (cmdis("history"))
		return builtin_history(info);
	else if (cmdis("builtin"))
		return builtin_builtin(info);
	else if (cmdis("export") || cmdis("alias"))
	{
		OUT2E("%s: %s: Not supported\n", argv0, info->parameters[0]);
		return 1;
	}
	return 0;
}
