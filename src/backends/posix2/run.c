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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "backend.h"
#include "builtin.h"
#include "libpsh/hash.h"
#include "libpsh/path_searcher.h"
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"
#include "psh.h"
#include "util.h"
#include "variable.h"

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

/* Error handling OUTSIDE child process */
#define DO_THIS_OR_FAIL_MAIN(cmd, why, val)                                    \
    do                                                                         \
    {                                                                          \
        if ((cmd))                                                             \
        {                                                                      \
            OUT2E("%s: " why ": %s\n", state->argv0, strerror(errno));         \
            return (val);                                                      \
        }                                                                      \
    } while (0)

/** Type of a file-descriptor backup. */
typedef int **fd_backup;

/** Set up redirections and optionally backup file descriptors.
 *
 * For the backup, originally closed fds are saved as -1, which is what dup
 * returns on EBADF, so we don't need to explicitly backup closed fds.
 *
 * @param state Psh internal state
 * @param redirect Redirections
 * @param if_backup Whether to back up redirected file descriptors.
 * @param backup If @ref if_backup is true, a list of int[2]s with the file
 * descriptors that were backed up is allocated and returned, in which [0]
 * is the original and [1] is the replaced. In this case, @ref backup should be
 * free()d. If @ref if_backup is 0, This argument is ignored.
 * @return 0 on success, 1 on error, but *@ref backup should still be free()d.
 */
static int set_up_redirection(psh_state *state, struct _psh_redirect *redirect,
                              int if_backup, fd_backup *backup)
{
    size_t have_size = 10, used_size = 0;
    /* Backup file descriptors for builtin commands */
    if (if_backup)
        *backup = xmalloc(sizeof(int) * 2 * have_size);
#define BACKUP_FD(fd)                                                          \
    do                                                                         \
    {                                                                          \
        if (!if_backup)                                                        \
            break;                                                             \
        (*backup)[used_size][0] = (fd);                                        \
        (*backup)[used_size][1] = dup((fd));                                   \
        if ((*backup)[used_size][1] < 0 && errno != EBADF)                     \
            /* Backup failed */                                                \
            return 2;                                                          \
        if (++used_size >= have_size)                                          \
            *backup = xrealloc(*backup, sizeof(int) * 2 * (have_size *= 2));   \
    } while (0)

    while (redirect)
    {
        switch (redirect->type)
        {
            case PSH_REDIR_NONE:
                /* This case currently exists, but should not. #4 TODO */
                break;
            case PSH_REDIR_OUT_REDIR:
            {
                int file_fd;
#ifdef DEBUG
                printf("output(%d, %s)\n", redirect->lhs.fd,
                       redirect->rhs.file);
#endif
                /* if piping and redirecting output, bash actually respects
                 * the redirect instead of the pipe, but I dislike that.
                 * However, here the pipe's fd is actually overriden. */
                /* Open output file */
                file_fd = open(redirect->rhs.file, O_WRONLY | O_CREAT | O_TRUNC,
                               0644);
                DO_THIS_OR_FAIL_MAIN((file_fd < 0), "open", 1);
                BACKUP_FD(redirect->lhs.fd);
                close(redirect->lhs.fd);
                DO_THIS_OR_FAIL_MAIN((dup2(file_fd, redirect->lhs.fd) < 0),
                                     "dup2", 1);
                close(file_fd);
                break;
            }
            case PSH_REDIR_OUT_APPN:
            {
                int file_fd;
#ifdef DEBUG
                printf("append(%d, %s)\n", redirect->lhs.fd,
                       redirect->rhs.file);
#endif
                file_fd = open(redirect->rhs.file,
                               O_WRONLY | O_CREAT | O_APPEND, 0644);
                DO_THIS_OR_FAIL_MAIN((file_fd < 0), "open", 1);
                BACKUP_FD(redirect->lhs.fd);
                close(redirect->lhs.fd);
                DO_THIS_OR_FAIL_MAIN((dup2(file_fd, redirect->lhs.fd) < 0),
                                     "dup2", 1);
                close(file_fd);
                break;
            }
            case PSH_REDIR_IN_REDIR:
            {
                int file_fd;
#ifdef DEBUG
                printf("input(%d, %s)\n", redirect->lhs.fd, redirect->rhs.file);
#endif
                file_fd = open(redirect->rhs.file, O_RDONLY, 0644);
                DO_THIS_OR_FAIL_MAIN((file_fd < 0), "open", 1);
                BACKUP_FD(redirect->lhs.fd);
                close(redirect->lhs.fd);
                DO_THIS_OR_FAIL_MAIN((dup2(file_fd, redirect->lhs.fd) < 0),
                                     "dup2", 1);
                close(file_fd);
                break;
            }
            case PSH_REDIR_FD2FD:
#ifdef DEBUG
                printf("fd_dup(%d, %d)\n", redirect->lhs.fd, redirect->rhs.fd);
#endif
                BACKUP_FD(redirect->lhs.fd);
                close(redirect->lhs.fd);
                DO_THIS_OR_FAIL_MAIN(
                    (dup2(redirect->rhs.fd, redirect->lhs.fd) < 0), "dup2", 1);
                break;
            case PSH_REDIR_CLOSEFD:
#ifdef DEBUG
                printf("close(%d, %d)\n", redirect->lhs.fd, redirect->rhs.fd);
#endif
                BACKUP_FD(redirect->lhs.fd);
                close(redirect->lhs.fd);
                break;
            case PSH_REDIR_OPENFN:
            {
                int file_fd;
#ifdef DEBUG
                printf("open(%d, %s)\n", redirect->lhs.fd, redirect->rhs.file);
#endif
                file_fd = open(redirect->rhs.file, O_RDWR | O_CREAT, 0644);
                DO_THIS_OR_FAIL_MAIN((file_fd < 0), "open", 1);
                BACKUP_FD(redirect->lhs.fd);
                close(redirect->lhs.fd);
                DO_THIS_OR_FAIL_MAIN((dup2(file_fd, redirect->lhs.fd) < 0),
                                     "dup2", 1);
                close(file_fd);
                break;
            }
            case PSH_REDIR_HEREXX:
            {
                int file_fd;
#ifdef DEBUG
                printf("herexx(%d, %p)\n", redirect->lhs.fd,
                       redirect->rhs.herexx);
#endif
                file_fd = open(redirect->rhs.file, O_RDONLY, 0644);
                DO_THIS_OR_FAIL_MAIN((file_fd < 0), "open", 1);
                BACKUP_FD(redirect->lhs.fd);
                close(redirect->lhs.fd);
                DO_THIS_OR_FAIL_MAIN((dup2(file_fd, redirect->lhs.fd) < 0),
                                     "dup2", 1);
                close(file_fd);
                break;
            }
            default:
                psh_code_fault(state, __FILE__, __LINE__);
        }
        redirect = redirect->next;
    }
    if (if_backup)
        (*backup)[used_size] = NULL;
    return 0;
}

