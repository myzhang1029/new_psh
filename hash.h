/*
   hash.h - hash functions and structures

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

typedef struct psh_hash_struct
{
	char *key;
	char *val;
	char used;
	size_t len;
}PSH_HASH;

int realloc_hash(PSH_HASH *, size_t);
PSH_HASH *new_hash(size_t);
int add_hash(PSH_HASH *, char *, char *);
char *get_hash(PSH_HASH *, char *);
int rm_hash(PSH_HASH *, char *);
void del_hash(PSH_HASH *);

unsigned int
hasher(const char *s, unsigned int ulimit);

