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
#include "pshell.h"

int last_command_status = 0;
jmp_buf reset_point;
char *argv0;

int main(int argc, char **argv)
{
    char prompt[MAX_PROMPT];
    struct command *info;
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
        show_prompt();
        if (read_command(prompt, info) <= 0)
            continue;

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
