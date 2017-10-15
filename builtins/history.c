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
	if(parameters!=NULL)
	{
		int count, ch, flags=0, n;
		char *optstr="a::w::r::n::p::s::cd:", **newparam=malloc(sizeof(char*)*(MAXARG+1)+2), filename[MAXEACHARG]="Init";
		struct option longopts[2]={
			{"help",0,NULL,'h'},
			{NULL,0,NULL,0}
		};
		if(newparam==NULL)
		{
			OUT2E("psh: %s: malloc failed\n",command);
			return 2;
		}

		/*Make argv[0] command*/ 
		memmove(newparam[1],parameters,sizeof(char*)*MAXARG+2);
		newparam[0]=command;
		/*Get argc for getopt*/
		for(count=0; newparam[count-1];count++);
		while((ch=getopt_long(count, newparam, optstr, longopts, NULL))!=-1)
		{
			switch(ch)
			{
				case 'a':
					flags|=AFLAG;
					if(optarg)
						strcpy(filename,optarg);
					break;
				case 'r':
					if(flags&AFLAG)
					{
						OUT2E("psh: %s: cannot use more than one of -anrw\n", command);
						USAGE();
						free(newparam);
						return 2;
					}
					flags|=RFLAG;
					if(optarg)
						strcpy(filename,optarg);
					break;
				case 'w':
					if(flags&AFLAG||flags&RFLAG)
					{
						OUT2E("psh: %s: cannot use more than one of -anrw\n", command);
						USAGE();
						free(newparam);
						return 2;
					}
					if(optarg)
						strcpy(filename,optarg);
					flags|=WFLAG;
					break;
				case 'n':
					if(flags&AFLAG||flags&RFLAG||flags&WFLAG)
					{
						OUT2E("psh: %s: cannot use more than one of -anrw\n", command);
						USAGE();
						free(newparam);
						return 2;
					}
					if(optarg)
						strcpy(filename,optarg);
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
					free(newparam);
					return 2;
				}
				if(!n)
				{
					int count2;
					for(count2=0; parameters[count][count2]; ++count2)
						if(parameters[count][count2]!='0'&&(!isspace(parameters[count][count2])))
						{
							OUT2E("psh: %s: %s: numeric argument required\n", command, parameters[count]);
							free(newparam);
							return 2;
						}
			}
					break;
				case '?':
					OUT2E("psh: %s: invalid option '-%c'\n",command, optopt);
					return 2;
				case ':':
					OUT2E("bash: %s: -d: option requires an argument\n", command);
					return 2;
			}
		}
#if 0
		int count, flags=0, first=0, n=-1;
		char args[MAXEACHARG][MAXARG];
		for(count=0; parameters[count]!=NULL; ++count)
		if(parameters[count][0]=='-')
		{
			int count2;
			for(count2=1;parameters[count][count2]!=' '&&parameters[count][count2]!=0;++count2)
			{
				switch(parameters[count][count2])
				{
					case 'a':
						flags|=AFLAG;
						break;
					case 'r':
						if(flags&AFLAG)
						{
							OUT2E("psh: %s: cannot use more than one of -anrw\n", command);
							USAGE();
							return 2;
						}
						flags|=RFLAG;
						break;
					case 'w':
						if(flags&AFLAG||flags&RFLAG)
						{
							OUT2E("psh: %s: cannot use more than one of -anrw\n", command);
							USAGE();
							return 2;
						}
						flags|=WFLAG;
						break;
					case 'n':
						if(flags&AFLAG||flags&RFLAG||flags&WFLAG)
						{
							OUT2E("psh: %s: cannot use more than one of -anrw\n", command);
							USAGE();
							return 2;
						}
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
						break;
					case '-':
						if(parameters[count][count2+1]==' '||parameters[count][count2+1]=='\0')
							goto noopts;
						else if(strcmp(parameters[count], "--help")==0)
						{
							USAGE();
							return 1;
						}
						else
						{
							OUT2E("psh: %s: --: invalid option\n", command);
							USAGE();
							return 2;
						}
						break;
					default:
						OUT2E("psh: %s: -%c: invalid option\n", command,  parameters[count][count2]);
						USAGE();
						return 2;
				}
			}
		}
		else
		{
			if(first==0)
				first=1;
			else
			{
				OUT2E("psh: %s: too many arguments\n", command);
				return 2;
			}
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
				return 1;
			}
		}
		if(!flags&&n==-1)
			goto noopts;
		if(flags&WFLAG)
			write_history(/*extern*/histfile);
		if(flags&RFLAG)
			read_history(histfile);
		if(flags&AFLAG)
			;
		if(flags&NFLAG)
			;
		if(flags&CFLAG)
			clear_history();
		if(flags&DFLAG)
			;
		if(flags&PFLAG)
			;
		if(flags&SFLAG)
			;

#endif
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

