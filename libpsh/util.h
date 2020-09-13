/** @file libpsh/util.h - @brief Psh utilities */
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

#ifndef _LIBPSH_UTIL_H
#define _LIBPSH_UTIL_H

#include <stdio.h>

/** Formatted print to stderr. */
#define OUT2E(...) fprintf(stderr, __VA_ARGS__)

/** Print prompt and get a line from @p fp.
 * @details This function always reads a whole line, stops when EOF or '\n' is
 * encountered.
 *
 * @param prompt Prompt to be printed to stdout if @p fp is stdin.
 * @param fp FILE to get string from.
 * @return The line read excluding EOF or newline, needs to be free()d.
 */
char *psh_fgets(char *prompt, FILE *fp);

/** Print prompt and get a line from stdin.
 * @details This function always reads a whole line, stops when EOF or '\n' is
 * encountered.
 *
 * @param prompt Prompt to be printed to stdout if @p fp is stdin.
 * @return The line read excluding EOF or newline, needs to be free()d.
 */
char *psh_gets(char *prompt);

/** Reliable strncpy by which a full copy occurs when @p size is
 * strlen(@ref src)
 *
 * @param dst Pointer to a memory block.
 * @param src Pointer to the string to be copied.
 * @param size Number of characters to copy excluding NUL at most.
 * @return The number of characters copied, excluding NUL.
 */
size_t psh_strncpy(char *dst, const char *src, size_t size);

/** Compare a string and the concatenation of two strings.
 * @details Pseudo code: strdblcmp = strcmp(str1, strcat(str2_1, str2_2))
 *
 * @param str1 The base string to be compared.
 * @param str2_1 The first part of the other string.
 * @param str2_2 The second part of the other string.
 * @return 0 if the strings are identical, otherwise str1[n] - str2[n] for the
 * first different character.
 */
int strdblcmp(const char *str1, const char *str2_1, const char *str2_2);

/** Duplicate a string.
 *
 * @param str The string to duplicate.
 * @return The duplicated string, needs to be free()d.
 */
char *psh_strdup(const char *str);

/** Get a dynamically allocated string from another function.
 * @details A lot of functions take two arguments: a memory pointer and the
 * corresponding size. This function receives the string from such a function.
 * @note This function works by calling @p func multiple times, thepore the
 * value got from @p func shouldn't change between several calls.
 *
 * @param func The function to get a string from.
 * @param result The return value of @p func, set to NULL if it doesn't
 * matter.
 * @return The string got.
 */
char *psh_getstring(void *(*func)(char *, size_t), void **result);

#endif /* _LIBPSH_UTIL_H */
