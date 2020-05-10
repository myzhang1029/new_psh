/*
    psh/builtins/builtin.h - builtin definitions of the psh
    Copyright 2017 Zhang Maiyun.

    This file is part of Psh, P shell.

    Psh is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Psh is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef BUILTIN_INCLUDED
#define BUILTIN_INCLUDED
#include "pshell.h"

#define ARGS struct command *info
#define bltin_argv (info->parameters)

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
int get_argc(char **argv);

struct builtin
{
    char *name;
    builtin_function proc;
};

extern const struct builtin builtins[];

#endif
