/*
   preprocess.c - input preprocesser

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

#include "pshell.h"

char *preprocess_cmdline(char *buffer)
{
	char *new_buffer=malloc(sizeof(char) * MAXLINE);
	int count=0,ncount=0;
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
		else
			new_buffer[ncount]=buffer[count];
	}
	while(ncount--)
	{
		if(isspace(new_buffer[ncount]))
			new_buffer[ncount]=0;
		else
			break;
	}
	return new_buffer;
}
