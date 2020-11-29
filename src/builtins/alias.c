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

#include "builtin.h"
#include "libpsh/util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char ***aliases;

static int alias_amount = 0;

int add_alias(char *alias, char *value)
{

    aliases = malloc(sizeof(char *));
    aliases[alias_amount] = calloc(2, sizeof(char *));
    aliases[alias_amount][0] = malloc(strlen(alias));
    aliases[alias_amount][1] = malloc(strlen(value));
    strcpy(aliases[alias_amount][0], alias);
    strcpy(aliases[alias_amount][1], value);

    puts(aliases[alias_amount][0]);
    puts(aliases[alias_amount][1]);

    alias_amount++;

    return 0;
}

int builtin_alias(int argc, char **argv, psh_state *state)
{

    int i;
    char *alias, *value, *argstr;

    argstr = malloc(strlen(argv[1]));
    strcpy(argstr, argv[1]);

    for(i = 2; i < argc; i++)
    {
        if (realloc(argstr, strlen(argstr) + strlen(argv[i]) + 1))
        {
            strcat(argstr, argv[i]);
            strcat(argstr, " ");
        } else
        {
            OUT2E("Not enough memory");
        }
    }

    for(i = 0; i < strlen(argstr); i++)
    {
        if (argstr[i] == '=')
        {
            alias = calloc(i, sizeof(char));
            memcpy(alias, argstr, i);
            value = calloc(strlen(argstr) - i, sizeof(char));
            strcpy(value, argstr + i + 1);
            break;
        }

    }

    puts(alias);
    puts(value);
    exit(0);

}
