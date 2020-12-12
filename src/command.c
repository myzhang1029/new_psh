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
#include "util.h"

/* Add one background job, keeping the linked-list sorted by id. */
void psh_job_add_bg(psh_state *state, struct _psh_job *job)
{
    int last = 0;
    struct _psh_job *add_before = state->bg_jobs, *add_after = NULL;
    if (job->id)
    {
        /* job already has an id, probably `bg` was called. */
        /* Insert the job while keeping the list sorted */
        if (add_before->id == job->id)
            /* No duplicate ids should ever be introduced */
            psh_code_fault(state, __FILE__, __LINE__);
        if (!add_before || add_before->id > job->id)
        {
            /* Either an empty job queue or adding the first job, prepend the
             * job */
            state->bg_jobs = job;
        }
        else
        {
            while (add_before && add_before->id < job->id)
                /* If add_before is NULL, add_after is the last one with value
                 */
                add_after = add_before;
            if (!add_after)
                /* Logically, this line should never be encountered, but some
                 * static analyzers doesn't agree */
                psh_code_fault(state, __FILE__, __LINE__);
            add_after->next = job;
        }
    }
    else
    {
        /* No job id exists */
        if (!add_before || add_before->id > 1)
        {
            /* Adding the first job, prepend the job */
            state->bg_jobs = job;
            job->id = 1;
        }
        else
        {
            while (add_before && add_before->id - last == 1)
            {
                last = add_before->id;
                /* Here add_after should at least be valid since the last if
                 * handled empty add_before */
                add_after = add_before;
                add_before = add_before->next;
            }
            if (!add_after)
                /* Logically, this line should never be encountered, but some
                 * static analyzers doesn't agree */
                psh_code_fault(state, __FILE__, __LINE__);
            job->id = add_after->id + 1;
            add_after->next = job;
        }
    }
    /* No matter what add_before is */
    job->next = add_before;
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

/** Free the argv field of a command struct.
 *
 * @param command Pointer to the redirect struct.
 */
static void free_argv(struct _psh_command *cmd)
{
    int count;
    for (count = 0; cmd->argv[count]; ++count)
    {
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