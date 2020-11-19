/*
    psh/backends/posix2/builtin_exec.c - builtin exec for POSIX
    Copyright 2020 Zhang Maiyun.

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
#include <string.h>
#include <unistd.h>

#include "backend.h"
#include "builtin.h"
#include "libpsh/util.h"

/* Builtin exec */
int builtin_exec(int argc, char **argv, psh_state *state)
{
    if (argc < 2)
        return 0; /* Do nothing */
    if (execv(argv[1], &argv[1]) == -1)
        OUT2E("exec: %s: %s\n", argv[1], strerror(errno));
    return 127;
}