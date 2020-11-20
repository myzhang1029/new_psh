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

/** Report a fault in programming.
 *
 * @param state Internal state.
 * @param file __FILE__
 * @param line __LINE__
 */
void code_fault(psh_state *state, char *file, int line)
    __attribute__((noreturn));

/** Exit after cleaning up.
 *
 * @param state Internal state.
 * @param status Exit status.
 */
void exit_psh(psh_state *state, int status) __attribute__((noreturn));
#endif
