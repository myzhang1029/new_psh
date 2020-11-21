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
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"
#include "prompts.h"
#include "util.h"

int last_command_status = 0; /* #8 TODO: $? */
char *argv0;
extern int optopt;

int main(int argc, char **argv)
{
    int stat;
    char *expanded_ps1, *buffer;
    char *ps1 =
        "\\[\\e[01;32m\\]\\u \\D{} " /* #8 TODO: $PS1 */
        "\\[\\e[01;34m\\]\\w\\[\\e[01;35m\\]\\012\\s-\\V\\[\\e[0m\\]\\$ ";

    /* TODO: Store this as shell arguments */
    argv0 = psh_strdup(
        (strrchr(argv[0], '/') == NULL ? argv[0] : strrchr(argv[0], '/') + 1));

    parse_shell_args(argc, argv);

    add_atexit_free(argv0);

    if (psh_backend_prepare() != 0)
        exit_psh(1);

#ifdef HAVE_WORKING_HISTORY
    using_history();
#endif
    while (1)
    {
        expanded_ps1 = ps_expander(ps1);
        stat = read_cmdline(expanded_ps1, &buffer);
        xfree(expanded_ps1);
        if (stat < 0)
            continue;

        execute_command(buffer);

    }
    return 0;
}
