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

#ifndef __POOL_H__
#define __POOL_H__

#include <pthread.h>

enum {
    WORKER_STATE_WAITING,
    WORKER_STATE_RUNNING,
};


struct _asyncqueue_item {
    void *data;
    struct _asyncqueue_item *next,
                            *last;
};

struct _asyncqueue {
    struct _asyncqueue_item *head;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
};

struct _asyncqueue *asyncqueue_new()
{
    struct _asyncqueue *queue = malloc(sizeof(*queue));
    if (!queue)
        abort();
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond, NULL);
    queue->head = NULL;

    return queue;
}

void asyncqueue_enqueue(struct _asyncqueue *queue, void *data)
{
    struct _asyncqueue_item *item;
    if (!data)
        abort();

    pthread_mutex_lock(&queue->mutex);
    item = malloc(sizeof(*item));
    if (!item)
        abort();

    item->next = NULL;
    item->last = NULL;
    item->data = data;

    if (!queue->head) {
        queue->head = item;
    } else {
        queue->head->last->next = item;
        queue->head->last = item;
    }
    pthread_mutex_unlock(&queue->mutex);

    printf("%s: broadcasting condition ...\n", __func__);
    pthread_cond_broadcast(&queue->cond);
}

void *asyncqueue_dequeue(struct _asyncqueue *queue)
{
    void *data;
    struct _asyncqueue_item *item;
    pthread_cond_wait(&queue->cond, &queue->mutex);
    printf("%s: got something ...\n", __func__);

    //pthread_mutex_lock(&queue->mutex);
  
    item = queue->head;
    if (!item)
        abort();

    queue->head = item->next;
    if (item->last) {
        if (item->last)
            queue->head->last = item->last;
        else
            queue->head->last = NULL;
    }

    //pthread_mutex_unlock(&queue->mutex);
   
    data = item->data;
    free(item);

    return data;
}

struct _thread_pool_thread {
    pthread_t thread;
    int state;
    void *result;
    struct _thread_pool_thread *next,
                               *last;
};

struct _thread_pool_task {
    void (*worker_func)(void *data);
    void *data;
};

struct thread_pool {
    size_t default_stack_size;
    int task_limit;
    pthread_mutex_t workers_lock,
                    tasks_lock;
    struct _thread_pool_thread *workers;
    struct _asyncqueue *tasks;
};

/*
 * thread_pool_init:
 *   @initial_threads - number of initial worker threads to create
 * Initializes the thread pool by creating the initial worker
 * threads.
 */
struct thread_pool *thread_pool_init(int initial_threads);

/*
 * thread_pool_run:
 *   @worker_func - a function that will perform the task
 *   @data - data to pass to the worker function
 * Adds a task on the task queue.
 */
void thread_pool_run(struct thread_pool *pool, void (*worker_func), void *data);

/*
 * thread_pool_run_now:
 *   @worker_func - a function that will perform the task
 *   @data - data to pass to the worker function
 * If no availabe threads, create a new thread and immediately
 * run the task.
 */
void thread_pool_run_now(void (*worker_func), void *data);

#endif /* __POOL_H__ */
