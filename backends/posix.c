/*
   posix.c - backend of POSIX systems

   Copyright 2013 wuyve.
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

#include <fcntl.h>
#include <pwd.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "backend.h"

pid_t ChdPid, ChdPid2;
pid_t BPTable[MAXPIDTABLE] = {0};
int pipe_fd[2], in_fd, out_fd;
extern char *argv0;	 /*main.c*/
extern jmp_buf reset_point; /*main.c*/

void sigchld_handler(int sig)
{
	pid_t pid;
	int i;
	for (i = 0; i < MAXPIDTABLE; i++)
		if (BPTable[i] != 0) /*only handler the background processes*/
		{
			pid = waitpid(BPTable[i], NULL, WNOHANG);
			if (pid > 0)
			{
				printf("[%d] %d done\n", i + 1, pid);
				BPTable[i] = 0; /*clear*/
			}
			else if (pid < 0)
			{
				if (errno != ECHILD)
					OUT2E("%s: waitpid error: %s", argv0,
					      strerror(errno));
			}
			/*else:do nothing.*/
			/*Not background processses has their waitpid() in
			 * wshell.*/
		}
	return;
}

void sigintabrt_hadler(int sig)
{
	last_command_status = sig;
	longjmp(reset_point, 1);
}

int prepare(void)
{
	int ret = 0;
	if (signal(SIGCHLD, sigchld_handler) == SIG_ERR)
		OUT2E("%s: signal error: %s", argv0, strerror(errno)), ret++;

	if (signal(SIGINT, sigintabrt_hadler) == SIG_ERR)
		OUT2E("%s: signal error: %s", argv0, strerror(errno)), ret++;

	if (signal(SIGQUIT, sigintabrt_hadler) == SIG_ERR)
		OUT2E("%s: signal error: %s", argv0, strerror(errno)), ret++;
	return ret;
}

char *gethd(void)
{
	struct passwd *pwd = getpwuid(getuid());
	if (pwd == NULL)
		return NULL;
	return pwd->pw_dir;
}

char *gethdnam(char *username)
{
	struct passwd *pwd = getpwnam(username);
	if (pwd == NULL)
		return NULL;
	return pwd->pw_dir;
}

char *getun(void)
{
	struct passwd *pwd = getpwuid(getuid());
	if (pwd == NULL)
		return NULL;
	return pwd->pw_name;
}

char *pshgetcwd(void)
{
	char *cwd = malloc(4097);
	if (cwd == NULL)
		return NULL;
	getcwd(cwd, 4097);
	return cwd;
}

int pshgethostname(char *hstnme, size_t len)
{
	return gethostname(hstnme, len);
}

int pshgetuid(void) { return geteuid(); }

int pshchdir(char *dir) { return chdir(dir); }

static int redir_spawnve(struct redirect *arginfo, char *cmd, char **argv,
			 char **env)
{
	pid_t pid;
	struct redirect *info = arginfo;
	if ((pid = fork()) == 0)
	{
		while (info)
		{
			switch (info->type)
			{
				case FD2FD:
					dup2(info->in.fd, info->out.fd);
					close(info->in.fd);
					break;
				case OUT_REDIR:
					dup2(open(info->out.file,
						  O_WRONLY | O_CREAT | O_TRUNC,
						  0644),
					     info->in.fd);
					break;
				case OUT_APPN:
					dup2(open(info->out.file,
						  O_WRONLY | O_CREAT | O_APPEND,
						  0644),
					     info->in.fd);
					break;
				case IN_REDIR:
					dup2(open(info->in.file,
						  O_RDONLY | O_CREAT, 0644),
					     info->out.fd);
					break;
				case CLOSEFD:
					close(info->in.fd);
					break;
				case OPENFN:
					dup2(open(info->in.file,
						  O_RDWR | O_CREAT, 0644),
					     info->out.fd);
					break;
			}
			info = info->next;
		}
		execve(cmd, argv, env);
	}

	return pid;
}

int do_run(struct command *arginfo)
{
	struct command *info = arginfo;
	while (1)
	{
		if (info->flag & PIPED)
			if (pipe(pipe_fd) < 0)
			{
				OUT2E("%s: pipe failed: %s\n", argv0,
				      strerror(errno));
				exit_psh(1);
			}
	}
	if (info->flag & PIPED) /*command is not null*/
	{
		if (pipe(pipe_fd) < 0)
		{
			OUT2E("%s: pipe failed: %s\n", argv0, strerror(errno));
			exit(0);
		}
	}
	if ((ChdPid = fork()) != 0) /*shell*/
	{
		if (info->flag & PIPED)
		{
			if ((ChdPid2 = fork()) == 0) /*command*/
			{
				close(pipe_fd[1]);
				close(fileno(stdin));
				dup2(pipe_fd[0], fileno(stdin));
				close(pipe_fd[0]);
				execvp(info->parameters[0],
				       (char **)info->parameters);
			}
			else
			{
				close(pipe_fd[0]);
				close(pipe_fd[1]);
				waitpid(ChdPid2, &last_command_status,
					0); /*wait command*/
			}
		}

		if (info->flag & BG_CMD)
		{
			int i;
			for (i = 0; i < MAXPIDTABLE; i++)
				if (BPTable[i] == 0)
				{
					BPTable[i] = ChdPid; /*register a
								background
								process*/
					break;
				}

			printf("[%d] %u\n", i + 1, ChdPid);
			if (i == MAXPIDTABLE)
				OUT2E("%s: Too much background processes\n",
				      argv0);
			usleep(5000);
		}
		else
		{
			waitpid(ChdPid, &last_command_status,
				0); /*wait command1*/
		}
	}
	else /*command1*/
	{

		if (info->flag & PIPED) /*command is not null*/
		{
			close(pipe_fd[0]);
			close(fileno(stdout));
			dup2(pipe_fd[1], fileno(stdout));
			close(pipe_fd[1]);
		}

		if (execvp(info->parameters[0], (char **)info->parameters) ==
		    -1)
		{
			if (errno == ENOENT)
			{
				OUT2E("%s: %s: command not found\n", argv0,
				      info->parameters[0]);
				last_command_status = 127;
			}
			else
			{
				OUT2E("%s: %s: %s\n", argv0,
				      info->parameters[0], strerror(errno));
				/* Exit the failed command child process */
				last_command_status = 126;
			}
			_Exit(1);
		}
	}
	return 0;
}

/* * *
 * Platform dependent builtins part
 *  * *
 */
#include "builtins/builtin.h"

/* Builtin exec */
int builtin_exec(ARGS)
{
	if (b_parameters[1] == NULL)
		return 1; /* Do nothing */
	if (execv(b_parameters[1], &b_parameters[1]) == -1)
		OUT2E("exec: %s: %s\n", b_parameters[1], strerror(errno));
	return 2;
}
