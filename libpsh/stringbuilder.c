/*
    libpsh/stringbuilder.c - string builder
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

#include <memory.h>
#include <stddef.h>

#include "libpsh/stringbuilder.h"
#include "libpsh/xmalloc.h"

/* Creater a new builder */
psh_stringbuilder *psh_stringbuilder_create()
{
    psh_stringbuilder *builder = xmalloc(sizeof(psh_stringbuilder));
    builder->total_length = 0;
    builder->first = xmalloc(sizeof(struct _psh_sb_item));
    builder->current = builder->first = NULL;
    return builder;
}

/* Append a string starting at *STRING with a length of LENGTH to the builder.
   STRING gets free()d if IF_FREE is 1 */
char *psh_stringbuilder_add_length(psh_stringbuilder *builder, char *string, size_t length, int if_free)
{
    builder->total_length += length;
    if (builder->current)
    {
        /* Not empty */
        builder->current->next = xmalloc(sizeof(struct _psh_sb_item));
        builder->current = builder->current->next;
    }
    else
    {
        /* Empty */
        builder->first = xmalloc(sizeof(struct _psh_sb_item));
        builder->current = builder->first;
    }
    /* Now current is empty */
    builder->current->length = length;
    builder->current->string = string;
    builder->current->if_free = if_free;
    builder->current->next = NULL;
    /* Now current is filled */
    return string;
}

/* Append STRING to the builder */
char *psh_stringbuilder_add(psh_stringbuilder *builder, char *string, int if_free)
{
    size_t length = strlen(string);
    return psh_stringbuilder_add_length(builder, string, length, if_free);
}

/* Generate a string from the builder */
char *psh_stringbuilder_yield(psh_stringbuilder *builder)
{
    struct _psh_sb_item *cur_from = builder->first;
    char *result = xmalloc(P_CS * builder->total_length + 1);
    char *cur_to = result;
    while (cur_from)
    {
        /* Copy the string without trailing NUL */
        memcpy(cur_to, cur_from->string, cur_from->length);
        /* Increase result pointer */
        cur_to += cur_from->length;
        /* Get next string */
        cur_from = cur_from->next;
    }
    /* NUL-terminate */
    result[builder->total_length] = 0;
    return result;
}

/* Free resources used by the builder */
void psh_stringbuilder_free(psh_stringbuilder *builder)
{
    struct _psh_sb_item *tmp, *cur = builder->first;
    while (cur)
    {
        tmp = cur;
        cur = cur->next;
        if (tmp->if_free)
            xfree(tmp->string);
        xfree(tmp);
    }
    xfree(builder);
}
