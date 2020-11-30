/*
    psh/backends/posix2/run.c - process-related posix backend
    Copyright 2020 Manuel Bertele

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

#include "backend.h"
#include "builtin.h"
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int add_alias(psh_hash *table, char *alias, char *value)
{

    psh_hash_add_chk(table, alias, value, 1);

    return 0;
}

static char *check_for_alias(psh_hash *table, char *alias)
{
    return psh_hash_get(table, alias);

}



int builtin_alias(int argc, char **argv, psh_state *state)
{

    int i;
    char *alias, *value, *argstr;

    argstr = strdup(argv[1]);

    /* Combine all argv pointers to one pointer */
    for (i = 2; i < argc; i++)
    {
        xrealloc(argstr, strlen(argstr) + strlen(argv[i]) + 1);
        strcat(argstr, argv[i]);
        strcat(argstr, " ");
    }

    /* Split the string at '=' */
    for (i = 0; i < strlen(argstr); i++)
    {
        if (argstr[i] == '=')
        {
            alias = xcalloc(i, sizeof(char));
            memcpy(alias, argstr, i);
            value = strdup(argstr + i + 1);
            break;
        }

    }

    xfree(argstr);

    if (check_for_alias(state->alias_table, alias) == NULL)
    {
        add_alias(state->alias_table, alias, value);
    } else
    {
        OUT2E("Alias %s already exists with %s\n", alias, check_for_alias(state->alias_table, alias));
    }

    xfree(alias);
    xfree(value);

    return 0;

}


int builtin_unalias(int argc, char **argv, psh_state *state)
{

    puts("Work in progress");
}


int expand_alias(struct _psh_command *cmd)
{


}
