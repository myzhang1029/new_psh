/*
   parser.c - input parser

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

int parse_info_init(struct parse_info *info)
{
	info->flag = 0;
	info->in_file = NULL;
	info->out_file = NULL;
	info->command = NULL;
	info->parameters = NULL;
	return 0;
}

int parsing(char **parameters,int ParaNum,struct parse_info *info)
{
	int i;
	parse_info_init(info);
	if(strcmp(parameters[ParaNum-1],"&") ==0)
	{
		info->flag |= BACKGROUND;
		parameters[ParaNum-1] = NULL;
		ParaNum--;
	}

	for(i=0; i<ParaNum;)
	{
		if(strcmp(parameters[i],"<<")==0 || strcmp(parameters[i],"<")==0)
		{
			info->flag |= IN_REDIRECT;
			info->in_file = parameters[i+1];
			parameters[i] = NULL;
			i+=2;
		}
		else if(strcmp(parameters[i],">")==0)
		{
			info->flag |= OUT_REDIRECT;
			info->out_file = parameters[i+1];
			parameters[i] = NULL;
			i+=2;
		}
		else if(strcmp(parameters[i],">>")==0)
		{
			info->flag |= OUT_REDIRECT_APPEND;
			info->out_file = parameters[i+1];
			parameters[i] = NULL;
			i+=2;
		}
		else if(strcmp(parameters[i],"|")==0)
		{
			char* pCh;
			info->flag |= IS_PIPED;
			parameters[i] = NULL;
			info->command = parameters[i+1];
			info->parameters = &parameters[i+1];
			for(pCh = info->parameters[0]+strlen(info->parameters[0]);
			        pCh!=&(info->parameters[0][0]) && *pCh!='/'; pCh--)
				;
			if(*pCh == '/')
				pCh++;
			info->parameters[0] = pCh;
			break;
		}
		else
			i++;
	}
#ifdef DEBUG
	printf("\nbackground:%s\n",info->flag&BACKGROUND?"yes":"no");
	printf("in redirect:");
	info->flag&IN_REDIRECT?printf("yes,file:%s\n",info->in_file):printf("no\n");
	printf("out redirect:");
	info->flag&OUT_REDIRECT?printf("yes,file:%s\n",info->out_file):printf("no\n");
	printf("pipe:");
	info->flag&IS_PIPED?printf("yes,command:%s %s %s\n",info->command,info->parameters[0],info->parameters[1]):printf("no\n");
#endif
	return 1;
}
