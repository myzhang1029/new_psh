#include "backend.h"
#include <pwd.h>
int pipe_fd[2], in_fd, out_fd;

char *gethd(void)
{
	struct passwd *pwd=getpwuid(getuid());
	if(pwd==NULL)
		return NULL;
	return pwd->pw_dir;
}

char *getun(void)
{
	struct passwd *pwd=getpwuid(getuid());
	if(pwd==NULL)
		return NULL;
	return pwd->pw_name;
}

int do_run(char *command, char **parameters, struct parse_info info)
{
	switch(run_builtin(command,parameters))
	{
	case 1:
		return 1;
	case 2:
		status=1;
		return 1;
	default:
		break;
	}
	if(info.flag & IS_PIPED) //command is not null
	{
		if(pipe(pipe_fd)<0)
		{
			OUT2E("psh: pipe failed: %s\n", strerror(errno));
			exit(0);
		}
	}
	if((ChdPid = fork())!=0) //shell
	{
		if(info.flag & IS_PIPED)
		{
			if((ChdPid2=fork()) == 0) //command
			{
				close(pipe_fd[1]);
				close(fileno(stdin));
				dup2(pipe_fd[0], fileno(stdin));
				close(pipe_fd[0]);
				execvp(info.command,info.parameters);
			}
			else
			{
				close(pipe_fd[0]);
				close(pipe_fd[1]);
				waitpid(ChdPid2,&status,0); //wait command
			}
		}

		if(info.flag & BACKGROUND)
		{
			int i;
			for(i=0; i<MAXPIDTABLE; i++)
				if(BPTable[i]==0)
				{
					BPTable[i] = ChdPid; //register a background process
					break;
				}

			printf("[%d] %u\n", i+1, ChdPid);
			if(i==MAXPIDTABLE)
				OUT2E("psh: Too much background processes\n");
			usleep(5000);
		}
		else
		{
			waitpid(ChdPid,&status,0);//wait command1
		}
	}
	else //command1
	{

		if(info.flag & IS_PIPED) //command is not null
		{
			if(!(info.flag & OUT_REDIRECT) && !(info.flag & OUT_REDIRECT_APPEND)) // ONLY PIPED
			{
				close(pipe_fd[0]);
				close(fileno(stdout));
				dup2(pipe_fd[1], fileno(stdout));
				close(pipe_fd[1]);
			}
			else //OUT_REDIRECT and PIPED
			{
				close(pipe_fd[0]);
				close(pipe_fd[1]);//send a EOF to command
				if(info.flag & OUT_REDIRECT)
					out_fd = open(info.out_file, O_WRONLY|O_CREAT|O_TRUNC, 0666);
				else
					out_fd = open(info.out_file, O_WRONLY|O_APPEND|O_TRUNC, 0666);
				close(fileno(stdout));
				dup2(out_fd, fileno(stdout));
				close(out_fd);
			}
		}
		else
		{
			if(info.flag & OUT_REDIRECT) // OUT_REDIRECT WITHOUT PIPE
			{
				out_fd = open(info.out_file, O_WRONLY|O_CREAT|O_TRUNC, 0666);
				close(fileno(stdout));
				dup2(out_fd, fileno(stdout));
				close(out_fd);
			}
			if(info.flag & OUT_REDIRECT_APPEND) // OUT_REDIRECT_APPEND WITHOUT PIPE
			{
				out_fd = open(info.out_file, O_WRONLY|O_CREAT|O_APPEND, 0666);
				close(fileno(stdout));
				dup2(out_fd, fileno(stdout));
				close(out_fd);
			}
		}

		if(info.flag & IN_REDIRECT)
		{
			in_fd = open(info.in_file, O_CREAT |O_RDONLY, 0666);
			close(fileno(stdin));
			dup2(in_fd, fileno(stdin));
			close(in_fd);
		}
		if(execvp(command,parameters)==-1)
		{
			OUT2E("psh: %s: %s\n", strerror(errno), command);
			/* Exit the failed command child process */
			_Exit(1);
		}
	}
	return 0;
}
