/*
    psh/backends/posix2/misc_impl.c - Implementations or stubs of platform-
    dependent function on POSIX platforms.
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

#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>

#include "backend.h"
#include "libpsh/util.h"

char *psh_backend_get_homedir(void)
{
    struct passwd *pwd = getpwuid(getuid());
    if (pwd == NULL)
        return NULL;
    return pwd->pw_dir;
}

char *psh_backend_get_homedir_username(char *username)
{
    struct passwd *pwd = getpwnam(username);
    if (pwd == NULL)
        return NULL;
    return pwd->pw_dir;
}

char *psh_backend_get_username(void)
{
    struct passwd *pwd = getpwuid(getuid());
    if (pwd == NULL)
        return NULL;
    return pwd->pw_name;
}

char *psh_backend_getcwd(char *wd, size_t len) { return getcwd(wd, len); }

char *psh_backend_getcwd_dm(void)
{
    /* Providing NULL to getcwd isn't mainstream POSIX */
    char *buf =
        psh_getstring((void *(*)(char *, size_t)) & psh_backend_getcwd, NULL);
    return buf;
}

int psh_backend_gethostname(char *dest, size_t len)
{
    return gethostname(dest, len);
}

char *psh_backend_gethostname_dm(void)
{
    char *buf = psh_getstring(
        (void *(*)(char *, size_t)) & psh_backend_gethostname, NULL);
    return buf;
}

int psh_backend_getuid(void) { return geteuid(); }

int psh_backend_chdir(char *dir) { return chdir(dir); }

int psh_backend_setenv(const char *name, const char *value, int overwrite)
{
    return setenv(name, value, overwrite);
}

int psh_backend_getopt(int argc, char **argv, const char *optstring)
{
    return getopt(argc, argv, optstring);
}
