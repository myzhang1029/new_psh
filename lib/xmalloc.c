/* xmalloc.c -- safe versions of malloc and realloc */
/* Define DEBUG to show all pointers manipulated and show call count which
 * should become 1 when the last xfree is called. */

/*
    Copyright (C) 1991-2003 Free Software Foundation, Inc.
    Copyright (C) 2020 Zhang Maiyun

   This file is part of GNU Readline, a library for reading lines
   of text with interactive input and history editing.
   This file is edited from the original implementation to add memory
   debug features and the calloc function to be used in psh.

   Readline is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Readline is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Readline.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>

#include "libpsh/xmalloc.h"

#ifdef DEBUG
/* Sequence counter to detect memory abuse */
int nref = 0;
#endif

/* **************************************************************** */
/*								    */
/*		   Memory Allocation and Deallocation.		    */
/*								    */
/* **************************************************************** */

static void memory_error_and_abort(char *fname)
{
    fprintf(stderr, "%s: out of virtual memory\n", fname);
    exit(2);
}

/* Return a pointer to free()able block of memory large enough
   to hold BYTES number of bytes.  If the memory cannot be allocated,
   print an error message and abort. */
void *xmalloc(size_t bytes)
{
    void *temp;

    temp = malloc(bytes);
#ifdef DEBUG
    fprintf(stderr, "[xmalloc] %p(malloc %d)\n", temp, ++nref);
#endif
    if (temp == 0)
        memory_error_and_abort("xmalloc");
    return (temp);
}

void *xcalloc(size_t nelem, size_t bytes)
{
    void *temp;

    temp = calloc(nelem, bytes);
#ifdef DEBUG
    fprintf(stderr, "[xmalloc] %p(malloc_calloc %d)\n", temp, ++nref);
#endif
    if (temp == 0)
        memory_error_and_abort("xcalloc");
    return (temp);
}

void *xrealloc(void *pointer, size_t bytes)
{
    void *temp;

    temp = pointer ? realloc(pointer, bytes) : malloc(bytes);
#ifdef DEBUG
    fprintf(stderr, "[xmalloc] %p(free_realloc %d)\n", pointer, nref);
    fprintf(stderr, "[xmalloc] %p(malloc_realloc %d)\n", temp, nref);
#endif

    if (temp == 0)
        memory_error_and_abort("xrealloc");
    return (temp);
}

void xfree(const void *string)
{
#if DEBUG
    if (string)
        fprintf(stderr, "[xmalloc] %p(free %d)\n", string, nref--);
#endif
    free((void *)string);
}
