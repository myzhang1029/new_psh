/*
   exec.c - builtin exec

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

int builtin_exec(ARGS)
{
	if(b_parameters[1]==NULL)
		return 1;/* Do nothing */
	if(execv(b_parameters[1], &b_parameters[1])==-1)
		OUT2E("exec: %s: %s\n", b_parameters[1], strerror(errno));
	return 2;
}
