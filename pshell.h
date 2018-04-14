/*
   pshell.h - Psh main header

   Copyright 2013 wuyve.
   Copyright 2017-2018 Zhang Maiyun.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef PSHELL_HEADER_INCLUDED
#define PSHELL_HEADER_INCLUDED

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef NO_READLINE
#include <readline/readline.h>
#endif
#ifndef NO_HISTORY
#include <readline/history.h>
#endif

#define MAX_PROMPT 1024
#define MAXLINE 262144
#define MAXARG 64
#define MAXEACHARG 4096
#define MAXPIDTABLE 1024
#define OUT2E(...) fprintf(stderr, __VA_ARGS__)
#undef strncpy
#define strncpy p_strncpy
#define PSH_VERSION "0.13.0"

struct command /* Everything about a command */
{
	enum flag
	{
		BG_CMD = 1,
		PIPED,
		RUN_AND,
		RUN_OR,
		MULTICMD
	} flag;
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
			FILE *herexx;/* temporary file created to store here document and here string values */
		} in;
		union out /* file than redirect to */
		{
			int fd;
			char *file;
		} out;
		struct redirect *next;
	} * rlist;
	char **parameters; /*argv*/
	struct command *next;
};

extern char *argv0;

void type_prompt(char *);
int read_command(char *prompt, struct command *info);
int run_builtin(struct command *info);
int filpinfo(char *buffer, struct command *info);
size_t p_strncpy(char *dst, const char *src, size_t size);
char *p_fgets(char *prompt, FILE *fp);
char *p_gets(char *prompt);
int new_command(struct command **info);
void free_command(struct command *info);
void code_fault(char *file, int line);
void exit_psh(int status);
int add_atexit_free(void *ptr);
#endif
