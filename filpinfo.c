/*
   filpinfo - function to fill parse info(merges original preprocesser, splitbuf, parser)

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

#include "pshell.h"

static int endwith(char* s,char c)
{
	if(s[strlen(s)-1]==c)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

static int parse_info_init(struct parse_info *info)
{
	info->paracount=0;
	info->flag = 0;
	info->in_file = NULL;
	info->out_file = NULL;
	info->buffer = NULL;
	memset(info->parameters, 0, MAXLINE);
	info->next = NULL;
	return 0;
}

/* Returns the nth next of the parse_info base, n begins with 1 */
static struct parse_info *getpos(struct parse_info *base, int n)
{
	int count;
	struct parse_info *info=base;
	for (count=1; count<n; ++count)
		info=info->next;
	return info;
}

/* Free a parse_info and its nexts */
void free_parse_info(struct parse_info *info)
{
	int i;
	while(1)
	{
		for(i=1;; ++i)
			if(getpos(info, i)->next==NULL)
			{
				free(getpos(info, i));
				break;
			}
		if(i==1)
			break;
	}
	free(info);

}

/* Fills a parse_info with a buffer, returns characters processed */
int filpinfo(char *buffer, struct parse_info *info)
{
	#define write_current() getpos(info, pos)->parameters[paracount][parametercount++]=buffer[count]
	#define write_char(c) getpos(info, pos)->parameters[paracount][parametercount++]=c
	#define escape (buffer[count-1]=='\\')
	#define ignore (isInDoubleQuote==1||isInSingleQuote==1||escape)
	/*
		write_current: Write the current char to parameters
		write_char: Write a character to parameters
		escape: determine whether the last character is '\\'
		ignore: determine whether a meta character should be ignored(not for a $(), or a ${})
	*/

	int len=strlen(buffer);
	int pos=1;
	int count=0, parametercount=0, paracount=0;
	/*
		count: count for buffer
		parametercount: count for current parameter element
		paracount: count representing how many elements are there in parameter
	*/
	int isInSingleQuote = 0, isInDoubleQuote = 0;
	for(;count<len;++count)
	{
		switch(buffer[count])
		{
			case '\'':
				if(isInDoubleQuote == 1)/* Already in a "" quote, just write a ' */
					write_current();

				else if(isInSingleQuote == 1)/* Get out of the quote */
					isInSingleQuote = 0;
				else
				{
					if(count == 0)
						isInSingleQuote = 1;
					else if(!escape)
						isInSingleQuote = 1;
					else /* count != 0 && buffer[count-1] == '\\' */
						/* Write a ' */
						write_current();
				}
				break;
			case '"':
				if(isInSingleQuote == 1)
					write_current();
				else
				{
					if(isInDoubleQuote == 1)
						if(escape)
							write_current();
						else
							isInDoubleQuote = 0;
					else
						if(escape)
							write_current();
						else
							isInDoubleQuote = 1;
				}
				break;
			case '\t':
			case ' ':
				if(ignore)
					write_current();
				else
					write_char(0);
					paracount++;
					while(count++)
						if(buffer[count]!=' ')
							break;
				break;
			case '&':
				if(ignore)
					write_current();
				else
				{
					if(buffer[count+1] == 0)/* End of input */
						info->flag|=BACKGROUND;
					else if(buffer[count+1] == '&')
					{
						info->flag|=RUN_AND;
						if(buffer[count+2] == 0)
						{
							char *cmdand_buf=malloc(MAXLINE);
							getpos(info, pos)->next=malloc(sizeof(struct parse_info));
#ifdef NO_READLINE
							printf("> ");
							fgets(cmdand_buf, MAXLINE, stdin);
#else
							cmdand_buf = readline("> ");
#endif
							strncat(buffer, cmdand_buf, MAXLINE-count-1);
							free(cmdand_buf);
						}
					}
					else
					{
						info->flag|=BACKGROUND;
					}

					info->next=malloc(sizeof(struct parse_info));
					pos++;
					parse_info_init(getpos(info, pos));
				}
				break;
			case '|':
				if(ignore)
					write_current();
				else
				{
					if(buffer[count+1] == '|')
					{
						info->flag|=RUN_OR;
						if(buffer[count+2] == 0)
						{
							char *cmdor_buf=malloc(MAXLINE);
							getpos(info, pos)->next=malloc(sizeof(struct parse_info));
#ifdef NO_READLINE
							printf("> ");
							fgets(cmdor_buf, MAXLINE, stdin);
#else
							cmdor_buf = readline("> ");
#endif
							strncat(buffer, cmdor_buf, MAXLINE-count-1);
							free(cmdor_buf);
						}
					}
					else
					{
						info->flag|=IS_PIPED;
						if(buffer[count+1] == 0)
						{
							char *pipe_buf=malloc(MAXLINE);
							getpos(info, pos)->next=malloc(sizeof(struct parse_info));
#ifdef NO_READLINE
							printf("> ");
							fgets(pipe_buf, MAXLINE, stdin);
#else
							pipe_buf = readline("> ");
#endif
							strncat(buffer, pipe_buf, MAXLINE-count-1);
							free(pipe_buf);
						}
					}
					info->next=malloc(sizeof(struct parse_info));
					pos++;
					parse_info_init(getpos(info, pos));
				}
				break;
			case '~':
				if(ignore)
					break;
				if(buffer[count+1]!=0 && buffer[count+1]!='\n'
						&& buffer[count+1]!='\t' && buffer[count+1]!=' '
						&& buffer[count+1]!='/')/* ~username */
				{
					char *username=malloc(sizeof(char)*256);
					char *posit;
					strncpy(username, &(buffer[count+1]));
					posit=strchr(username, '/');
					if(posit!=NULL)
						*posit=0;/* Terminate the string */
					else
						while(--posit!=username)
							if(*posit!=' ' && *posit!='\t')
								break;/* Remove blank */
					char *hdir=gethdnam(username);
					if(hdir==NULL)
						/* No such user, treat as a normal ~ as in bash */
						write_current();
					strncpy(&(info->parameters[paracount][parametercount]), hdir, 4094-parametercount);
					count+=strlen(username);
				}
				else/* ~/ and ~ */
				{
					char *hdir=gethd();
					strncpy(&(info->parameters[paracount][parametercount]), hdir, 4094-parametercount);
				}
				break;
			case '`':
				/* TODO: Write command substitude code here */
			case '$':
				/* TODO: Write variable, variable cut,
				 * ANSI-C style escape, command substitude,
				 * arithmetic expansion code here */
			case '>':
				/* TODO: Write output redirect code here */
			case '<':
				/* TODO: Write input redirect and heredoc code here */
				break;
			default:
				write_current();
		}
	}
	write_char(0);
	return count;
}

