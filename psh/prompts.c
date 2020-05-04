/*
    prompts.c - Prompt generator and printer
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
/* `$PS1` escape sequences
\a	bell (ascii 07)
\d	the date in Day Mon Date format
\e	escape (ascii 033)
\h	the hostname up to the first `.'
\H	the hostname
\j	the number of active jobs
\l	the basename of the shell's tty device name
\n	CRLF
\r	CR
\s	the name of the shell
\t	the time in 24-hour hh:mm:ss format
\T	the time in 12-hour hh:mm:ss format
\@	the time in 12-hour hh:mm am/pm format
\A	the time in 24-hour hh:mm format
\D{fmt}	the result of passing FMT to strftime(3)
\u	your username
\v	the version of sh (e.g., 2.00)
\V	the release of sh, version + patchlevel (e.g., 2.00.0)
\w	the current working directory
\W	the last element of $PWD
\!	the history number of this command
\#	the command number of this command
\$	a $ or a # if you are root
\nnn	character code nnn in octal
\\	a backslash
\[	begin a sequence of non-printing chars
\]	end a sequence of non-printing chars
*/

#include <time.h>

#include "backend.h"
#include "libpsh/stringbuilder.h"
#include "libpsh/xmalloc.h"
#include "pshell.h"

/* Expands $PS1-4, result needs to be free()d */
char *ps_expander(char *prompt)
{
    #define reset_start(newloc) cur = start = newloc
    char *result;
    /* Our approach alters the original PROMPT, so duplicate it */
    char *start = pshstrdup(prompt);
    char *save_start = start;
    char *cur = start;
    /* Count of current number of charactors to write */
    int count = 0;
    int is_backslash = 0;
    psh_stringbuilder *builder = psh_stringbuilder_create();

    do
    {
        /* This branching only handles COUNT */
        if (*cur != '\\')/* likely */
            ++count;
        switch (*cur)
        {
            /* XXX: Should this be ordered by freq? */
            case '\\':
                if (is_backslash)
                {
                    /* A backslash was before this backslash */
                    /* Add one of them to the builder */
                    /* Performance will be a issue if people use multiple '\\\\'s */
                    is_backslash = 0;
                    psh_stringbuilder_add_length(builder, start, count+1/* for '/' */, 0);
                    reset_start(++prompt);
                }
                else
                    is_backslash = 1;
                break;
            case 'a':
                if (is_backslash)
                {
                    is_backslash = 0;
                    *(cur-1)/* the '\\' */ = '\a';
                    psh_stringbuilder_add_length(builder, start, count, 0);
                    reset_start(++prompt);
                }
                /* else write a */
                break;
            case 'd':
                if (is_backslash)
                {
                    char *timestr = xmalloc(P_CS * 11);
                    time_t rt;
                    is_backslash = 0;
                    struct tm *ti;

                    time(&rt);
                    ti = localtime(&rt);
                    strftime(timestr, 11, "%a %b %e", ti);

                    psh_stringbuilder_add_length(builder, start, count-1, 0);
                    psh_stringbuilder_add(builder, timestr, 1);
                    reset_start(++prompt);
                }
                /* else write d */
                break;
            case 'e':
            case 'h':
            case 'H':
            case 'j':
            case 'l':
            case 'n':
            case 'r':
            case 's':
            case 't':
            case 'T':
            case '@':
            case 'A':
            case 'D':
            case 'u':
            case 'v':
            case 'V':
            case 'w':
            case 'W':
            case '!':
            case '#':
            case '$':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '0':
            case '[':
            case ']':
                break;
            default:
                /* When the escape is unknown, bash and dash keeps both the 
                   backslash and the charactor. */
                if (is_backslash)
                    is_backslash = 0;
                break;
        }
    } while (++count,*++cur/* when *cur == 0, exit */);
    result = psh_stringbuilder_yield(builder);
    psh_stringbuilder_free(builder);
    xfree(save_start);
    return result;
}

void show_prompt(char *prompt)
{
    char *ps1 = "\\u@\\h:\\w\\$"; /* TODO: Actually get $PS1 after #8 */
    char *expanded = xmalloc(0);
}
