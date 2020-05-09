/*
    libpsh.h - Psh utilities
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

#ifndef _LIBPSH_UTIL_H
#define _LIBPSH_UTIL_H

#include <stdio.h>

#define MAXLINE 262144
#define MAX_PROMPT 1024
#define MAXARG 64
#define MAXEACHARG 4096
#define MAXPIDTABLE 1024
#define OUT2E(...) fprintf(stderr, __VA_ARGS__)

char *psh_fgets(char *prompt, FILE *fp);
char *psh_gets(char *prompt);
size_t psh_strncpy(char *dst, const char *src, size_t size);
char *psh_strdup(const char *str);
char *psh_getstring(void *(*func)(char *, size_t), void **result);

#endif /* _LIBPSH_UTIL_H */
