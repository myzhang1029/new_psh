/** @file /psh/util.h - @brief Psh-specific utilities */
/*
    Copyright 2018-2020 Zhang Maiyun

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

#ifndef _PSH_UTIL_H
#define _PSH_UTIL_H

#include "psh.h" /* For psh_state and __attribute__ */

/** Search a command in the command hash table for its path on disk.
 *  If not found, search against $PATH and add the result to the hash table.
 *
 * @param cmd Name of the command to be searched.
 * @return The path, do not free().
 */
char *psh_get_cmd_realpath(psh_state *state, char *cmd);

/** Report a fault in programming.
 *
 * @param state Internal state.
 * @param file __FILE__
 * @param line __LINE__
 */
void psh_code_fault(psh_state *state, char *file, int line)
    __attribute__((noreturn));

/** Exit after cleaning up.
 *
 * @param state Internal state.
 * @param status Exit status.
 */
void psh_exit(psh_state *state, int status) __attribute__((noreturn));
#endif
