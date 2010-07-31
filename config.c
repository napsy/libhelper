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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#ifdef __linux__
#include <unistd.h>
#endif

#include "map.h"

enum CONFIG_VALUES {
    CONF_VALUE_GROUP,
    CONF_VALUE_TAG
};

int _config_read_next(FILE *in, char *value1, char *value2)
{
    int skip_whitespace = 1, ch, idx = 0;
    int is_group = 0, is_value = 0;

    while ((ch = fgetc(in)) != EOF) {
        if ((ch == ' ' || ch == '\t') && skip_whitespace)
            continue;

        switch (ch) {
            case '[':
                is_group = 1;
                skip_whitespace = 0;
                idx = 0;
                break;
            case ']':
                value1[idx] = 0;
                return CONF_VALUE_GROUP;
            case '=':
                value1[idx] = 0;
                is_value = 1;
                skip_whitespace = 0;
                idx = 0;
                break;
            case '\n':
            case '\r':
                if (!is_value)
                    continue;
                value2[idx] = 0;
                return CONF_VALUE_TAG;
            default:
                if (is_group)
                    value1[idx++] = ch;
                else if (is_value)
                    value2[idx++] = ch;
                else /* default to tag*/
                    value1[idx++] = ch;
        }

    }
    return -1;
}

struct _map *config_read(const char *filename)
{
    FILE *in = fopen(filename, "r");
    int r;
    char *key, *value;
    struct _map *map;
    if (!in)
        return NULL;

    map = map_new();
    if (!map)
        return NULL;

   
    while (!feof(in)) {
        key = malloc(128);
        if (errno == ENOMEM)
            return NULL;
        value = malloc(128);
        if (errno == ENOMEM)
            return NULL;

        r = _config_read_next(in, key, value);
        if (r == CONF_VALUE_GROUP) {
            free(key); free(value);
            fprintf(stderr, "%s: groups not supported, ignoring ...\n", __func__);
        }
        else if (r == CONF_VALUE_TAG)
            map_add(map, key, value);
    }
    fclose(in);
    return map;

}

void config_free(struct _map *config_map)
{
    fprintf(stderr, "%s: TODO\n", __func__);
    map_free(config_map);
}
void _config_test()
{
#ifdef __linux__ /* run test only if we can properly delete the created file. */
    const char *filename = "config_test.txt";
    char *val;
    {
        FILE *dummy_file = fopen(filename, "w+");
        if (!dummy_file)
            return;
        fprintf(dummy_file, "restore_defaults =    true\n\n   help_visible = 0\n");
        fclose(dummy_file);
    }

    struct _map *config;

    config = config_read(filename);
    unlink(filename);

    val = map_get(config, "restore_defaults");
    assert(val != NULL);
    if (strcmp(val, "true") != 0) {
        fprintf(stderr, "%s: '%s' != '%s'\n", __func__, val, "true");
        abort();
    }
    val = map_get(config, "help_visible");
    assert(val != NULL);
    if (strcmp(val, "0") != 0) {
        fprintf(stderr, "%s: '%s' != '%s'\n", __func__, val, "0");
        abort();
    }
    config_free(config);

#endif
}
