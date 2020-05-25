/*
    psh/command.c - psh commands
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

#include <string.h>

#include "command.h"
#include "libpsh/xmalloc.h"

void redirect_init(struct redirect *redir)
{
    redir->in.fd = 0;
    redir->in.file = NULL;
    redir->out.fd = 0;
    redir->out.file = NULL;
    redir->type = 0;
    redir->next = NULL;
}

void free_redirect(struct redirect *redir)
{
    struct redirect *temp;
    while (redir != NULL)
    {
        temp = redir;
        redir = redir->next;
        xfree(temp);
        temp = NULL;
    }
}

/* Malloc a command, enNULL all elements, malloc the first element in argv[] and
 * a struct redirect */
int new_command(struct command **info)
{
    /* XXX: Remove MAXARG, MAXEACHARG */
    *info = xmalloc(sizeof(struct command));
    command_init(*info);
    (*info)->argv = xmalloc(sizeof(char *) * MAXARG);
    memset((*info)->argv, 0, MAXARG); /* This will be used to detect
                           whether an element is used */
    (*info)->argv[0] = xmalloc(P_CS * MAXEACHARG);
    memset((*info)->argv[0], 0, MAXEACHARG);
    (*info)->rlist = xmalloc(sizeof(struct redirect));
    redirect_init((*info)->rlist);
    return 0;
}

void command_init(struct command *info)
{
    info->flag = 0;
    info->rlist = NULL;
    info->argv = NULL;
    info->next = NULL;
}

/* Free a command and its nexts */
void free_command(struct command *info)
{
    struct command *temp;
    while (info != NULL)
    {
        temp = info;
        info = info->next;
        free_argv(temp);
        free_redirect(temp->rlist);
        xfree(temp);
        temp = NULL;
    }
}

void free_argv(struct command *info)
{
    int count;
    for (count = 0; count < MAXARG; ++count)
    {
        if (info->argv[count] != NULL)
        {
            xfree(info->argv[count]);
            info->argv[count] = NULL;
        }
        else
            break; /* All args should be freed after here */
    }
    xfree(info->argv);
    info->argv = NULL;
}
