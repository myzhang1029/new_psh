/*
    psh/backends/posix2.c - psh backend for POSIX systems
    Copyright 2020 Zhang Maiyun

    This file is part of Psh, P shell.

    Psh is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Psh is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "backend.h"
#include "libpsh/util.h"

/*extern*/ char *argv0;
/*extern*/ int last_command_status;

void sig_ignore(int sig) { last_command_status = 128 + sig; }

int psh_backend_prepare(void)
{
    int ret = 0;
    if (signal(SIGCHLD, sig_ignore) == SIG_ERR)
        OUT2E("%s: Error setting signal handler: %s", argv0, strerror(errno)),
            ret++;

    if (signal(SIGINT, sig_ignore) == SIG_ERR)
        OUT2E("%s: Error setting signal handler: %s", argv0, strerror(errno)),
            ret++;

    if (signal(SIGQUIT, sig_ignore) == SIG_ERR)
        OUT2E("%s: Error setting signal handler: %s", argv0, strerror(errno)),
            ret++;
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

/** Spawn a process and set up redirection. */
static pid_t redir_spawnve(struct redirect *info, char *cmd, char **argv,
                           char **env)
{
    pid_t pid;
    if ((pid = fork()) == 0)
    {
        /* Child process */
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
                    close(info->in.fd);
                    break;
                case OUT_APPN:
                    dup2(open(info->out.file, O_WRONLY | O_CREAT | O_APPEND,
                              0644),
                         info->in.fd);
                    close(info->in.fd);
                    break;
                case IN_REDIR:
                    dup2(open(info->in.file, O_RDONLY | O_CREAT, 0644),
                         info->out.fd);
                    close(info->in.fd);
                    break;
                case CLOSEFD:
                    close(info->in.fd);
                    break;
                case OPENFN: {
                    int newfd = open(info->in.file, O_RDWR | O_CREAT, 0644);
                    dup2(newfd, info->out.fd);
                    break;
                }
                case HEREXX:
                    dup2(fileno(info->in.herexx), info->out.fd);
                    fclose(info->in.herexx);
                    break;
            }
            info = info->next;
        }
        execve(cmd, argv, env);
        OUT2E("%s: execve failed: %s", argv0, strerror(errno));
        return -1;
    }
    else if (pid > 0)
        return pid;
    else
    {
        OUT2E("%s: Fork failed: %s", argv0, strerror(errno));
        return -1;
    }
}

#include <ctype.h>
#include <stdlib.h>
#include "command.h"
#include "libpsh/xmalloc.h"

#define streq(str1, str2) (strcmp((str1), (str2)) == 0)
#define move(to, wherefrom)                                                    \
    do                                                                         \
    {                                                                          \
        int n = 0;                                                             \
        do                                                                     \
        {                                                                      \
            *((to) + (n++)) = *((wherefrom) + (n++));                          \
        } while ((to)[n - 1]);                                                 \
    } while (0);

