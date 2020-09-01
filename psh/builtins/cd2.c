/*
    psh/builtins/cd.c - builtin cd
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
/*
cd: cd [-L|[-P [-e]] [-@]] [dir]
    Change the shell working directory.

    Change the current directory to DIR.  The default DIR is the value of the
    HOME shell variable.

    The variable CDPATH defines the search path for the directory containing
    DIR.  Alternative directory names in CDPATH are separated by a colon (:).
    A null directory name is the same as the current directory.  If DIR begins
    with a slash (/), then CDPATH is not used.

    If the directory is not found, and the shell option `cdable_vars' is set,
    the word is assumed to be  a variable name.  If that variable has a value,
    its value is used for DIR.

    Options:
      -L	force symbolic links to be followed: resolve symbolic
            links in DIR after processing instances of `..'
      -P	use the physical directory structure without following
            symbolic links: resolve symbolic links in DIR before
            processing instances of `..'
      -e	if the -P option is supplied, and the current working
            directory cannot be determined successfully, exit with
            a non-zero status
      -@	on systems that support it, present a file with extended
            attributes as a directory containing the file attributes

    The default is to follow symbolic links, as if `-L' were specified.
    `..' is processed by removing the immediately previous pathname component
    back to a slash or the beginning of DIR.

    Exit Status:
    Returns 0 if the directory is changed, and if $PWD is set successfully when
    -P is used; non-zero otherwise.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "backend.h"
#include "builtin.h"
#include "libpsh/util.h"

#define LFLAG 0x1
#define PFLAG 0x2
#define EFLAG 0x4
#define ATFLAG 0x8

extern char *argv0;

/* TODO */
static char *create_pwd(char *path, int flags) { return path; }

int builtin_cd(int argc, char **argv)
{
    char *destination, *path = NULL;
    int current_arg, flags = 0;

    /* Parse args. Ignore any args after path */
    /* skip argv[0] */
    for (current_arg = 1; current_arg < argc; ++current_arg)
    {
        if (argv[current_arg][0] == '-')
        {
            int current_char, length = strlen(argv[current_arg]);
            /* skip '-' */
            for (current_char = 1; current_char < length; ++current_char)
            {
                switch (argv[current_arg][current_char])
                {
                    case 'L':
                        flags |= LFLAG;
                        /* Unset -P, latter matters */
                        flags &= ~PFLAG;
                        break;
                    case 'P':
                        flags |= PFLAG;
                        /* Unset -L, latter matters */
                        flags &= ~LFLAG;
                        break;
                    case 'e':
                        /* -e has no effect without -P */
                        flags |= EFLAG;
                        break;
                    case '@':
                        flags |= ATFLAG;
                        break;
                    default:
                        OUT2E("%s: %s: -%c: invalid option\n", argv0, argv[0],
                              argv[current_arg][current_char]);
                        OUT2E("cd: usage: cd [-L|[-P [-e]] [-@]] [dir]\n");
                        return 1;
                }
            }
        }
        else if (!path)
            path = argv[current_arg];
        else
        {
            OUT2E("%s: %s: too many arguments\n", argv0, argv[0]);
            return 1;
        }
    }
    /* cd without args, then path == $HOME. */
    if (!path)
        path = getenv("HOME"); /* #8 TODO */
    if (!path)
    {
        OUT2E("%s: %s: HOME not set\n", argv0, argv[0]);
        return 1;
    }
    if (strcmp(path, "-") == 0)
        path = getenv("OLDPWD"); /* #8 TODO */

    destination = create_pwd(path, flags);
    if (pshchdir(destination) != 0)
        OUT2E("%s: %s: %s: %s\n", argv0, argv[0], path, strerror(errno));
    else
    {
        pshsetenv("OLDPWD", getenv("PWD"), 1); /* #8 TODO */
        pshsetenv("PWD", destination, 1);      /* #8 TODO */
    }
}