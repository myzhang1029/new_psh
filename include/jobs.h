/** @file psh/jobs.h - @brief Psh jobs */
/*
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

#ifndef _PSH_JOBS_H
#define _PSH_JOBS_H

#include "command.h"
#include "psh.h"

/** @brief Job statuses. */
enum _psh_job_status
{
    PSH_JOB_RUNNING = 1,
    PSH_JOB_SIGNALED,
    PSH_JOB_STOPPED,
    PSH_JOB_DONE
};

/** @brief Psh background jobs. */
struct _psh_jobs
{
    /** Command argv[0]. */
    char *name;
    /** Pid of the command. */
    int pid;
    /** Command status. */
    enum _psh_job_status status;
    /** Job type. */
    enum _psh_cmd_type type;
    /** Status returned by wait(). */
    int wait_stat;
    /** Signal received. */
    int sig;
    /** Whether the status of this process has been notified. */
    int notified;
    /** Next ones. */
    struct _psh_jobs *next;
};

/** Add one job.
 *
 * @param state Psh internal state.
 * @param cmd Command name.
 * @param pid Job PID.
 * @param type Job type (pipe or background).
 * @return 0 on success.
 */
int psh_jobs_add(psh_state *state, char *cmd, int pid, enum _psh_cmd_type type);

/** Free jobs.
 *
 * @param state Psh internal state.
 * @param sighup Whether to send a hangup signal.
 */
void psh_jobs_free(psh_state *state, int sighup);
#endif
