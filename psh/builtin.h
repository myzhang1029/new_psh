/*
    psh/builtin.h - builtin definitions of the psh
    Copyright 2017-2020 Zhang Maiyun.

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

typedef int (*builtin_function)(int argc, char **argv);
struct builtin
{
    char *name;
    builtin_function proc;
};

int builtin_exec(int argc, char **argv);
int builtin_echo(int argc, char **argv);
int builtin_exit(int argc, char **argv);
int builtin_true(int argc, char **argv);
int builtin_false(int argc, char **argv);
int builtin_cd(int argc, char **argv);
int builtin_pwd(int argc, char **argv);
int builtin_history(int argc, char **argv);
int builtin_builtin(int argc, char **argv);

builtin_function find_builtin(char *name);
int get_argc(char **argv);

#endif
