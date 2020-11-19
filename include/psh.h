/** @file psh/psh.h - @brief Psh generic definitions */
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

#ifndef _PSH_H
#define _PSH_H
#ifndef __GNUC__
/** GNU attribute macro */
#define __attribute__(x)
#endif
/** Version of psh. */
#define PSH_VERSION "0.18.0"

#include "libpsh/hash.h"

/** @brief The internal state of psh. */
typedef struct _psh_state
{
    /** A list of pending signals. */
    int *signals_pending;
    /* Using two tables here, because both env vars and global vars live
     * throughout the life of the shell, while local vars can override them without
     * altering their value. */
    /** Environmental and global variables */
    psh_hash *variable_table_eg;
    /** Local variables */
    psh_hash *variable_table_l;
    /** Command hash table */
    psh_hash *command_table;
    /** @deprecated Shell argv[0]. */
    char *argv0;
    /** @deprecated $?. */
    int last_command_status; /* #8 TODO: $? */
    /** Verbose flag. */
    unsigned int verbose:1;
    /** Placeholder. */
    unsigned int other_flags:3;
} psh_state;
#endif
