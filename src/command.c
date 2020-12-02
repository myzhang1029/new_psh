/*
    psh/command.c - psh commands and jobs
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

#include <string.h>

#include "backend.h"
#include "command.h"
#include "libpsh/xmalloc.h"
#include "psh.h"

/* Add one background job, keeping the linked-list sorted by id. */
void psh_job_add_bg(psh_state *state, struct _psh_job *job)
{
    int last = 0;
    struct _psh_job *add_before = state->bg_jobs, *add_after;
    if (job->id)
    {
        /* job already has an id, probably `bg` was called. */
        while (add_before->id < job->id)
            add_after = add_before;
        add_after->next = job;
        job->next = add_before;
    }
    else
    {
        /* Allocate a new minimal id. */
        while (add_before->id - last == 1)
        {
            last = add_before->id;
            add_after = add_before;
            add_before = add_before->next;
        }
        job->id = add_before->id - 1;
        add_after->next = job;
        job->next = add_before;
    }
}

void free_redirect(struct _psh_redirect *redir)
{
    struct _psh_redirect *temp;
    while (redir != NULL)
    {
        temp = redir;
        redir = redir->next;
        xfree(temp);
        temp = NULL;
    }
}

/* Malloc a command, enNULL all elements, malloc the first element in argv[] and
 * a struct _psh_redirect */
struct _psh_command *new_command()
{
    /* TODO: Remove MAXARG, MAXEACHARG */
    struct _psh_command *cmd;
    cmd = xcalloc(1, sizeof(struct _psh_command));
    /* Setting to '\0' will be used to detect
                           whether an element is used */
    cmd->argv = xcalloc(MAXARG, P_CS);
    cmd->argv[0] = xcalloc(MAXEACHARG, P_CS);
    cmd->rlist = xcalloc(1, sizeof(struct _psh_redirect));
    return cmd;
}

/** Free the argv field of a command struct.
 *
 * @param command Pointer to the redirect struct.
 */
static void free_argv(struct _psh_command *cmd)
{
    int count;
    for (count = 0; count < MAXARG; ++count)
    {
        if (cmd->argv[count] == NULL)
            break; /* All args should be freed after here */
        xfree(cmd->argv[count]);
        cmd->argv[count] = NULL;
    }
    xfree(cmd->argv);
    cmd->argv = NULL;
}

/* Free a command and its nexts */
void free_command(struct _psh_command *cmd)
{
    struct _psh_command *temp;
    while (cmd != NULL)
    {
        temp = cmd;
        cmd = cmd->next;
        free_argv(temp);
        free_redirect(temp->rlist);
        xfree(temp);
        temp = NULL;
    }
}

/* Free the jobs table and all linked jobs */
void psh_jobs_free(psh_state *state, int hup)
{
    struct _psh_job *cur = state->bg_jobs, *tmp;
    while (cur)
    {
        if (hup)
            psh_backend_hup(cur->commands);
        free_command(cur->commands);
        tmp = cur->next;
        xfree(cur);
        cur = tmp;
    }
    if (state->fg_job)
    {
        if (hup)
            psh_backend_hup(state->fg_job->commands);
        free_command(state->fg_job->commands);
    }
}