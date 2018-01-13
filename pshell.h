/*
   pshell.h - Psh main header

   Copyright 2013 wuyue.
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
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
#define OUT2E(...) fprintf(stderr,__VA_ARGS__)
#define PSH_VERSION "0.7.42"

#define BACKGROUND		0x01 /*cmd&*/
#define IN_REDIRECT		0x02 /*cmd<f*/
#define OUT_REDIRECT		0x04 /*cmd>f*/
#define OUT_REDIRECT_APPEND	0x08 /*cmd>>f*/
#define IS_PIPED		0x10 /*cmd|cmd*/
#define RUN_AND			0x20 /*cmd&&cmd*/
#define RUN_OR			0x40 /*cmd||cmd*/
#define HEREDOC			0x80 /*cmd<<id*/

struct parse_info
{
	int flag;
	char *in_file;
	char *out_file;
	char *buffer;
	char *command;
	char **parameters;
	struct parse_info *next;
};

extern char *argv0;

char *preprocess_cmdline(char *buffer);
void type_prompt(char*);
int read_command(char **,char **,char*);
int parsing(char **,int,struct parse_info *);
int parse_info_init(struct parse_info *info);
int run_builtin(char *command, char **parameters);
int split_buffer(char **command, char **parameters, char *buffer);

#endif
