/*
    psh/expansion.c - everything about expansion
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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libpsh/stringbuilder.h"
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"
#include "psh.h"
#include "util.h"
#include "variable.h"

/* Tested */
/** Expand $"", @p start_point pointing to the $. Quotes included in the
 * returning value. */
static char *expand_translation(psh_state *state, const char *start_point,
                                const char **last_char)
{
    /* TODO: Gettext support */
    char *cur, *result;
    int n_backslash = 0;
    cur = result = psh_strdup(start_point + 1); /* '"' */

    while (*++cur)
    {
        if (*cur == '"')
        {
            if (cur[-1] == '\\')
            {
                ++n_backslash;
                memmove(cur - 1, cur, strlen(cur) + 1);
                --cur;
            }
            else
            {
                if (last_char)
                    *last_char = cur - result + start_point;
                return result;
            }
        }
    }
    psh_code_fault(state, __FILE__, __LINE__);
}

#if defined(DEBUG) && defined(TESTING_EXPAND_TRANSLATION)
int main(void)
{
    psh_state state;
    char *val;
    state.argv0 = "test";
    val = expand_translation(&state, "$\"tran\\\"slated\"", NULL);
    puts(val); /* should yield "\"tran\"slated\"""" */
    xfree(val);
    return 0;
}
#endif

/* Tested */
/** Expand ANSI-C Quoting, @sa
 * https://www.gnu.org/software/bash/manual/html_node/ANSI_002dC-Quoting.html
 */
static char *expand_ansi_quote(psh_state *state, const char *start_point,
                               const char **last_char)
{
    /* The result is single-quoted, so the first quote comes from start_point */
    const char *cur_char = ++start_point; /* points to the first '\'' now */
    size_t cur_len = 0;
    int last_char_is_backslash = 0;
    psh_stringbuilder *builder = psh_stringbuilder_create();

#define ANSI_QUOTE_COMMON_ADD(str)                                             \
    do                                                                         \
    {                                                                          \
        psh_stringbuilder_add(builder, (str), 0);                              \
        start_point = cur_char + 1;                                            \
        cur_len = 0;                                                           \
        last_char_is_backslash = 0;                                            \
    } while (0)
    while (1)
    {
        ++cur_len;  /* first-run: 1, for '\'' */
        ++cur_char; /* First-run: points to the char after '\'' */
        switch (*cur_char)
        {
            case 0:
                /* This should not happen. Incomplete patterns should be
                 * trapped by someone else */
                psh_stringbuilder_free(builder);
                psh_code_fault(state, __FILE__, __LINE__);
            case '\\':
                if (last_char_is_backslash)
                {
                    ANSI_QUOTE_COMMON_ADD("\\");
                }
                else
                {
                    psh_stringbuilder_add_length(builder, start_point, cur_len,
                                                 0);
                    last_char_is_backslash = 1;
                    /* It is the escaped character's duty to set start_point
                     */
                }
                break;
            case 'a':
                if (last_char_is_backslash)
                    ANSI_QUOTE_COMMON_ADD("\a");
                break;
            case 'b':
                if (last_char_is_backslash)
                    ANSI_QUOTE_COMMON_ADD("\b");
                break;
            case 'e':
                if (last_char_is_backslash)
                    ANSI_QUOTE_COMMON_ADD("\033");
                break;
            case 'f':
                if (last_char_is_backslash)
                    ANSI_QUOTE_COMMON_ADD("\f");
                break;
            case 'n':
                if (last_char_is_backslash)
                    ANSI_QUOTE_COMMON_ADD("\n");
                break;
            case 'r':
                if (last_char_is_backslash)
                    ANSI_QUOTE_COMMON_ADD("\r");
                break;
            case 't':
                if (last_char_is_backslash)
                    ANSI_QUOTE_COMMON_ADD("\t");
                break;
            case 'v':
                if (last_char_is_backslash)
                    ANSI_QUOTE_COMMON_ADD("\v");
                break;
            case '"':
                if (last_char_is_backslash)
                    ANSI_QUOTE_COMMON_ADD("\"");
                break;
            case '?':
                if (last_char_is_backslash)
                    ANSI_QUOTE_COMMON_ADD("?");
                break;
            case 'c':
                if (last_char_is_backslash)
                {
                    char *c = xmalloc(1 * P_CS);
                    *c = cur_char[1] - 'a' + 1;
                    psh_stringbuilder_add_length(builder, c, 1, 1);
                    ++cur_char;
                    start_point = cur_char + 1;
                    cur_len = 0;
                    last_char_is_backslash = 0;
                }
                break;
            case '\'':
            {
                char *result;
                if (last_char_is_backslash)
                {
                    /* Even '"' cannot escape '\'', to test, try $'"'" in
                     * bash */
                    ANSI_QUOTE_COMMON_ADD("'");
                    break;
                }
                /* This function is guaranteed to return given that the
                 * pattern is complete */
                psh_stringbuilder_add_length(
                    builder, start_point, cur_len + 1 /* Include this '\'' */,
                    0);
                result = psh_stringbuilder_yield(builder);
                psh_stringbuilder_free(builder);
                if (last_char)
                    *last_char = cur_char;
                return result;
            }
            case '0':
                if (last_char_is_backslash)
                {
                    char *num = xmalloc(4 * P_CS), *endptr;
                    psh_strncpy(num, cur_char, 3);
                    /* num is reused to avoid another allocation */
                    num[0] = (char)strtol(num, &endptr, 8);
                    psh_stringbuilder_add_length(builder, num, 1, 1);
                    start_point = cur_char + (endptr - num);
                    cur_char = start_point - 1;
                    cur_len = 0;
                    last_char_is_backslash = 0;
                }
                break;
            case 'x':
                if (last_char_is_backslash)
                {
                    char *num = xmalloc(3 * P_CS), *endptr;
                    psh_strncpy(num, ++cur_char, 2);
                    /* num is reused to avoid another allocation */
                    num[0] = (char)strtol(num, &endptr, 16);
                    psh_stringbuilder_add_length(builder, num, 1, 1);
                    start_point = cur_char + (endptr - num);
                    cur_char = start_point - 1;
                    cur_len = 0;
                    last_char_is_backslash = 0;
                }
                break;
            case 'u':
                /* XXX: UTF-16, unsupported. */
                if (last_char_is_backslash)
                {
                    char *num = xmalloc(5 * P_CS), *endptr;
                    psh_strncpy(num, ++cur_char, 4);
                    /* num is reused to avoid another allocation */
                    num[0] = (int)strtol(num, &endptr, 16);
                    psh_stringbuilder_add_length(builder, num, 1, 1);
                    start_point = cur_char + (endptr - num);
                    cur_char = start_point - 1;
                    cur_len = 0;
                    last_char_is_backslash = 0;
                }
                break;
            case 'U':
                /* XXX: UTF-32, unsupported. */
                if (last_char_is_backslash)
                {
                    char *num = xmalloc(9 * P_CS), *endptr;
                    psh_strncpy(num, ++cur_char, 8);
                    /* num is reused to avoid another allocation */
                    num[0] = (int)strtol(num, &endptr, 16);
                    psh_stringbuilder_add_length(builder, num, 1, 1);
                    start_point = cur_char + (endptr - num);
                    cur_char = start_point - 1;
                    cur_len = 0;
                    last_char_is_backslash = 0;
                }
                break;
            default:;
        }
    }
}