static int isnumber(const char *s)
{
    while (*s)
        if (!isdigit(*s++))
            return 0;
    return 1;
}
int main(int argc, char **argv, char **envp)
{
    struct redirect *info = xmalloc(sizeof(struct redirect)), *info_cur = info;
    char **argv_cur = argv++;
    redirect_init(info);
    while (*++argv_cur)
    {
        if (streq(*argv_cur, ">"))
        {
            int from_fd = STDOUT_FILENO;
            info_cur->out.file = argv_cur[1];
            if (isnumber(argv_cur[-1]))
            {
                from_fd = atoi(argv_cur[-1]);
                move(argv_cur - 1, argv_cur + 2);
                argv_cur -= 2;
            }
            else
            {
                move(argv_cur, argv + 2);
                --argv_cur;
            }
            info_cur->in.fd = from_fd;
            info_cur->type = OUT_REDIR;
            info_cur->next = xmalloc(sizeof(struct redirect));
            info_cur = info_cur->next;
            redirect_init(info_cur);
        }
        else if (streq(*argv_cur, ">>"))
        {
            int from_fd = STDOUT_FILENO;
            info_cur->out.file = argv_cur[+1];
            if (isnumber(argv_cur[-1]))
            {
                from_fd = atoi(argv_cur[-1]);
                move(argv_cur - 1, argv_cur + 2);
                argv_cur -= 2;
            }
            else
            {
                move(argv_cur, argv + 2);
                --argv_cur;
            }
            info_cur->in.fd = from_fd;
            info_cur->type = OUT_REDIR;
            info_cur->next = xmalloc(sizeof(struct redirect));
            info_cur = info_cur->next;
            redirect_init(info_cur);
        }
        else if (streq(*argv_cur, "<"))
        {
            int to_fd = STDIN_FILENO;
            info_cur->in.file = argv_cur[+1];
            if (isnumber(argv_cur[-1]))
            {
                to_fd = atoi(argv_cur[-1]);
                move(argv_cur - 1, argv_cur + 2);
                argv_cur -= 2;
            }
            else
            {
                move(argv_cur, argv + 2);
                --argv_cur;
            }
            info_cur->out.fd = to_fd;
            info_cur->type = IN_REDIR;
            info_cur->next = xmalloc(sizeof(struct redirect));
            info_cur = info_cur->next;
            redirect_init(info_cur);
        }
        else if (streq(*argv_cur, "<<"))
        {
            int to_fd = STDIN_FILENO;
            char *word = argv_cur[+1], *str;
            info_cur->in.herexx = tmpfile();
            if (isnumber(argv_cur[-1]))
            {
                to_fd = atoi(argv_cur[-1]);
                move(argv_cur - 1, argv_cur + 2);
                argv_cur -= 2;
            }
            else
            {
                move(argv_cur, argv + 2);
                --argv_cur;
            }
            info_cur->out.fd = to_fd;
            info_cur->type = HEREXX;
            while (str = psh_fgets("HEREDOC> ", stdin))
            {
                if (streq(str, word))
                    break;
                fputs(str, info_cur->in.herexx);
                fputs("\n", info_cur->in.herexx);
            }
            info_cur->next = xmalloc(sizeof(struct redirect));
            info_cur = info_cur->next;
            redirect_init(info_cur);
        }
        else if (streq(*argv_cur, "<<<"))
        {
            int to_fd = STDIN_FILENO;
            char *word = argv_cur[+1];
            info_cur->in.herexx = tmpfile();
            if (isnumber(argv_cur[-1]))
            {
                to_fd = atoi(argv_cur[-1]);
                move(argv_cur - 1, argv_cur + 2);
                argv_cur -= 2;
            }
            else
            {
                move(argv_cur, argv + 2);
                --argv_cur;
            }
            info_cur->out.fd = to_fd;
            info_cur->type = HEREXX;
            fputs(argv_cur[+1], info_cur->in.herexx);
            info_cur->next = xmalloc(sizeof(struct redirect));
            info_cur = info_cur->next;
            redirect_init(info_cur);
        }
        else if (streq(*argv_cur, "<>"))
        {
            int to_fd = STDOUT_FILENO;
            info_cur->in.file = argv_cur[+1];
            if (isnumber(argv_cur[-1]))
            {
                to_fd = atoi(argv_cur[-1]);
                move(argv_cur - 1, argv_cur + 2);
                argv_cur -= 2;
            }
            else
            {
                move(argv_cur, argv + 2);
                --argv_cur;
            }
            info_cur->out.fd = to_fd;
            info_cur->type = OPENFN;
            info_cur->next = xmalloc(sizeof(struct redirect));
            info_cur = info_cur->next;
            redirect_init(info_cur);
        }
        else if (streq(*argv_cur, "&"))
        {
            *argv_cur = NULL;
            printf("[n] bg %d\n", redir_spawnve(info, argv[0], argv, envp));
            argv = ++argv_cur;
            free_redirect(info);
            info = info_cur = xmalloc(sizeof(struct redirect));
            redirect_init(info);
        }
        else if (streq(*argv_cur, "&&"))
        {
            *argv_cur = NULL;
            printf("[n] and %d\n", redir_spawnve(info, argv[0], argv, envp));
            argv = ++argv_cur;
            free_redirect(info);
            info = info_cur = xmalloc(sizeof(struct redirect));
            redirect_init(info);
        }
        else if (streq(*argv_cur, "|"))
        {
        }
        else if (streq(*argv_cur, "||"))
        {
        }
        else if (streq(*argv_cur, ";"))
        {
        }
    }
    printf("[n] cm %d\n", redir_spawnve(info, argv[0], argv, envp));
    free_redirect(info);
    return 0;
}