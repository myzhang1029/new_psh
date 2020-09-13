/*
    libpsh/hash.c - hash table manage functions of the psh

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

#include <stdio.h>
#include <string.h>

#include "libpsh/hash.h"
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"

#define FULL_RATE 0.7

/* Variable naming in this file:
    table: the psh_hash or _psh_hash_container structure in operation;
    using: the _psh_hash_internal structure currently in operation or iterating
over;
    this: the _psh_hash_item structure currently in operation or iterating
over.
*/

/* NUL-init the internal table */
static void internal_initializer(struct _psh_hash_internal *internal,
                                 size_t len)
{
    memset(internal, 0, sizeof(struct _psh_hash_internal) * len);
}

/* Allocate a new hash table, return the table if succeeded */
psh_hash *psh_hash_create(size_t len)
{
    psh_hash *table = xmalloc(sizeof(psh_hash));

    table->len = len;
    table->used = 0;
    /* zero length gets handled too */
    table->table = xmalloc(sizeof(struct _psh_hash_internal) * len);

    internal_initializer(table->table, len);

    return table;
}

/* Same as psh_hash_add, but resizes the hash table if the number of items gets
greater.
 * Table is potentially modified so a reference is passed in.
 */
int psh_hash_add_chk(psh_hash **ptable, const char *key, void *value,
                     int if_free)
{
    /* = for zero-length'd initial allocation */
    if (FULL_RATE * (*ptable)->len <= (*ptable)->used)
    {
        /* The table is almost full, performance degrades */
        /* x<<1 is always greater than FULL_RATE*x where FULL_RATE < 2, so a
         * infinite loop cannot occur */
        *ptable =
            psh_hash_realloc(*ptable, (*ptable)->len ? (*ptable)->len << 1 : 1);
    }
    return psh_hash_add(*ptable, key, value, if_free);
}

/* Add or edit a hash element.
 * If IF_FREE is set, VALUE will be free()d upon the
 * deallocation of the hash table. Returns 0 if succeeded, 1 if not */
int psh_hash_add(psh_hash *table, const char *key, void *value, int if_free)
{
    size_t hash_result;
    struct _psh_hash_internal *using;

    hash_result = hasher(key, table->len);
    using = &(table->table[hash_result]);
    if (using->used == 0)
    {
        /* This hash value is still empty, initialize, and put the key-value
         * pair to add to the first place */
        using->head = xmalloc(sizeof(struct _psh_hash_item));
        using->tail = using->head;
    }
    else
    {
        /* This hash value's taken, first try to find duplicate keys and edit,
         * then, if that failed, add a new one to the linked-list */
        size_t count;
        struct _psh_hash_item *this = using->head;
        /* Iterate over the existing ones to see if an edit should occur, also
         * filtering out any duplicate keys */
        for (count = 0; count < using->used; ++count)
        {
            if (strcmp(key, this->key) == 0)
            {
                if (this->if_free)
                    xfree(this->value);
                this->value = value;
                /* Skip any further iterates, as every items should have been
                 * done this with before being added */
                return 0;
            }
            /* Last one: this->next is random, but the corresponding loop
             * iterate won't be run, so no SEGV.
             * "++count" gets run before evaluating "(count < using->used)", so
             * count == using->used indicates that the for loop reached the end
             * without finding a duplicate key */
            this = this->next;
        } /* for */
        /* No duplicate keys found, append the key-value pair to add to the
         * table */
        using->tail->next = xmalloc(sizeof(struct _psh_hash_item));
        using->tail = using->tail->next;
    }
    /* Duplicate key to prevent further modification */
    using->tail->key = psh_strdup(key);
    using->tail->value = value;
    using->tail->if_free = if_free;
    using->used++;
    table->used++;
    return 0;
}

/* Get a hash value by key, return value if success, NULL if not */
void *psh_hash_get(psh_hash *table, const char *key)
{
    struct _psh_hash_internal *using;
    struct _psh_hash_item *this;
    size_t count;
    size_t hash_result = hasher(key, table->len);

    using = &(table->table[hash_result]);
    this = using->head;

    for (count = 0; count < using->used; ++count)
    {
        if (strcmp(key, this->key) == 0)
            return this->value;
        /* Same discussion as above */
        this = this->next;
    }
    return NULL;
}

/* Resize the hash table.
 * A new table is always created and original items are moved in.
 */
psh_hash *psh_hash_realloc(psh_hash *table, size_t newlen)
{
    struct _psh_hash_internal *using;
    struct _psh_hash_item *this;
    size_t count, count2;
    psh_hash *newtable = psh_hash_create(newlen);
#ifdef DEBUG
    fprintf(stderr, "[hash] realloc %zu\n", newlen);
#endif

    /* Go over the old table and settle the items into the new table */
    for (count = 0, using = table->table; count < table->len; ++count, ++using)
    {
        this = using->head;
        for (count2 = 0; count2 < using->used; ++count2)
        {
            psh_hash_add(newtable, this->key, this->value, this->if_free);
            this = this->next;
        }
    }
    /* free the old table */
    psh_hash_free(table, 0);

    return newtable;
}

/* Remove an element from the hash table, return 0 if success, 1 if specified
 * item not found */
int psh_hash_rm(psh_hash *table, const char *key)
{
    struct _psh_hash_internal *using;
    struct _psh_hash_item *this, *old_this;
    size_t count;
    size_t hash_result = hasher(key, table->len);

    using = &(table->table[hash_result]);
    old_this = NULL;
    this = using->head;

    for (count = 0; count < using->used; ++count)
    {
        if (strcmp(key, this->key) == 0)
        {
            xfree(this->key);
            xfree(this->value);
            if (!old_this)
            {
                /* Removing the first element, using->head == this */
                using->head = this->next;
                /* No modification to tail needed */
            }
            else
                old_this->next = this->next;

            /* Decrease local count */
            using->used--;
            /* Decrease global count */
            table->used--;
            /* Deallocate linked-list item */
            xfree(this);
            return 0;
        }
        /* Same discussion */
        old_this = this;
        this = this->next;
    }
    return 1;
}

/* Free a hash table. if if_free_val is 0, val won't be deallocated,
 * useful in realloc_hash() */
void psh_hash_free(psh_hash *table, int if_free_val)
{
    struct _psh_hash_internal *using;
    struct _psh_hash_item *this, *tmp;
    size_t count, count2;

    for (count = 0, using = table->table; count < table->len; ++count, ++using)
    {
        this = using->head;
        for (count2 = 0; count2 < using->used; ++count2)
        {
            xfree(this->key);
            if (if_free_val && this->if_free)
                xfree(this->value);
            tmp = this;
            this = this->next;
            xfree(tmp);
        }
    }
    xfree(table->table);
    xfree(table);
}
