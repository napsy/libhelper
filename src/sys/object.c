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

#include "object.h"

#include <stdio.h>
#include <assert.h>

void *obj_new(size_t size)
{
    struct _obj *obj = malloc(size + sizeof(struct _obj));
    obj->refcount = 1;
    return (void *)(obj - sizeof(struct _obj));
}

void *obj_unref(void *obj_ptr)
{
    assert(obj_ptr);
    unsigned int refcount;
    struct _obj *obj = (struct _obj*)obj_ptr + sizeof(struct _obj);
#ifdef __linux__
    /* GCC extension for atomic operations. */
    refcount = __sync_sub_and_fetch(&obj->refcount, 1);
#endif
    assert(refcount >= 0);
    if (refcount == 0) {
        free(obj);
        return NULL;
    }
    return obj_ptr;
}

void *obj_ref(void *obj_ptr)
{
    assert(obj_ptr);
    struct _obj *obj = (struct _obj*)obj_ptr + sizeof(struct _obj);
#ifdef __linux__
    /* GCC extension for atomic operations. */
    __sync_fetch_and_add(&obj->refcount, 1);
#endif
    return obj_ptr;
}
void obj_inspect(void *obj_ptr)
{
    assert(obj_ptr);
    struct _obj *obj = (struct _obj*)obj_ptr + sizeof(struct _obj);
    fprintf(stderr, "- object reference count: %i\n", obj->refcount);
}

void _obj_test()
{
    void *a = obj_new(sizeof(int));
    void *b = obj_ref(a);
    obj_inspect(a);
    a = obj_unref(a);
    obj_inspect(b);
    a = obj_unref(a);
    obj_inspect(a);
    a = obj_unref(b);
    //obj_inspect(a);
}

