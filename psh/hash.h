/*
   hash.h - hash functions and structures

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
#include <stdio.h>

typedef struct psh_hash_struct
{
    char *key;
    char *val;
    unsigned used : 1;
    unsigned int len;              /* Only used in the first element */
    unsigned next_count : 6;       /* count for nexts */
    struct psh_hash_struct *nexts; /* array with 64 elements */
} PSH_HASH;

PSH_HASH *realloc_hash(PSH_HASH *, unsigned int);
PSH_HASH *new_hash(unsigned int);
int add_hash(PSH_HASH **, char *, char *);
char *get_hash(PSH_HASH *, char *);
int rm_hash(PSH_HASH *, char *);
void del_hash(PSH_HASH *);

unsigned int hasher(const char *s, unsigned int ulimit);
