/*
   builtin.h - builtin definitions of the psh

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
#ifndef BUILTIN_INCLUDED
#define BUILTIN_INCLUDED
#include "pshell.h"

#define ARGS struct command *info
#define b_command (info->parameters[0])
#define b_parameters (info->parameters)

typedef int (*builtin_function)(ARGS);

int builtin_exec(ARGS);
int builtin_echo(ARGS);
int builtin_exit(ARGS);
int builtin_true(ARGS);
int builtin_false(ARGS);
int builtin_cd(ARGS);
int builtin_pwd(ARGS);
int builtin_history(ARGS);
int builtin_builtin(ARGS);
builtin_function find_builtin(char *name);

struct builtin
{
        char *name;
        builtin_function proc;
};

extern const struct builtin builtins[];

#endif
