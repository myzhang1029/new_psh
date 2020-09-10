/*
   psh/backend.h - backend definitions of psh

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

extern int last_command_status;
extern int pipe_fd[2], in_fd, out_fd;
#define MAXPIDTABLE 1024

/* Platform dependent shell initializaion.
 *
 * @return A non-zero return value aborts shell shartup.
 */
int psh_backend_prepare(void);
char *psh_backend_get_homedir(void);
char *psh_backend_get_homedir_username(char *);
char *psh_backend_get_username(void);
char *psh_backend_getcwd(char *, size_t);
char *psh_backend_getcwd_dm(void);
int psh_backend_gethostname(char *, size_t);
char *psh_backend_gethostname_dm(void);
int psh_backend_setenv(const char *, const char *, int);
int psh_backend_do_run(struct command *info);
int psh_backend_getuid(void);
int psh_backend_chdir(char *);

#endif /* _PSH_BACKEND_H*/
