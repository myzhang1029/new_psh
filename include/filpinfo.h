/** @file psh/filpinfo.h - @brief Function to fill parse info (struct
 * _psh_command)
 * @details merges original preprocesser, splitbuf, parser.
 */
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
#ifndef _PSH_FILPINFO_H
#define _PSH_FILPINFO_H
#include "psh.h"

/** Fill struct _psh_command with an input string.
 *
 * @param state Psh internal state.
 * @param buffer Input string.
 * @param command The struct _psh_command to fill.
 * @return The number of characters processed. */
int filpinfo(psh_state *state, char *buffer, struct _psh_command *command);
#endif
