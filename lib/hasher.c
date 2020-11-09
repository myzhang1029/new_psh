/* hashlib.c -- functions to manage and access hash tables for bash. */
/* hasher.c -- function to generate hash for psh. */

/* Copyright (C) 1987,1989,1991,1995,1998,2001,2003,2005,2006,2008,2009 Free
   Software Foundation, Inc. Copyright (C) 2017-2020 Zhang Maiyun.

   This file is part of GNU Bash, the Bourne Again SHell.

   Bash is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Bash is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Bash.  If not, see <http://www.gnu.org/licenses/>.
*/

/* Originally found in bash source code `hashlib.c`.
 * Edits:
 *	- Rename function and use new-style argument list declaration.
 *  - Add macros to select appropriate constants according to the bitness.
 * */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stddef.h>
#include <stdint.h>

/* This is the best 32-bit string hash function I found. It's one of the
   Fowler-Noll-Vo family (FNV-1).

   The magic is in the interesting relationship between the special prime
   16777619 (2^24 + 403) and 2^32 and 2^8. */

#if SIZE_MAX == UINT64_MAX
/* If you want to use 64 bits, use */
#define FNV_OFFSET 14695981039346656037U
#define FNV_PRIME 1099511628211U
#elif SIZE_MAX == UINT32_MAX
#define FNV_OFFSET 2166136261U
#define FNV_PRIME 16777619U
#else
#error                                                                         \
    "Not 32-bit or 64-bit. Please edit this file and choose whatever makes sense."
#endif

/* The `khash' check below requires that strings that compare equally with
   strcmp hash to the same value. */
size_t hasher(const char *s)
{
    register size_t i;

    for (i = FNV_OFFSET; *s; s++)
    {
        i *= FNV_PRIME;
        i ^= *s;
    }

    return i;
}