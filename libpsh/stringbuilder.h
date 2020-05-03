/*
    libpsh/stringbuilder.h - string builder
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

#ifndef _LIBPSH_STRINGBUILD_H
#define _LIBPSH_STRINGBUILD_H

/* For size_t */
#include <stddef.h>

struct _psh_sb_item
{
    char *string;
    size_t length;
    struct _psh_sb_item *next;
};

typedef struct _psh_stringbuilder
{
    /* Total string length of the builder */
    size_t total_length;
    /* Linked-list of strings */
    struct _psh_sb_item *first;
    /* The last non-empty one */
    struct _psh_sb_item *current;
} psh_stringbuilder;

psh_stringbuilder *psh_stringbuilder_create();
char *psh_stringbuilder_add_length(psh_stringbuilder *builder, char *string, size_t length);
char *psh_stringbuilder_add(psh_stringbuilder *builder, char *string);
char *psh_stringbuilder_yield(psh_stringbuilder *builder);
void psh_stringbuilder_free(psh_stringbuilder *builder);
#endif
