/*
   cd.c - builtin cd

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

#include "builtin.h"
#include "backends/backend.h"

int builtin_cd(ARGS)
{
	char *cd_path = NULL, *hdir=gethd();

	if(b_parameters[1] == NULL/* 'cd' */)
	{
		cd_path=malloc(strlen(hdir)+1);
		if(cd_path == NULL)
		{
			OUT2E("%s: malloc failed: %s\n", b_command, strerror(errno));
			return 2;
		}
		strcpy(cd_path, hdir);
	}
	else
	{
		cd_path = malloc(strlen(b_parameters[1]+1));
		if(cd_path == NULL)
		{
			OUT2E("%s: malloc failed: %s\n", b_command, strerror(errno));
			return 2;
		}
		strcpy(cd_path, b_parameters[1]);
	}
	if(chdir(cd_path)!= 0)
		OUT2E("%s: %s: %s\n", b_command, strerror(errno), cd_path);
	free(cd_path);
	return 1;
}
