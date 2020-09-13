/** @file psh/command.h - @brief Psh commands */
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

#ifndef _PSH_COMMAND_H
#define _PSH_COMMAND_H

#include <stdio.h>

/** @deprecated Maximum characters in a line */
#define MAXLINE 262144
/** @deprecated Maximum number of arguments */
#define MAXARG 64
/** @deprecated Maximum characters in an argument */
#define MAXEACHARG 4096

/** @brief Redirection of a command.
 * @sa The Open Group Base Specifications Issue 7, 2018 edition, section 2.7
 */
struct redirect
{
    /** @brief The type of this redirection. */
    enum redir_type
    {
        /** Duplicate a file discripter. \n
         * @ref redirect::in::fd -> @ref redirect::out::fd \n
         * forms: [n] > &n or [n] < &n
         * @sa section 2.7.5, 2.7.6
         */
        FD2FD = 1,
        /** Redirecting output. \n
         * @ref redirect::in::fd -> @ref redirect::out::file \n
         * forms: [n] > filename or [n] >| filename
         * @sa section 2.7.2
         */
        OUT_REDIR,
        /** Appending redirected output. \n
         * @ref redirect::in::fd -> @ref redirect::out::file \n
         * forms: [n] >> filename
         * @sa section 2.7.3
         */
        OUT_APPN,
        /** Redirecting input. \n
         * @ref redirect::in::file -> @ref redirect::out::fd \n
         * forms: [n] < filename
         * @sa section 2.7.1
         */
        IN_REDIR,
        /** Close a file descripter. \n
         * @ref redirect::in::fd = -1 -> @ref redirect::out::fd \n
         * forms: [n] < &- \n
         * @ref redirect::in::fd -> @ref redirect::out::fd = -1 \n
         * forms: [n] > &-
         * @sa section 2.7.5, 2.7.6
         */
        CLOSEFD,
        /** Open file for reading and writing. \n
         * @ref redirect::in::file -> @ref redirect::out::fd \n
         * forms: [n] <> filename
         * @sa section 2.7.7
         */
        OPENFN,
        /** Here document and here strings. \n
         * @ref redirect::in::herexx -> @ref redirect::out::fd \n
         * forms: [n] << delimiter \n
         * [n] <<< string
         * @sa section 2.7.4
         */
        HEREXX
    } type; /**< The type of this redirection. */
    /** @brief Input of this redirection no matter what type it is. */
    union in
    {
        /** File descripter. */
        int fd;
        /** File path. */
        char *file;
        /** Temporary file created to store here document and here strings. */
        FILE *herexx;
    } in; /**< The input of this redirection. */
    /** @brief Output of this redirection no matter what type it is. */
    union out
    {
        /** File descripter. */
        int fd;
        /** File path. */
        char *file;
    } out; /**< The output of his redirection. */
    /** Next redirection in the chain. */
    struct redirect *next;
};

/** @brief Everything about a command. */
struct command
{
    /** @brief The type of the command. */
    enum flag
    {
        /** Simple command. \n
         * @sa section 2.9.1
         */
        SINGLE = 0,
        /** Asynchronous list. \n
         * @sa section 2.9.3
         */
        BACKGROUND,
        /** Pipeline. \n
         * @sa section 2.9.2
         */
        PIPED,
        /** AND list. \n
         * @sa section 2.9.3
         */
        RUN_AND,
        /** OR list. \n
         * @sa section 2.9.3
         */
        RUN_OR,
        /** Sequential list. \n
         * @sa section 2.9.3
         */
        MULTICMD
    } flag; /**< The type of this command. */

    /** Redirection sequence. */
    struct redirect *rlist;
    /** List of arguments */
    char **argv;
    /** The next command in the list. */
    struct command *next;
};

/** Initialize a redirect struct.
 *
 * @param redir Pointer to the redirect struct.
 */
void redirect_init(struct redirect *redir);

/** Deallocate a redirect struct.
 *
 * @param redir Pointer to the redirect struct.
 */
void free_redirect(struct redirect *redir);

/** Allocate a command.
 *
 * @return Poinnter to the allocated struct.
 */
struct command *new_command();

/** Initialize a command.
 *
 * @param command Pointer to the redirect struct.
 */
void command_init(struct command *command);

/** Deallocate a command.
 *
 * @param command Pointer to the redirect struct.
 */
void free_command(struct command *command);

/** Free the argv field of a command struct.
 *
 * @param command Pointer to the redirect struct.
 */
void free_argv(struct command *command);

#endif
