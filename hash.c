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

#include "pshell.h"
#include "hash.h"
int realloc_hash(PSH_HASH *table, size_t newlen)
{
	size_t oldlen=table->len;

	if(! (newlen > oldlen))
		return 1;

	if(realloc(table, newlen)==NULL)
	{
		OUT2E("%s: realloc_hash: %s\n", argv0, strerror(errno));
		return 2;
	}
	else
		table->len=newlen;

	for(;oldlen<newlen;++oldlen)
		table[oldlen].used=0;

	return 0;
}

/* Search for empty hash slot, malloc more in case of table is full */
static int search_hash_empty(PSH_HASH *table)
{
	int i;
	for(i=0; i < table->len; ++i)
		if(table[i].used==0)
			return i;
	if(realloc_hash(table, table->len+8))
		return -1;
	return table->len-7;
}
	
	
/* Allocate a new hash table */
PSH_HASH *new_hash(size_t len)
{
	PSH_HASH *table=malloc(sizeof(PSH_HASH) * len);
	if(table==NULL)
	{
		OUT2E("%s: new_hash: %s\n", argv0, strerror(errno));
		return NULL;
	}

	table[0].len=len;

	for(int i=0; i<len; ++i)
	{
		table[i].used=0;
	}

	return table;
}

static int edit_hash_elem(PSH_HASH elem, char *val)
{
	/* Need more space/free extra space */
	if(realloc(elem.val, strlen(val)+1)==NULL)
	{
		OUT2E("%s: Unable to realloc: %s\n", argv0, strerror(errno));
		strncpy(elem.val, val, strlen(elem.val));
		return 0;
	}
	strcpy(elem.val, val);
	return 0;
}

static int search_for_element_by_key(PSH_HASH *table, char *key)
{
	int i;
	for(i=0; i < table->len; ++i)
		if(strcmp(table[i].key, key)==0)
			return 1;
	return -1;
}

/* Add or edit a hash element */
int add_hash(PSH_HASH *table, char *key, char *val)
{
	int i, hash_result=hasher(key, table->len);
	if(table[hash_result].used!=0)
	{
		/* Doing edit */
		if(strcmp(table[hash_result].key,key)==0)
		{
			return edit_hash_elem(table[hash_result], val);
		}
		if((i=search_for_element_by_key(table, key))>=0)
			return edit_hash_elem(table[i], val);
		hash_result=search_hash_empty(table);
		if(hash_result==-1)
		{
			OUT2E("%s: add_hash: Unable to add more element\n", argv0);
			return 1;
		}
	}
	table[hash_result].used=1;
	table[hash_result].key=malloc(strlen(key)+1);
	strcpy(table[hash_result].key, key);
	/* Write element */
	return edit_hash_elem(table[hash_result], val);
}

