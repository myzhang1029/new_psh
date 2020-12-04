/*
   psh/parser.c - the parser and the tokenizer
   Copyright 2018-2020 Zhang Maiyun.

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stddef.h>

#include "backend.h"
#include "libpsh/xmalloc.h"
#include "parser.h"
#include "psh.h"

psh_token *parse(char *token) { return NULL; }
char **expand_and_tokenize(psh_state *state, const char *buffer,
                           struct _psh_tokenize_state **ptokenize_state)
{
    struct _psh_tokenize_state *tokenize_state;
    if (!*ptokenize_state)
    {
        tokenize_state = *ptokenize_state =
            xmalloc(sizeof(struct _psh_tokenize_state));
        tokenize_state->result;
        /* TODO: init result */
    }
    /* TODO: tokenize, if not quoted to double-quoted, expand parameter, if not
     * quoted, re-split words. */
}