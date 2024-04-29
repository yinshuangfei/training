/*
 * @Author: Alan Yin
 * @Date: 2024-04-20 23:39:09
 * @LastEditTime: 2024-04-21 14:30:51
 * @LastEditors: Alan Yin
 * @FilePath: /self_project/03_thread_pool/thread_pool.c
 * @Description:
 *
 * Copyright (c) 2024 by HT706, All Rights Reserved.
 */

#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "thread_pool.h"

static void threadpool_free(threadpool_t *pool)
{
	if (pool != NULL) {
		SAFE_FREE(pool->threads);
		SAFE_FREE(pool->queue);

		/**
		 * Because we allocate pool->threads after initializing the
		 * mutex and condition variable, we're sure they're initialized.
		 * Let's lock the mutex just in case.
		 * */
		pthread_mutex_lock(&pool->lock);
		pthread_mutex_destroy(&pool->lock);
		pthread_cond_destroy(&pool->notify);
	}
}

static void *threadpool_threadfunc(void *threadpool)
{
	threadpool_t *pool = (threadpool_t *)threadpool;
	threadpool_task_t task;

	pr_info("Thread Info: STID[%ld]", pthread_self());

	for (;;) {
		pthread_mutex_lock(&pool->lock);
		while (pool->count == 0 && pool->shutdown == 0) {
			/**
			 * !!!持有锁的前提下等待条件!!!
			 * 不满足条件则在内部释放锁，等待条件满足
			 * 满足条件后，持有锁，返回
			 *  */
			pthread_cond_wait(&pool->notify, &pool->lock);
		}

		/**
		 * grachful_shutdown 会将队列中的任务执行完再退出
		 * */
		if (pool->shutdown == immediate_shutdown ||
		    (pool->shutdown == grachful_shutdown && pool->count == 0)) {
			break;
		}

		task.func = pool->queue[pool->head].func;
		task.arg = pool->queue[pool->head].arg;

		pool->head = (pool->head + 1) % pool->queue_size;
		pool->count--;

		pthread_mutex_unlock(&pool->lock);

		/** 执行任务 */
		(*(task.func))(task.arg);
	}

	pool->started--;
	pr_info("Thread Info: STID[%ld], still start [%d/%d]", pthread_self(),
		 pool->started, pool->thread_count);

	pthread_mutex_unlock(&pool->lock);
	pthread_exit(NULL);
}

static int threadpool_add(threadpool_t *pool, void (*func)(void *), void *arg)
{
	int ret = 0;

	if (pool == NULL || func == NULL) {
		pr_err();
		return -EINVAL;
	}

	pthread_mutex_lock(&pool->lock);
	do {
		if (pool->count == pool->queue_size) {
			ret = -1;
			pr_err("thread pool queue is full");
			break;
		}

		if (pool->shutdown) {
			ret = -1;
			pr_err("thread pool is shutdown");
			break;
		}

		pool->queue[pool->tail].func = func;
		pool->queue[pool->tail].arg = arg;
		pool->tail = (pool->tail + 1) % pool->queue_size;
		pool->count++;

		/** 唤醒一个线程执行任务 */
		if (pthread_cond_signal(&pool->notify) != 0) {
			ret = -1;
			pr_err("call pthread_cond_signal failed (%d:%s)",
				errno, strerror(errno));
			break;
		}
	} while (0) ;

	pthread_mutex_unlock(&pool->lock);

	return ret;
}

static threadpool_t *threadpool_create(int thread_count, int queue_size)
{
	threadpool_t *pool = NULL;

	if (thread_count <= 0 ||
	    thread_count > MAX_THREADS ||
	    queue_size <= 0 ||
	    queue_size > MAX_QUEUE) {
		pr_err("threadpool_create with invalid param");
		return pool;
	}

	pool = malloc(sizeof(threadpool_t));
	if (pool == NULL) {
		pr_err("malloc threadpool failed");
		goto err;
	}

	pool->thread_count = thread_count;
	pool->started = 0;

	pool->queue_size = queue_size;
	pool->head = pool->tail = pool->count = 0;

	pool->shutdown = 0;

	pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);
	if (pool->threads == NULL) {
		pr_err("malloc threads failed");
		goto err;
	}

	pool->queue = (threadpool_task_t *)
		      malloc(sizeof(threadpool_task_t) * queue_size);
	if (pool->queue == NULL) {
		pr_err("malloc queue failed");
		goto err;
	}

	if (pthread_mutex_init(&pool->lock, NULL) != 0) {
		pr_err("pthread_mutex_init failed, (%d:%s)", errno,
			strerror(errno));
		goto err;
	}

	if (pthread_cond_init(&pool->notify, NULL) != 0) {
		pr_err("pthread_cond_init failed, (%d:%s)", errno,
			strerror(errno));
		goto err;
	}

	for (int i = 0; i < thread_count; i++) {
		if (pthread_create(&pool->threads[i], NULL,
				   threadpool_threadfunc, (void *)pool) != 0) {
			pr_err("pthread_create failed, (%d:%s)", errno,
				strerror(errno));
			goto err;
		}
		pool->started++;
	}

	pr_info("Threadpool Info: %d/%d started", pool->started, thread_count);

	return pool;

err:
	if (pool) {
		threadpool_free(pool);
	}

	return NULL;
}

int threadpool_init(int thread_count, int queue_size, threadpool_t **threadpool)
{
	threadpool_t *pool = threadpool_create(thread_count, queue_size);
	if (pool == NULL)
		return -1;

	*threadpool = pool;
	return 0;
}

int threadpool_dispath(threadpool_t *pool, void (*func)(void *), void *arg)
{
	return threadpool_add(pool, func, arg);
}

void threadpool_destroy(threadpool_t *pool,
		       threadpool_showdown_t showdown_mode)
{
	if (pool == NULL || pool->shutdown) {
		pr_info("pool has been destroyed");
		return ;
	}

	pthread_mutex_lock(&pool->lock);

	pool->shutdown = showdown_mode;
	/** 唤醒所有的工作线程 */
	pthread_cond_broadcast(&pool->notify);

	pthread_mutex_unlock(&pool->lock);

	for (int i = 0; i < pool->thread_count; i++) {
		pthread_join(pool->threads[i], NULL);
	}

	threadpool_free(pool);
}