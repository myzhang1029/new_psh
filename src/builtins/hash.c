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

#include "builtin.h"
#include "libpsh/hash.h"
#include "libpsh/util.h"
#include "psh.h"

#define DELETE 0x01
#define CLEAR 0x02
#define REUSABLE 0x04
#define SET 0x08
int builtin_hash(int argc, char **argv, psh_state *state)
{
    int flags = 0;
    int count = 0;
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
                path = argv[++count];
                break;
            case 'r':
                flags |= CLEAR;
                break;
            case 't':
                /* This is the default, different from bash */
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
    /* TODO: Go over hash table. */
    return 0;
}
