/*
   libpsh/hash.h - hash functions and structures

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

/* A single key-value pair */
struct _psh_hash_item
{
    char *key;
    int if_free;
    void *value;
    struct _psh_hash_item *next;
};

/* Internal structure. Used as arrays to hold the hash table */
struct _psh_hash_internal
{
    /* Number of used items */
    size_t used;
    /* Array. all data with the same hash value goes here.
     */
    struct _psh_hash_item *head;
    /* The last item for easy insert */
    struct _psh_hash_item *tail;
};

/* The exported structure for hash tables */
typedef struct _psh_hash_container
{
    /* size of this _psh_hash_internal list */
    size_t len;
    size_t used;
    struct _psh_hash_internal *table;
} psh_hash;

psh_hash *realloc_hash(psh_hash *, size_t);
psh_hash *new_hash(size_t);
int add_hash(psh_hash *, const char *, void *, int if_free);
int add_hash_chk(psh_hash **, const char *, void *, int if_free);
void *get_hash(psh_hash *, const char *);
int rm_hash(psh_hash *, const char *);
void free_hash(psh_hash *, int if_free_val);

size_t hasher(const char *s, size_t ulimit);
