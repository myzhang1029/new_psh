/*
    psh/builtins/echo.c - builtin echo
    Copyright 2017 Zhang Maiyun.

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

#include "builtin.h"

int builtin_echo(ARGS)
{
    if (bltin_argv[1] == NULL)
    {
        /* A blank line */
        printf("\n");
        return 1;
    }
    else if (bltin_argv[1][0] == '-')
    {
        switch (bltin_argv[1][1])
        {
            case 0:
                /* Another blank line */
                puts("");
                return 1;
            case 'n':
                if (bltin_argv[2] == NULL)
                    /* No more blank line */
                    return 1;
                else
                {
                    int cnt = 2;
                    printf("%s", bltin_argv[cnt]);
                    while (bltin_argv[++cnt] != NULL)
                    {
                        printf(" %s", bltin_argv[cnt]);
                    }
                    return 1;
                }
            default: {
                int cnt = 1;
                printf("%s", bltin_argv[cnt]);
                while (bltin_argv[++cnt] != NULL)
                {
                    printf(" %s", bltin_argv[cnt]);
                    cnt++;
                }
                puts("");
                return 1;
            }
        } /* switch-case */
    }
    else
    {
        int cnt = 1;
        printf("%s", bltin_argv[cnt]);
        while (bltin_argv[++cnt] != NULL)
        {
            printf(" %s", bltin_argv[cnt]);
            cnt++;
        }
        puts("");
        return 1;
    }
}
