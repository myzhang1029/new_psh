/*
   pshell.h - Psh main header

   Copyright 2013 wuyve.
   Copyright 2017 Zhang Maiyun.

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
#define strncpy p_sstrncpy
#define PSH_VERSION "0.12.2"

#define BACKGROUND 0x01		 /*cmd&*/
#define IN_REDIRECT 0x02	 /*cmd<f*/
#define OUT_REDIRECT 0x04	/*cmd>f*/
#define OUT_REDIRECT_APPEND 0x08 /*cmd>>f*/
#define IS_PIPED 0x10		 /*cmd|cmd*/
#define RUN_AND 0x20		 /*cmd&&cmd*/
#define RUN_OR 0x40		 /*cmd||cmd*/
#define HEREDOC 0x80		 /*cmd<<id*/

struct command
{
	int flag;
	struct redirect
	{
		union in
		{
			int fd;
			char *file;
		}in;
		union out
		{
			int fd;
			char *file;
		}out;
		struct redirect *next;
	}*rlist;
	char **parameters; /*argv*/
	struct command *next;
};

extern char *argv0;

void type_prompt(char *);
int read_command(char *prompt, struct command *info);
int run_builtin(struct command *info);
int filpinfo(char *buffer, struct command *info);
size_t p_sstrncpy(char *dst, const char *src, size_t size);
int new_command(struct command **info);
void free_command(struct command *info);
void exit_psh(int status);
#endif
