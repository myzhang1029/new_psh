/** @file psh/input.h - @brief Read input */
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
#ifndef _PSH_INPUT_H
#define _PSH_INPUT_H
#include "psh.h"

/** Read command line.
 *
 * @param state Psh internal state.
 * @param prompt Prompt to print.
 * @param result Pointer to the resulting string, the string needs to be
 * free()d.
 * @return 0 if everything goes well; -1 if the cmd doesn't need to be run; -2
 * if anything went wrong, @p result is untouched.
 */
int read_cmdline(psh_state *state, char *prompt, char **result);
#endif
