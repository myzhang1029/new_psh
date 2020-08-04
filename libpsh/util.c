/*
    libpsh/util.c - libpsh utilities for everyone
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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
        size_t charcount = 0, nowhave = 256;
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

/* Compare the string str1 to a string as were produced by strcat(str2_1,
 * str2_2). */
int strdblcmp(const char *str1, const char *str2_1, const char *str2_2)
{
    int diff;
    /* End loop if str2_1 is empty */
    while (*str2_1)
    {
        printf("%d %d\n", *str1, *str2_1);
        diff = *(str1++) - *(str2_1++);
        if (diff)
            return diff;
    }
    do
    {
        printf("%d %d\n", *str1, *str2_2);
        diff = *str1 - *str2_2;
        if (diff)
            return diff;
    } while (*(str2_2++) && *(str1++));
    return 0;
}

char *psh_strdup(const char *str)
{
    size_t length = strlen(str) + 1;
    char *dest = xmalloc(P_CS * length);
    memmove(dest, str, length);
    return dest;
}

/* Get a string from FUNC, where the first argument is a string pointer,
 * while the second one is the length of the buffer.
 * Original function's return value is copied into *RESULT.
 * If that doesn't matter, use NULL, and RESULT will be left untouched.
 * returned pointer needs to be free()d
 */
char *psh_getstring(void *(*func)(char *, size_t), void **result)
{
    size_t len = 256;
    char *oldtry = NULL, *newtry = NULL;
    for (;;)
    {
        newtry = xmalloc(P_CS * len);
        if (result)
            *result = (*func)(newtry, len);
        else
            (*func)(newtry, len);
        if (oldtry && strcmp(oldtry, newtry) == 0) /* Identical */
            break;
        xfree(oldtry);
        oldtry = newtry;
        len *= 2;
    }
    xfree(oldtry);
    newtry = xrealloc(newtry, strlen(newtry) + 1);
    return newtry;
}
