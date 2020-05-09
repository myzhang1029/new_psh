/*
   main.c - main file of the psh

   Copyright 2013 wuyve.
   Copyright 2017-present Zhang Maiyun.
*/

#include <memory.h>
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
    char *ps1 = "\\u@\\h:\\w\\$ "; /* TODO: Actually get $PS1 after #8 */
    char *expanded_ps1;
    struct command *info;
    int read_stat;
    argv0 = psh_strdup(
        (strrchr(argv[0], '/') == NULL ? argv[0] : strrchr(argv[0], '/') + 1));

    if (argv0 == NULL)
    {
        OUT2E("psh: strdup: No memory\n");
        exit(1);
    }
    add_atexit_free(argv0);
    prepare();
#ifndef NO_HISTORY
    using_history();
#endif
    if (setjmp(reset_point) == 1)
        printf("\n");
    while (1)
    {
        if (new_command(&info) == -1)
        {
            OUT2E("%s: malloc failed\n", argv0);
            longjmp(reset_point, 1);
        }
        expanded_ps1 = ps_expander(ps1);
        read_stat = read_command(expanded_ps1, info);
        xfree(expanded_ps1);
        if (read_stat <= 0)
        {
            free_command(info);
            continue;
        }
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
