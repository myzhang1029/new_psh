/*
    psh/backends/posix2/lifecycle.c - lifecycle-related posix backend
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

#include <errno.h>
#include <signal.h>
#include <string.h>

#include "backend.h"
#include "libpsh/util.h"
#include "psh.h"

volatile int last_sig;

static void signals_handler(int sig) { last_sig = sig; }
/* TODO */
int psh_backend_prepare(psh_state *state)
{
    if (state->interactive)
    {
        if (signal(SIGINT, signals_handler) == SIG_ERR)
        {
            OUT2E("%s: error setting signal handlers: %s\n", state->argv0,
                  strerror(errno));
            return 1;
        }
        if (signal(SIGTERM, signals_handler) == SIG_ERR)
        {
            OUT2E("%s: error setting signal handlers: %s\n", state->argv0,
                  strerror(errno));
            return 1;
        }
        if (signal(SIGQUIT, signals_handler) == SIG_ERR)
        {
            OUT2E("%s: error setting signal handlers: %s\n", state->argv0,
                  strerror(errno));
            return 1;
        }
    }
    if (signal(SIGCHLD, signals_handler) == SIG_ERR)
    {
        OUT2E("%s: error setting signal handlers: %s\n", state->argv0,
              strerror(errno));
        return 1;
    }
    return 0;
}