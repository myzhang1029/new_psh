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
