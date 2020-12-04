/*
    psh/builtins/alias.c - process-related posix backend
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

char **list_of_aliases = NULL;
unsigned int amount_of_aliases = 0;

void add_alias(psh_hash *table, char *alias, char *value)
{
    psh_hash_add_chk(table, alias, value, 0);

    if (list_of_aliases == NULL)
    {
        list_of_aliases = malloc(sizeof(char *));
    }else
    {
        list_of_aliases = realloc(list_of_aliases, (amount_of_aliases + 1) * sizeof(char *));
    }
    list_of_aliases[amount_of_aliases] = alias;
    amount_of_aliases++;

}

char *check_for_alias(psh_hash *table, char *alias)
{
    return psh_hash_get(table, alias);
}



int builtin_alias(int argc, char **argv, psh_state *state)
{
    int i;

    if (argc < 2)
    {
        for (i = 0; i < amount_of_aliases; i++)
        {
            if (list_of_aliases[i] != NULL)
                printf("%s='%s'\n", list_of_aliases[i], check_for_alias(state->alias_table, list_of_aliases[i]));
        }

    }

    /* Iterate over every argument */
    for (i = 1; i < argc; i++)
    {
        if (strstr(argv[i], "=") == NULL)
        {
            OUT2E("Alias missing a value\n");
            return -1;
        }

        char *alias;
        char *value;

        /* Set alias pointer to beginning of argument */
        alias = argv[i];

        int j;
        /* Iterate over every char of argument */
        for (j = 0; j < strlen(argv[i]); j++)
        {
            /* Replace char = with \0 */
            if (argv[i][j] == '=')
            {
                /* Replace = with \0 to end alias pointer */
                argv[i][j] = '\0';
                /* Set value to char after \0 */
                value = &argv[i][j] + 1;
                /* values null terminator is the end of the argument */
                break;
            }

        }

        char *key = strdup(alias);
        char *val = strdup(value);


        /* If  alias is already set print error otherwise add it */
        if (check_for_alias(state->alias_table, alias) == NULL)
        {
            add_alias(state->alias_table, key, val);

        } else
        {
            OUT2E("The alias: %s already exists with %s\n", key, check_for_alias(state->alias_table, key));
        }

    }


    return 0;

}


int builtin_unalias(int argc, char **argv, psh_state *state)
{
    if (argc < 2)
    {
        OUT2E("Not enough arguments");
        return -1;
    }

    int i;
    for (i = 1; i < argc; i++)
    {
        /* Remove alias from hash table */
        psh_hash_rm(state->alias_table, argv[i]);

        int j;
        for (j = 0; j < amount_of_aliases; j++)
        {
            if (list_of_aliases[j] != NULL)
            {

                if (!strcmp(argv[i], list_of_aliases[j]))
                {
                    /* Remove alias from list_of_aliases */
                    free(list_of_aliases[j]);
                    list_of_aliases[j] = NULL;
                    break;
                }
            }
        }
    }
   
    return 0;
}


int expand_alias(struct _psh_command *cmd)
{


}
