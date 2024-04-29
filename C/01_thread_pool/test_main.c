/*
 * @Author: Alan Yin
 * @Date: 2024-04-20 23:12:37
 * @LastEditTime: 2024-04-21 22:43:37
 * @LastEditors: Alan Yin
 * @FilePath: /self_project/03_thread_pool/test_main.c
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by HT706, All Rights Reserved.
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "thread_pool.h"
#include "../common/utils.h"

// 静态初始化锁，用于保证第 15 行的完整输出
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void func2(void *arg)
{
	static int num = 0;

	pthread_mutex_lock(&mutex);

	// 为方便观察，故特意输出该语句，并使用num来区分不同的任务
	pr("这是执行的第 %d 个任务", ++num);

	// 模拟任务耗时，100ms
	usleep(100000);
	pthread_mutex_unlock(&mutex);

	return;
}

int main()
{
	threadpool_t *threadpool = NULL;
	int ret;

	ret = threadpool_init(4, 128, &threadpool);
	if (ret || threadpool == NULL)
		pr_err("threadpool init error");

	/** 生产者，向任务队列中塞入任务 */
	for (int i = 0; i < 20; i++) {
		threadpool_dispath(threadpool, func2, NULL);
	}

	// threadpool_destroy(threadpool, immediate_shutdown);
	threadpool_destroy(threadpool, grachful_shutdown);

	return 0;
}
