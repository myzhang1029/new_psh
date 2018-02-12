/*
   builtin.h - builtin definitions of the psh

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

#include "../pshell.h"

#define ARGS struct parse_info *info
#define b_command (info->parameters[0])
#define b_parameters (info->parameters)

int builtin_exec(ARGS);
int builtin_echo(ARGS);
int builtin_cd(ARGS);
int builtin_pwd(ARGS);
int builtin_history(ARGS);
int builtin_builtin(ARGS);

