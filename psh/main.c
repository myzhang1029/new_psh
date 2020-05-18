/*
   main.c - main file of the psh

   Copyright 2013 wuyve.
   Copyright 2017-present Zhang Maiyun.
*/

#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "backend.h"
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"
#include "pshell.h"

int last_command_status = 0;
jmp_buf reset_point;
char *argv0;

int main(int argc, char **argv)
{
    char *ps1 = "\\u@\\h:\\W\\12\\$ "; /* TODO: Actually get $PS1 after #8 */
    /* Gitpod style */
    // char *ps1 = "\\[\033[01;32m\\]\\u \\[\033[01;34m\\]\\w\\[\033[0m \\$ ";
    char *expanded_ps1 = NULL;
    char *buffer = NULL;
    struct command *info = NULL;
    int read_stat;
    argv0 = psh_strdup(
        (strrchr(argv[0], '/') == NULL ? argv[0] : strrchr(argv[0], '/') + 1));

    add_atexit_free(argv0);
    prepare();
#ifndef NO_HISTORY
    using_history();
#endif
    /* Recovered from a signal */
    if (setjmp(reset_point) == 1)
    {
        printf("\n");
        xfree(expanded_ps1);
        free_command(info);
    }
    while (1)
    {
        new_command(&info);
        expanded_ps1 = ps_expander(ps1);
        read_stat = read_cmdline(expanded_ps1, &buffer);
        xfree(expanded_ps1);
        if (read_stat < 0 /* evaluate first */ || filpinfo(buffer, info) < 0)
        {
            xfree(buffer);
            free_command(info);
            continue;
        }
        xfree(buffer);
        switch (run_builtin(info))
        {
            case 1:
                last_command_status = 0;
                break;
            case 2:
                last_command_status = 1;
                break;
            default:
                do_run(info);
                break;
        }
        free_command(info);
    }
    return 0;
}
