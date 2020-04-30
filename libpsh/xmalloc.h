/*
    libpsh/xmalloc.h - safe versions of malloc and realloc
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
    along with this program.  If not, see <https://www.gnu.org/licenses/>..

*/

#ifndef _LIBPSH_XMALLOC_H
#define _LIBPSH_XMALLOC_H

/* sizeof char. Keep this although c99 says it's always 1. */
#define P_CS sizeof(char)

/* Generic pointer type. */
#include <stddef.h>

#ifndef PTR_T
#  if defined (__STDC__)
#    define PTR_T void *
#  else
#    define PTR_T char *
#  endif
#endif /* PTR_T */
PTR_T xmalloc (size_t bytes);
PTR_T xrealloc (PTR_T pointer, size_t bytes);
void xfree (PTR_T pointer);

#endif /* _LIBPSH_XMALLOC_H */
