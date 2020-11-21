/** @file psh/include/args.h - @brief psh argument parser */
/*
    Copyright 2020 Manuel Bertele
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

#ifndef _PSH_ARGS_H_
#define _PSH_ARGS_H_

/** Parse shell parameters and set appropriate variables.
 *
 * @param argc The first parameter to main()
 * @param argv The second parameter to main()
 */
void parse_shell_args(int argc, char **argv);

#endif
