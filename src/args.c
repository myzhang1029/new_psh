/*
    psh/src/args.c - psh argument control
    Copyright 2020 Manuel Bertele

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

#include <string.h>
#include <stdlib.h>

void print_version_exit();

/* Set variable thats globally avaliable */
int VerbosE = 0;
extern int optopt;
extern char *argv0;

void parse_shell_args(int argc, char **argv)
{

    /* Only checks for -- arguments */
    short i;
    for(i = 0; i < argc; i++)
    {
        if(strcmp(argv[i], "--version") == 0)
            print_version_exit();
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
                break;
            case '?':
            default:
                OUT2E("%s: unknown option -%c\n", argv0, optopt);
                break;
        }
    }
}

void print_version_exit()
{

    puts("psh version: "PSH_VERSION);
    exit(0);

}
