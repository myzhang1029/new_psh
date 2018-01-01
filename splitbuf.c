/*
   splitbuf.c - buffer spliter 

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
int split_buffer(char **command, char **parameters, char *buffer)
{
	char *pStart,*pEnd;
	int count = 0;
	int isFinished = 0;
	pStart = pEnd = buffer;
	while(isFinished == 0)
	{
		while((*pEnd == ' ' && *pStart == ' ') || (*pEnd == '\t' && *pStart == '\t'))
		{
			pStart++;
			pEnd++;
		}

		if(*pEnd == '\0' || *pEnd == '\n')
		{
			if(count == 0)
				return -1;
			break;
		}

		while(*pEnd != ' ' && *pEnd != '\0' && *pEnd != '\n')
			pEnd++;


		if(count == 0)
		{
			char *p = pEnd;
			*command = pStart;
			while(p!=pStart && *p !='/')
				p--;
			if(*p == '/')
				p++;
			//else //p==pStart
			parameters[0] = p;
			count += 2;
#ifdef DEBUG
			printf("\ncommand:%s\n",*command);
#endif
		}
		else if(count <= MAXARG)
		{
			parameters[count-1] = pStart;
			count++;
		}
		else
		{
			break;
		}

		if(*pEnd == '\0' || *pEnd == '\n')
		{
			*pEnd = '\0';
			isFinished = 1;
		}
		else
		{
			*pEnd = '\0';
			pEnd++;
			pStart = pEnd;
		}
	}

	parameters[count-1] = NULL;
	return count;
}

