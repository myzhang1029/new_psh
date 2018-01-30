/*
   builtin.c - builtin command builtin

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

int builtin_builtin(ARGS)
{
	if(b_parameters[1]==NULL)/* No args */
		return 1;
	info->parameters++;/* Increase parameters */
	if(run_builtin(info)==0)
	{
		info->parameters--;
		OUT2E("%s: %s: %s: not a shell builtin\n", argv0, info->parameters[0], info->parameters[1]);
		return 2;
	}
	info->parameters--;
	return 1;
}

