/*
 * =====================================================================================
 *	   Filename:  wshell.h
 *	Description:
 *		Version:  1.0
 *		Created:  2013.10.16 20h15min26s
 *		 Author:  wuyue (wy), vvuyve@gmail.com
 *		Company:  UESTC
 * =====================================================================================
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
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
#define PSH_VERSION "0.7.35"

extern char *argv0;
struct parse_info;

char *preprocess_cmdline(char *buffer);
void type_prompt(char*);
int read_command(char **,char **,char*);
int parsing(char **,int,struct parse_info *);
int parse_info_init(struct parse_info *info);
int run_builtin(char *command, char **parameters);
int split_buffer(char **command, char **parameters, char *buffer);

#ifndef STRUCT_PARSE_INFO
#define STRUCT_PARSE_INFO
#define BACKGROUND		0x01
#define IN_REDIRECT		0x02
#define OUT_REDIRECT		0x04
#define OUT_REDIRECT_APPEND	0x08
#define IS_PIPED		0x10
#define RUN_AND			0x20
#define RUN_OR			0x40
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
#endif
