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
#ifndef __OBJPOOL_H__
#define __OBJPOOL_H__

struct _objpool {
    int total_size,
        object_size,
        used_n;
    void *pool;
};

/*
 * objpool_new:
 *  @pool_size - number of objects we want space for
 *  @object_size - size of one object
 *
 * Creates a new memory pool that is large enough to store
 * pool_size objects of object_size size.
 */
struct _objpool *objpool_new(int pool_size, int object_size);

/*
 * objpool_get:
 *  @pool - the memory pool we want 
 * Returns a free block inside the given memory pool.
 */
void *objpool_get(struct _objpool *pool);

/*
 * objpool_ret:
 *  @pool - the memory pool
 *  @ptr - block to return to the pool
 *
 * Returns the block to the pool.
 */
void objpool_ret(struct _objpool *pool, void *ptr);

#endif

