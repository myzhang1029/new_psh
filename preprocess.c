/*
   preprocess.c - input preprocesser

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
#include "backends/backend.h"

/* XXX: This kind of preprocess might cause SEGV */
char *preprocess_cmdline(char *buffer)
{
	char *new_buffer=malloc(sizeof(char) * MAXLINE);
	int count=0,ncount=0,nncount;
	/* count: old counter
	 * ncount: preprocessed count, will be decreased to 0 in the '\' deletion
	 * nncount: kept preprocessed count
	 */
	for(; buffer[count]!=0; count++,ncount++)
	{
		if(buffer[count]=='>'&&buffer[count+1]=='>'&&buffer[count-1]!='\\')
		{
			new_buffer[ncount++]=' ';
			new_buffer[ncount++]='>';
			new_buffer[ncount++]='>';
			new_buffer[ncount]=' ';
			count++;
		}
		else if(buffer[count]=='<'&&buffer[count+1]=='<'&&buffer[count-1]!='\\')
		{
			new_buffer[ncount++]=' ';
			new_buffer[ncount++]='<';
			new_buffer[ncount++]='<';
			new_buffer[ncount]=' ';
			count++;
		}
		else if(buffer[count]=='&'&&buffer[count+1]=='&'&&buffer[count-1]!='\\')
		{
			new_buffer[ncount++]=' ';
			new_buffer[ncount++]='&';
			new_buffer[ncount++]='&';
			new_buffer[ncount]=' ';
			count++;
		}
		else if(buffer[count]=='|'&&buffer[count+1]=='|'&&buffer[count-1]!='\\')
		{
			new_buffer[ncount++]=' ';
			new_buffer[ncount++]='|';
			new_buffer[ncount++]='|';
			new_buffer[ncount]=' ';
			count++;
		}
		else if((buffer[count]=='|'||buffer[count]=='>'||buffer[count]=='<')&&buffer[count-1]!='\\')
		{
			new_buffer[ncount++]=' ';
			new_buffer[ncount++]=buffer[count];
			new_buffer[ncount]=' ';
		}
		else if(buffer[count]=='&'&&buffer[count-1]!='\\')
		{
			new_buffer[ncount++]=' ';
			new_buffer[ncount]='&';
		}
		else if(buffer[count]=='~'&&buffer[count-1]!='\\')
		{
			if(buffer[count+1]=='/')/* Replace to home dir */
			{
				char *hdir=gethd();
				int len=strlen(hdir)+1;
				int i;
				for(i=0;i<len;++i,++ncount)
											new_buffer[ncount]=hdir[i];
			}
			else /* TODO:~user, replace to user's home dir */
				;
		}

		else /* Normal char */
			new_buffer[ncount]=buffer[count];
	}
	while(ncount--)
	{
		if(isspace(new_buffer[ncount]))
			new_buffer[ncount]=0;
		else
			break;
	}
	nncount=ncount;
	while(ncount--)
	{
		if(new_buffer[ncount]=='\\')
		{
			int i;
			if(new_buffer[ncount-1]=='\\')
				/* Keep one */
				ncount--;
			/* Remove the '\' and move forward */
			for(i=ncount;i<=nncount;++i)
				new_buffer[i]=new_buffer[i+1];
		}
	}

	return new_buffer;
}
