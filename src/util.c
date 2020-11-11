/*
    psh/util.c - psh-specific utilities
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdint.h>
#include <stdlib.h>

#include "libpsh/util.h"
#include "psh.h"
#include "util.h"

extern char *argv0;

/* freeptrs[0] holds the current number of items */
void *freeptrs[16] = {NULL};

/* Store a pointer to be free()d when exit_psh is called */
int add_atexit_free(void *ptr)
{
    /* Current number of items */
    intptr_t cur_nitems = (intptr_t)freeptrs[0];
    if ((cur_nitems += 1) == 16)
        /* No more pointers can be held */
        /* now list index can only reach 15 */
        return -1;
    /* First run: cur_nitems == 1 */
    /* Store the new pointer */
    freeptrs[cur_nitems] = ptr;
    /* Update the count */
    freeptrs[0] = (void *)cur_nitems;
    return cur_nitems;
}

/* Some unexpected things happened */
__attribute__((noreturn)) void code_fault(char *file, int line)
{
    OUT2E("%s: Programming error at %s: %d\n", argv0, file, line);
    OUT2E("Shell version: %s", PSH_VERSION);
    OUT2E("Please create a GitHub Issue with above info\n");
    exit_psh(1);
}

/* Exit psh after cleaning up */
void exit_psh(int status)
{
    int count;
    for (count = 1; count < (intptr_t)freeptrs[0]; ++count)
        free(freeptrs[count]);
    exit(status);
}
