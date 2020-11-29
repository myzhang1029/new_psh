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

#include <stdlib.h>

static void load_shell_vars(psh_state *state)
{
    union _psh_vfa_value payload = {psh_backend_getcwd_dm()};
    psh_vf_set(state, "PWD", PSH_VFA_STRING, payload, 0, 0, 0);
    psh_vf_unset(state, "OLDPWD", 0);
    payload.integer = 0;
    psh_vf_set(state, "?", PSH_VFA_INTEGER, payload, 0, 0, 0);
    payload.string = psh_strdup(
        "\\[\\e[01;32m\\]\\u \\D{} "
        "\\[\\e[01;34m\\]\\w\\[\\e[01;35m\\]\\012\\s-\\V\\[\\e[0m\\]\\$ ");
    psh_vf_set(state, "PS1", PSH_VFA_STRING, payload, 0, 0, 0);
    /* #5 #12 #14 TODO: Retrieve all env vars,
     * for generic, only try to read those important to shell, such as HOME,
     * PATH, etc. */
    psh_backend_get_all_env(state);
}

int main(int argc, char **argv)
{

    builtin_function bltin;
    psh_state *state;
    int stat;
    struct _psh_command *cmd = NULL;
    char *expanded_ps1, *buffer;

    /* Initiate the internal state */
    state = xcalloc(1, sizeof(psh_state));
    psh_vfa_new_context(state);
    load_shell_vars(state);
#ifdef DEBUG
    {
        union _psh_vfa_value payload = {(char *)1};
        psh_vf_set(state, "p", PSH_VFA_INTEGER | PSH_VFA_EXPORT, payload, 0, 0,
                   0);
        const struct _psh_vfa_container *cnt = psh_vf_get(state, "p", 0, 0);
        printf("%d = 1\n", cnt->payload);
        printf("%s = 1\n", getenv("p"));
        psh_vf_unset(state, "p", 0);
        cnt = psh_vf_get(state, "p", 0, 0);
        printf("0x%x = 0x0\n", cnt);
    }
#endif
    state->command_table = psh_hash_create(32);
    /* TODO: Store this as shell arguments */
    state->argv0 = psh_strdup(
        (strrchr(argv[0], '/') == NULL ? argv[0] : strrchr(argv[0], '/') + 1));

    parse_shell_args(state, argc, argv);

    if (psh_backend_prepare(state) != 0)
        exit_psh(state, 1);

#ifdef HAVE_WORKING_HISTORY
    using_history();
#endif
    while (1)
    {
        expanded_ps1 = ps_expander(state, psh_vf_getstr(state, "PS1"));
        stat = read_cmdline(state, expanded_ps1, &buffer);
        xfree(expanded_ps1);
        if (stat == 1)
        {
            puts("");
            exit_psh(state, psh_vf_getint(state, "?"));
        }
        if (stat < 0)
            continue;
        cmd = new_command();
        stat = filpinfo(state, buffer, cmd);
        if (stat < 0)
        {
            free_command(cmd);
            continue;
        }
        psh_backend_do_run(state, cmd);
        free_command(cmd);
    }
    return 0;
}
