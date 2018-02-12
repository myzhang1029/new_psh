/*
   show.c - Prompt generater

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
#include "backends/backend.h"

const int max_name_len = 256;
const int max_path_len = 1024;

void type_prompt(char *prompt)
{
	char hostname[max_name_len];
	char *pathname=pshgetcwd();
	int length;
	char *hdir=gethd(), *username=getun();

	if(pshgethostname(hostname,max_name_len)==0)
		sprintf(prompt,"%s@%s:",username,strtok(hostname,"."));
	else
		sprintf(prompt,"%s@unknown:",username);
#ifdef DEBUG
	printf("pathname: %s,length:%d\npw_dir:%s,length:%d\n",
	       pathname,strlen(pathname),pwd->pw_dir,strlen(pwd->pw_dir));
#endif
	length = strlen(prompt);
	if(strlen(pathname) < strlen(hdir) ||
	        strncmp(pathname,hdir,strlen(hdir))!=0)
		sprintf(prompt+length,"%s",pathname);
	else
		sprintf(prompt+length,"~%s",pathname+strlen(hdir));
	length = strlen(prompt);
	if(geteuid()==0)
		sprintf(prompt+length,"# ");
	else
		sprintf(prompt+length,"$ ");
	free(pathname);
	return;
}

