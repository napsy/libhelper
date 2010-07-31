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
#include <errno.h>
#include <assert.h>

#include "list.h"

struct _list *list_append(struct _list *list, void *data)
{
    if (!data)
        return list;

    struct _list *node = malloc(sizeof(struct _list));
    if (errno == ENOMEM)
        return list;

    if (!list) {
        list = node;
        node->prev = NULL;
        node->last = list;
    } else {
        node->prev = list->last;
        if (list->last)
            list->last->next = node;
        node->prev = list->prev;
        node->last = NULL;
    }

    list->last = node;
    node->data = data;
    node->next = NULL;

    return list;
}

struct _list *list_prepend(struct _list *list, void *data)
{
    if (!data)
        return NULL;

    struct _list *node = malloc(sizeof(struct _list));
    if (errno == ENOMEM)
        return list;

    if (list) {
        node->last = list->last;
    }

    node->data = data;
    node->next = list;
    node->last = list->last;
    node->prev = NULL;
    list->last = NULL;

    return node;
}

int list_length(struct _list *list)
{
    int length = 0;
    for (; list != NULL; list = list->next) 
        length++;
    return length;
}

void _list_print(struct _list *list)
{
    for (; list != NULL; list = list->next) {
        int *data = (int *)list->data;
        printf("%i, ", *data);
    }
    printf("\n");
}

void list_free(struct _list *list)
{
    struct _list *tmp;

    while (list) {
        tmp = list->next;
        free(list);
        list = tmp;
    }
}
void _list_test()
{
    int a = 6, b = 12, c = 1234, d = 4321,
        i = 0, len;
    struct _list *list = NULL, *tmp;

    /* The order of data should be 4321, 12, 6, 1234 */
    list = list_append(list, &a);
    list = list_prepend(list, &b);
    list = list_append(list, &c);
    list = list_prepend(list, &d);

    len = list_length(list);
    assert(len == 4);

    for (tmp = list; tmp != NULL; tmp = tmp->next, i++) {
        int *data = (int *)tmp->data;
        switch (i) {
            case 0: assert(*data == 4321); break;
            case 1: assert(*data == 12); break;
            case 2: assert(*data == 6); break;
            case 3: assert(*data == 1234); break;
            default: fprintf(stdout, "%s: unknown condition while checking list data\n", __func__);
        }
    }
    list_free(list);
    fprintf(stderr, "%s: all tests passed\n", __func__);
}

