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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stddef.h>
#include <string.h>
#ifdef DEBUG
#include <stdio.h>
#endif

#include "libpsh/stringbuilder.h"
#include "libpsh/xmalloc.h"

/* Create a new builder */
psh_stringbuilder *psh_stringbuilder_create()
{
    psh_stringbuilder *builder = xmalloc(sizeof(psh_stringbuilder));
    builder->total_length = 0;
    builder->current = builder->first = NULL;
    return builder;
}

/* Append a string starting at *STRING with a length of LENGTH to the builder.
   STRING gets free()d if IF_FREE is 1 */
const char *psh_stringbuilder_add_length(psh_stringbuilder *builder,
                                         const char *string, size_t length,
                                         int if_free)
{
    struct _psh_sb_item *previous;
    /* Don't waste memory here */
    if (length == 0)
        return string;
    builder->total_length += length;
    if (builder->current)
    {
        /* Not empty */
#ifdef DEBUG
        printf("[psh_stringbuilder_add_length]\n");
        printf("orig this: %p\n", builder->current->next);
#endif
        builder->current->next = xmalloc(sizeof(struct _psh_sb_item));
#ifdef DEBUG
        printf("this: %p\n", builder->current->next);
        printf("last: %p\n", builder->current);
        printf("orig string: %p\n", builder->current->next->string);
        printf("string: %s\n", string);
#endif
        previous = builder->current;
        builder->current = builder->current->next;
    }
    else
    {
        /* Empty */
        builder->first = xmalloc(sizeof(struct _psh_sb_item));
        builder->current = builder->first;
        previous = NULL;
    }
    /* Now current is to be filled */
    builder->current->previous = previous;
    builder->current->length = length;
    builder->current->string = string;
    builder->current->if_free = if_free;
    builder->current->next = NULL;
    /* Now current is filled */
    return string;
}

/* Append STRING to the builder */
const char *psh_stringbuilder_add(psh_stringbuilder *builder,
                                  const char *string, int if_free)
{
    size_t length = strlen(string);
    return psh_stringbuilder_add_length(builder, string, length, if_free);
}

/* Remove the last member of the builder */
void psh_stringbuilder_pop(psh_stringbuilder *builder)
{
    builder->total_length -= builder->current->length;
    if (builder->current->if_free)
        xfree((char *)builder->current->string);
    builder->current = builder->current->previous;
    xfree(builder->current->next);
    builder->current->next = NULL;
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
        memmove(cur_to, cur_from->string, cur_from->length);
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
    struct _psh_sb_item *cur = builder->first;
    while (1)
    {
        if (cur->if_free)
            xfree((char *)cur->string);
        xfree(cur->previous);
        if (!cur->next)
        {
            xfree(cur);
            break;
        }
        cur = cur->next;
    }
    xfree(builder);
}
