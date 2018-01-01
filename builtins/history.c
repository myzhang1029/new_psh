/*
   history.c - builtin history

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

#include "builtin.h"
#include <getopt.h>

#define USAGE() OUT2E("history: usage: history [-c] [-d offset] [n] or history -awrn [filename] or history -ps arg [arg...]\n")
#define AFLAG   0x01
#define RFLAG   0x02
#define WFLAG   0x04
#define NFLAG   0x08
#define SFLAG   0x10
#define PFLAG   0x20
#define CFLAG   0x40
#define DFLAG   0x80

int builtin_history(char *command, char **parameters)
{
#ifdef NO_HISTORY
	OUT2E("%s: libhistory not compiled!\n", command);
	return 2;
#else
	if(parameters[1]!=NULL)
	{
		int count, ch, flags=0, n;
		char *filename=malloc(sizeof(char)*MAXEACHARG);
		struct option longopts[]={
			{"help",no_argument,NULL,'h'},
			{NULL,0,NULL,0}
		};
		
		/*Get argc for getopt*/
		for(count=0; parameters[count];count++);
		while((ch=getopt_long(count, parameters, ":a::w::r::n::p::s::cd:", longopts, NULL))!=-1)
		{
			switch(ch)
			{
				case 'a':
					flags|=AFLAG;
					if(optarg)
						strncpy(filename,optarg,sizeof(char)*MAXEACHARG-1);
					break;
				case 'r':
					if(flags&AFLAG)
					{
						OUT2E("psh: %s: cannot use more than one of -anrw\n", command);
						USAGE();
						return 2;
					}
					flags|=RFLAG;
					if(optarg)
						strncpy(filename,optarg,sizeof(char)*MAXEACHARG-1);
					break;
				case 'w':
					if(flags&AFLAG||flags&RFLAG)
					{
						OUT2E("psh: %s: cannot use more than one of -anrw\n", command);
						USAGE();
						return 2;
					}
					if(optarg)
						strncpy(filename,optarg,sizeof(char)*MAXEACHARG-1);
					flags|=WFLAG;
					break;
				case 'n':
					if(flags&AFLAG||flags&RFLAG||flags&WFLAG)
					{
						OUT2E("psh: %s: cannot use more than one of -anrw\n", command);
						USAGE();
						return 2;
					}
					if(optarg)
						strncpy(filename,optarg,sizeof(char)*MAXEACHARG-1);
					flags|=NFLAG;
					break;
				case 's':
					flags|=SFLAG;
					break;
				case 'p':
					flags|=PFLAG;
					break;
				case 'c':
					flags|=CFLAG;
					break;
				case 'd':
					flags|=DFLAG;
					n=atoi(parameters[count]);
					if(n<0)
					{
						OUT2E("psh: %s: %d: invalid option\n", command, n);
						return 2;
					}
					if(!n)
					{
						int count2;
						for(count2=0; parameters[count][count2]; ++count2)
							if(parameters[count][count2]!='0'&&(!isspace(parameters[count][count2])))
							{
								OUT2E("psh: %s: %s: numeric argument required\n", command, parameters[count]);
								return 2;
							}
					}
					break;
				case '?':
					OUT2E("psh: %s: invalid option '-%c'\n",command, optopt);
					return 2;
				case ':':
					OUT2E("psh: %s: -d: option requires an argument\n", command);
					return 2;
			}
		}
		if(flags==0)
			goto noopts;
	}
	else
noopts:
	{
		HIST_ENTRY **histlist;
		int i;

		histlist = history_list ();
		if (histlist)
			for (i = 0; histlist[i]; i++)
				printf ("    %d  %s\n", i + history_base, histlist[i]->line);
	}
	return 1;
}
#endif			

