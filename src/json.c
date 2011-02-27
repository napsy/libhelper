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
#include <string.h>
#include <stdlib.h>

enum {
    JSON_STRING,
    JSON_OBJECT,
    JSON_ARRAY,
};

struct _json_attribute {
    char *name;
    int  value_type;
    void *value;
    struct _json_attribute *children,
                           *next;
};

struct _json_attribute *get_next_attribute(FILE *json_input, struct _json_attribute *prev_attr);

struct _json_attribute *json_next(FILE *json_input)
{
    int ch;
    struct _json_attribute *attrs = NULL;

    while (1) {
        ch = fgetc(json_input);
        if (ch == EOF)
            break;
        switch (ch) {
            case ' ':
            case '\t':
                continue;
            case '{':
                attrs = get_next_attribute(json_input, NULL);
                break;
        }
    }
    return attrs;
}

struct _json_attribute *get_next_attribute(FILE *json_input, struct _json_attribute *prev_attr)
{
    struct _json_attribute *attr, *value_attr, *next_attr;
    int ch, i = 0,
        inside_quote = 0,
        got_name = 0,
        got_object = 0;
    char name[128],
         value[128];

    attr = malloc(sizeof(*attr));

    while (1) {
        ch = fgetc(json_input);
        switch (ch) {
            case EOF:
                goto end_loop;
            case '{':
                if (!got_name)
                    continue;
                printf("(value of '%s' is an object)\n", name);
                ungetc(ch, json_input);
                value_attr = json_next(json_input);
                got_object = 1;
                break;
            case '}':
                goto end_loop;
            case '\"':
                if (!inside_quote)
                    inside_quote = 1;
                else {
                    inside_quote = 0;
                }
                break;
            case ',':

                next_attr = get_next_attribute(json_input, prev_attr);
                //goto end_loop;
                break;
            case ':':
                got_name = 1;
                name[i] = 0;
                i = 0;
                break;
            case '\n':
                continue;
            case ' ':
                if (!inside_quote)
                    continue;
            default:
                if (!got_name)
                    name[i++] = ch;
                else
                    value[i++] = ch;
        }
    }
end_loop:
    value[i] = 0;
    printf("'%s' : '%s'\n", name, value);


    attr->name = strdup(name);
    if (got_object) {
        attr->value = value_attr;
        attr->value_type = JSON_OBJECT;
    } else {
        attr->value = strdup(value);
        attr->value_type = JSON_STRING;
    }
    if (prev_attr) { // prepend attribute to list.
        attr->next = prev_attr->children;
        prev_attr->children = attr;
    }
    return attr;
}

int main()
{
    FILE *json_input = fopen("example.json", "r");
    if (!json_input)
        return -1;

    json_next(json_input);
    fclose(json_input);
    return 0;
}
