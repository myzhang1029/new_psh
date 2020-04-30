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

#include "backends/backend.h"
#include "pshell.h"

/* Expands $PS1-4 */
char *ps_expander(char *prompt, char *result)
{

}

void show_prompt(char *prompt)
{
    char *ps1 = "\u@\h:\w\$";/* TODO: Actually get $PS1 after #8 */
    char *expanded = malloc();
}