#if defined(DEBUG) && defined(TESTING_EXPAND_ANSI_QUOTE)
int main(void)
{
    psh_state state;
    char *val;
    state.argv0 = "test";
    val = expand_ansi_quote(&state, "$'a\\na'", NULL);
    puts(val); /* should yield "'a<newline>a'" */
    xfree(val);
    val = expand_ansi_quote(&state, "$'a\\012a'", NULL);
    puts(val); /* should yield "'a<newline>a'" */
    xfree(val);
    val = expand_ansi_quote(&state, "$'a\\cja'", NULL);
    puts(val); /* should yield "'a<newline>a'" */
    xfree(val);
    val = expand_ansi_quote(&state, "$'a\\x0aa'", NULL);
    puts(val); /* should yield "'a<newline>a'" */
    xfree(val);
    val = expand_ansi_quote(&state, "$'a\\u000aa'", NULL);
    puts(val); /* should yield "'a<newline>a'" */
    xfree(val);
    val = expand_ansi_quote(&state, "$'a\\U0000000aa'", NULL);
    puts(val); /* should yield "'a<newline>a'" */
    xfree(val);
    return 0;
}
#endif

/** Expand dollar signs. The caller is responsible of supplying a good pattern,
 * or an error will be emitted.
 *
 * @param state Psh internal state
 * @param start_point Pointer to '$'
 * @param[out] last_char Optionally set to the last character belonging to the
 * expansion
 * @return the expanded string, need to be free()d.
 */
static char *expand_dollar(psh_state *state, const char *start_point,
                           const char **last_char)
{
    if (start_point[1] == '\'')
        return expand_ansi_quote(state, start_point, last_char);
    if (start_point[1] == '{')
    {
        const char *base_name_start = (start_point += 2);
        char *base_name;
        struct _psh_vfa_container *value;
        while (*start_point)
        {
            switch (*start_point)
            {
                case ':':
                    if (start_point == base_name)
                    {
                        OUT2E("%s: %s: bad substitution\n", state->argv0,
                              start_point);
                        return NULL;
                    }
                    base_name =
                        xmalloc((start_point - base_name_start + 1) * P_CS);
                    psh_strncpy(base_name, base_name_start,
                                start_point - base_name_start);
                    value = psh_vf_get(state, base_name, 0, 0);
                    if (start_point[1] == '-')
                        if (value)
                            ;
                case '/':
                case '#':
                case '%':
                case '^':
                case '@':
                default:
                    ++start_point;
            }
        }
    }
    if (start_point[1] == '"')
        return expand_translation(state, start_point, last_char);

    if (start_point[1] == '(')
        ; /* TODO */
}
/* Parameter, command, and arithmetic expansion */
char *psh_expand_parameter(psh_state *state, const char *oldstring)
{
    psh_stringbuilder *builder = psh_stringbuilder_create();
    size_t cur_len = 0;

    while (*oldstring)
    {
        /* First iter: cur_len becomes 1 */
        ++cur_len;
        if (*oldstring == '$')
        {
            char *this_string;
            const char *endpoint;
            if (!oldstring[1])
                /* Terminating at $ */
                break;
            this_string = expand_dollar(state, oldstring, &endpoint);
            psh_stringbuilder_add(builder, this_string, 1);
            oldstring = endpoint;
            cur_len = 0;
        }
        if (*oldstring == '`')
            ;
        ++oldstring;
    }
}