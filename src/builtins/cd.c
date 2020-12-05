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
#include <stdlib.h>
#include <string.h>

#include "backend.h"
#include "builtin.h"
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"
#include "psh.h"
#include "variable.h"

#define LFLAG 0x1
#define PFLAG 0x2
#define EFLAG 0x4
#define ATFLAG 0x8

/* TODO */
/* Get rid of ../"s and "./"s and extra "/"s.
 * Resolve to realpath before expanding "../"s is ABSOLUTE is 1.
 * Return NULL if PATH is NULL or cwd cannot be determined. */
static char *canonicalize_path(psh_state *state, const char *path, int flags)
{
    /* Not using psh_stringbuilder here because the frequent need to yield */
    /* Excluding NUL */
    size_t have_size;
    char *allocated_path, *new_path, *xpath, *saved_xpath;
    const char *slash = "/";

    if (path == NULL)
        return NULL;
    if (*path == '\0')
        return NULL;

    /* Ignoring flags TODO */
    if (*path == *slash)
    {
        have_size = strlen(path) * 2;
        new_path = allocated_path = xmalloc(have_size + 2);
        xpath = psh_strdup(path);
    }
    else
    {
        /* This is usually an real path TODO */
        /* bash says: cd: error retrieving current directory: getcwd:
         * cannot access parent directories: No such file or directory */
        size_t len_cwd, len_path;
        if (flags & PFLAG)
            xpath = psh_backend_getcwd_dm();
        else
        {
            xpath = psh_strdup(psh_vf_getstr(state, "PWD"));
            if (!xpath)
                xpath = psh_backend_getcwd_dm();
        }
        if (!xpath)
        {
            OUT2E("%s: cd: error retrieving current directory: %s\n",
                  state->argv0, strerror(errno));
            /* bash won't fail in this case, but we do */
            return NULL;
        }
        len_cwd = strlen(xpath);
        len_path = strlen(path);
        /* +2 for slash and \0 */
        have_size = (len_cwd + len_path) * 2;
        new_path = allocated_path = xmalloc(have_size + 3);
        xpath = xrealloc(xpath, len_cwd + len_path + 2);
        *(xpath + len_cwd) = *slash;
        psh_strncpy(xpath + len_cwd + 1, path, len_path);
    }
    saved_xpath = xpath;
    *new_path++ = *slash;
    while (*xpath)
    {
        /* Get rid of extra slashes */
        if (*xpath == *slash)
        {
            ++xpath;
            continue;
        }
        if (*xpath == '.')
        {
            /* /.* */
            if (xpath[1] == '\0' || xpath[1] == *slash)
            {
                /* /./ || /.NUL */
                ++xpath;
                continue;
            }
            if (xpath[1] == '.' && (xpath[2] == '\0' || xpath[2] == *slash))
            {
                if (new_path - allocated_path != 1)
                {
                    /* Find last slash */
                    --new_path;
                    while (*--new_path != *slash)
                        ;
                    *++new_path = '\0';
                }
                /* else: ../ at root */
                /* skip one character and continue */
                xpath += 2;
                continue;
            }
        }
        do
        {
            size_t length = new_path - allocated_path;
            if (length >= have_size)
            {
                allocated_path = xrealloc(allocated_path, (have_size *= 2) + 2);
                /* Deal with possible relocation of ALLOCATED_PATH */
                new_path = allocated_path + length;
            }
            *new_path++ = *xpath++;
        } while (*xpath && *xpath != *slash);
        *new_path++ = *slash;
    }
    /* Don't leave a trailing slash */
    if (new_path[-1] == *slash && new_path - allocated_path != 1)
        new_path[-1] = '\0';
    else
        /* No need to check here because we've always left a byte */
        *new_path = '\0';
    xfree(saved_xpath);
    allocated_path = xrealloc(allocated_path, strlen(allocated_path) + 1);
    return allocated_path;
}

#if defined(TESTING_CANONICALIZE) && defined(DEBUG)
#include <assert.h>
psh_state test_state = {

};
int main(int argc, char **argv)
{
    char *path = canonicalize_path(
        "/..//aaaaa/b//////c/../../../../../d/./r./.d/asdf/../as/", 0);
    puts(path);
    assert(strcmp(path, "/d/r./.d/as") == 0);
    xfree(path);
    return 0;
}
#endif

int builtin_cd(int argc, char **argv, psh_state *state)
{
    char *destination, *path = NULL;
    size_t current_arg;
    unsigned int flags = 0;

    /* Parse args. Ignore any args after path */
    /* skip argv[0] */
    for (current_arg = 1; current_arg < argc; ++current_arg)
    {
        if (argv[current_arg][0] == '-')
        {
            int current_char, length = strlen(argv[current_arg]);
            if (length == 1)
                /* '-' */
                path = argv[current_arg];
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
                        OUT2E("%s: %s: -%c: invalid option\n", state->argv0,
                              argv[0], argv[current_arg][current_char]);
                        OUT2E("cd: usage: cd [-L|[-P [-e]] [-@]] [dir]\n");
                        return 1;
                }
            }
        }
        else if (!path)
            path = argv[current_arg];
        else
        {
            OUT2E("%s: %s: too many arguments\n", state->argv0, argv[0]);
            return 1;
        }
    }
    /* cd without args, then path == $HOME. */
    if (!path)
        path = (char *)psh_vf_getstr(state, "HOME");
    if (!path)
    {
        OUT2E("%s: %s: HOME not set\n", state->argv0, argv[0]);
        return 1;
    }
    if (strcmp(path, "-") == 0)
        path = (char *)psh_vf_getstr(state, "OLDPWD");
    if (!path)
    {
        OUT2E("%s: %s: OLDPWD not set\n", state->argv0, argv[0]);
        return 1;
    }

    destination = canonicalize_path(state, path, flags);
    if (!destination)
        return 1;
    if (psh_backend_chdir(destination) != 0)
    {
        OUT2E("%s: %s: %s: %s\n", state->argv0, argv[0], path, strerror(errno));
        xfree(destination);
        return 1;
    }
    union _psh_vfa_value payload = {psh_strdup(psh_vf_getstr(state, "PWD"))};
    psh_vf_set(state, "OLDPWD", PSH_VFA_STRING,
               (const union _psh_vfa_value)payload, 0, 0, 0);
    payload.string = destination;
    psh_vf_set(state, "PWD", 0, (const union _psh_vfa_value)payload, 0, 0, 0);
    /* Destination not free()d */
    return 0;
}
