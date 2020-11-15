/*
   backends/posix.c - backend of POSIX systems

   Copyright 2013 wuyve.
   Copyright 2017-present Zhang Maiyun.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "backend.h"
#include "command.h"
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"

pid_t ChdPid, ChdPid2;
pid_t BPTable[MAXPIDTABLE] = {0};
int pipe_fd[2], in_fd, out_fd;

extern char *argv0;

void sigchld_handler(__attribute__((unused)) int sig)
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
                    OUT2E("%s: waitpid error: %s", argv0, strerror(errno));
            }
            /*else:do nothing.*/
            /*Not background processes has their waitpid() in
             * wshell.*/
        }
    return;
}

void sigintabrt_hadler(int sig) { last_command_status = sig; }

int psh_backend_prepare(void)
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

char *psh_backend_get_homedir(void)
{
    struct passwd *pwd = getpwuid(getuid());
    if (pwd == NULL)
        return NULL;
    return pwd->pw_dir;
}

char *psh_backend_get_homedir_username(char *username)
{
    struct passwd *pwd = getpwnam(username);
    if (pwd == NULL)
        return NULL;
    return pwd->pw_dir;
}

char *psh_backend_get_username(void)
{
    struct passwd *pwd = getpwuid(getuid());
    if (pwd == NULL)
        return NULL;
    return pwd->pw_name;
}

char *psh_backend_getcwd(char *wd, size_t len) { return getcwd(wd, len); }

char *psh_backend_getcwd_dm(void)
{
    /* Providing NULL to getcwd isn't mainstream POSIX */
    char *buf =
        psh_getstring((void *(*)(char *, size_t)) & psh_backend_getcwd, NULL);
    return buf;
}

int psh_backend_gethostname(char *dest, size_t len)
{
    return gethostname(dest, len);
}

char *psh_backend_gethostname_dm(void)
{
    char *buf = psh_getstring(
        (void *(*)(char *, size_t)) & psh_backend_gethostname, NULL);
    return buf;
}

int psh_backend_getuid(void) { return geteuid(); }

int psh_backend_chdir(char *dir) { return chdir(dir); }

int psh_backend_setenv(const char *name, const char *value, int overwrite)
{
    return setenv(name, value, overwrite);
}

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
                    dup2(open(info->out.file, O_WRONLY | O_CREAT | O_TRUNC,
                              0644),
                         info->in.fd);
                    break;
                case OUT_APPN:
                    dup2(open(info->out.file, O_WRONLY | O_CREAT | O_APPEND,
                              0644),
                         info->in.fd);
                    break;
                case IN_REDIR:
                    dup2(open(info->in.file, O_RDONLY | O_CREAT, 0644),
                         info->out.fd);
                    break;
                case CLOSEFD:
                    close(info->in.fd);
                    break;
                case OPENFN:
                    dup2(open(info->in.file, O_RDWR | O_CREAT, 0644),
                         info->out.fd);
                    break;
                case HEREXX: /* Some magic TODO */
                    break;
            }
            info = info->next;
        }
        execve(cmd, argv, env);
    }

    return pid;
}

int psh_backend_do_run(struct command *arginfo, char verbose)
{
    struct command *info = arginfo;

    //verbose can be 0 or 1, if is only true when verbose == 1
    if(verbose){
        int i = 0;
        printf("--**--\nstub!\nflags won't be read\n");
        printf("info position: %p\n", (void *)arginfo);
        while (++i)
        {
            int j;
            printf("part %d:\n"
                   "command: %s\n"
                   "params:\n",
                   i, info->argv[0]);
            for (j = 0; info->argv[j]; ++j)
                printf("%s\n", info->argv[j]);
            printf("flag: %d\n", info->flag);
            printf("redir type: %d\n",
                   (info->rlist != NULL) ? info->rlist->type : 0);

            if (info->next == NULL)
                break;
            info = info->next;
        }
        printf("--*END*--\n");
    }
    if ((ChdPid = fork()) != 0) /*shell*/
    {
        waitpid(ChdPid, &last_command_status, 0); /*wait command1*/
    }
    else /*command1*/
    {
        if (execvp(info->argv[0], (char **)info->argv) == -1)
        {
            if (errno == ENOENT && !strchr(info->argv[0], '/'))
            {
                OUT2E("%s: %s: command not found\n", argv0, info->argv[0]);
                _Exit(127);
            }
            else
            {
                OUT2E("%s: %s: %s\n", argv0, info->argv[0], strerror(errno));
                /* Exit the failed command child process */
                _Exit(126);
            }
        }
    }
    return 0;
}
/* * *
 * Platform dependent builtins part
 * * *
 */
#include "builtin.h"

/* Builtin exec */
int builtin_exec(int argc, char **argv)
{
    if (argc < 2)
        return 0; /* Do nothing */
    if (execv(argv[1], &argv[1]) == -1)
        OUT2E("exec: %s: %s\n", argv[1], strerror(errno));
    return 127;
}
