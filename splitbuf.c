#include "pshell.h"
int split_buffer(char **command, char **parameters)
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

