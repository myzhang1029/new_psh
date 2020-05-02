/*
   input.c - input reader

   Copyright 2013 wuyve.
   Copyright 2017 Zhang Maiyun.
*/

#include <setjmp.h>
#include <stdlib.h>

#include "libpsh/util.h"
#include "pshell.h"

extern jmp_buf reset_point;
extern int last_command_status;

/*return value: number of parameters
  0 represents only command without any parameters
  -1 represents wrong input*/

int read_command(char *prompt, struct command *info)
{
    int count;
    char *buffer;
    buffer = psh_gets(prompt);
    if (feof(stdin) || !buffer) /* EOF reached */
    {
        printf("\n");
        exit_psh(last_command_status);
    }
#ifndef NO_HISTORY
    if (buffer && *buffer)
    {
        char *expans;
        int res;
        res = history_expand(buffer, &expans);
        if (res < 0)
        {
            OUT2E("%s: Error on history expansion\n", argv0);
            free(expans);
            longjmp(reset_point, 1);
        }
        if (res == 1 || res == 2)
            printf("%s\n", expans);
        if (res == 2)
        {
            free(expans);
            return -2;
        }
#ifdef NO_READLINE
        psh_strncpy(buffer, expans, MAXLINE - 1);
#else
        psh_strncpy(buffer, expans, strlen(buffer));
#endif
        free(expans);
        add_history(buffer);
    }
#endif
    if (buffer == NULL || buffer[0] == '\0')
        return -1;
    count = filpinfo(buffer, info);
    return count;
}
