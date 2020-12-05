/*
   psh/builtins/hash.c - builtin command hash

   Copyright 2020 Zhang Maiyun.

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "backend.h"
#include "builtin.h"
#include "libpsh/hash.h"
#include "libpsh/path_searcher.h"
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"
#include "psh.h"
#include "variable.h"

#define DELETE 0x01
#define CLEAR 0x02
#define REUSABLE 0x04
#define SET 0x08
#define CORRESPOND 0x10
int builtin_hash(int argc, char **argv, psh_state *state)
{
    unsigned int flags = 0;
    int count = 0, return_value = 0;
    char *path;
    /* count always points to the next possible command name */
    while (++count < argc)
    {
        if (argv[count][0] != '-')
            break;
        switch (argv[count][1])
        {
            case '-':
                /* Skip this -- */
                ++count;
            case '\0':
                goto endwhile;
            case 'd':
                flags |= DELETE;
                break;
            case 'l':
                flags |= REUSABLE;
                break;
            case 'p':
                flags |= SET;
                if (++count == argc)
                {
                    OUT2E("%s: %s: -p: option requires an argument\n",
                          state->argv0, argv[0]);
                    return 1;
                }
                path = argv[count];
                break;
            case 'r':
                flags |= CLEAR;
                break;
            case 't':
                flags |= CORRESPOND;
                break;
            default:
                OUT2E("%s: %s: unrecognized argument -%c\n", state->argv0,
                      argv[0], argv[count][1]);
                return 1;
        }
    }
endwhile:
    if (flags & CLEAR)
    {
        /* I don't really want to optimize this */
        size_t size = state->command_table->len;
        psh_hash_free(state->command_table);
        state->command_table = psh_hash_create(size);
        return 0;
    }
    if (count == argc) /* Commands not present */
    {
        if (flags & CORRESPOND)
        {
            OUT2E("%s: %s: -t: option requires an argument\n", state->argv0,
                  argv[0]);
            return 1;
        }
        if (flags & REUSABLE)
            ITER_TABLE(state->command_table,
                       printf("builtin hash -p '%s' '%s'\n",
                              (char *)this->value, this->key););
        else
            ITER_TABLE(state->command_table,
                       printf("'%s' '%s'\n", this->key, (char *)this->value););
        return 0;
    }
    for (; count < argc; ++count)
    {
        if (flags & SET)
        {
            psh_hash_add_chk(state->command_table, argv[count], strdup(path),
                             1);
        }
        else if (flags & DELETE) /* If -d and -p are both supplied, it is set
                                    but not deleted */
        {
            if (psh_hash_rm(state->command_table, argv[count]) == 1)
            {
                OUT2E("%s: %s: %s: not found\n", state->argv0, argv[0],
                      argv[count]);
                return_value = 1;
            }
        }
        else if (flags & CORRESPOND)
        {
            char *path = psh_hash_get(state->command_table, argv[count]);
            if (path == NULL)
            {
                OUT2E("%s: %s: %s: not found\n", state->argv0, argv[0],
                      argv[count]);
                return_value = 1;
            }
            else
                printf("'%s' '%s'\n", argv[count], path);
        }
        else
        {
            char *name = xmalloc(strlen(argv[count]) + 2);
            char *path;
            name[0] = '/';
            psh_strncpy(name + 1, argv[count], strlen(argv[count]));
            path = psh_search_path(psh_vf_getstr(state, "PATH"),
                                   psh_backend_path_separator, name,
                                   &psh_backend_file_exists);
            xfree(name);
            if (path == NULL)
            {
                OUT2E("%s: %s: %s: not found\n", state->argv0, argv[0],
                      argv[count]);
                return_value = 1;
            }
            else
                psh_hash_add(state->command_table, argv[count], path, 1);
        }
    }
    return return_value;
}
