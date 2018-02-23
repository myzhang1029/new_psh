
/*
   exit.c - logout and exit

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

void code_fault(char *file, int line)
{
	OUT2E("%s: Programming error at %s: %d\n", argv0, file, line);
	OUT2E("Shell version: %s", PSH_VERSION);
	OUT2E("Please create a GitHub Issue with above info\n");
	exit_psh(1);
}

void exit_psh(int status)
{
	free(argv0);
	exit(status);
}
