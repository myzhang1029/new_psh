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

#include <stdio.h> /* For FILE */

/** @deprecated Maximum characters in a line */
#define MAXLINE 262144
/** @deprecated Maximum number of arguments */
#define MAXARG 64
/** @deprecated Maximum characters in an argument */
#define MAXEACHARG 4096

/** @brief Redirection of a command.
 * @sa The Open Group Base Specifications Issue 7, 2018 edition, section 2.7
 */
struct _psh_redirect
{
    /** @brief The type of this redirection. */
    enum _psh_redir_type
    {
        PSH_REDIR_NONE = 0,
        /** Duplicate a file discripter. \n
         * @ref redirect::lhs::fd -> @ref redirect::rhs::fd \n
         * forms: [n] > &n or [n] < &n
         * @sa section 2.7.5, 2.7.6
         */
        PSH_REDIR_FD2FD = 1,
        /** Redirecting output. \n
         * @ref redirect::lhs::fd -> @ref redirect::rhs::file \n
         * forms: [n] > filename or [n] >| filename
         * @sa section 2.7.2
         */
        PSH_REDIR_OUT_REDIR,
        /** Appending redirected output. \n
         * @ref redirect::lhs::fd -> @ref redirect::rhs::file \n
         * forms: [n] >> filename
         * @sa section 2.7.3
         */
        PSH_REDIR_OUT_APPN,
        /** Redirecting input. \n
         * @ref redirect::rhs::file -> @ref redirect::lhs::fd \n
         * forms: [n] < filename
         * @sa section 2.7.1
         */
        PSH_REDIR_IN_REDIR,
        /** Close a file descripter. \n
         * @ref redirect::rhs::fd = -1 -> @ref redirect::lhs::fd \n
         * forms: [n] < &- \n
         * @ref redirect::lhs::fd -> @ref redirect::rhs::fd = -1 \n
         * forms: [n] > &-
         * @sa section 2.7.5, 2.7.6
         */
        PSH_REDIR_CLOSEFD,
        /** Open file for reading and writing. \n
         * @ref redirect::rhs::file -> @ref redirect::lhs::fd \n
         * forms: [n] <> filename
         * @sa section 2.7.7
         */
        PSH_REDIR_OPENFN,
        /** Here document and here strings. \n
         * @ref redirect::rhs::herexx -> @ref redirect::lhs::fd \n
         * forms: [n] << delimiter \n
         * [n] <<< string
         * @sa section 2.7.4
         */
        PSH_REDIR_HEREXX
    } type; /**< The type of this redirection. */
    /** @brief Right-hand side of this redirection no matter what type it is. */
    union _psh_redir_rhs
    {
        /** File descripter. */
        int fd;
        /** File path. */
        char *file;
        /** Temporary file created to store here document and here strings. */
        FILE *herexx;
    } rhs; /**< The right operand of this redirection. */
    /** @brief Left-hand side of this redirection no matter what type it is.
     * This side is always the side that needs backing up. */
    union _psh_redir_lhs
    {
        /** File descripter. */
        int fd;
        /** File path. */
        char *file;
    } lhs; /**< The left operand of his redirection. */
    /** Next redirection in the chain. */
    struct _psh_redirect *next;
};

/** @brief The type of the command. */
enum _psh_cmd_type
{
    /** Simple command. \n
     * @sa section 2.9.1
     */
    PSH_CMD_SINGLE = 0,
    /** Asynchronous list. \n
     * @sa section 2.9.3
     */
    PSH_CMD_BACKGROUND,
    /** Pipeline. \n
     * @sa section 2.9.2
     */
    PSH_CMD_PIPED,
    /** AND list. \n
     * @sa section 2.9.3
     */
    PSH_CMD_RUN_AND,
    /** OR list. \n
     * @sa section 2.9.3
     */
    PSH_CMD_RUN_OR,
    /** Sequential list. \n
     * @sa section 2.9.3
     */
    PSH_CMD_MULTICMD
};
/** @brief Everything about a command. */
struct _psh_command
{
    /** The type of this command. */
    enum _psh_cmd_type type;
    /** Redirection sequence. */
    struct _psh_redirect *rlist;
    /** List of arguments */
    char **argv;
    /** The next command in the list. */
    struct _psh_command *next;
};

/** Initialize a redirect struct.
 *
 * @deprecated This function has been removed. Use xcalloc to allocate struct
 * _psh_redirect instead.
 * @param redir Pointer to the redirect struct.
 */
void redirect_init(struct _psh_redirect *redir);

/** Deallocate a redirect struct.
 *
 * @param redir Pointer to the redirect struct.
 */
void free_redirect(struct _psh_redirect *redir);

/** Allocate a command.
 *
 * @return Pointer to the allocated struct.
 */
struct _psh_command *new_command();

/** Initialize a command.
 *
 * @deprecated This function has been removed. Use xcalloc to allocate struct
 * _psh_command instead.
 * @param command Pointer to the redirect struct.
 */
void command_init(struct _psh_command *command);

/** Deallocate a command.
 *
 * @param command Pointer to the redirect struct.
 */
void free_command(struct _psh_command *command);

/** Free the argv field of a command struct.
 *
 * @param command Pointer to the redirect struct.
 */
void free_argv(struct _psh_command *command);
#endif
