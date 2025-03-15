/*
 * @Author: Alan Yin
 * @Date: 2024-04-21 00:40:01
 * @LastEditTime: 2024-04-21 23:41:57
 * @LastEditors: Alan Yin
 * @FilePath: /self_project/04_lockfree_queue/test_main.c
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by HT706, All Rights Reserved.
 */

#include <threads.h>
#include <assert.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#include "lockfree_queue.h"
#include "../common/utils.h"

#define THREADS		2

#ifdef __STDC_NO_ATOMICS__
#error "Implementation does not support atomic types."
#endif

#ifdef __STDC_NO_THREADS__
#error "Implementation does not support multi-threads."
#endif

atomic_int x = 0;
int a = 2;
int b = 3;

int func1(void *arg)
{
	b = 7;
	atomic_store_explicit(&x, 5, memory_order_release);

	thrd_exit(0);
}

int func2(void *arg)
{
	int temp;

	while (!(temp = atomic_load_explicit(&x, memory_order_consume)));

	a = temp;
	assert(b == 7);

	thrd_exit(0);
}

int main()
{
	thrd_t threads[THREADS];

	if (thrd_create(&threads[0], func1, NULL) != thrd_success) {
		pr_err("thrd_create error");
		exit(EXIT_FAILURE);
	}

	if (thrd_create(&threads[1], func2, NULL) != thrd_success) {
		pr_err("thrd_create error");
		exit(EXIT_FAILURE);
	}

	thrd_join(threads[0], NULL);
	thrd_join(threads[1], NULL);

	return 0;
}