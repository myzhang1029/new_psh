/*
    psh/builtins.c - Builtin commands driver
    Copyright 2017-2020 Zhang Maiyun.

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

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtin.h"
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"
#include "psh.h"
#include "variable.h"

static int builtin_unsupported(int argc, char **argv, psh_state *state);
static int builtin_getstat_handler(int argc, char **argv, psh_state *state);

/* List of all builtins, sorted by name */
const struct builtin builtins[] = {{".", &builtin_unsupported},
                                   {":", &builtin_true},
                                   {"alias", &builtin_alias},
                                   {"bg", &builtin_unsupported},
                                   {"bind", &builtin_unsupported},
                                   {"break", &builtin_unsupported},
                                   {"builtin", &builtin_builtin},
                                   {"case", &builtin_unsupported},
                                   {"cd", &builtin_cd},
                                   {"chdir", &builtin_cd},
                                   {"command", &builtin_unsupported},
                                   {"continue", &builtin_unsupported},
                                   {"declare", &builtin_unsupported},
                                   {"do", &builtin_unsupported},
                                   {"done", &builtin_unsupported},
                                   {"echo", &builtin_echo},
                                   {"elif", &builtin_unsupported},
                                   {"else", &builtin_unsupported},
                                   {"esac", &builtin_unsupported},
                                   {"eval", &builtin_unsupported},
                                   {"exec", &builtin_exec},
                                   {"exit", &builtin_exit},
                                   {"export", &builtin_unsupported},
                                   {"false", &builtin_false},
                                   {"fc", &builtin_unsupported},
                                   {"fg", &builtin_unsupported},
                                   {"fi", &builtin_unsupported},
                                   {"for", &builtin_unsupported},
                                   {"getopts", &builtin_unsupported},
                                   {"getstat", &builtin_getstat_handler},
                                   {"hash", &builtin_hash},
                                   {"help", &builtin_help},
                                   {"history", &builtin_history},
                                   {"if", &builtin_unsupported},
                                   {"jobid", &builtin_unsupported},
                                   {"jobs", &builtin_unsupported},
                                   {"local", &builtin_unsupported},
                                   {"logout", &builtin_exit},
                                   {"popd", &builtin_unsupported},
                                   {"pushd", &builtin_unsupported},
                                   {"pwd", &builtin_pwd},
                                   {"quit", &builtin_exit},
                                   {"read", &builtin_unsupported},
                                   {"readonly", &builtin_unsupported},
                                   {"return", &builtin_unsupported},
                                   {"set", &builtin_unsupported},
                                   {"setvar", &builtin_unsupported},
                                   {"shift", &builtin_unsupported},
                                   {"source", &builtin_unsupported},
                                   {"test", &builtin_unsupported},
                                   {"then", &builtin_unsupported},
                                   {"times", &builtin_unsupported},
                                   {"trap", &builtin_unsupported},
                                   {"true", &builtin_true},
                                   {"type", &builtin_unsupported},
                                   {"ulimit", &builtin_unsupported},
                                   {"umask", &builtin_unsupported},
                                   {"unalias", &builtin_unalias},
                                   {"unset", &builtin_unsupported},
                                   {"until", &builtin_unsupported},
                                   {"wait", &builtin_unsupported},
                                   {"which", &builtin_unsupported},
                                   {"while", &builtin_unsupported}};

int get_argc(char **argv)
{
    int argc = 0;
    for (; argv[argc]; ++argc)
        ;
    return argc;
}

static int builtin_unsupported(ATTRIB_UNUSED int argc,
                               ATTRIB_UNUSED char **argv,
                               ATTRIB_UNUSED psh_state *state)
{
    OUT2E("%s: %s: Not supported, coming soon\n", state->argv0, argv[0]);
    return 127;
}

static int builtin_getstat_handler(ATTRIB_UNUSED int argc,
                                   ATTRIB_UNUSED char **argv, psh_state *state)
{
    printf("%" PRIdMAX "\n", psh_vf_getint(state, "?"));
    return 0;
}

int compare_builtin(const void *key, const void *cur)
{
    const struct builtin *bkey = (const struct builtin *)key;
    const struct builtin *builtin = (const struct builtin *)cur;
    return strcmp(bkey->name, builtin->name);
}

builtin_function find_builtin(char *name)
{
    struct builtin *key = xmalloc(sizeof(struct builtin));
    struct builtin *result;

    key->name = name;
    result = (struct builtin *)bsearch(
        key, builtins, sizeof(builtins) / sizeof(struct builtin),
        sizeof(struct builtin), &compare_builtin);

    xfree(key);

    return result != NULL ? result->proc : (builtin_function)0;
}
