/** @file libpsh/stringbuilder.h - @brief String builder */
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

#ifndef _LIBPSH_STRINGBUILD_H
#define _LIBPSH_STRINGBUILD_H

/* For size_t */
#include <stddef.h>

/** @brief A single slice of string. */
struct _psh_sb_item
{
    /** Pointer to the start of the string. */
    char *string;
    /** Number of characters to be included. */
    size_t length;
    /** Whether @ref string should be free()d upon deallocation. */
    int if_free;
    /** Next string to process. */
    struct _psh_sb_item *next;
    /** Previous string. */
    struct _psh_sb_item *previous;
};

/** @brief String builder type. */
typedef struct _psh_stringbuilder
{
    /** Total string length of the builder */
    size_t total_length;
    /** Linked-list of strings */
    struct _psh_sb_item *first;
    /** The last non-empty one */
    struct _psh_sb_item *current;
} psh_stringbuilder;

/** Create a new stringbuilder.
 *
 * @return Pointer to the created psh_stringbuilder structure (_the builder_).
 */
psh_stringbuilder *psh_stringbuilder_create();

/** Add a string to the builder.
 *
 * @param builder The string bulider to use.
 * @param string The string to be added.
 * @param length Number of characters to include.
 * @param if_free Whether @ref string should be free()d upon deallocation.
 * @return @ref string.
 */
char *psh_stringbuilder_add_length(psh_stringbuilder *builder, char *string,
                                   size_t length, int if_free);

/** Add a string to the builder.
 *
 * @param builder The string bulider to use.
 * @param string The string to be added.
 * @param if_free Whether @ref string should be free()d upon deallocation.
 * @return @ref string.
 */
char *psh_stringbuilder_add(psh_stringbuilder *builder, char *string,
                            int if_free);

/** Remove the last string from the builder.
 *
 * @param builder The string bulider to operate.
 */
void psh_stringbuilder_pop(psh_stringbuilder *builder);

/** Create string from the builder.
 *
 * @param builder The string bulider to operate.
 * @return The created string, need to be free()d.
 */
char *psh_stringbuilder_yield(psh_stringbuilder *builder);

/** Deallocate a builder.
 *
 * @param builder The string bulider to operate.
 */
void psh_stringbuilder_free(psh_stringbuilder *builder);
#endif
