/*
  filpinfo - function to fill parse info(merges original preprocesser, splitbuf,
  parser) and some other functions for command magaging

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

#include <ctype.h>
#include "backends/backend.h"
#include "pshell.h"

static int command_init(struct command *info)
{
	info->flag = 0;
	info->rlist = NULL;
	info->parameters = NULL;
	info->next = NULL;
	return 0;
}

/* Returns the nth next of the command base, n begins with 1 */
static struct command *getpos(struct command *base, int n)
{
	int count;
	struct command *info = base;
	for (count = 1; count < n; ++count)
		info = info->next;
	return info;
}

static void free_parameters(struct command *info)
{
	int count;
	for (count = 0; count < MAXARG; ++count)
	{
		if (info->parameters[count] != NULL)
		{
			free(info->parameters[count]);
			info->parameters[count] = NULL;
		}
		else
			break; /* All parameters should be freed after here */
	}
	free(info->parameters);
	info->parameters = NULL;
}

/* Get the index of the last char in the buffer */
static int ignore_IFSs(char *buffer, int count)
{
	do
	{
		if (!buffer[count])/* EOL */
			return -5;
		if (buffer[count] != ' ' &&
		buffer[count] != '\t')
			return --count;
	}while(++count);
	return -6;/* Reaching here impossible */
}
	

/* Malloc a command, enNULL all elements, malloc the first parameter[] */
int new_command(struct command **info)
{
	*info = malloc(sizeof(struct command));
	if ((*info) == NULL)
		return -1;
	command_init(*info);
	(*info)->parameters = malloc(sizeof(char *) * MAXARG);
	if ((*info)->parameters == NULL)
	{
		free(*info);
		return -1;
	}
	memset((*info)->parameters, 0,
	       MAXARG); /* This will be used to detect whether an element is
			   used or not */
	(*info)->parameters[0] = malloc(sizeof(char) * MAXEACHARG);
	if ((*info)->parameters[0] == NULL)
	{
		free((*info)->parameters);
		free(*info);
		return -1;
	}
	memset((*info)->parameters[0], 0, MAXEACHARG);
	return 0;
}

/* Free a command and its nexts */
void free_command(struct command *info)
{
	struct command *temp;
	while (info != NULL)
	{
		temp = info;
		info = info->next;
		free_parameters(temp);
		free(temp);
		temp = NULL;
	}
}

