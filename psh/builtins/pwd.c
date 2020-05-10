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

#include <stdlib.h>
#include <string.h>

#include "backend.h"
#include "builtin.h"
#include "libpsh/util.h"

int builtin_pwd(ARGS)
{
    int flag = 0;
    char *path = NULL;
    /* Ignore any args after [1]
    unless it is started by '-' as in bash */
    {
        int argc;
        for (argc = 1; bltin_argv[argc] && bltin_argv[argc][0] == '-'; ++argc)
        {
            switch (bltin_argv[argc][1])
            {
                case 'P':
                    flag = 1;
                    break;
                case 'L':
                    break;
                default: /* Invalid option */
                    OUT2E("%s: %s: -%c: invalid option\n", argv0, bltin_argv[0],
                          bltin_argv[argc][1]);
                    return 2;
            }
        }
    }
    if (!flag) /* No -P */
    {
        char *wd = getenv("PWD");
        char *p;
        int use_logical = 1;

        if (!wd || wd[0] != '/')
            use_logical = 0;
        p = wd;
        while ((p = strstr(p, "/.")))
        {
            if (!p[2] || p[2] == '/' || (p[2] == '.' && (!p[3] || p[3] == '/')))
                use_logical = 0;
            p++;
        }
        if (use_logical)
        {
            puts(wd);
            return 1;
        }
        else
            path = pshgetcwd_dm();
    }
    else
        path = pshgetcwd_dm();

    puts(path);
    free(path);
    return 1;
}
