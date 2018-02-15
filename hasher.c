/* hashlib.c -- functions to manage and access hash tables for bash. */
/* hasher.c -- function to gen hash for psh, */

/* Copyright (C) 1987,1989,1991,1995,1998,2001,2003,2005,2006,2008,2009 Free
   Software Foundation, Inc. Cpoyright (C) 2017 Zhang Maiyun.

   This file is part of GNU Bash, the Bourne Again SHell.

   Bash is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Bash is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Bash.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdio.h>

int hasher(const char *s, int ulimit)
{
	int i;
	for (i = 0; *s; s++)
	{
		i *= 16777619;
		i ^= *s;
	}

	return i % ulimit;
}
