/*
    psh/command.h - psh commands
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

#ifndef _PSH_COMMAND_H
#define _PSH_COMMAND_H

#include <stdio.h>

#define MAXLINE 262144
#define MAXARG 64
#define MAXEACHARG 4096

struct redirect
{
    enum redir_type
    {
        FD2FD = 1, /* fd to fd, n>&n; n<&n */
        OUT_REDIR, /* fd to filename,
        n>name; n<name; &>name;  */
        OUT_APPN,
        IN_REDIR, /* filename to fd */
        CLOSEFD,  /* n>&-; n<&- */
        OPENFN,   /* n<>name */
        HEREXX    /* heredoc, herestring */
    } type;
    union in /* file that redirect from */
    {
        int fd;
        char *file;
        FILE *herexx; /* temporary file created to store here document and
                         here string values */
    } in;
    union out /* file that redirect to */
    {
        int fd;
        char *file;
    } out;
    struct redirect *next;
};

struct command /* Everything about a command */
{
    enum flag
    {
        SINGLE = 0,
        BACKGROUND,
        PIPED,
        RUN_AND,
        RUN_OR,
        MULTICMD
    } flag;
    struct redirect *rlist;
    char **argv;
    struct command *next;
};

void redirect_init(struct redirect *redir);
void free_redirect(struct redirect *redir);
int new_command(struct command **info);
void command_init(struct command *info);
void free_command(struct command *info);
void free_argv(struct command *info);

#endif
