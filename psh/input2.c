/*
    psh/input2.c - new input handler
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

#include <stdio.h>
/* Some evil implementations include no stdio.h is history.h */
#ifndef NO_HISTORY
#include <readline/history.h>
#endif

#include "libpsh/util.h"
#include "libpsh/xmalloc.h"
#include "pshell.h"

extern int last_command_status;

/* read command line, shows PROMPT and result goes into *RESULT
 * *RESULT needs to be free()d
 * returns 0 if everything goes well;
 * and -1 if the cmd doesn't need to be run;
 * and -2 if anything went wrong, RESULT is untouched.
 */
int read_cmdline(char *prompt, char **result)
{
    char *buffer, *expanded;
    int stat;
    buffer = psh_gets(prompt);
#if 0
	/* This branch seems redundant, keep and see if things break */
    if (feof(stdin)) /* EOF reached */
    {
        puts("");
        exit_psh(last_command_status);
    }
#endif
    if (!buffer) /* EOF reached */
    {
        puts("");
        exit_psh(last_command_status);
    }
    if (*buffer == 0)
    {
        xfree(buffer);
        return -1;
    }
    *result = buffer;
#ifndef NO_HISTORY
    stat = history_expand(buffer, &expanded);
    if (stat < 0)
    {
        OUT2E("%s: Error on history expansion: %s\n", argv0, expanded);
        xfree(expanded);
        return -2;
    }
    if (stat == 1 || stat == 2)
        printf("%s\n", expanded);
    if (stat == 2)
    {
        /* cmd need not run */
        xfree(expanded);
        return -1;
    }
    xfree(buffer);
    add_history(expanded);
    *result = expanded;
#endif /* NO_HISTORY */
    return 0;
}