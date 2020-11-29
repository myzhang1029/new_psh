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

void redirect_init(struct _psh_redirect *redir)
{
    redir->in.fd = 0;
    redir->in.file = NULL;
    redir->out.fd = 0;
    redir->out.file = NULL;
    redir->type = 0;
    redir->next = NULL;
}

void free_redirect(struct _psh_redirect *redir)
{
    struct _psh_redirect *temp;
    while (redir != NULL)
    {
        temp = redir;
        redir = redir->next;
        xfree(temp);
        temp = NULL;
    }
}

/* Malloc a command, enNULL all elements, malloc the first element in argv[] and
 * a struct _psh_redirect */
struct _psh_command *new_command()
{
    /* XXX: Remove MAXARG, MAXEACHARG */
    struct _psh_command *cmd;
    cmd = xmalloc(sizeof(struct _psh_command));
    command_init(cmd);
    cmd->argv = xmalloc(sizeof(char *) * MAXARG);
    memset(cmd->argv, 0, MAXARG); /* This will be used to detect
                           whether an element is used */
    cmd->argv[0] = xmalloc(P_CS * MAXEACHARG);
    memset(cmd->argv[0], 0, MAXEACHARG);
    cmd->rlist = xmalloc(sizeof(struct _psh_redirect));
    redirect_init(cmd->rlist);
    return cmd;
}

void command_init(struct _psh_command *cmd)
{
    cmd->type = 0;
    cmd->rlist = NULL;
    cmd->argv = NULL;
    cmd->next = NULL;
}

/* Free a command and its nexts */
void free_command(struct _psh_command *cmd)
{
    struct _psh_command *temp;
    while (cmd != NULL)
    {
        temp = cmd;
        cmd = cmd->next;
        free_argv(temp);
        free_redirect(temp->rlist);
        xfree(temp);
        temp = NULL;
    }
}

void free_argv(struct _psh_command *cmd)
{
    int count;
    for (count = 0; count < MAXARG; ++count)
    {
        if (cmd->argv[count] == NULL)
            break; /* All args should be freed after here */
        xfree(cmd->argv[count]);
        cmd->argv[count] = NULL;
    }
    xfree(cmd->argv);
    cmd->argv = NULL;
}
