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

#include "args.h"
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
#include "variable.h"

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

    /* Initiate the internal state */
    internal_state = xmalloc(sizeof(psh_state));
    memset(internal_state, 0, sizeof(psh_state));
    psh_vfa_new_context(internal_state);
#ifdef DEBUG
    {
        union _psh_vfa_value payload = {1};
        psh_vf_set(internal_state, "p", PSH_VFA_INTEGER, payload, 0, 0, 0);
        const struct _psh_vfa_container *cnt =
            psh_vf_get(internal_state, "p", 0);
        printf("%d = 1\n", cnt->payload);
        psh_vf_unset(internal_state, "p", 0);
        cnt = psh_vf_get(internal_state, "p", 0);
        printf("0x%x = 0x4\n", cnt->attributes);
    }
#endif
    internal_state->command_table = psh_hash_create(32);
    /* TODO: Store this as shell arguments */
    internal_state->argv0 = psh_strdup(
        (strrchr(argv[0], '/') == NULL ? argv[0] : strrchr(argv[0], '/') + 1));

    parse_shell_args(internal_state, argc, argv);

    if (psh_backend_prepare() != 0)
        exit_psh(internal_state, 1);

#ifdef HAVE_WORKING_HISTORY
    using_history();
#endif
    while (1)
    {
        expanded_ps1 = ps_expander(internal_state, ps1);
        printf("%p\n", expanded_ps1);
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
