/*
    psh/jobs.h - psh jobs
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

#include "jobs.h"
#include "backend.h"
#include "command.h"
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"
#include "psh.h"

/* Add one job */
int psh_jobs_add(psh_state *state, char *cmd, int pid, enum _psh_cmd_type type)
{
    struct _psh_jobs *job = xcalloc(1, sizeof(struct _psh_jobs));
    if (!state->jobs)
        state->jobs = job;
    else
    {
        struct _psh_jobs *where_to_add = state->jobs;
        while (where_to_add->next)
            where_to_add = where_to_add->next;
        where_to_add->next = job;
    }
    job->name = psh_strdup(cmd);
    job->notified = 1;
    job->pid = pid;
    job->type = type;
    return 0;
}

void psh_jobs_free(psh_state *state, int sighup)
{
    struct _psh_jobs *cur = state->jobs, *tmp;
    while (cur)
    {
        xfree(cur->name);
        if (sighup)
            psh_backend_hup(cur->pid);
        tmp = cur->next;
        xfree(cur);
        cur = tmp;
    }
}