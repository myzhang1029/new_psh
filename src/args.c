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

/* Set variable thats globally avaliable */
int VerbosE = 0;
extern int optopt;
extern char *argv0;

void parse_shell_args(int argc, char **argv)
{

    /* Only checks for -- options */
    int i;
    for (i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "--") == 0)
            break;
        if (strcmp(argv[i], "--version") == 0)
            print_version_exit();
        if (strcmp(argv[i], "--help") == 0)
        {
            print_help_info();
            exit_psh(0);
        }
        if (strcmp(argv[i], "--verbose") == 0)
        {
            VerbosE = 1;
            argv[i][0] = '\0';
        }
        else if (strstr(argv[i], "--") == argv[i])
        {
            print_help_info();
            OUT2E("%s: unknown option %s\n", argv0, argv[i]);
            exit_psh(2);
        }
    }

    int arg;
    const char *optstring = ":v";

    /* Parse shell options */
    while ((arg = psh_backend_getopt(argc, argv, optstring)) != -1)
    {
        switch (arg)
        {
            /* Verbose flag */
            case 'v':
                VerbosE = 1;
                break;
            case ':':
                OUT2E("%s: option requires an argument\n", argv0);
                exit_psh(1);
            case '?':
            default:
                print_help_info();
                OUT2E("%s: unknown option -%c\n", argv0, optopt);
                exit_psh(2);
        }
    }
}

static void print_help_info()
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
           argv0);
}

static void print_version_exit()
{
    puts("psh version: " PSH_VERSION);
    exit_psh(0);
}
