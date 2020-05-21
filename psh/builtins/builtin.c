/*
   psh/builtins/builtin.c - builtin command builtin

   Copyright 2017-2020 Zhang Maiyun.

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

#include <stdio.h>
#include <stdlib.h>

#include "builtin.h"
#include "libpsh/util.h"

int builtin_builtin(int argc, char **argv)
{
    builtin_function bltin;
    if (argc < 2)
        return 0;
    bltin = find_builtin(argv[1]);
    if (bltin == 0)
    {
        OUT2E("%s: %s: %s: not a shell builtin\n", argv0, argv[0], argv[1]);
        return 1;
    }
    return (*bltin)(--argc, ++argv);
}
