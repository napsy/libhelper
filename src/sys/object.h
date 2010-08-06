#ifndef __OBJECT_H__
#define __OBJECT_H__
#include <stdlib.h>

struct _obj {
    unsigned int refcount;
};

/*
 * obj_new:
 * @size: size for the new object in bytes
 * Creates a new reference-counted object with given size.
 */
void *obj_new(size_t size);
void *obj_unref(void *obj_ptr);
void *obj_ref(void *obj_ptr);
void *obj_copy(void *obj_ptr);
#endif