/* Malloc and fill a command with a buffer, return characters processed */
int filpinfo(char *buffer, struct command *info)
{
#define ignIFS() \
	do\
	{\
		int tmp;\
		if((tmp=ignore_IFSs(buffer, count))==-5)\
			goto done;\
		count=tmp;\
	}while(0)

#define ignIFS_from_next_char() \
	do\
	{\
		int tmp;\
		if((tmp=ignore_IFSs(buffer, ++count))==-5)\
			goto done;\
		count=tmp;\
	}while(0)

#define malloc_one(n)                                                          \
	(getpos(info, pos)->parameters[n]) =                                   \
	    malloc(sizeof(char) * MAXEACHARG);                                 \
	memset(getpos(info, pos)->parameters[n], 0, MAXEACHARG)

/* Write the current char in buffer to current command, increase retcount
 * only if current not blank or 0 */
#define write_current()                                                        \
	do                                                                     \
	{                                                                      \
		getpos(info, pos)->parameters[paracount][parametercount++] =   \
		    buffer[count];                                             \
		if (strchr(" \t", buffer[count]) == NULL &&                    \
		    buffer[count] != 0) /* current char not blank */           \
			retcount++;                                            \
	} /* Make the semicolon happy */ while (0)

/* Write any char to current command, increase retcount only if c != 0 */
#define write_char(c)                                                          \
	do                                                                     \
	{                                                                      \
		getpos(info, pos)->parameters[paracount][parametercount++] =   \
		    c;                                                         \
		if (strchr(" \t", c) == NULL && c != 0)                        \
			retcount++;                                            \
	} while (0)

#define escape (count!=0 && buffer[count - 1] == '\\')
#define ignore (isInDoubleQuote == 1 || isInSingleQuote == 1 || escape)
	/*
		escape: determine whether the last character is '\\'
		ignore: determine whether a meta character should be ignored(not
	   for a dollar)
	*/

	int len = strlen(buffer);
	int pos = 1;
	int count = 0, parametercount = 0, paracount = 0, retcount = 0;
	int oldpc = 0;
	/*
		count: count for buffer
		parametercount: count for current parameter element
		paracount: count representing how many elements are there in
	   parameter retcount: characters actually wrote to the command,
	   returned
	*/
	int isInSingleQuote = 0, isInDoubleQuote = 0;
	if (info == NULL)
	{
		OUT2E("%s: filpinfo: info is NULL\n", argv0);
		return -1;
	}
	ignIFS();
	++count;
	/* The input command should be initialized */
	for (; count < len; ++count)
	{
		switch (buffer[count])
		{
			case '\'':
				if (isInDoubleQuote ==
				    1) /* Already in a "" quote, just write a '
					*/
					write_current();

				else if (isInSingleQuote ==
					 1) /* Get out of the quote */
					isInSingleQuote = 0;
				else
				{
					if (count == 0)
						isInSingleQuote = 1;
					else if (!escape)
						isInSingleQuote = 1;
					else /* count != 0 && buffer[count-1] ==
						'\\' */
						/* Write a ' */
						write_current();
				}
				break;
			case '"':
				if (isInSingleQuote == 1)
					write_current();
				else
				{
					if (isInDoubleQuote == 1)
						if (escape)
							write_current();
						else
							isInDoubleQuote = 0;
					else if (escape)
						write_current();
					else
						isInDoubleQuote = 1;
				}
				break;
			case '\t':
			case ' ':
				if (ignore)
					write_current();
				else
				{
					ignIFS();
					write_char(0);
					paracount++;
					oldpc=parametercount;
					parametercount = 0;
					malloc_one(paracount);
				}
				break;
			case '&':
				if (ignore)
					write_current();
				else
				{
					if(parametercount == 0)/* Previously a blank reached */
					{
						parametercount=oldpc;
						free(getpos(info, pos)->parameters[paracount]);
						getpos(info, pos)->parameters[paracount] = NULL;
						paracount--;
					}
					if (ignore_IFSs(buffer, count + 1/* the char after & */) == -5)/* EOL */
					{
						/* done */ 
						info->flag |= BACKGROUND;/* cmd & \0 */
						goto done;
					}
					else if (buffer[count + 1] == '&')
					{
						if(new_command(&(getpos(info, pos)->next)) == -1)
						{
							OUT2E("%s: filpinfo: malloc failed\n", argv0);
							retcount = -1;
							goto done;
						}
						info->flag |= RUN_AND;
						if (ignore_IFSs(buffer, count + 2/* the char after || */) == -5)/* EOL */
						{
							char *cmdand_buf;
#ifdef NO_READLINE
							cmdand_buf = malloc(MAXLINE);
							printf("> ");
							fgets(cmdand_buf,
							      MAXLINE, stdin);
#else
							cmdand_buf =
							    readline("> ");
							buffer = realloc(buffer, strlen(buffer) + strlen(cmdand_buf) +1 /* \0 */);
#endif
							strncat(buffer,
								cmdand_buf,
								MAXLINE -
								    count - 1);
							free(cmdand_buf);
						}
					}
					else
					{
						if(new_command(&(getpos(info, pos)->next)) == -1)
						{
							/* malloc failed, cleanup */
							OUT2E("%s: filpinfo: malloc failed\n", argv0);
							retcount = -1;
							goto done;
						}
						info->flag |= BACKGROUND;
					}
					pos++;
					paracount = 0;
					parametercount = 0;
					ignIFS_from_next_char();
				}
				break;
			case '|':
				if (ignore)
					write_current();
				else
				{
					if(parametercount == 0)/* Previously a blank reached */
					{
						parametercount=oldpc;
						free(getpos(info, pos)->parameters[paracount]);
						getpos(info, pos)->parameters[paracount] = NULL;
					}
					if(new_command(&(getpos(info, pos)->next)) == -1)
					{
						/* malloc failed, cleanup */
						OUT2E("%s: filpinfo: malloc failed\n", argv0);
						retcount = -1;
						goto done;
					}
					if (buffer[count + 1] == '|')
					{
						info->flag |= RUN_OR;
						if (ignore_IFSs(buffer, count + 2/* the char after || */) == -5)/* EOL */ 
						{
							char *cmdor_buf;
#ifdef NO_READLINE
							cmdor_buf = malloc(MAXLINE);
							printf("> ");
							fgets(cmdor_buf,
							      MAXLINE, stdin);
#else
							cmdor_buf =
							    readline("> ");
							buffer = realloc(buffer, strlen(buffer) + strlen(cmdor_buf) +1 /* \0 */);
#endif
							strncat(
							    buffer, cmdor_buf,
							    MAXLINE - count -
								1) /*\0*/;
							free(cmdor_buf);
						}
					}
					else
					{
						info->flag |= IS_PIPED;
						if (ignore_IFSs(buffer, count + 2/* the char after | */) == -5)/* EOL */
						{
							char *pipe_buf;
#ifdef NO_READLINE
							pipe_buf = malloc(MAXLINE);
							printf("> ");
							fgets(pipe_buf, MAXLINE,
							      stdin);
#else
							pipe_buf =
							    readline("> ");
							buffer = realloc(buffer, strlen(buffer) + strlen(pipe_buf) +1 /* \0 */);
#endif
							strncat(buffer,
								pipe_buf,
								MAXLINE -
								    count - 1);
							free(pipe_buf);
						}
					}
					pos++;
					paracount = 0;
					parametercount = 0;
					ignIFS_from_next_char();
				}
				break;
			case '~': /* This feature stable */
				if (ignore)
				{
					write_current();
					break;
				}
				if (buffer[count + 1] != 0 &&
				    buffer[count + 1] != '\n' &&
				    buffer[count + 1] != '\t' &&
				    buffer[count + 1] != ' ' &&
				    buffer[count + 1] != '/') /* ~username */
				{
					char *username =
					    malloc(sizeof(char) * 256);
					char *posit;
					strncpy(username, &(buffer[count + 1]),
						256);
					posit = strchr(username, '/');
					if (posit != NULL)
					{
						while (
						    --posit !=
						    username) /* Remove blank */
							if (*posit != ' ' &&
							    *posit != '\t')
							{
								*(++posit) =
								    0; /* Terminate
									  the
									  string
									*/
								break;
							}
					}
					else
					{
						int usernamelen =
						    strlen(username);
						for (--usernamelen;
						     usernamelen != 0;
						     --usernamelen)
							if (username[usernamelen] !=
								' ' &&
							    username[usernamelen] !=
								'\t')
							{
								username
								    [++usernamelen] =
									0; /* Terminate
									      the
									      string*/
								break;
							}
					}
					char *hdir = gethdnam(username);
					if (hdir == NULL)
					{
						/* No such user, treat as a
						 * normal ~ as in bash */
						write_current();
						break;
					}
					strncpy(info->parameters[paracount],
						hdir, 4094 - parametercount);
					count += strlen(username);
					parametercount += strlen(hdir);
					free(username);
				}
				else /* ~/ and ~ */
				{
					char *hdir = gethd();
					strncpy(info->parameters[paracount],
						hdir, 4094 - parametercount);
					parametercount += strlen(hdir);
				}
				break;
			case '\\':
			{
				int case_count;
				for (case_count = 1; buffer[count];
				     ++case_count, ++count)
				{
					if (buffer[count] != '\\')
					{
						--count;
						break;
					}
					else /* Print the '\' at a even
						location, and ignore the odd
						ones, the same behavior as in
						bash */
					{
						if (case_count &
						    1) /* Odd number */
							continue;
						else /* Even number */
							write_current();
					}
				}
			}
			case '`':
				/* TODO: Write command substitude code here */

			case '$':
			/* TODO: Write variable, variable cut,
			 * ANSI-C style escape, command substitude,
			 * arithmetic expansion code here */
			case '>':
				if (ignore)
				{
					write_current();
					break;
				}
			case '<':
				/* TODO: Write input redirect and heredoc code
				 * here */
				write_current();
				break;
			case '#':
				if (ignore)
				{
					write_current();
					break;
				}
				write_char(0);
				return retcount;
			case '(':
			case ')':
			/* TODO: Write command sequence code here */
			case ';':
			/* TODO: Write muiltiple command process code here */
			default:
				write_current();
		}
	}
done:
	write_char(0);
	return retcount;
}