/** Restore file descriptors that were backed up */
static int restore_fds(psh_state *state, fd_backup restore_spec)
{
    while (*restore_spec)
    {
        /* Close the original, restore_spec[0][0] */
        close(**restore_spec);
        /* -1 indicates that the fd was originally closed. */
        if ((*restore_spec)[1] >= 0)
        {
            /* Replace the original with the restored */
            DO_THIS_OR_FAIL_MAIN((dup2((*restore_spec)[1], **restore_spec) < 0),
                                 "dup2", 1);
            /* Close the restored */
            close((*restore_spec)[1]);
        }
        /* Get the next one */
        ++restore_spec;
    }
    return 0;
}

/** Execute one command.
 *
 * @param state Psh internal state.
 * @param cmd Command.
 * @param pipe_in Pipe for stdin.
 * @param pipe_out Pipe for stdout.
 * @param pipe_close1 The other end of the pipe to be closed.
 * @param pipe_close2 The other end of the pipe to be closed.
 * @param builtin A builtin function if this command is a builtin.
 * @param cmd_realpath Full path to the command.
 * @note If both @ref builtin and @ref cmd_realpath are NULL, nothing will be
 * executed, but a process will still be created.
 * @return The PID of the forked process.
 */
static pid_t execute_single_cmd(psh_state *state, struct _psh_command *cmd,
                                int pipe_in, int pipe_out, int pipe_close1,
                                int pipe_close2, builtin_function builtin,
                                char *cmd_realpath)
{
    pid_t pid = fork();
    DO_THIS_OR_FAIL_MAIN((pid < 0), "fork", -1);
    if (pid > 0)
    {
/* Parent process */
#ifdef DEBUG
        printf("Parent received pid: %d for %s\n", pid, cmd->argv[0]);
#endif
        return pid;
    }
    if (pid == 0)
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
#ifdef DEBUG
            printf("%s = dup2(%d, %d)\n", cmd->argv[0], pipe_in, STDIN_FILENO);
#endif
            DO_THIS_OR_FAIL((dup2(pipe_in, STDIN_FILENO) < 0), "dup2");
            close(pipe_in);
        }
        if (pipe_out)
        {
            close(STDOUT_FILENO);
#ifdef DEBUG
            printf("%s = dup2(%d, %d)\n", cmd->argv[0], pipe_out,
                   STDOUT_FILENO);
#endif
            DO_THIS_OR_FAIL((dup2(pipe_out, STDOUT_FILENO) < 0), "dup2");
            close(pipe_out);
        }
        /* Close the other end of the pipes */
        if (pipe_close1)
            close(pipe_close1);
        if (pipe_close2)
            close(pipe_close2);
        /* Process other redirections */
        if (set_up_redirection(state, redirect, 0, NULL))
            _Exit(1);
        /* Run the command */
        if (builtin)
            /* Run a builtin */
            _Exit((*builtin)(get_argc(cmd->argv), cmd->argv, state));
        if (cmd_realpath)
        {
            /* An on-disk command */
            execv(cmd_realpath, cmd->argv);
            OUT2E("%s: %s: %s\n", state->argv0, cmd_realpath, strerror(errno));
            _Exit(127);
        }
        _Exit(127);
    }
    /* Control shouldn't reach here */
    psh_code_fault(state, __FILE__, __LINE__);
}

