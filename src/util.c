/*
    psh/util.c - psh-specific utilities
    Copyright 2018-2020 Zhang Maiyun

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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "backend.h"
#include "command.h"
#include "libpsh/hash.h"
#include "libpsh/path_searcher.h"
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"
#include "psh.h"
#include "util.h"
#include "variable.h"

/* Search a command against $PATH */
char *psh_get_cmd_realpath(psh_state *state, char *cmd)
{
    if (strchr(cmd, '/'))
        /* A command with a path */
        return cmd;
    if (*cmd == '\0')
    {
        /* This is an empty command, don't search path for it */
        OUT2E("%s: : command not found\n", state->argv0);
        return NULL;
    }
    /* Search PATH and run command */
    char *exec_path;
    /* Try to find a cached command path. */
    exec_path = psh_hash_get(state->command_table, cmd);
    if (exec_path == NULL)
    {
        /* No cached commands, search PATH for it */
        char *name = xmalloc(strlen(cmd) + 2);
        name[0] = '/';
        psh_strncpy(name + 1, cmd, strlen(cmd));
        exec_path = psh_search_path(psh_vf_getstr(state, "PATH"),
                                    psh_backend_path_separator, name,
                                    &psh_backend_file_exists);
        xfree(name);
        if (exec_path == NULL)
        {
            OUT2E("%s: %s: command not found\n", state->argv0, cmd);
            return NULL;
        }
        psh_hash_add(state->command_table, cmd, exec_path, 1);
    }
#ifdef DEBUG
    printf("Cached Path: %s\n", exec_path);
#endif
    return exec_path;
}

/* Some unexpected things happened */
ATTRIB_NORETURN void psh_code_fault(psh_state *state, const char *file, int line)
{
    OUT2E("%s: Programming error at %s:%d\n", state->argv0, file, line);
    OUT2E("Shell version: %s\n", PSH_VERSION);
    OUT2E("Please create a GitHub Issue at "
          "https://github.com/myzhang1029/psh/issues with above info\n");
    psh_exit(state, 1);
}

/* Exit psh after cleaning up */
void psh_exit(psh_state *state, int status)
{
    xfree(state->argv0);
    psh_vfa_free(state);
    psh_hash_free(state->command_table);
    psh_jobs_free(state, 1);
    xfree(state);
    exit(status);
}
