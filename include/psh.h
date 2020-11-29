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
#define PSH_VERSION "0.19.0"

#include "libpsh/hash.h"

/* jobs.h depends on our psh_state, so this forward decl is used instead */
struct _psh_jobs;

/** @brief The internal state of psh. */
typedef struct _psh_state
{
    /** Context frames of variables and functions, similar to stack frames in
     * other languages */
    struct _psh_vf_context
    {
        /** Variables */
        psh_hash *variable_table;
        /** Global functions */
        psh_hash *function_table;
    } * contexts; /**< An array of context frames. */
    /** The index of the current context frame. */
    size_t context_idx;
    /** The number of available context frames. */
    size_t context_slots;
    /** Background jobs. */
    struct _psh_jobs *jobs;
    /* Local functions is a psh extension */
    /** Aliases hash table */
    psh_hash *alias_table;
    /** Command hash table */
    psh_hash *command_table;
    /** Shell argv[0]. */
    char *argv0;
    /** Verbose flag. */
    unsigned int verbose : 1;
    /** Interactive flag. */
    unsigned int interactive : 1;
} psh_state;
#endif