/** Execute a single job.
 *
 * @param state Psh internal state.
 * @param job The job structure.
 * @return 0 on success.
 */
static int execute_one_job(psh_state *state, struct _psh_job *job)
{
    int error_level = 0;
    int i = 0;
    int last_pipe_fd[2] = {0};
    int should_be_run = 1;
    struct _psh_command *cmd = job->commands;

    job->status = PSH_JOB_RUNNING;
    if (job->type == PSH_JOB_BACKGROUND)
    {
        /* Fork a subshell */
        pid_t pid = fork();
        DO_THIS_OR_FAIL_MAIN((pid < 0), "fork", -1);
        if (pid > 0)
        {
            job->controller = pid;
            return 0;
        }
        /* Leaving the child process executing the rest. */
    }

    /* Go over each command and execute them. */
    while (++i, cmd)
    {
        int pipe_fd[2] = {0};
        pid_t pid;
        int status;
        builtin_function builtin;
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
        if (builtin && cmd->type != PSH_CMD_PIPED)
        {
            /* Execute without forking if async execution is not needed */
            fd_backup backed_up;
            /* Builtin commands can be redirected too */
            if (set_up_redirection(state, cmd->rlist, 1, &backed_up))
            {
                /* Even if set_up_redirection failed, this must still be free()d
                 */
                xfree(backed_up);
                ++error_level;
                goto cont;
            }
            /* Run the builtin */
            psh_vf_get(state, "?", 0, 0)->payload.integer =
                (*builtin)(get_argc(cmd->argv), cmd->argv, state);
            /* Restore file descriptors as we are returning to shell */
            if (builtin != builtin_exec)
                restore_fds(state, backed_up);
            xfree(backed_up);
            goto cont;
        }
        if (cmd->type == PSH_CMD_PIPED)
        {
            /* Create pipe */
            if (pipe(pipe_fd) != 0)
            {
                OUT2E("%s: pipe: %s\n", state->argv0, strerror(errno));
                goto cont;
            }
#ifdef DEBUG
            printf("pipe(%d, %d)\n", pipe_fd[1], pipe_fd[0]);
#endif
        }
        if (builtin)
            pid =
                execute_single_cmd(state, cmd, last_pipe_fd[0], pipe_fd[1],
                                   pipe_fd[0], last_pipe_fd[1], builtin, NULL);
        else
        {
            char *cmd_realpath = psh_get_cmd_realpath(state, cmd->argv[0]);
            pid = execute_single_cmd(state, cmd, last_pipe_fd[0], pipe_fd[1],
                                     pipe_fd[0], last_pipe_fd[1], builtin,
                                     cmd_realpath);
        }
        /* Make sure the used fds of the pipe are closed in the parent process.
         */
        if (last_pipe_fd[0])
            close(last_pipe_fd[0]);
        if (pipe_fd[1])
            close(pipe_fd[1]);
        /* Backup the pipe created for this process and possibly the next
         * process */
        last_pipe_fd[0] = pipe_fd[0];
        last_pipe_fd[1] = pipe_fd[1];
        if (pid < 0)
            return 1;
        /* Store the pid. */
        cmd->pid = pid;
        switch (cmd->type)
        {
            case PSH_CMD_PIPED:
                break;
            case PSH_CMD_RUN_AND:
                waitpid(pid, &status, 0);
                cmd->wait_stat = status;
                psh_vf_get(state, "?", 0, 0)->payload.integer = status;
                should_be_run = pid == 0 ? 1 : 0;
                break;
            case PSH_CMD_RUN_OR:
                waitpid(pid, &status, 0);
                cmd->wait_stat = status;
                psh_vf_get(state, "?", 0, 0)->payload.integer = status;
                should_be_run = pid == 0 ? 0 : 1;
                break;
            case PSH_CMD_SINGLE:
                waitpid(pid, &status, 0);
                cmd->wait_stat = status;
                psh_vf_get(state, "?", 0, 0)->payload.integer = status;
                should_be_run = 1;
                break;
            default:
                /* commands shouldn't have other types */
                psh_code_fault(state, __FILE__, __LINE__);
        }
    cont:
        cmd = cmd->next;
    }
    return 0;
}

int psh_backend_do_run(psh_state *state, struct _psh_job *jobs)
{
    int i;
    while (jobs)
    {
        if (jobs->type == PSH_JOB_BACKGROUND)
            psh_job_add_bg(state, jobs);
        else
        {
            if (state->fg_job)
                /* To be clear, this field must be cleared. */
                psh_code_fault(state, __FILE__, __LINE__);
            state->fg_job = jobs;
        }
        if (execute_one_job(state, jobs))
        {
            OUT2E("%s: job execution failed, further execution skipped.\n",
                  state->argv0);
            return 1;
        }
        if (jobs->type == PSH_JOB_FOREGROUND)
        {
            free_command(state->fg_job->commands);
            state->fg_job->commands = NULL;
        }
        jobs = jobs->next;
    }
    return 0;
}
