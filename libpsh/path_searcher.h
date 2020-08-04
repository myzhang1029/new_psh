/*
    libpsh/path_searcher.c - path searcher
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

#ifndef _LIBPSH_PATH_SEARCHER_H
#define _LIBPSH_PATH_SEARCHER_H

/* Call CHK_FUNC on each (substr + TARGET) concatenated string in PATH
 * separated by SEPARATOR.
 * Returns the first conctenated string for which CHK_FUNC returns non-zero;
 * or NULL if none succeeded. REsult should be free()d */
char *psh_search_path(char *path, int separator, const char *target,
                      int (*chk_func)(const char *));
#endif
