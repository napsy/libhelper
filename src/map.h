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

#ifndef __MAP_H__
#define __MAP_H__

#define _MAP_DEFAULT_SIZE 1024
#define _MAP_BUCKET_TRESHOLD 16

extern struct _map;

/*
 * map_new:
 * Creates a new map object.
 */
struct _map *map_new();

/*
 * map_add:
 *  @map - a map object
 *  @key - the data identifier
 *  @value - the value to associate with the key
 *
 * Adds a new value to hashmap with the key as the identifier.
 */
void map_add(struct _map *map, const char *key, void *value);

/*
 * map_get:
 *  @map - the map object
 *  @key - the data identifier
 *
 * Retrieves a value with key as the identifier. 
 */
void *map_get(struct _map *map, const char *key);

/*
 * map_clean:
 * @map - the map object
 *
 * Removes any values stored in the hashmap
 */
void map_clear(struct _map *map);

/*
 * map_free:
 * @map - the map object
 *
 * Cleans up the hashmap and frees the memory.
 */
void map_free(struct _map *map);

/*
 * _map_test:
 * Test cases for the map structure.
 */
void _map_test();

#endif

