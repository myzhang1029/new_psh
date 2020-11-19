/*
    psh/main.c - psh entry point
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

#include <stdio.h>
#include <string.h>

/* Some evil implementations include no stdio.h is history.h */
#ifdef HAVE_READLINE_HISTORY_H
#include <readline/history.h>
#endif

#include "backend.h"
#include "builtin.h"
#include "command.h"
#include "filpinfo.h"
#include "input.h"
#include "libpsh/hash.h"
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"
#include "prompts.h"
#include "psh.h"
#include "util.h"

extern int optopt;

int main(int argc, char **argv)
{
    builtin_function bltin;
    psh_state *internal_state;
    int stat;
    struct command *cmd = NULL;
    char *expanded_ps1, *buffer;
    char *ps1 =
        "\\[\\e[01;32m\\]\\u \\D{} " /* #8 TODO: $PS1 */
        "\\[\\e[01;34m\\]\\w\\[\\e[01;35m\\]\\012\\s-\\V\\[\\e[0m\\]\\$ ";
    int arg;

    /* Initiate the internal state */
    internal_state = xmalloc(sizeof(psh_state));
    memset(internal_state, 0, sizeof(psh_state));
    internal_state->command_table = psh_hash_create(32);
    /* TODO: Store this as shell arguments */
    internal_state->argv0 = psh_strdup(
        (strrchr(argv[0], '/') == NULL ? argv[0] : strrchr(argv[0], '/') + 1));
    if (psh_backend_prepare() != 0)
        exit_psh(internal_state, 1);

    /* Parse shell options */
    while ((arg = psh_backend_getopt(argc, argv, ":v")) != -1)
    {
        switch (arg)
        {
            /* Verbose flag */
            case 'v':
                internal_state->verbose = 1;
                break;
            case ':':
                OUT2E("%s: option requires an argument\n",
                      internal_state->argv0);
                break;
            case '?':
            default:
                OUT2E("%s: unknown option -%c\n", internal_state->argv0,
                      optopt);
                break;
        }
    }

#ifdef HAVE_WORKING_HISTORY
    using_history();
#endif
    while (1)
    {
        expanded_ps1 = ps_expander(internal_state, ps1);
        stat = read_cmdline(internal_state, expanded_ps1, &buffer);
        xfree(expanded_ps1);
        if (stat < 0)
            continue;
        cmd = new_command();
        stat = filpinfo(internal_state, buffer, cmd);
        xfree(buffer);
        if (stat < 0)
        {
            free_command(cmd);
            continue;
        }

        /* Temporary work-around. #2 #5 #9 TODO, invoke bltin in
         * psh_backend_do_run() */

        bltin = find_builtin(cmd->argv[0]);
        if (bltin)
        {
            internal_state->last_command_status =
                (*bltin)(get_argc(cmd->argv), cmd->argv, internal_state);
        }
        else
        {
            psh_backend_do_run(cmd);
        }
        free_command(cmd);
    }
    return 0;
}
