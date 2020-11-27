/*
    psh/builtins/exit.c - builtin exit
    Copyright 2017 Zhang Maiyun.

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

#include <stdlib.h>

#include "builtin.h"
#include "psh.h"
#include "util.h"
#include "variable.h"

int builtin_exit(int argc, char **argv, psh_state *state)
{
    if (argc < 2)
        exit_psh(state, psh_vf_getint(state, "?"));
    else
    {
        int i = atoi(argv[1]);
        exit_psh(state, i);
    }
    return 1; /* Noreturn */
}
