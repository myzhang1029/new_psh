/*
    psh/backends/posix2/run.c - process-related posix backend
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

#include "backend.h"
#include "psh.h"
/* TODO */
int psh_backend_do_run(psh_state *state, struct _psh_command *cmd)
{
    int i = 0;
    printf("command position: %p\n", cmd);
    while (++i)
    {
        int j;
        printf("part %d:\n"
               "command: %s\n",
               i, cmd->argv[0]);
        for (j = 0; cmd->argv[j]; ++j)
            printf("argv[%d] = %s\n", j, cmd->argv[j]);
        printf("flag: %d\n", cmd->flag);
        printf("redirect type: %d\n",
               (cmd->rlist != NULL) ? cmd->rlist->type : 0);

        if (cmd->next == NULL)
            break;
        cmd = cmd->next;
    }
    return 1;
}
