/*
    psh/variables.c - psh variables, functions and aliases
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
/* From `bash -c "help declare"`:
Variables:
      -a        to make NAMEs indexed arrays (if supported)
      -A        to make NAMEs associative arrays (if supported)
      -i        to make NAMEs have the `integer' attribute
      -n        make NAME a reference to the variable named by its value
      -r        to make NAMEs readonly
      -t        to make NAMEs have the `trace' attribute
      -x        to make NAMEs export
*/
/* Variables, functions and aliases are all managed in this file.
 * The implementation of the scope of functions and variables is like
 * that in other languages: layers of context frames are used.
 * One psh extension is local functions.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>

#include "command.h"
#include "libpsh/hash.h"
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"
#include "psh.h"
#include "util.h"
#include "variable.h"

static inline void free_vf_table(psh_hash *vfa_table)
{
    ITER_TABLE(vfa_table, {
        int attributes = ((struct _psh_vfa_container *)this->value)->attributes;
        if (attributes & PSH_VFA_PARSED)
            free_command(
                ((struct _psh_vfa_container *)this->value)->payload.code);
        else if (attributes & PSH_VFA_ASSOC_ARRAY ||
                 attributes & PSH_VFA_INDEX_ARRAY)
        {
            /* Recursively free an array */
            if (attributes & PSH_VFA_INTEGER)
                /* Integer arrays are easy */
                xfree(((struct _psh_vfa_container *)this->value)
                          ->payload.int_array);
            else
            {
                /* String arrays need another loop */
                char **array = ((struct _psh_vfa_container *)this->value)
                                   ->payload.string_array;
                while (((struct _psh_vfa_container *)this->value)->array_size--)
                    xfree(*array++);
            }
        }
        else if (!attributes & PSH_VFA_INTEGER)
            xfree(((struct _psh_vfa_container *)this->value)->payload.string);
        /* else: integers don't need to be free()d */
    });
    psh_hash_free(vfa_table);
}

/* Increment internal frame counter, allocate more if needed.
 * Initialize variable, function, and alias tables. Try to get environment
 * parameters if this is the first context frame. */
void psh_vfa_new_context(psh_state *state)
{
    if (state->contexts == NULL && state->context_idx == 0)
    {
        /* Initialize the first context frame. */
        state->contexts = xmalloc(sizeof(struct _psh_vf_context) * 4);
        state->context_slots = 4;
        state->alias_table = psh_hash_create(32);
        /* #5 #12 #13 TODO: Retrieve all env vars,
         * for generic, only try to read those important to shell, such as HOME,
         * PATH, etc. */
        (void)getenv("PATH");
    }
    else if (++state->context_idx == state->context_slots + 1)
    {
        state->context_slots *= 2;
        state->contexts =
            xrealloc(state->contexts,
                     sizeof(struct _psh_vf_context) * (state->context_slots));
    }
    state->contexts[state->context_idx].variable_table = psh_hash_create(32);
    state->contexts[state->context_idx].function_table = psh_hash_create(8);
}

/* Set or update a variable or function, if updating, it is the caller's
 * responsibility to make sure the original container is free()d, by either
 * reusing or reallocating. When the variable tables are destroyed, the payloads
 * and the pointers in the payload are free()d. if is_local is 1, the variable
 * is created/updated in the current context frame, otherwise, it is set in the
 * outmost context frame or updated in the innermost frame in which this
 * variable is found.
 */
int psh_vf_set(psh_state *state, const char *varname,
               const enum _psh_vfa_attributes attrib,
               const union _psh_vfa_value payload, size_t array_size,
               int is_local, int is_func)
{
    struct _psh_vfa_container *container = NULL;
    if (!is_local)
    {
        size_t ctx_idx_to_use = state->context_idx + 1;
        do
        {
            --ctx_idx_to_use;
            if ((container = psh_hash_get(
                     (is_func ? state->contexts[ctx_idx_to_use].function_table
                              : state->contexts[ctx_idx_to_use].variable_table),
                     varname)))
                break;
        } while (ctx_idx_to_use);
        if (!container)
        {
            /* Not updating, and this is frame 0 */
            if (ctx_idx_to_use != 0)
                code_fault(state, __FILE__, __LINE__);
            container = xmalloc(sizeof(struct _psh_vfa_container));
            psh_hash_add_chk(
                (is_func ? state->contexts[ctx_idx_to_use].function_table
                         : state->contexts[ctx_idx_to_use].variable_table),
                varname, container, 1);
        }
    }
    else
    {
        if (!(container = psh_hash_get(
                  (is_func
                       ? state->contexts[state->context_idx].function_table
                       : state->contexts[state->context_idx].variable_table),
                  varname)))
            container = xmalloc(sizeof(struct _psh_vfa_container));
    }
    container->attributes = attrib;
    container->payload = payload;
    container->array_size = array_size;
    return 0;
}

/* Get a variable's value. */
const struct _psh_vfa_container *psh_vf_get(psh_state *state,
                                            const char *varname, int is_func)
{
    struct _psh_vfa_container *container = NULL;
    size_t ctx_idx_searching = state->context_idx + 1;
    do
    {
        --ctx_idx_searching;
        if ((container = psh_hash_get(
                 (is_func ? state->contexts[ctx_idx_searching].function_table
                          : state->contexts[ctx_idx_searching].variable_table),
                 varname)))
            break;
    } while (ctx_idx_searching);
    return container;
}

/* Clear all variables and functions local to this scope. */
void psh_vf_exit_local(psh_state *state)
{
    if (state->context_idx == 0)
        /* Exiting the root context is not expected to happen. */
        code_fault(state, __FILE__, __LINE__);
    free_vf_table(state->contexts[state->context_idx].variable_table);
    free_vf_table(state->contexts[state->context_idx--].function_table);
}

/* Unset a variable. Removes only the innermost entry. Returns 0 if something is
 * removed, 1 if not found. */
int psh_vf_unset(psh_state *state, const char *varname, int is_func)
{
    struct _psh_vfa_container *container = NULL;
    size_t ctx_idx_searching = state->context_idx + 1;
    do
    {
        --ctx_idx_searching;
        if ((container = psh_hash_get(
                 (is_func ? state->contexts[ctx_idx_searching].function_table
                          : state->contexts[ctx_idx_searching].variable_table),
                 varname)))
        {
            if (ctx_idx_searching != 0)
            {
                /* Removing a local variable */
                /* TODO: dealloc data */
                container->attributes |= PSH_VFA_UNSET;
            }
            else
            {
                /* TODO: dealloc data */
            }
        }
    } while (ctx_idx_searching);
}

/* Called upon shell exit, destroy the whole variable database */
void psh_vfa_free(psh_state *state)
{
    while (state->context_idx)
        psh_vf_exit_local(state);
    free_vf_table(state->contexts[0].variable_table);
    free_vf_table(state->contexts[0].function_table);
    xfree(state->contexts);
    psh_hash_free(state->alias_table);
}