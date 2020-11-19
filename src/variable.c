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

Different from what bash does, aliases are also psh variables.
To avoid name collisions, functions have a prefix of "f_", aliases have "a_",
and variables have "v_".
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include "libpsh/hash.h"
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"
#include "psh.h"
#include "variable.h"

/* Init the two tables above, and try to get environment parameters */
void psh_variable_init(psh_state *state)
{
    state->variable_table_eg = psh_hash_create(10);
    state->variable_table_l = psh_hash_create(10);
    /* #5 #12 #13 TODO: Retrieve all env vars,
     * for generic, only try to read those important to shell, such as HOME,
     * PATH, etc. */
}

/* Set or update a variable, either name or scope.
 * Moving a variable among scopes should be fast, as they should be only pointer
 * operations. */
int psh_variable_set(psh_state *state, const char *varname, const char *value,
                     psh_variable_scope scope)
{
}

/* Get a variable's value, based on scope precedence, i.e. local->global,env */
char *psh_variable_get(psh_state *state, const char *varname) {}

/* Clear all local variables. */
void psh_variable_exit_local(psh_state *state) {}

/* Unset a variable. Removes any matching entry in all three tables. */
void psh_variable_unset(psh_state *state, const char *varname) {}

/* Called upon shell exit, destroy the whole variable database */
void psh_variable_destroy(psh_state *state) {}