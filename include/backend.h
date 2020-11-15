/** @file psh/backend.h - @brief Backend definitions of psh */
/*

   Copyright 2017-2020 Zhang Maiyun.

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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.*/

#ifndef _PSH_BACKEND_H
#define _PSH_BACKEND_H

#include <stdio.h>

#include "command.h" /* For struct command */

/** The exit status of the previous command, $? */
extern int last_command_status;
/** @deprecated Maximum number of background jobs. */
#define MAXPIDTABLE 1024

/** Platform dependent shell initializaion.
 *
 * @return A non-zero return value aborts shell shartup.
 */
int psh_backend_prepare(void);

/** Get the home directory of current user.
 *
 * @return Path of the home directory, no need to free.
 */
char *psh_backend_get_homedir(void);

/** Get the home directory of a user.
 *
 * @param username The username of which we want to know about.
 * @return Path of the home directory, no need to free.
 */
char *psh_backend_get_homedir_username(char *username);

/** Get current user's username.
 *
 * @return Username, no need to free.
 */
char *psh_backend_get_username(void);

/** Get the pathname of the current working directory.
 *
 * @param buffer Buffer to which the path will be stored.
 * @param size Size of BUFFER.
 * @return NULL if the directory couldn't be determined or SIZE was too small,
 * BUF if successful.
 */
char *psh_backend_getcwd(char *buffer, size_t size);

/** Get the pathname of the current working directory.
 *
 * @return The current working directory, needs to be free()d.
 */
char *psh_backend_getcwd_dm(void);

/** Get host name.
 *
 * @param buffer Buffer to which the host name will be stored.
 * @param size Size of BUFFER.
 * @return NULL if the directory couldn't be determined or SIZE was too small,
 * BUF if successful.
 */
int psh_backend_gethostname(char *buffer, size_t size);

/** Get host name.
 *
 * @return The host name, needs to be free()d.
 */
char *psh_backend_gethostname_dm(void);

/** Set environmental variable.
 *
 * @param name The name of the variable.
 * @param value The corresponding value.
 * @param overwrite Whether we can overwrite existing value.
 * @return Zero if succeeded.
 */
int psh_backend_setenv(const char *name, const char *value, int overwrite);

/** Get the user id of the current user.
 *
 * @return UID.
 */
int psh_backend_getuid(void);

/** Change working directory.
 *
 * @return Zero if succeeded.
 */
int psh_backend_chdir(char *);

/** Run a command.
 *
 * @param command The command struct about command details.
 * @return Zero if succeeded.
 */
int psh_backend_do_run(struct command *command, char __verbose);

#endif /* _PSH_BACKEND_H*/
