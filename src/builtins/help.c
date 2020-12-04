/*
   psh/builtins/hash.c - builtin command hash

   Copyright 2020 Zhang Maiyun.

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
#include <stdlib.h>
#include <string.h>

#include "builtin.h"
#include "libpsh/util.h"
#include "psh.h"

#define MAN 0x00
#define SHORT 0x01
#define USAGE 0x02
#define NBUILTIN (sizeof(builtin_helps) / sizeof(builtin_help_t))
/* Help string of all builtins */
#ifndef WITHOUT_BUILTIN_HELP
typedef char *builtin_help_t[4];
const builtin_help_t builtin_helps[63] = {
    {".", "filename [arguments]",
     "Execute commands from a file in the current shell.",
     "Read and execute commands from FILENAME in the current shell.  The "
     "entries in $PATH are used to find the directory containing FILENAME. If "
     "any ARGUMENTS are supplied, they become the positional parameters when "
     "FILENAME is executed.\n"
     "\tExit Status:\n"
     "\tReturns the status of the last command executed in FILENAME; fails if "
     "FILENAME cannot be read."},
    {":", "", "", ""},
    {"alias", "", "", ""},
    {"bg", "", "", ""},
    {"bind", "", "", ""},
    {"break", "", "", ""},
    {"builtin", "", "", ""},
    {"case", "", "", ""},
    {"cd", "", "", ""},
    {"chdir", "", "", ""},
    {"command", "", "", ""},
    {"continue", "", "", ""},
    {"declare", "", "", ""},
    {"do", "", "", ""},
    {"done", "", "", ""},
    {"echo", "", "", ""},
    {"elif", "", "", ""},
    {"else", "", "", ""},
    {"esac", "", "", ""},
    {"eval", "", "", ""},
    {"exec", "", "", ""},
    {"exit", "exit [n]", "Exit the shell.",
     "Exits the shell with a status of N.  If N is omitted, the exit status is "
     "that of the last command executed."},
    {"export", "", "", ""},
    {"false", "", "", ""},
    {"fc", "", "", ""},
    {"fg", "", "", ""},
    {"fi", "", "", ""},
    {"for", "", "", ""},
    {"getopts", "", "", ""},
    {"getstat", "", "", ""},
    {"hash", "", "", ""},
    {"help", "", "", ""},
    {"history", "", "", ""},
    {"if", "", "", ""},
    {"jobid", "", "", ""},
    {"jobs", "", "", ""},
    {"local", "", "", ""},
    {"logout", "", "", ""},
    {"popd", "", "", ""},
    {"pushd", "", "", ""},
    {"pwd", "", "", ""},
    {"quit", "", "", ""},
    {"read", "", "", ""},
    {"readonly", "", "", ""},
    {"return", "", "", ""},
    {"set", "", "", ""},
    {"setvar", "", "", ""},
    {"shift", "", "", ""},
    {"source", "", "", ""},
    {"test", "", "", ""},
    {"then", "", "", ""},
    {"times", "", "", ""},
    {"trap", "", "", ""},
    {"true", "", "", ""},
    {"type", "", "", ""},
    {"ulimit", "", "", ""},
    {"umask", "", "", ""},
    {"unalias", "", "", ""},
    {"unset", "", "", ""},
    {"until", "", "", ""},
    {"wait", "", "", ""},
    {"which", "", "", ""},
    {"while", "", "", ""}};
#endif
static int compare_builtin_helps(const void *key, const void *cur)
{
    return strcmp((char *)key, (*(builtin_help_t *)cur)[0]);
}

int builtin_help(int argc, char **argv, psh_state *state)
{
#ifndef WITHOUT_BUILTIN_HELP
    int type = MAN;
    int count = 0;
    /* count always points to the next possible command name */
    while (++count < argc)
    {
        if (argv[count][0] != '-')
            break;
        if (argv[count][1] == '-')
        {
            /* Skip this -- */
            ++count;
            break;
        }
        if (argv[count][1] == '\0')
            /* Treat '-' as an argument. */
            break;
        if (argv[count][1] == 'm')
            type = MAN;
        else if (argv[count][1] == 's')
            type = SHORT;
        else if (argv[count][1] == 'd')
            type = USAGE;
        else
        {
            OUT2E("%s: %s: unrecognized argument -%c\n", state->argv0, argv[0],
                  argv[count][1]);
            return 1;
        }
    }
    if (count == argc)
    {
        size_t count2;
        /* Print all help */
        for (count2 = 0; count2 < NBUILTIN; ++count2)
        {
            printf("%s %s\n", builtin_helps[count2][0],
                   builtin_helps[count2][1]);
        }
        return 0;
    }
    /* Go over all positional parameters, print their help */
    for (; count < argc; ++count)
    {
        builtin_help_t *itm =
            bsearch(argv[count], builtin_helps, NBUILTIN,
                    sizeof(builtin_help_t), &compare_builtin_helps);
        if (itm == NULL)
        {
            OUT2E("%s: %s: no help topics match `%s'.  Try `help help' or `man "
                  "-k %s' or `info %s'.\n",
                  state->argv0, argv[0], argv[count], argv[count], argv[count]);
            continue;
        }
        if (type == MAN)
            /* Print help in man format. */
            printf("NAME\n\t%s - %s\n\nSYNOPSIS\n\t%s "
                   "%s\n\nDESCRIPTION\n\t%s\n\nSEE "
                   "ALSO\n\tpsh(1)\n\nIMPLEMENTATION\n\tpsh\n",
                   (*itm)[0], (*itm)[2], (*itm)[0], (*itm)[1], (*itm)[3]);
        else if (type == SHORT)
            printf("%s - %s\n", (*itm)[0], (*itm)[2]);
        else if (type == USAGE)
            printf("%s: %s %s\n", (*itm)[0], (*itm)[0], (*itm)[1]);
    }
    return 0;
#else
    OUT2E("%s: %s: Built without builtin help.\n", state->argv0, argv[0]);
    return 1;
#endif
}
