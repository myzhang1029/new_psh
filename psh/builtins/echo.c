/*
   echo.c - builtin echo

   Copyright 2013 wuyve.
   Copyright 2017 Zhang Maiyun.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
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
