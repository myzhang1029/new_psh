/*
    psh/backends/posix2/run.c - process-related posix backend
    Copyright 2020 Zhang Maiyun

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "backend.h"
#include "builtin.h"
#include "libpsh/hash.h"
#include "libpsh/path_searcher.h"
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"
#include "psh.h"
#include "variable.h"

#define DEBUG

/* Error handling INSIDE child process */
#define DO_THIS_OR_FAIL(cmd, why)                                              \
    do                                                                         \
    {                                                                          \
        if ((cmd))                                                             \
        {                                                                      \
            OUT2E("%s: " why ": %s\n", state->argv0, strerror(errno));         \
            _Exit(1);                                                          \
        }                                                                      \
    } while (0)

/** Execute one command. */
static pid_t execute_single_cmd(psh_state *state, struct _psh_command *cmd,
                                int pipe_in, int pipe_out,
                                builtin_function builtin)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        OUT2E("%s: fork: %s\n", state->argv0, strerror(errno));
        return -1;
    }
    else if (pid > 0)
    {
        /* Parent process */
        return pid;
    }
    else if (pid == 0)
    {
        /* Child process */
        struct _psh_redirect *redirect = cmd->rlist;
/* Set up pipes. Bash always sets up pipes prior to processing
 * redirection */
#ifdef DEBUG
        printf("%s = pipe(%d, %d)\n", cmd->argv[0], pipe_in, pipe_out);
#endif
        if (pipe_in)
        {
            close(STDIN_FILENO);
            DO_THIS_OR_FAIL((dup2(pipe_in, STDIN_FILENO) < 0), "dup2");
            close(pipe_in);
        }
        if (pipe_out)
        {
            close(STDOUT_FILENO);
            DO_THIS_OR_FAIL((dup2(pipe_out, STDOUT_FILENO) < 0), "dup2");
            close(pipe_out);
        }
        /* Process other redirections */
        while (redirect)
        {
            switch (redirect->type)
            {
                case PSH_REDIR_OUT_REDIR:
                {
                    int file_fd;
#ifdef DEBUG
                    printf("%s = output(%d, %s)\n", cmd->argv[0],
                           redirect->in.fd, redirect->out.file);
#endif
                    /* if piping and redirecting output, bash actually respects
                     * the redirect instead of the pipe, but I dislike that.
                     * However, here the pipe's fd is actually overriden. */
                    /* Open output file */
                    file_fd = open(redirect->out.file,
                                   O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    DO_THIS_OR_FAIL((file_fd < 0), "open");
                    close(redirect->in.fd);
                    DO_THIS_OR_FAIL((dup2(file_fd, redirect->in.fd) < 0),
                                    "dup2");
                    close(file_fd);
                    break;
                }
                case PSH_REDIR_OUT_APPN:
                {
                    int file_fd;
#ifdef DEBUG
                    printf("%s = append(%d, %s)\n", cmd->argv[0],
                           redirect->in.fd, redirect->out.file);
#endif
                    file_fd = open(redirect->out.file,
                                   O_WRONLY | O_CREAT | O_APPEND, 0644);
                    DO_THIS_OR_FAIL((file_fd < 0), "open");
                    close(redirect->in.fd);
                    DO_THIS_OR_FAIL((dup2(file_fd, redirect->in.fd) < 0),
                                    "dup2");
                    close(file_fd);
                    break;
                }
            }
            redirect = redirect->next;
        }
        /* Run the command */
        if (builtin)
            /* Run a builtin */
            _Exit((*builtin)(get_argc(cmd->argv), cmd->argv, state));
        else if (strchr(cmd->argv[0], '/'))
        {
            /* Run a command with a path */
            if (execv(cmd->argv[0], cmd->argv))
            {
                OUT2E("%s: %s: %s\n", state->argv0, cmd->argv[0],
                      strerror(errno));
                _Exit(127);
            }
        }
        else
        {
            /* Search PATH and run command */
            char *exec_path;
            /* Try to find a cached command path. */
            exec_path = psh_hash_get(state->command_table, cmd->argv[0]);
            if (exec_path == NULL)
            {
                char *name = xmalloc(strlen(cmd->argv[0]) + 2);
                name[0] = '/';
                psh_strncpy(name + 1, cmd->argv[0], strlen(cmd->argv[0]));
                exec_path = psh_search_path(psh_vf_getstr(state, "PATH"),
                                            psh_backend_path_separator, name,
                                            &psh_backend_file_exists);
                xfree(name);
                if (exec_path == NULL)
                {
                    OUT2E("%s: %s: command not found\n", state->argv0,
                          cmd->argv[0]);
                    _Exit(127);
                }
                else
                    psh_hash_add(state->command_table, cmd->argv[0], exec_path,
                                 1);
            }
#ifdef DEBUG
            printf("Cached Path: %s\n", exec_path);
#endif
            if (execv(exec_path, cmd->argv))
            {
                OUT2E("%s: %s: %s\n", state->argv0, cmd->argv[0],
                      strerror(errno));
                _Exit(127);
            }
        }
    }
}

int psh_backend_do_run(psh_state *state, struct _psh_command *cmd)
{
    int i = 0;
    int last_cmd_pipe_output = 0;
    builtin_function builtin;
#ifdef DEBUG
    printf("command position: %p\n", cmd);
#endif
    while (++i, cmd)
    {
        int pipe_fd[2] = {0};
        int status;
        pid_t pid;
#ifdef DEBUG
        printf("part %d:\n"
               "command: %s\n",
               i, cmd->argv[0]);
        int j;
        for (j = 0; cmd->argv[j]; ++j)
            printf("argv[%d] = %s\n", j, cmd->argv[j]);
        printf("flag: %d\n", cmd->type);
#endif
        /* First try to find a builtin command TODO: functions */
        builtin = find_builtin(cmd->argv[0]);
        if (builtin && cmd->type != PSH_CMD_PIPED &&
            cmd->type != PSH_CMD_BACKGROUND)
        {
            /* Execute without forking if async execution is not needed */
            psh_vf_get(state, "?", 0, 0)->payload.integer =
                (*builtin)(get_argc(cmd->argv), cmd->argv, state);
            cmd = cmd->next;
            continue;
        }
        if (cmd->type == PSH_CMD_PIPED)
        {
            /* Create pipe */
            if (pipe(pipe_fd) != 0)
            {
                OUT2E("%s: pipe: %s\n", state->argv0, strerror(errno));
                return 1;
            }
#ifdef DEBUG
            printf("pipe(%d, %d)\n", pipe_fd[0], pipe_fd[1]);
#endif
        }
        /* Receive pipe from the previous command */
        pid = execute_single_cmd(state, cmd, last_cmd_pipe_output, pipe_fd[0],
                                 builtin);
        last_cmd_pipe_output = pipe_fd[1];
        if (pid < 0)
            return 1;
        switch (cmd->type)
        {
            case PSH_CMD_BACKGROUND:
            case PSH_CMD_PIPED:
                /* Add to job control #9 TODO */
                break;
            case PSH_CMD_RUN_AND:
                waitpid(pid, &status, 0);
            case PSH_CMD_RUN_OR:
                waitpid(pid, &status, 0);
            case PSH_CMD_SINGLE:
            case PSH_CMD_MULTICMD:
                waitpid(pid, &psh_vf_get(state, "?", 0, 0)->payload.integer, 0);
        }
        cmd = cmd->next;
    }
    return 0;
}
