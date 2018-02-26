/*
   hash.c - hash table manage functions of the psh

   Copyright 2017 Zhang Maiyun.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "hash.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pshell.h"

/* Resize the hash table, the new size cannot be lower than the old size,
 * otherwise return 1. return 2 if realloc failed, 0 if success */
int realloc_hash(PSH_HASH *table, int newlen)
{
	int oldlen = table->len;

	if (!(newlen > oldlen))
		return 1;

	if (realloc(table, newlen) == NULL)
	{
		OUT2E("%s: realloc_hash: %s\n", argv0, strerror(errno));
		return 2;
	}
	else
		table->len = newlen;

	for (; oldlen < newlen; ++oldlen)
		table[oldlen].used = 0;

	return 0;
}

/* Search for empty hash slot, malloc more in case of table is full, return the
 * found position, -1 if failed */
static int search_hash_empty(PSH_HASH *table)
{
	int i;
	for (i = 0; i < table->len; ++i)
		if (table[i].used == 0)
			return i;
	if (realloc_hash(table, table->len + 8))
		return -1;
	return table->len - 7;
}

/* Allocate a new hash table, return the table if success, NULL if not */
PSH_HASH *new_hash(int len)
{
	int i;
	PSH_HASH *table = malloc(sizeof(PSH_HASH) * len);
	if (table == NULL)
	{
		OUT2E("%s: new_hash: %s\n", argv0, strerror(errno));
		return NULL;
	}

	table[0].len = len;

	for (i = 0; i < len; ++i)
	{
		table[i].key = table[i].val = NULL;
		table[i].used = 0;
	}

	return table;
}

/* Edit the vaule of an element, return 0 if success, 1 if not */
static int edit_hash_elem(PSH_HASH *elem, char *val)
{
	/* Need more space/free extra space */
	if (((*elem).val = realloc((*elem).val, strlen(val) + 1)) == NULL)
	{
		OUT2E("%s: Unable to realloc: %s\n", argv0, strerror(errno));
		strncpy((*elem).val, val, strlen((*elem).val));
		return 1;
	}
	strcpy((*elem).val, val);
	return 0;
}

/* Search for an element by key, return position if found, -1 if not */
static int search_for_element_by_key(PSH_HASH *table, char *key)
{
	int i;
	for (i = 0; i < (table->len); ++i)
		if (table[i].key != NULL && strcmp(table[i].key, key) == 0)
			return i;
	return -1;
}

/* Add or edit a hash element, return 0 if success, 1 if not */
int add_hash(PSH_HASH *table, char *key, char *val)
{
	int i;
	int hash_result = hasher(key, table->len);
	if (table[hash_result].used != 0)
	{
		/* Doing edit */
		if (strcmp(table[hash_result].key, key) == 0)
		{
			return edit_hash_elem(&table[hash_result], val);
		}
		if ((i = search_for_element_by_key(table, key)) >= 0)
			return edit_hash_elem(&table[i], val);
		hash_result = search_hash_empty(table);
		if (hash_result == -1)
		{
			OUT2E("%s: add_hash: Unable to add more element\n", argv0);
			return 1;
		}
	}
	table[hash_result].used = 1;
	table[hash_result].key = malloc(strlen(key) + 1);
	strcpy(table[hash_result].key, key);
	/* Write element */
	return edit_hash_elem(&table[hash_result], val);
}

/* Get a hash value by key, return value if success, NULL if not */
char *get_hash(PSH_HASH *table, char *key)
{
	int hash_result = hasher(key, table->len);
	if (table[hash_result].key != NULL)
		if (strcmp(table[hash_result].key, key) == 0)
			return table[hash_result].val;
	hash_result = search_for_element_by_key(table, key);
	if (hash_result == -1 || table[hash_result].used == 0)
		return NULL;
	return table[hash_result].val;
}

/* Remove an element from the hash table, return 0 if success, 1 if specified
 * element not found */
int rm_hash(PSH_HASH *table, char *key)
{
	int hash_result = hasher(key, table->len);
	if (strcmp(table[hash_result].key, key) == 0)
	{
		if (table[hash_result].used == 0)
			return 1;
		table[hash_result].used = 0;
		free(table[hash_result].key);
		table[hash_result].key = NULL;
		free(table[hash_result].val);
		table[hash_result].val = NULL;
		return 0;
	}
	hash_result = search_for_element_by_key(table, key);
	if (hash_result == -1 || table[hash_result].used == 0)
		return 1;
	table[hash_result].used = 0;
	free(table[hash_result].key);
	free(table[hash_result].val);
	return 0;
}

/* Free a hash table. This function don’t return a value */
void del_hash(PSH_HASH *table)
{
	int i;
	for (i = 0; i < (table->len); ++i)
	{
		if (table[i].used != 0)
		{
			free(table[i].key);
			free(table[i].val);
		}
	}
	free(table);
}
