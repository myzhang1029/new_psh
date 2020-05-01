/*
    util.c - psh-specific utilities
    Copyright 2018-2020 Zhang Maiyun

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

#include <stdlib.h>

#include "pshell.h"
#include "libpsh/util.h"

void *freeptrs[16] = {NULL};

int add_atexit_free(void *ptr)
{
    if ((freeptrs[0] = ((int)(freeptrs[0]) + 1)) == (void *)16)
        return -1;
    freeptrs[(int)(freeptrs[0])] = ptr;
    return (int)freeptrs[0];
}

__attribute__((noreturn)) void code_fault(char *file, int line)
{
    OUT2E("%s: Programming error at %s: %d\n", argv0, file, line);
    OUT2E("Shell version: %s", PSH_VERSION);
    OUT2E("Please create a GitHub Issue with above info\n");
    exit_psh(1);
}

void exit_psh(int status)
{
    int count;
    for (count = 1; count < (int)freeptrs[0]; ++count)
        free(freeptrs[count]);
    exit(status);
}
