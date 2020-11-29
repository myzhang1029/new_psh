/** @file psh/builtin.h - @brief Builtin definitions of the psh */
/*
    Copyright 2017-2020 Zhang Maiyun.

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
#ifndef BUILTIN_INCLUDED
#define BUILTIN_INCLUDED

#include "psh.h"

/** Type for builtin entrypoints. */
typedef int (*builtin_function)(int argc, char **argv, psh_state *state);
/** Structure for bsearch()ing builtins */
struct builtin
{
    /** Name of the builtin */
    char *name;
    /** Entrypoint of the builtin */
    builtin_function proc;
};

int builtin_alias(int argc, char **argv, psh_state *state);
/** Builtin exec */
int builtin_exec(int argc, char **argv, psh_state *state);
/** Builtin echo */
int builtin_echo(int argc, char **argv, psh_state *state);
/** Builtin exit */
int builtin_exit(int argc, char **argv, psh_state *state);
/** Builtin true */
int builtin_true(int argc, char **argv, psh_state *state);
/** Builtin false */
int builtin_false(int argc, char **argv, psh_state *state);
/** Builtin cd */
int builtin_cd(int argc, char **argv, psh_state *state);
/** Builtin pwd */
int builtin_pwd(int argc, char **argv, psh_state *state);
/** Builtin hash */
int builtin_hash(int argc, char **argv, psh_state *state);
/** Builtin help */
int builtin_help(int argc, char **argv, psh_state *state);
/** Builtin history */
int builtin_history(int argc, char **argv, psh_state *state);
/** Builtin builtin */
int builtin_builtin(int argc, char **argv, psh_state *state);

/** Find the entrypoint of a builtin by name.
 *
 * @param name Name of the builtin to look for.
 * @return The found entrypoint.
 */
builtin_function find_builtin(char *name);

/** Count number of items in a NULL-terminated array.
 *
 * @param argv The array to count.
 * @return The count.
 */
int get_argc(char **argv);

#endif
