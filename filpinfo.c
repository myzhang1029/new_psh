/*
  filpinfo - function to fill parse info(merges original preprocesser, splitbuf,
  parser) and some other functions for command magaging

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

#include <ctype.h>
#include "backends/backend.h"
#include "pshell.h"

static void command_init(struct command *info)
{
	info->flag = 0;
	info->rlist = NULL;
	info->parameters = NULL;
	info->next = NULL;
}

static void redirect_init(struct redirect *redir)
{
	redir->in.fd = 0;
	redir->in.file = NULL;
	redir->out.fd = 0;
	redir->out.file = NULL;
	redir->type = 0;
	redir->next = NULL;
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

static void free_redirect(struct redirect *redir)
{
	struct redirect *temp;
	while (redir != NULL)
	{
		temp = redir;
		redir = redir->next;
		free(temp);
		temp = NULL;
	}
}

/* Get the index of the last char in the buffer */
static int ignore_IFSs(char *buffer, int count)
{
	do
	{
		if (!buffer[count]) /* EOL */
			return -5;
		if (buffer[count] != ' ' && buffer[count] != '\t')
			return --count;
	} while (++count);
	return -6; /* Reaching here impossible */
}

/* Malloc a command, enNULL all elements, malloc the first parameter[] and a
 * struct redirect */
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
	memset((*info)->parameters, 0, MAXARG); /* This will be used to detect whether an element is
						   used */
	(*info)->parameters[0] = malloc(sizeof(char) * MAXEACHARG);
	if ((*info)->parameters[0] == NULL)
	{
		free((*info)->parameters);
		free(*info);
		return -1;
	}
	memset((*info)->parameters[0], 0, MAXEACHARG);
	(*info)->rlist = malloc(sizeof(struct redirect));
	redirect_init((*info)->rlist);
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
		free_redirect(temp->rlist);
		free(temp);
		temp = NULL;
	}
}

/* Malloc and fill a command with a buffer, free() buffer, return characters
 * processed */
