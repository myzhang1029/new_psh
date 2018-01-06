/*
   main.c - main file of the psh

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
#include "backends/backend.h"

pid_t BPTable[MAXPIDTABLE]= {0};
int status=0;
char *argv0;

void sigchld_handler(int sig)
{
	pid_t pid;
	int i;
	for(i=0; i<MAXPIDTABLE; i++)
		if(BPTable[i] != 0) /*only handler the background processes*/
		{
			pid = waitpid(BPTable[i],NULL,WNOHANG);
			if(pid > 0)
			{
				printf("[%d] %d done\n", i+1, pid);
				BPTable[i] = 0; /*clear*/
			}
			else if(pid < 0)
			{
				if(errno != ECHILD)
					OUT2E("%s: waitpid error: %s", argv0, strerror(errno));
			}
			/*else:do nothing.*/
			/*Not background processses has their waitpid() in wshell.*/
		}
	return;
}

void sigintabrt_hadler(int sig)
{
	status=sig;
	return;
}

void proc(void)
{
	char *command = NULL;
	int ParaNum;
	char prompt[MAX_PROMPT];
	struct parse_info info;
	char **parameters = malloc(sizeof(char *)*(MAXARG+2));
	if(parameters == NULL)
	{
		OUT2E("%s: malloc failed: %s\n", argv0, strerror(errno));
		return;
	}
	/*arg[0] is command
	  arg[MAXARG+1] is NULL*/

	if(signal(SIGCHLD,sigchld_handler) == SIG_ERR)
		OUT2E("%s: signal error: %s", argv0, strerror(errno));

	if(signal(SIGINT,sigintabrt_hadler) == SIG_ERR)
		OUT2E("%s: signal error: %s", argv0, strerror(errno));

	if(signal(SIGQUIT,sigintabrt_hadler) == SIG_ERR)
		OUT2E("%s: signal error: %s", argv0, strerror(errno));
	using_history();	
	while(1)
	{
		memset(parameters, 0, sizeof(char *)*(MAXARG+2));
		type_prompt(prompt);
		ParaNum = read_command(&command,parameters,prompt);
		if(ParaNum<0)
			continue;
		ParaNum--;/*count of units in buffer*/
		parsing(parameters,ParaNum,&info);
		switch(run_builtin(command,parameters))
		{
		case 1:
			break;
		case 2:
			status=1;
			break;
		default:
			do_run(command, parameters, info);
			break;
		}
	}
}

int main(int argc, char **argv)
{
	argv0=strdup((strrchr(argv[0], '/')==NULL?argv[0]:strrchr(argv[0], '/')+1));

	if(argv0==NULL)
	{
		OUT2E("psh: strdup: No memory\n");
		exit(1);
	}
	proc();
	return 0;
}
