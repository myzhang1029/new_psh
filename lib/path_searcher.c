/*
    libpsh/path_searcher.c - path searcher
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

#include <stddef.h>
#include <string.h>

#include "libpsh/xmalloc.h"

/* Call CHK_FUNC on each (substr + TARGET) concatenated string in PATH
 * separated by SEPARATOR.
 * Returns the first concatenated string for which CHK_FUNC returns non-zero;
 * or NULL if none succeeded. Result should be free()d */
char *psh_search_path(const char *path, int separator, const char *target,
                      int (*chk_func)(const char *))
{
    /* +1 for '\0' */
    size_t len_substr, len_target = strlen(target) + 1,
                       size_allocated = 2 * len_target;
    char *concatenated = xmalloc(size_allocated), *occur;

    while (*path)
    {
        occur = strchr(path, separator);
        len_substr = occur ? (size_t)(occur - path) : strlen(path);

        /* Realloc concatenated if it's too small */
        if (len_substr + len_target > size_allocated)
        {
            size_allocated = len_target + len_substr;
            concatenated = xrealloc(concatenated, size_allocated);
        }

        /* Copy substr */
        memcpy(concatenated, path, len_substr);
        /* Copy target str */
        memcpy(concatenated + len_substr, target, len_target);
        /* End string */
        *(concatenated + len_target + len_substr - 1) = 0;
        if (chk_func(concatenated))
            return concatenated;
        /* Jump over the substr */
        path += len_substr + 1; /* +1 for separator */
    }
    xfree(concatenated);
    return NULL;
}
