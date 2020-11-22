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
      -n        make NAME a reference to the variable named by its value [TODO]
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

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "backend.h"
#include "command.h"
#include "libpsh/hash.h"
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"
#include "psh.h"
#include "util.h"
#include "variable.h"

static inline void clear_single_var(psh_state *state,
                                    struct _psh_vfa_container *var)
{
    int attributes = var->attributes;
    if (attributes & PSH_VFA_PARSED)
        free_command(var->payload.code);
    else if (attributes & PSH_VFA_ASSOC_ARRAY ||
             attributes & PSH_VFA_INDEX_ARRAY)
    {
        /* Recursively free an array */
        if (attributes & PSH_VFA_INTEGER)
            /* Integer arrays are easy */
            xfree(var->payload.int_array);
        else
        {
            /* String arrays need another loop */
            char **array = var->payload.string_array;
            while (var->array_size--)
                xfree(*array++);
        }
    }
    else if (!(attributes & PSH_VFA_INTEGER))
    {
        if (!(attributes & PSH_VFA_STRING))
            /* No more types exist, this is a mistake */
            code_fault(state, __FILE__, __LINE__);
        xfree(var->payload.string);
    }
    /* else: integers don't need to be free()d */
}

static inline void free_vf_table(psh_state *state, psh_hash *vfa_table)
{
    ITER_TABLE(vfa_table, {
        clear_single_var(state, (struct _psh_vfa_container *)this->value);
    });
    psh_hash_free(vfa_table);
}

/* Increment internal frame counter, allocate more if needed.
 * Initialize variable, function, and alias tables. */
void psh_vfa_new_context(psh_state *state)
{
    if (state->contexts == NULL)
    {
        /* Initialize the first context frame. */
        state->contexts = xmalloc(sizeof(struct _psh_vf_context) * 4);
        state->context_slots = 4;
        state->alias_table = psh_hash_create(32);
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

/* Set or update a variable or function, if updating, the original string is
 * free()d if needed. Therefore, for each variable, a new string should be
 * duplicated, even if it is a copy. XXX: COW. When the variable tables are
 * destroyed, the payloads and the pointers in the payload are free()d. if
 * is_local is 1, the variable is created/updated in the current context frame,
 * otherwise, it is set in the outmost context frame or updated in the innermost
 * frame in which this variable is found.
 */
int psh_vf_set(psh_state *state, const char *varname,
               const enum _psh_vfa_attributes attrib,
               const union _psh_vfa_value payload, size_t array_size,
               int is_local, int is_func)
{
    struct _psh_vfa_container *container = NULL;
    if (is_local && !(attrib & PSH_VFA_EXPORT))
    {
        if (!(container = psh_hash_get(
                  (is_func
                       ? state->contexts[state->context_idx].function_table
                       : state->contexts[state->context_idx].variable_table),
                  varname)))
        {
            if (!attrib)
                /* New variables must have attrib */
                code_fault(state, __FILE__, __LINE__);
            container = xmalloc(sizeof(struct _psh_vfa_container));
        }
        else
            clear_single_var(state, container);
    }
    else
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
        if (container)
            clear_single_var(state, container);
        else
        {
            /* Not updating, and this is frame 0 */
            if (ctx_idx_to_use != 0)
                code_fault(state, __FILE__, __LINE__);
            if (!attrib)
                /* New variables must have attrib */
                code_fault(state, __FILE__, __LINE__);
            container = xmalloc(sizeof(struct _psh_vfa_container));
            psh_hash_add_chk(
                (is_func ? state->contexts[ctx_idx_to_use].function_table
                         : state->contexts[ctx_idx_to_use].variable_table),
                varname, container, 1);
            /* Put to environ if export */
            if (attrib & PSH_VFA_EXPORT)
            {
                if (attrib & PSH_VFA_INTEGER)
                {
                    int negative = payload.integer < 0;
                    intmax_t a = negative ? -payload.integer : payload.integer;
                    size_t length =
                        negative ? 3 : 2; /* With '\0' and negative sign */
                    char *str;
                    while ((a /= 10) != 0)
                        length += 1;
                    str = xmalloc(length);
                    snprintf(str, length, "%" PRIdMAX, payload.integer);
                    psh_backend_setenv(varname, str, 1);
                    xfree(str);
                }
                else if (attrib & 0xc0a)
                    /* Arrays, references, code, or unset */
                    ;
                else
                    psh_backend_setenv(varname, payload.string, 1);
            }
        }
    }
    if (attrib)
        container->attributes = attrib;
    container->payload = payload;
    container->array_size = array_size;
    return 0;
}

/* Get a variable's value. Returned value should never be free()d. */
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
    free_vf_table(state, state->contexts[state->context_idx].variable_table);
    free_vf_table(state, state->contexts[state->context_idx--].function_table);
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
            int attrib = container->attributes;
            if (attrib & PSH_VFA_EXPORT && !(attrib & 0xc0a))
                /* Don't touch arrays, references, code, or unset */
                psh_backend_setenv(varname, NULL, 1);
            if (ctx_idx_searching != 0)
            {
                /* Removing a local variable */
                clear_single_var(state, container);
                container->attributes |= PSH_VFA_UNSET;
            }
            else
            {
                clear_single_var(state, container);
                psh_hash_rm(
                    (is_func
                         ? state->contexts[ctx_idx_searching].function_table
                         : state->contexts[ctx_idx_searching].variable_table),
                    varname);
            }
        }
    } while (ctx_idx_searching);
}

/* Called upon shell exit, destroy the whole variable database */
void psh_vfa_free(psh_state *state)
{
    while (state->context_idx)
        psh_vf_exit_local(state);
    free_vf_table(state, state->contexts[0].variable_table);
    free_vf_table(state, state->contexts[0].function_table);
    xfree(state->contexts);
    psh_hash_free(state->alias_table);
}
