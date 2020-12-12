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

/** Clear a variable container and free() the values.
 *
 * @param state Psh internal state.
 * @param var The variable to be cleared and free()d.
 */
static inline void clear_single_var(psh_state *state,
                                    struct _psh_vfa_container *var)
{
    unsigned int attributes = var->attributes;
    if (attributes & PSH_VFA_PARSED)
        free_command(var->payload.code);
    else if (attributes & PSH_VFA_ASSOC_ARRAY)
        psh_hash_free(var->payload.assoc_array);
    else if (attributes & PSH_VFA_INDEX_ARRAY)
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
            psh_code_fault(state, __FILE__, __LINE__);
        xfree(var->payload.string);
    }
    /* else: integers don't need to be free()d */
}

/** Free a variable hash table.
 *
 * @param state Psh internal state.
 * @param vfa_table The hash table to be free()d.
 */
static inline void free_vf_table(psh_state *state, psh_hash *vfa_table)
{
    ITER_TABLE(vfa_table, {
        clear_single_var(state, (struct _psh_vfa_container *)this->value);
    });
    psh_hash_free(vfa_table);
}

/** Convert a intmax_t to a dynamically allocated string.
 *
 * @param integer The integer to convert.
 * @return String representation of the integer, needs to be free()d.
 */
static inline char *itoa_dm(intmax_t integer)
{
    int negative = integer < 0;
    intmax_t a = negative ? -integer : integer;
    size_t length = negative ? 3 : 2; /* With '\0' and negative sign */
    char *str;

    while ((a /= 10) != 0)
        length += 1;
    str = xmalloc(length);
    snprintf(str, length, "%" PRIdMAX, integer);
    return str;
}

/** Put a variable to the environment.
 *
 * This function does not use psh_vf_stringify, because doing so will cause two
 * different behavior from bash's. One is when an array is exported, the first
 * element is used, while in bash, nothing happens. The other is when exporting
 * references, bash does nothing as well.
 * @param attrib Variable attributes.
 * @param payload Variable value.
 * @param varname Variable name.
 */
static inline void put_to_environ(unsigned int attrib,
                                  const union _psh_vfa_value payload,
                                  const char *varname)
{
    if (attrib & PSH_VFA_INTEGER)
    {
        char *str = itoa_dm(payload.integer);
        psh_backend_setenv(varname, str, 1);
        xfree(str);
    }
    else if (attrib & 0xc0a)
        /* Arrays, references, code, or unset */
        ;
    else
        psh_backend_setenv(varname, payload.string, 1);
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
int psh_vf_set(psh_state *state, const char *varname, unsigned int attrib,
               const union _psh_vfa_value payload, size_t array_size,
               int is_local, int is_func)
{
    struct _psh_vfa_container *container = NULL;
    if (is_local && !(attrib & PSH_VFA_EXPORT))
        container = psh_vf_get(state, varname, 1, is_func);
    else
        container = psh_vf_get(state, varname, 0, is_func);
    /* Put to environ if export */
    if (attrib & PSH_VFA_EXPORT)
        put_to_environ(attrib, payload, varname);
    if (!container)
        /* Such a variable doesn't exist, create new. */
        return psh_vf_add_raw(state, varname, attrib, payload, array_size,
                              is_local, is_func);
    /* Replace original content. */
    clear_single_var(state, container);
    container->payload = payload;
    if (attrib)
        container->attributes = attrib;
    container->array_size = array_size;
    return 0;
}

/* Add a variable, not used for updating. */
int psh_vf_add_raw(psh_state *state, const char *varname, unsigned int attrib,
                   const union _psh_vfa_value payload, size_t array_size,
                   int is_local, int is_func)
{
    struct _psh_vfa_container *container =
        xmalloc(sizeof(struct _psh_vfa_container));
    container->attributes = attrib;
    container->payload = payload;
    container->array_size = array_size;
    if (!attrib)
        /* New variables must have attrib */
        psh_code_fault(state, __FILE__, __LINE__);
    if (is_local && !(attrib & PSH_VFA_EXPORT))
        return psh_hash_add(
            (is_func ? state->contexts[state->context_idx].function_table
                     : state->contexts[state->context_idx].variable_table),
            varname, container, 1);
    return psh_hash_add((is_func ? state->contexts[0].function_table
                                 : state->contexts[0].variable_table),
                        varname, container, 1);
}

/* Get the reference to a variable or a function. Returned value should never be
 * free()d. This function handles variable-by-reference TODO, so all accesses to
 * variables must be made through this function. */
struct _psh_vfa_container *psh_vf_get(psh_state *state, const char *varname,
                                      int force_local, int is_func)
{
    struct _psh_vfa_container *container;
    size_t ctx_idx_searching = state->context_idx + 1;
    if (force_local)
        return psh_hash_get(
            (is_func ? state->contexts[state->context_idx].function_table
                     : state->contexts[state->context_idx].variable_table),
            varname);
    do
    {
        --ctx_idx_searching;
        if ((container = psh_hash_get(
                 (is_func ? state->contexts[ctx_idx_searching].function_table
                          : state->contexts[ctx_idx_searching].variable_table),
                 varname)))
            return container;
    } while (ctx_idx_searching);
    return NULL;
}

/* Clear all variables and functions local to this scope. */
void psh_vf_exit_local(psh_state *state)
{
    if (state->context_idx == 0)
        /* Exiting the root context is not expected to happen. */
        psh_code_fault(state, __FILE__, __LINE__);
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
            return 0;
        }
    } while (ctx_idx_searching);
    return 1;
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

/* Convert variables of various types to a string, need to be free()d. */
char *psh_vf_stringify(psh_state *state,
                       const struct _psh_vfa_container *variable)
{
    if (variable->attributes & 0xc00)
        /* PARSED or UNSET */
        return psh_strdup("");
    if (variable->attributes & PSH_VFA_INDEX_ARRAY)
    {
        if (variable->attributes & PSH_VFA_INTEGER)
            return itoa_dm(variable->payload.int_array[0]);
        if (variable->attributes & PSH_VFA_STRING)
            return psh_strdup(variable->payload.string_array[0]);
        psh_code_fault(state, __FILE__, __LINE__);
    }
    if (variable->attributes & PSH_VFA_ASSOC_ARRAY)
    {
        union _psh_vfa_value *value =
            psh_hash_get_random(variable->payload.assoc_array);
        if (variable->attributes & PSH_VFA_INTEGER)
            return itoa_dm(value->integer);
        if (variable->attributes & PSH_VFA_STRING)
            return psh_strdup(value->string);
        psh_code_fault(state, __FILE__, __LINE__);
    }
    if (variable->attributes & PSH_VFA_REFERENCE) /* TODO: Circular reference */
        return psh_vf_stringify(
            state, psh_vf_get(state, variable->payload.string, 0, 0));
    if (variable->attributes & PSH_VFA_INTEGER)
        return itoa_dm(variable->payload.integer);
    if (variable->attributes & PSH_VFA_STRING)
        return psh_strdup(variable->payload.string);
    psh_code_fault(state, __FILE__, __LINE__);
}