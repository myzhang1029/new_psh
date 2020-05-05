/*
    util.c - libpsh utilities for everyone
    Copyright 2018-2020 Zhang Maiyun

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

#include <stdio.h>
#include <string.h>
#ifndef NO_READLINE
#include <readline/readline.h>
#endif

#include "libpsh/util.h"
#include "libpsh/xmalloc.h"

/* Get a line from file FP with prompt PROMPT.
 * The momory is allocated automatically.
 * Returns the content when an EOF or \n is read,
 * not including the trailing EOF or \n. */
char *psh_fgets(char *prompt, FILE *fp)
{
#ifndef NO_READLINE
    if (fp == stdin)
        return readline(prompt);
#endif
    if (fp == NULL)
        return NULL;
    if (fp == stdin)
        printf("%s", prompt);
    {
        size_t charcount = 0, nowhave = MAXLINE;
        char *result = xmalloc(P_CS * nowhave);
        char *ptr = result;
        if (result == NULL)
            return NULL;
        while (1)
        {
            *ptr = fgetc(fp);
            if (*ptr == EOF)
            {
                if (ptr == result) /* nothing read */
                {
                    xfree(result);
                    return NULL;
                }
                break;
            }
            if (*ptr == '\n')
                break;
            ++ptr;
            if ((++charcount) == nowhave)
                result = xrealloc(result, P_CS * (nowhave <<= 1));
        }
        *ptr = 0; /* Replace EOF or \n with NUL */
        result = xrealloc(result, P_CS * (strlen(result) +
                                          1)); /* Resize the array to minimum */
        return result;
    }
}

char *psh_gets(char *prompt) { return psh_fgets(prompt, stdin); }

size_t psh_strncpy(char *dst, const char *src, size_t size)
{
    register char *d = dst;
    register const char *s = src;
    register size_t n = ++size;

    if (n != 0 && --n != 0)
    {
        do
        {
            if ((*d++ = *s++) == 0)
                break;
        } while (--n != 0);
    }

    if (n == 0)
    {
        if (size != 0)
            *d = '\0';
        while (*s++)
            ;
    }

    return (s - src - 1);
}

char *psh_strdup(const char *str)
{
    size_t length = strlen(str) + 1;
    char *dest = xmalloc(P_CS * length);
    memcpy(dest, str, length);
    return dest;
}