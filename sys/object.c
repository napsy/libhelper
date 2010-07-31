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

