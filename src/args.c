/*
    psh/args.c - psh argument parser
    Copyright 2020 Manuel Bertele
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

#include "backend.h"
#include "libpsh/util.h"
#include "psh.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>

static void print_help_info();
static void print_version_exit();

extern int optopt;

void parse_shell_args(psh_state *state, int argc, char **argv)
{

    /* Only checks for -- options */
    int i;
    for (i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "--") == 0)
            break;
        if (strcmp(argv[i], "--version") == 0)
            print_version_exit(state);
        if (strcmp(argv[i], "--help") == 0)
        {
            print_help_info(state);
            psh_exit(state, 0);
        }
        if (strcmp(argv[i], "--interactive") == 0)
        {
            state->interactive = 1;
            argv[i][0] = '\0';
        }
        if (strcmp(argv[i], "--verbose") == 0)
        {
            state->verbose = 1;
            argv[i][0] = '\0';
        }
        else if (strstr(argv[i], "--") == argv[i])
        {
            print_help_info(state);
            OUT2E("%s: unknown option %s\n", state->argv0, argv[i]);
            psh_exit(state, 2);
        }
    }

    int arg;
    const char *optstring = ":vi";

    /* Parse shell options */
    while ((arg = psh_backend_getopt(argc, argv, optstring)) != -1)
    {
        switch (arg)
        {
            /* Interactive flag */
            case 'i':
                state->interactive = 1;
                break;
            /* Verbose flag */
            case 'v':
                state->verbose = 1;
                break;
            case ':':
                OUT2E("%s: option requires an argument\n", state->argv0);
                psh_exit(state, 1);
            case '?':
            default:
                print_help_info(state);
                OUT2E("%s: unknown option -%c\n", state->argv0, optopt);
                psh_exit(state, 2);
        }
    }
}

static void print_help_info(psh_state *state)
{
    printf("psh, version " PSH_VERSION "\n"
           "Copyright (C) 2017-2020 Zhang Maiyun, Manuel Bertele\n"
           "This program comes with ABSOLUTELY NO WARRANTY.\n"
           "This is free software, and you are welcome to redistribute it\n"
           "under certain conditions.\n\n"
           "Usage: %s [options]\n"
           "Options:\n"
           "\t-v, --verbose: Enable verbose mode\n"
           "\t--help: Show this text and exit\n"
           "\t--version: Print psh version and exit\n",
           state->argv0);
}

static void print_version_exit(psh_state *state)
{
    puts("psh version: " PSH_VERSION);
    psh_exit(state, 0);
}
