/** @file libpsh/path_searcher.h - @brief Path searcher */
/*

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

/** @brief Search path list for the first path matching a condition.
 * @details Call CHK_FUNC on each (substr + TARGET) concatenated string in PATH
 * separated by SEPARATOR.
 *
 * @param path List of paths separated by @p separator to call @p chk_func
 * with.
 * @param separator Separator of @p path.
 * @param target The string to be appended to each item in @p path before
 * calling @p chk_func.
 * @param chk_func Function to determine whether the correct path is found.
 * @return The first concatenated string for which @p chk_func returns non-zero;
 * or NULL if none succeeded, should be free()d.
 */
char *psh_search_path(char *path, int separator, const char *target,
                      int (*chk_func)(const char *));
#endif
