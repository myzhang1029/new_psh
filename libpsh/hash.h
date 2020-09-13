/** @file libpsh/hash.h - @brief Hash functions and structures */
/*
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

/** @brief A single key-value pair with @ref next pointer. */
struct _psh_hash_item
{
    /** The key. */
    char *key;
    /** Whether @ref value should be free()d afterwards. */
    int if_free;
    /** The value */
    void *value;
    /** Linked-list of all items with the same hash value. */
    struct _psh_hash_item *next;
};

/** @brief Internal structure used as arrays to hold the hash table. */
struct _psh_hash_internal
{
    /** Number of used items. */
    size_t used;
    /** Linked-list of all data with the same hash value. */
    struct _psh_hash_item *head;
    /** Pointer to the last item. */
    struct _psh_hash_item *tail;
};

/** @brief Container structure of hash tables. */
typedef struct _psh_hash_container
{
    /** Number of total slots in @ref table. */
    size_t len;
    /** Number of used slots in @ref table. */
    size_t used;
    /** List of items. */
    struct _psh_hash_internal *table;
} psh_hash;

/** Resize the hash table.
 *
 * @param table The table to resize.
 * @param newsize New size of the table.
 * @return A new table with the new size and original data.
 */
psh_hash *psh_hash_realloc(psh_hash *table, size_t newsize);

/** Create a new hash table.
 *
 * @param size Initial size of the table.
 * @return Pointer to the created psh_hash structure (_the table_).
 */
psh_hash *psh_hash_create(size_t size);

/** Add an item to the hash table.
 *
 * @param table The table to operate.
 * @param key The key.
 * @param value The value.
 * @param if_free Whether @p value should be free()d upon table deallocation.
 * @return 0 if succeeded, 1 if not.
 */
int psh_hash_add(psh_hash *table, const char *key, void *value, int if_free);

/** Add an item to the hash table, expand @p table if FULL_RATE is reached.
 *
 * @param ptable Pointer to the table to operate.
 * @param key The key.
 * @param value The value.
 * @param if_free Whether @p value should be free()d upon table deallocation.
 * @return 0 if succeeded, 1 otherwise.
 */
int psh_hash_add_chk(psh_hash **ptable, const char *key, void *value,
                     int if_free);

/** Get an item by key.
 *
 * @param table The table to look up from.
 * @param key The key.
 * @return The corresponding value if found, NULL otherwise.
 */
void *psh_hash_get(psh_hash *table, const char *key);

/** Remove an item by key.
 *
 * @param table The table to operate.
 * @param key The key.
 * @return 0 if succeeded, 1 if no such item exists.
 */
int psh_hash_rm(psh_hash *table, const char *key);

/** Deallocate a hash table.
 *
 * @param table The table free.
 * @param if_free_val Set to 0 if values shouldn't be free()d/
 */
void psh_hash_free(psh_hash *table, int if_free_val);

/** Hash function.
 *
 * @param s The key.
 * @param ulimit Upper limit of the value.
 * @return The value.
 */
size_t hasher(const char *s, size_t ulimit);
