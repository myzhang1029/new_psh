/** @file psh/prompts.h - @brief Prompt generator and printer */
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
#ifndef _PSH_PROMPTS_H
#define _PSH_PROMPTS_H

#include "psh.h" /* for psh_state */

/** Expand $PSn.
 *
 * @param prompt The $PSn string to be expanded.
 * @return Expanded prompt.
 */
char *ps_expander(psh_state *state, const char *prompt);
#endif
