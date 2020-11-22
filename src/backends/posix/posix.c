/*
   backends/posix.c - backend of POSIX systems

   Copyright 2013 wuyve.
   Copyright 2017-present Zhang Maiyun.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "backend.h"
#include "command.h"
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"
#include "psh.h"

#define MAXPIDTABLE 1024
pid_t ChdPid, ChdPid2;
pid_t BPTable[MAXPIDTABLE] = {0};
int pipe_fd[2], in_fd, out_fd;

void sigchld_handler(__attribute__((unused)) int sig)
{
    pid_t pid;
    int i;
    for (i = 0; i < MAXPIDTABLE; i++)
        if (BPTable[i] != 0) /*only handler the background processes*/
        {
            pid = waitpid(BPTable[i], NULL, WNOHANG);
            if (pid > 0)
            {
                printf("[%d] %d done\n", i + 1, pid);
                BPTable[i] = 0; /*clear*/
            }
            else if (pid < 0)
            {
                if (errno != ECHILD)
                    OUT2E("%s: waitpid error: %s", "psh", strerror(errno));
            }
            /*else:do nothing.*/
            /*Not background processes has their waitpid() in
             * wshell.*/
        }
    return;
}

void sigintabrt_hadler(int sig) {}

int psh_backend_prepare(psh_state *state)
{
    int ret = 0;
    if (signal(SIGCHLD, sigchld_handler) == SIG_ERR)
        OUT2E("%s: signal error: %s", state->argv0, strerror(errno)), ret++;

    if (signal(SIGINT, sigintabrt_hadler) == SIG_ERR)
        OUT2E("%s: signal error: %s", state->argv0, strerror(errno)), ret++;

    if (signal(SIGQUIT, sigintabrt_hadler) == SIG_ERR)
        OUT2E("%s: signal error: %s", state->argv0, strerror(errno)), ret++;
    return ret;
}
int psh_backend_do_run(psh_state *state, struct _psh_command *arginfo)
{
    struct _psh_command *info = arginfo;

    if (state->verbose)
    {
        int i = 0;
        printf("--**--\nstub!\nflags won't be read\n");
        printf("info position: %p\n", (void *)arginfo);
        while (++i)
        {
            int j;
            printf("part %d:\n"
                   "command: %s\n"
                   "params:\n",
                   i, info->argv[0]);
            for (j = 0; info->argv[j]; ++j)
                printf("%s\n", info->argv[j]);
            printf("flag: %d\n", info->flag);
            printf("redir type: %d\n",
                   (info->rlist != NULL) ? info->rlist->type : 0);

            if (info->next == NULL)
                break;
            info = info->next;
        }
        printf("--*END*--\n");
    }
    if ((ChdPid = fork()) != 0) /*shell*/
    {
        waitpid(ChdPid, &state->last_command_status, 0); /*wait command1*/
    }
    else /*command1*/
    {
        if (execvp(info->argv[0], (char **)info->argv) == -1)
        {
            if (errno == ENOENT && !strchr(info->argv[0], '/'))
            {
                OUT2E("%s: %s: command not found\n", state->argv0,
                      info->argv[0]);
                _Exit(127);
            }
            else
            {
                OUT2E("%s: %s: %s\n", state->argv0, info->argv[0],
                      strerror(errno));
                /* Exit the failed command child process */
                _Exit(126);
            }
        }
    }
    return 0;
}
/* * *
 * Platform dependent builtins part
 * * *
 */
#include "builtin.h"

/* Builtin exec */
int builtin_exec(int argc, char **argv, psh_state *state)
{
    if (argc < 2)
        return 0; /* Do nothing */
    if (execv(argv[1], &argv[1]) == -1)
        OUT2E("exec: %s: %s\n", argv[1], strerror(errno));
    return 127;
}
