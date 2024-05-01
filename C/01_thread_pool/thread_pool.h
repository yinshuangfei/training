/*
 * @Author: Alan Yin
 * @Date: 2024-04-20 23:39:02
 * @LastEditTime: 2024-04-21 22:43:54
 * @LastEditors: Alan Yin
 * @FilePath: /self_project/03_thread_pool/thread_pool.h
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */

#ifndef YSF_THREAD_POOL_H
#define YSF_THREAD_POOL_H

/**
 * 线程池的三个要素：
 * 1.任务队列；
 * 2.工作线程；
 * 3.管理线程；
 *
 * https://blog.csdn.net/qw_6918966011/article/details/132124331
 *
*/
#include <pthread.h>

#include "../common/utils.h"

#define MAX_THREADS	16		// 线程池最大工作线程个数
#define MAX_QUEUE	256		// 线程池工作队列上限

typedef struct {
	void (*func)(void *);
	void *arg;
} threadpool_task_t;

typedef struct {
	pthread_mutex_t lock;		// 线程池锁, 保护下列所有变量
	pthread_cond_t notify;		// 告知线程池任务来了

	int thread_count;		// 工作线程数
	pthread_t *threads;		// 工作线程
	int started;			// 正在工作的线程数

	threadpool_task_t *queue;	// 环形任务队列
	int queue_size;			// 任务队列最大任务数
	int head;			// 队头 -> 取任务
	int tail;			// 队尾 -> 放任务
	int count;			// 队列中剩余任务个数

	int shutdown;			// 线程池状态，0 表示可用，其他表示关闭
} threadpool_t;

typedef enum {
	immediate_shutdown = 1,		// 立即关闭线程池
	grachful_shutdown,		// 等任务全部处理完成后，再关闭线程池
} threadpool_showdown_t;

int threadpool_init(int thread_count, int queue_size,
		    threadpool_t **threadpool);
int threadpool_dispath(threadpool_t *pool, void (*func)(void *), void *arg);
void threadpool_destroy(threadpool_t *pool,
			threadpool_showdown_t showdown_mode);

#endif /** YSF_THREAD_POOL_H */