int filpinfo(char *buffer, struct command *info)
{
/* Report a syntax error bashly */
#define synerr(token) OUT2E("%s: syntax error near unexpected token `%s'\n", argv0, (token))

/* Increase cnt_buffer to the last space from buffer[cnt_buffer] */
#define ignIFS()                                                                                                       \
	do                                                                                                             \
	{                                                                                                              \
		int tmp;                                                                                               \
		if ((tmp = ignore_IFSs(buffer, cnt_buffer)) == -5)                                                     \
			goto done;                                                                                     \
		cnt_buffer = tmp;                                                                                      \
	} while (0)

/* Increase cnt_buffer to the last space from buffer[cnt_buffer+1] */
#define ignIFS_from_next_char()                                                                                        \
	do                                                                                                             \
	{                                                                                                              \
		int tmp;                                                                                               \
		if ((tmp = ignore_IFSs(buffer, ++cnt_buffer)) == -5)                                                   \
			goto done;                                                                                     \
		cnt_buffer = tmp;                                                                                      \
	} while (0)

/* malloc() and zero-initialize an element in parameters[][] */
#define malloc_one(n) (cmd_lastnode->parameters[n]) = calloc(MAXEACHARG, sizeof(char))

/* Write the current char in buffer to current command, increase cnt_return
 * only if current is neither blank nor 0 */
#define write_current()                                                                                                \
	do                                                                                                             \
	{                                                                                                              \
		if (stat_parsing_redirect == 0)                                                                        \
			cmd_lastnode->parameters[cnt_argument_element][cnt_argument_char++] = buffer[cnt_buffer];      \
		if (stat_parsing_redirect == 1) /* for an fd */                                                        \
		{                                                                                                      \
			if (redir_lastnode == NULL)                                                                    \
			{                                                                                              \
				redir_lastnode = malloc(sizeof(struct redirect));                                      \
				redirect_init(redir_lastnode);                                                         \
			}                                                                                              \
			if (!isdigit(buffer[cnt_buffer]))                                                              \
			{                                                                                              \
				OUT2E("%s: %c: Digit input required\n", argv0, buffer[cnt_buffer]);                    \
				cnt_return = -2;                                                                       \
				goto done;                                                                             \
			}                                                                                              \
		}                                                                                                      \
		if (strchr(" \t", buffer[cnt_buffer]) == NULL && buffer[cnt_buffer] != 0) /* current char not blank */ \
			cnt_return++;                                                                                  \
	} while (0) /* Make the semicolon happy */

/* Write any char to current command, increase cnt_return only if c != 0 */
#define write_char(c)                                                                                                  \
	do                                                                                                             \
	{                                                                                                              \
		cmd_lastnode->parameters[cnt_argument_element][cnt_argument_char++] = c;                               \
		if (strchr(" \t", c) == NULL && c != 0)                                                                \
			cnt_return++;                                                                                  \
	} while (0)

#define escape (cnt_buffer != 0 && buffer[cnt_buffer - 1] == '\\')
#define ignore (stat_in_dquote == 1 || stat_in_squote == 1 || escape)
	/*
		escape: determine whether last character is '\\'
		ignore: determine whether a meta character should be ignored(not
	   for a dollar sign)
	*/
	struct command *cmd_lastnode = info /* The last node of the command list */;
	struct redirect *redir_lastnode = info ? info->rlist : NULL;
	int stat_in_squote = 0, stat_in_dquote = 0, stat_parsing_redirect = 0;
	int cnt_buffer = 0, cnt_argument_char = 0, cnt_argument_element = 0, cnt_return = 0, cnt_old_parameter = 0,
	    cnt_first_nonIFS = 0;
	/*
	-- Variable prefixes:
		- cnt: count;
		- stat: status.
	-- Variable description:
		- stat_in_squote: whether in a '' quote;
		- stat_in_dquote: whether in a "" quote;
		- stat_parsing_redirect:
		x = 0: Not parsing for redirect;
		x = 1: Parsing for a fd (like 3>&1, 2<&7);
		x = 2: Parsing for a filename (like 1>output, 2>/dev/null);
		- cnt_buffer: count for buffer;
		- cnt_argument_char: count for current parameter element;
		- cnt_argument_element: count representing how many elements are there in parameter;
		- cnt_return: characters actually wrote to the command, returned;
		- cnt_old_parameter: saved cnt_argument_char for undo IFS delim;
		- cnt_first_nonIFS: the first non-IFS char in buffer.
	*/
	/* The input command should be initialized in main.c, otherwise report a programming error */
	if (info == NULL)
		code_fault(__FILE__, __LINE__);
	ignIFS(); /* Ignore starting spaces */
	cnt_first_nonIFS = ++cnt_buffer;
	do
	{
		switch (buffer[cnt_buffer])
		{
			case '\'':
				if (stat_in_dquote == 1) /* Already in a "" quote, just write a '
							  */
					write_current();

				else if (stat_in_squote == 1) /* Get out of the quote */
					stat_in_squote = 0;
				else
				{
					if (cnt_buffer == cnt_first_nonIFS)
						stat_in_squote = 1;
					else if (!escape)
						stat_in_squote = 1;
					else /* not the first char and a '\\' is there */
						/* Write a ' */
						write_current();
				}
				break;
			case '"':
				if (stat_in_squote == 1)
					write_current();
				else
				{
					if (stat_in_dquote == 1)
						if (escape)
							write_current();
						else
							stat_in_dquote = 0;
					else if (escape)
						write_current();
					else
						stat_in_dquote = 1;
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
					cnt_argument_element++;
					cnt_old_parameter = cnt_argument_char;
					cnt_argument_char = 0;
					malloc_one(cnt_argument_element);
				}
				break;
			case '&':
				if (ignore)
					write_current();
				else
				{
					if (cnt_argument_char == 0) /* Previously a blank reached */
					{
						cnt_argument_char = cnt_old_parameter;
						free(cmd_lastnode->parameters[cnt_argument_element]);
						cmd_lastnode->parameters[cnt_argument_element] = NULL;
						cnt_argument_element--;
					}
					if (ignore_IFSs(buffer, cnt_buffer + 1 /* the char after & */) == -5) /* EOL */
					{
						/* done */
						if (cmd_lastnode->flag == 0)
							cmd_lastnode->flag = BG_CMD; /* cmd & \0
										      */
						else
						{
							synerr("&");
							cnt_return = -2;
						}
						goto done;
					}
					else if (buffer[cnt_buffer + 1] == '&')
					{
						if (new_command(&(cmd_lastnode->next)) == -1)
						{
							OUT2E("%s: filpinfo: "
							      "malloc failed\n",
							      argv0);
							cnt_return = -1;
							goto done;
						}
						if (cmd_lastnode->flag == 0)
							cmd_lastnode->flag = RUN_AND;
						else
						{
							synerr("&&");
							cnt_return = -2;
							goto done;
						}
						if (ignore_IFSs(buffer, cnt_buffer + 2 /* the char after && */) ==
						    -5) /* EOL */
						{
							char *cmdand_buf;
							cmdand_buf = p_gets("> ");
							buffer = realloc(buffer, strlen(buffer) + strlen(cmdand_buf) +
										     1 /* \0 */);
							strncat(buffer, cmdand_buf, strlen(cmdand_buf));
							free(cmdand_buf);
						}
						++cnt_buffer;
					}
					else
					{
						if (new_command(&(cmd_lastnode->next)) == -1)
						{
							/* malloc failed,
							 * cleanup */
							OUT2E("%s: filpinfo: "
							      "malloc failed\n",
							      argv0);
							cnt_return = -1;
							goto done;
						}
						if (cmd_lastnode->flag == 0)
							cmd_lastnode->flag = BG_CMD;
						else
						{
							synerr("&");
							cnt_return = -2;
							goto done;
						}
					}
					cmd_lastnode = cmd_lastnode->next;
					cnt_argument_element = 0;
					cnt_argument_char = 0;
					ignIFS_from_next_char();
				}
				break;
			case '|':
				if (ignore)
					write_current();
				else
				{
					if (cnt_argument_char == 0) /* Previously a blank reached */
					{
						cnt_argument_char = cnt_old_parameter;
						free(cmd_lastnode->parameters[cnt_argument_element]);
						cmd_lastnode->parameters[cnt_argument_element] = NULL;
					}
					if (new_command(&(cmd_lastnode->next)) == -1)
					{
						/* malloc failed, cleanup */
						OUT2E("%s: filpinfo: malloc "
						      "failed\n",
						      argv0);
						cnt_return = -1;
						goto done;
					}
					if (buffer[cnt_buffer + 1] == '|')
					{
						if (cmd_lastnode->flag == 0)
							cmd_lastnode->flag = RUN_OR;
						else
						{
							synerr("||");
							cnt_return = -2;
							goto done;
						}
						if (ignore_IFSs(buffer, cnt_buffer + 2 /* the char after || */) ==
						    -5) /* EOL */
						{
							char *cmdor_buf;
							cmdor_buf = p_gets("> ");
							buffer = realloc(buffer, strlen(buffer) + strlen(cmdor_buf) +
										     1 /* \0 */);
							strncat(buffer, cmdor_buf, strlen(cmdor_buf));
							free(cmdor_buf);
						}
						++cnt_buffer;
					}
					else
					{
						if (cmd_lastnode->flag == 0)
							cmd_lastnode->flag = PIPED;
						else
						{
							synerr("|");
							cnt_return = -2;
							goto done;
						}
						if (ignore_IFSs(buffer, cnt_buffer + 2 /* the char after | */) ==
						    -5) /* EOL */
						{
							char *pipe_buf;
							pipe_buf = p_gets("> ");
							buffer = realloc(buffer, strlen(buffer) + strlen(pipe_buf) +
										     1 /* \0 */);
							strncat(buffer, pipe_buf, strlen(pipe_buf));
							free(pipe_buf);
						}
					}
					cmd_lastnode = cmd_lastnode->next;
					cnt_argument_element = 0;
					cnt_argument_char = 0;
					ignIFS_from_next_char();
				}
				break;
			case '~': /* $HOME spanding stable */
			/* TODO: more tlide */
				if (ignore)
				{
					write_current();
					break;
				}
				if (buffer[cnt_buffer + 1] != 0 && buffer[cnt_buffer + 1] != '\n' &&
				    buffer[cnt_buffer + 1] != '\t' && buffer[cnt_buffer + 1] != ' ' &&
				    buffer[cnt_buffer + 1] != '/') /* ~username */
				{
					char *username = malloc(sizeof(char) * 256);
					char *posit;
					strncpy(username, &(buffer[cnt_buffer + 1]), 256);
					posit = strchr(username, '/');
					if (posit != NULL)
					{
						while (--posit != username) /* Remove blank */
							if (*posit != ' ' && *posit != '\t')
							{
								*(++posit) = 0; /* Terminate
										   the
										   string
										 */
								break;
							}
					}
					else
					{
						int usernamelen = strlen(username);
						for (--usernamelen; usernamelen != 0; --usernamelen)
							if (username[usernamelen] != ' ' &&
							    username[usernamelen] != '\t')
							{
								username[++usernamelen] = 0; /* Terminate
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
					strncpy(cmd_lastnode->parameters[cnt_argument_element], hdir,
						4094 - cnt_argument_char);
					cnt_buffer += strlen(username);
					cnt_argument_char += strlen(hdir);
					free(username);
				}
				else /* ~/ and ~ */
				{
					char *hdir = gethd();
					strncpy(cmd_lastnode->parameters[cnt_argument_element], hdir,
						4094 - cnt_argument_char);
					cnt_argument_char += strlen(hdir);
				}
				break;
			case '\\':
			{
				int case_count;
				for (case_count = 1; buffer[cnt_buffer]; ++case_count, ++cnt_buffer)
				{
					if (buffer[cnt_buffer] != '\\')
					{
						--cnt_buffer;
						break;
					}
					else /* Print the '\' at a even
						location, and ignore the odd
						ones, the same behavior as in
						bash */
					{
						if (case_count & 1) /* Odd number */
							continue;
						else /* Even number */
							write_current();
					}
				}
			}
			case 0: /* final EOL reached*/
				if (cnt_buffer == cnt_first_nonIFS || !ignore)
					goto done;
				/* Line: command args... \
				 */
				char *newline_buf;
				newline_buf = p_gets("> ");
				buffer = realloc(buffer, strlen(buffer) + strlen(newline_buf) + 1 /* \0 */);
				strncat(buffer, newline_buf, strlen(newline_buf));
				free(newline_buf);
				break;
			case '>':
				if (ignore)
				{
					write_current();
					break;
				}
				if (buffer[cnt_buffer + 1] != '>')
				{
					if (redir_lastnode->type != 0)
						code_fault(__FILE__, __LINE__);
                    else if (buffer[cnt_buffer+1] == '&')
                        redir_lastnode->type = FD2FD;
                    else if (buffer[cnt_buffer+1] == 0)
                    {
                        synerr("newline");
                        cnt_return = -2;
                        goto done;
                    }
                    else
						redir_lastnode->type = OUT_REDIR;
				}
				if (cnt_buffer == cnt_first_nonIFS)
					redir_lastnode->in.fd = 1 /* stdout */;
				else
				{
					if (cnt_argument_char == 0) /* Previously a blank reached */
					{
						cnt_argument_char = cnt_old_parameter;
						free(cmd_lastnode->parameters[cnt_argument_element]);
						cmd_lastnode->parameters[cnt_argument_element] = NULL;
						cnt_argument_element--;
					}
					int case_count = cnt_buffer, case_buf_cnt = 0;
					char buf[MAXLINE] = {0};
					while (--case_count, 1 /* infinity loop */)
					{
						if (case_count == cnt_first_nonIFS || !isdigit(buffer[case_count]))
						{
							if (buf[0] == 0)
								buf[0] = 1 /* stdout */;
							break;
						}
						else /* digit */
							buf[case_buf_cnt++] = buffer[case_count];
					}
					int len = strlen(buf);
					char temp;
					int i;
					for (i = 0; i < len / 2; i++) /* reverse buf */
					{
						temp = buf[i];
						buf[i] = buf[len - i - 1];
						buf[len - i - 1] = temp;
					}
					redir_lastnode->in.fd = atoi(buf);
				}
				switch (buffer[cnt_buffer + 1])
				{
					case '&':
						++cnt_buffer; /* Increase cnt_buffer to the '&' */
						if (ignore_IFSs(buffer, ++cnt_buffer) ==
						    -5) /* and remove all following blanks */
						{
							synerr("newline"); /* if EOL met, report error */
							cnt_return = -2;
							goto done;
						}
						stat_parsing_redirect = 1; /* Parsing for fd */
						break;
					case ' ':
					case '\t':
					case '|':
						++cnt_buffer; /* Increase cnt_buffer to the [' ''\t''|'] */
						if (ignore_IFSs(buffer, ++cnt_buffer) ==
						    -5) /* and remove all following blanks */
						{
							synerr("newline"); /* if EOL met, report error */
							cnt_return = -2;
							goto done;
						}
						stat_parsing_redirect = 2; /* Parsing for filename */
						break;
					case '>': /* Out append */
						if (redir_lastnode->type != 0)
							code_fault(__FILE__, __LINE__);
						redir_lastnode->type = OUT_APPN;
						++cnt_buffer;
						switch (buffer[cnt_buffer + 1])
						{
							case '&':
								++cnt_buffer; /* Increase cnt_buffer to the '&' */
								if (ignore_IFSs(buffer, ++cnt_buffer) ==
								    -5) /* and remove all following blanks */
								{
									synerr(
									    "newline"); /* if EOL met, report error */
									cnt_return = -2;
									goto done;
								}
								stat_parsing_redirect = 1; /* Parsing for fd */
								break;
							case ' ':
							case '\t':
							case '|':
								++cnt_buffer; /* Increase cnt_buffer to the [' ''\t''|']
									       */
								if (ignore_IFSs(buffer, ++cnt_buffer) ==
								    -5) /* and remove all following blanks */
								{
									synerr(
									    "newline"); /* if EOL met, report error */
									cnt_return = -2;
									goto done;
								}
								stat_parsing_redirect = 2; /* Parsing for filename */
								break;
							case '>': /* >>> */
								synerr(">");
								cnt_return = -2;
								goto done;
								break;
							case 0: /* EOL */
								synerr("newline");
								cnt_return = -2;
								goto done;
							default:
								stat_parsing_redirect = 2;
						}
						break;
					case 0:
						synerr("newline");
						cnt_return = -2;
						goto done;
					default:			   /* I don't need to handle the rest */
						stat_parsing_redirect = 2; /* Parsing for filename */
						break;
				}
				break;
			case '`':
				/* TODO: Write command substitude code here */

			case '$':
			/* TODO: Write variable, variable cut,
			 * ANSI-C style escape, command substitude,
			 * arithmetic expansion code here */
			case '(':
			case ')':
			/* TODO: Write command sequence code here */
			case '<':
				/* TODO: Write input redirect, here string and here document code
				 * here */
				write_current();
				break;
			case '#':
				if (cnt_buffer == cnt_first_nonIFS ||
				    (!strchr(" \t", buffer[cnt_buffer - 1])) /* Is IFS */)
				{
					write_current();
					break;
				}
				if (cnt_argument_char == 0) /* Previously a blank reached */
				{
					cnt_argument_char = cnt_old_parameter;
					free(cmd_lastnode->parameters[cnt_argument_element]);
					cmd_lastnode->parameters[cnt_argument_element] = NULL;
					cnt_argument_element--;
				}
				write_char(0);
				return cnt_return;
			case ';':
				/* TODO: Write muiltiple command process code here */
				if (ignore)
					write_current();
				else
				{
					if (cnt_argument_char == 0) /* Previously a blank reached */
					{
						cnt_argument_char = cnt_old_parameter;
						free(cmd_lastnode->parameters[cnt_argument_element]);
						cmd_lastnode->parameters[cnt_argument_element] = NULL;
					}
					if (new_command(&(cmd_lastnode->next)) == -1)
					{
						/* malloc failed, cleanup */
						OUT2E("%s: filpinfo: malloc "
						      "failed\n",
						      argv0);
						cnt_return = -1;
						goto done;
					}
					if (cmd_lastnode->flag == 0)
						cmd_lastnode->flag = MULTICMD;
					else
					{
						synerr(";");
						cnt_return = -2;
						goto done;
					}
					if (ignore_IFSs(buffer, cnt_buffer + 1 /* the char after ; */) == -5) /* EOL */
					{
						goto done; /* Ending `;', end parsing */
					}
					++cnt_buffer;
				}
				cmd_lastnode = cmd_lastnode->next;
				cnt_argument_element = 0;
				cnt_argument_char = 0;
				ignIFS_from_next_char();
				do /* Remove all following `;'s. I'll mess up with case later. TODO */
				{
					if (!buffer[cnt_buffer]) /* EOL */
						;		 /* No new command required */
					if (buffer[cnt_buffer] != ';')
					{
						--cnt_buffer;
						break;
					}
				} while (++cnt_buffer);
				break;
			default:
				write_current();
		}
	} while (++cnt_buffer);
done:
	if (cnt_return > 0)
		write_char(0);
	free(buffer);
	return cnt_return;
}
