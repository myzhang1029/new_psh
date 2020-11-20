/** @file psh/variables.h - @brief psh variables, functions and aliases */
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

#ifndef _PSH_VARIABLE_H
#define _PSH_VARIABLE_H

#include <stdint.h>

/** @brief Container of variables, functions, and aliases. */
struct _psh_vfa_container
{
    /** @brief Attributes of variables, functions, and aliases. */
    enum _psh_vfa_attributes
    {
        /** Indexed arrays. */
        PSH_VFA_INDEX_ARRAY = 0x001,
        /** Associative arrays. */
        PSH_VFA_ASSOC_ARRAY = 0x002,
        /** Integer. */
        PSH_VFA_INTEGER = 0x004,
        /** Reference to the variable by its value. */
        PSH_VFA_REFERENCE = 0x008,
        /** Readonly. */
        PSH_VFA_READONLY = 0x010,
        /** Trace. */
        PSH_VFA_TRACE = 0x020,
        /** Exported. */
        PSH_VFA_EXPORT = 0x040,
        /** Uppercase on assignment. */
        PSH_VFA_UPPER = 0x080,
        /** Lowercase on assignment. */
        PSH_VFA_LOWER = 0x100,
        /** First-letter-capitalized on assignment. */
        PSH_VFA_CAPI = 0x200,
        /** Resolved to code piece. */
        PSH_VFA_PARSED = 0x400,
        /** Together with a local scope, this marks a variable as
         * locally-unset, and stops psh from travelling up to find a value. */
        PSH_VFA_UNSET = 0x800
    } attributes; /**< The attributes for this variable. */
    /** Resolved code or string value of the variable */
    union _psh_vfa_value
    {
        /** String value */
        char *string;
        struct command *code;
        intmax_t *int_array;
        char **string_array;
        intmax_t integer;
    } payload;
    /** Size of the array if it is one */
    size_t array_size;
};

/** Initialize variable and function tables of a new context frame.
 * Try to get environment parameters if this is the 0th context frame.
 *
 * @param state Psh internal state.
 */
void psh_vfa_new_context(psh_state *state);

/** Set or update a variable or function.
 *
 * @param state Psh internal state.
 * @param varname Name of the variable or function.
 * @param attrib Variable attributes.
 * @param payload Value of the function or variable.
 * @param array_size Length if this is an array, 0 if not.
 * @param is_local Whether to treat as local variable.
 * @param is_func Whether this is a function.
 * @return 0 if succeed, 1 if not.
 */
int psh_vf_set(psh_state *state, const char *varname,
               const enum _psh_vfa_attributes attrib,
               const union _psh_vfa_value payload, size_t array_size,
               int is_local, int is_func);

/** Get a variable's value.
 *
 * @param state Psh internal state.
 * @param varname Name of the variable or function.
 * @param is_func Whether this is a function.
 * @return the variable container, NULL if not found.
 */

const struct _psh_vfa_container *psh_vf_get(psh_state *state,
                                            const char *varname, int is_func);
/** Clear all variables and functions local to the current context frame.
 *
 * @param state Psh internal state.
 */
void psh_vf_exit_local(psh_state *state);

/** Unset a variable. Removes only the innermost entry.
 *
 * @param state Psh internal state.
 * @param varname Name of the variable or function.
 * @param is_func Whether this is a function.
 * @return 0 if something is removed, 1 if not found.
 */
int psh_vf_unset(psh_state *state, const char *varname, int is_func);

/** Destroy all variable tables.
 *
 * @param state Psh internal state.
 */
void psh_vfa_free(psh_state *state);
#endif
