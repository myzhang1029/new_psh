/*
    psh/builtins/echo.c - builtin echo
    Copyright 2017-2020 Zhang Maiyun.

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

int builtin_echo(int argc, char **argv)
{
    if (argv[1] == NULL)
    {
        /* A blank line */
        puts("");
        return 0;
    }
    else if (argv[1][0] == '-')
    {
        switch (argv[1][1])
        {
            case 0:
                /* Another blank line */
                puts("");
                return 0;
            case 'n':
                if (argv[2] == NULL)
                    /* No more blank line */
                    return 0;
                else
                {
                    int cnt = 2;
                    printf("%s", argv[cnt]);
                    while (argv[++cnt] != NULL)
                    {
                        printf(" %s", argv[cnt]);
                    }
                    return 0;
                }
            default: {
                int cnt = 1;
                printf("%s", argv[cnt]);
                while (argv[++cnt] != NULL)
                {
                    printf(" %s", argv[cnt]);
                    cnt++;
                }
                puts("");
                return 0;
            }
        } /* switch-case */
    }
    else
    {
        int cnt = 1;
        printf("%s", argv[cnt]);
        while (argv[++cnt] != NULL)
        {
            printf(" %s", argv[cnt]);
            cnt++;
        }
        puts("");
        return 0;
    }
}
