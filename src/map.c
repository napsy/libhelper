/*
 * Copyright (c) 2010, Luka Napotnik <luka.napotnik@gmail.com>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "map.h"

struct _map_entry {
    unsigned long hash;
    void *value;
    struct _map_entry *next;
};

struct _map {
    struct _map_entry *buckets;
    int size;
};

/*
public domain, from http://www.cse.yorku.ca/~oz/hash.html
 */
static unsigned long _sdbm( unsigned char *str)
{
    unsigned long hash = 0;
    int c;
    while ((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;
    return hash;
}


static unsigned long hash_sdbm(unsigned char *str, int table_size)
{
    unsigned long hash = _sdbm(str);
    return (hash < 0) ? (hash*-1)%table_size : hash % table_size;
}


struct _map *map_new()
{
    struct _map *map = malloc(sizeof(struct _map));
    if (errno == ENOMEM)
        return NULL;
    map->size = _MAP_DEFAULT_SIZE;
    map->buckets = malloc(_MAP_DEFAULT_SIZE * sizeof(struct _map_entry));
    memset(map->buckets, 0, _MAP_DEFAULT_SIZE);
    if (errno == ENOMEM)
        return NULL;

    return map;
}

void map_add(struct _map *map, const char *key, void *value)
{
    if (!map || !key || !value)
        return;

    struct _map_entry *entry;

    unsigned long hash = hash_sdbm((unsigned char *)key, map->size);
    struct _map_entry *last_entry = &map->buckets[hash];
    if (!map->buckets[hash].value) { /* bucket is empty. */
        entry = &map->buckets[hash];
    } else {
        for (; last_entry->next != NULL; last_entry = last_entry->next);
        entry = malloc(sizeof(struct _map_entry));
    }
    if (last_entry != entry)
        last_entry->next = entry;
    entry->value = value;
    entry->hash = hash;
    entry->next = NULL;

}

void *map_get(struct _map *map, const char *key)
{
    int hash = hash_sdbm((unsigned char *)key, map->size);
    struct _map_entry *entry = &map->buckets[hash];

    for (; entry; entry = entry->next) { /* linear search. */
        if (entry->hash == hash)
            break;
    }
    if (!entry) /* We haven't found the key. */
        return NULL;

    return entry->value;
}

void map_clear(struct _map *map)
{
    int i;
    for (i = 0; i < map->size; i++) {
        struct _map_entry *first_entry = &map->buckets[i],
                          *tmp;
        if (!first_entry->value)
            continue;
        struct _map_entry *entry = first_entry->next; /* skip the first entry. */
        while (entry) {
            tmp = entry->next;
            free(entry);
            entry = tmp;
        }
        first_entry->value = NULL;
    }
}

void _map_resize(struct _map *map)
{
    /* Increase the bucket numbers and rehash the entire map. */
    fprintf(stderr, "%s: TODO\n", __func__);
}

void map_free(struct _map *map)
{
    if (!map)
        return;

    map_clear(map);
    free(map->buckets);
    free(map);
}

void _map_test()
{
    int a = 5, b = 12, c = 12345, d = 54321;
    int *aa, *bb, *cc, *dd, *ff;
    int repeat = 1;
    char *e = "Immanuel Kant",
         *ee;

    struct _map *map = map_new();
    assert(map != NULL);

    map_add(map, "rest not", &a);
    map_add(map, "life is sweeping by", &b);
    map_add(map, "go and do before you die", &c);
    map_add(map, "something mighty and sublime", &d);
    map_add(map, "leave behind to conquer time", e);

_repeat:
    aa = map_get(map, "rest not");
    if (repeat)
        assert(*aa == 5);
    else
        assert(aa == NULL);

    bb = map_get(map, "life is sweeping by");
    if (repeat)
        assert(*bb == 12);
    else
        assert(bb == NULL);
    cc = map_get(map, "go and do before you die");
    if (repeat)
        assert(*cc == 12345);
    else
        assert(cc == NULL);
    dd = map_get(map, "something mighty and sublime");
    if (repeat)
        assert(*dd == 54321);
    else
        assert(dd == NULL);
    ee = map_get(map, "leave behind to conquer time");
    if (repeat)
        assert(ee == e);
    else
        assert(ee == NULL);
    ff = map_get(map, "this is a nonexistent key");
    assert(ff == NULL);
    if (repeat) {
        repeat = 0;
        map_clear(map);
        goto _repeat;
    }
    fprintf(stdout, "%s: all tests passed\n", __func__);
    map_free(map);
}

