/*
Copyright (c) 2011, Luka Napotnik <luka.napotnik@gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "pool.h"

void *thread_pool_worker(void *data)
{
    struct _thread_pool_task *task;
    struct thread_pool *pool = (struct thread_pool *)data;
    while (1) {
        printf("%s: waiting for task ...\n", __func__);
        task = asyncqueue_dequeue(pool->tasks);
    }
}

void thread_pool_setup(struct thread_pool *pool, size_t stack_size, int task_limit)
{
    pool->default_stack_size = stack_size;
    pool->task_limit = task_limit;
}

void thread_pool_add_worker(struct thread_pool *pool) 
{
    struct _thread_pool_thread *worker;
    pthread_attr_t attr;

    worker = malloc(sizeof(*worker));
    if (!worker)
        abort();

    worker->state = WORKER_STATE_WAITING;
    pthread_mutex_lock(&pool->workers_lock);
    worker->next = pool->workers;
    pool->workers = worker;
    pthread_mutex_unlock(&pool->workers_lock);

    // NOTE: check if gcc 4.6 and use -fsplit-stack
    //pthread_attr_setstacksize(&attr, pool->default_stack_size);
    pthread_create(&worker->thread, NULL, thread_pool_worker, pool);
}

struct thread_pool *thread_pool_init(int initial_threads)
{
    if (initial_threads < 1)
        abort();

    struct thread_pool *pool = malloc(sizeof(*pool));
    pool->tasks = asyncqueue_new();
    pool->workers = NULL;
    pool->default_stack_size = 2<<20;
    pool->task_limit = -1; // unlimited
    pthread_mutex_init(&pool->workers_lock, NULL);
    pthread_mutex_init(&pool->tasks_lock, NULL);

    int i = 0;

    for (; i < initial_threads; i++) {
        thread_pool_add_worker(pool);
    }

    return pool;
}

void thread_pool_run(struct thread_pool *pool, void (*worker_func), void *data)
{
    struct _thread_pool_task *task = malloc(sizeof(*task));
    if (!task)
        abort();

    task->worker_func = worker_func;
    task->data = data;

    asyncqueue_enqueue(pool->tasks, task);
}

void thread_pool_run_now(void (*worker_func), void *data)
{
}

void worker1(void *data)
{
    printf("%s: working ...\n", __func__);
}

int main()
{
    struct thread_pool *pool = thread_pool_init(3);

    usleep(2*1000000);
    printf("Adding task ...\n");
    thread_pool_run(pool, worker1, NULL);

    usleep(10*10000000);
    return 0;
}

