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

/** @brief Attributes of variables, functions, and aliases. */
enum _psh_vfa_attributes
{
    /** No update */
    PSH_VFA_NO_UPDATE = 0x0,
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
    PSH_VFA_UNSET = 0x800,
    /** String */
    PSH_VFA_STRING = 0x1000
};
/** @brief Value of a variable. */
union _psh_vfa_value
{
    /** String value. */
    char *string;
    /** Parsed code. */
    struct _psh_command *code;
    /** Integer array. */
    intmax_t *int_array;
    /** String array. */
    char **string_array;
    /** Integer. */
    intmax_t integer;
};
/** @brief Container of variables, functions, and aliases. */
struct _psh_vfa_container
{
    /** The attributes for this variable. */
    unsigned int attributes;
    /** The value of this variable. */
    union _psh_vfa_value payload;
    /** Size of the array if it is one. */
    size_t array_size;
};

/** Initialize variable and function tables of a new context frame.
 * Try to get environment parameters if this is the 0th context frame.
 *
 * @param state Psh internal state.
 */
void psh_vfa_new_context(psh_state *state);

/** Add or update a variable or function.
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
int psh_vf_set(psh_state *state, const char *varname, unsigned int attrib,
               const union _psh_vfa_value payload, size_t array_size,
               int is_local, int is_func);

/** Add a variable or function. Don't use this function to update, or memory
 * leak might happen.
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
int psh_vf_add_raw(psh_state *state, const char *varname, unsigned int attrib,
                   const union _psh_vfa_value payload, size_t array_size,
                   int is_local, int is_func);

/** Get the reference to a variable or function.
 *
 * @param state Psh internal state.
 * @param varname Name of the variable or function.
 * @param force_local Whether to get only local variables.
 * @param is_func Whether this is a function.
 * @return the variable container, NULL if not found.
 */

struct _psh_vfa_container *psh_vf_get(psh_state *state, const char *varname,
                                      int force_local, int is_func);
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

/** Get a string value
 *
 * @param state Psh internal state.
 * @param name Name of the variable.
 * @return value if found, shouldn't be free()d, NULL if not.
 */
static inline const char *psh_vf_getstr(psh_state *state, char *name)
{
    const struct _psh_vfa_container *container =
        psh_vf_get((state), (name), 0, 0);
    return container ? container->payload.string : NULL;
}

/** Get a string value
 *
 * @param state Psh internal state.
 * @param name Name of the variable.
 * @return value if found, 0 if not (dangerous!).
 */
static inline intmax_t psh_vf_getint(psh_state *state, char *name)
{
    const struct _psh_vfa_container *container =
        psh_vf_get((state), (name), 0, 0);
    return container ? container->payload.integer : 0;
}

#endif
