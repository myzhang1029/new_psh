/*
    pwd.c - builtin pwd
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
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"
#include "variable.h"

int builtin_pwd(int argc, char **argv, psh_state *state)
{
    int flag = 0;
    char *path = NULL;
    /* Ignore any args after [1]
    unless it is started by '-' as in bash */
    {
        int arg_count;
        for (arg_count = 1; argv[arg_count] && argv[arg_count][0] == '-';
             ++arg_count)
        {
            switch (argv[arg_count][1])
            {
                case 'P':
                    flag = 1;
                    break;
                case 'L':
                    break;
                default: /* Invalid option */
                    OUT2E("%s: %s: -%c: invalid option\n", state->argv0,
                          argv[0], argv[arg_count][1]);
                    return 1;
            }
        }
    }
    if (!flag) /* No -P */
    {
        char *path = (char *)psh_vf_getstr(state, "PWD"), *p;

        if (!path || path[0] != '/')
            goto use_p;
        p = path;
        while ((p = strstr(p, "/.")))
        {
            if (!p[2] || p[2] == '/' || (p[2] == '.' && (!p[3] || p[3] == '/')))
                goto use_p;
            ++p;
        }
        puts(path);
        return 0;
    }
use_p:
    path = psh_backend_getcwd_dm();
    puts(path);
    xfree(path);
    return 0;
}
