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

#include "psh.h" /* For __attribute__ */

/** Report a fault in programming.
 *
 * @param file __FILE__
 * @param line __LINE__
 */
void code_fault(char *file, int line) __attribute__((noreturn));

/** Exit after cleaning up.
 *
 * @param status Exit status.
 */
void exit_psh(int status) __attribute__((noreturn));

/** Add a pointer to be free()d upon exit.
 *
 * @param ptr The pointer.
 * @return The index of this pointer.
 */
int add_atexit_free(void *ptr);
#endif
