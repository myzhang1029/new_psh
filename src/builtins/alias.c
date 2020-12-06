/*
    psh/builtins/alias.c - builtin alias
    Copyright 2020 Manuel Bertele
    Copyright 2020 Zhang Maiyun

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtin.h"
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"

int builtin_alias(int argc, char **argv, psh_state *state)
{
    int i, return_value = 0;

    /* No arguments provided, print. */
    if (argc < 2 || argc == 2 && strstr(argv[1], "-p") == argv[1])
    {
        ITER_TABLE(state->alias_table, {
            printf("alias %s='%s'\n", this->key, (char *)this->value);
        });
    }

    /* Iterate over every argument */
    for (i = 1; i < argc; ++i)
    {
        char *equal = strchr(argv[i], '=');
        char *alias_name = argv[i];
        char *value;

        if (!equal || equal == argv[i] /* '=' is the first char */)
        {
            /* Print its value if not setting. */
            char *value = psh_hash_get(state->alias_table, argv[i]);
            if (!value)
            {
                OUT2E("%s: %s: %s: not found\n", state->argv0, argv[0],
                      argv[i]);
                return_value = 1;
            }
            else
                printf("alias %s='%s'\n", argv[i], value);
        }
        else
        {
            /* End `alias_name */
            *equal = '\0';
            value = psh_strdup(equal + 1);
            psh_hash_add_chk(state->alias_table, alias_name, value, 1);
        }
    }
    return return_value;
}

int builtin_unalias(int argc, char **argv, psh_state *state)
{
    int i;

    if (argc < 2)
    {
        OUT2E("%s: usage: unalias [-a] name [name ...]\n", argv[0]);
        return 2;
    }

    if (strcmp(argv[1], "-a") == 0)
    {
        /* Remove all aliases */
        size_t old_len = state->alias_table->len;
        psh_hash_free(state->alias_table);
        state->alias_table = psh_hash_create(old_len);
    }
    else
    {
        /* Remove given aliases */
        for (i = 1; i < argc; ++i)
            psh_hash_rm(state->alias_table, argv[i]);
    }
    return 0;
}

/* Recursively expand aliases and create a new line with the result. */
char *expand_alias(psh_state *state, const char *oldbuffer)
{
    size_t len_remainder = 0;
    char *first_word;
    char *newresult, *result = psh_strdup(oldbuffer);

    while (1) /* TODO: Break circular expansion */
    {
        size_t expansion_length, other_word_length;
        /* Find the first word in result */
        const char *end_word1 = strpbrk(result, " \t\r\n;|&)");
        char *lookup_result;
        if (end_word1)
        {
            /* Multi-word buffer */
            first_word = xmalloc((end_word1 - result + 1) * P_CS);
            psh_strncpy(first_word, result, end_word1 - result);
            lookup_result = psh_hash_get(state->alias_table, first_word);
            xfree(first_word);
            other_word_length = strlen(end_word1);
        }
        else
        {
            /* The whole buffer is a word */
            lookup_result = psh_hash_get(state->alias_table, result);
            other_word_length = 0;
        }
        if (!lookup_result)
            /* The first word in result is not an alias */
            break;
        expansion_length = strlen(lookup_result);
        newresult = xmalloc((expansion_length + other_word_length + 1) * P_CS);
        memcpy(newresult, lookup_result, expansion_length);
        psh_strncpy(newresult + expansion_length, end_word1, other_word_length);
        xfree(result);
        result = newresult;
    }
    return result;
}
