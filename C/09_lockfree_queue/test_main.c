/**
 * @file test_main.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-03-03
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#include "../common/utils.h"
#include "lockfree_queue.h"
#include "mutex_queue.h"

size_t TOTAL_QUEUE_NODES = 0;
size_t PRODUCE_THREADS = 0;
size_t CONSUME_THREADS = 0;

/** 通用函数指针 */
typedef void*(*produce_fn)(void *args);
typedef void*(*consume_fn)(void *args);
typedef void(*init_fn)();
typedef void(*destroy_fn)();
typedef queue_ctr_t*(*get_fn)();

/** 队列函数指针描述结构体 */
typedef struct _queue_fn {
	init_fn init;
	destroy_fn destroy;
	produce_fn produce;
	consume_fn consume;
	get_fn get_ctr;
	char *name;
} queue_fn;

/** 队列函数指针集合 */
const queue_fn queuefun_array[] = {
	// {
	// 	.init = init_mutex_queue,
	// 	.destroy = destroy_mutex_queue,
	// 	.produce = mutex_queue_produce_func,
	// 	.consume = mutex_queue_consume_func,
	// 	.get_ctr = get_mutex_queue_ctr,
	// 	.name = "mutex_queue",
	// },
	{
		.init = init_lockfree_queue,
		.destroy = destroy_lockfree_queue,
		.produce = lockfree_queue_produce_func,
		.consume = lockfree_queue_consume_func,
		.get_ctr = get_lockfree_queue_ctr,
		.name = "lockfree_queue",
	}
};

void print_help(const char *name)
{
	printf("%s <total_num> <produce_num> <consume_num>\n\n"
	       "        total_num   : total enter queue item number\n"
	       "        produce_num : produce thread number\n"
	       "        consume_num : consume thread number\n",
	       name);
}

int main(int argc, char *argv[])
{
	int rc = 0;
	struct timespec start_tp, end_tp, diff_tp;

	if (argc < 3) {
		print_help(argv[0]);
		exit(0);
	}

	TOTAL_QUEUE_NODES = atol(argv[1]);
	PRODUCE_THREADS = atol(argv[2]);
	CONSUME_THREADS = atol(argv[3]);

	pthread_t produce_threads[PRODUCE_THREADS];
	pthread_t consume_threads[CONSUME_THREADS];

	for (int q_i = 0; q_i < ARRAY_SIZE(queuefun_array); q_i++) {
		queuefun_array[q_i].init();

		/** 起始时间 */
		rc = clock_gettime(0, &start_tp);
		if (-1 == rc) {
			pr_stderr("get start_tp error");
			return -1;
		}

		for (int i = 0; i < PRODUCE_THREADS; i++) {
			if (pthread_create(&produce_threads[i], NULL,
					   queuefun_array[q_i].produce,
					   (void *)&TOTAL_QUEUE_NODES) != 0) {
				pr_stderr("pthread_create [%d] error", i);
				exit(EXIT_FAILURE);
			}
		}

		for (int i = 0; i < CONSUME_THREADS; i++) {
			if (pthread_create(&consume_threads[i], NULL,
					   queuefun_array[q_i].consume,
					   (void *)&TOTAL_QUEUE_NODES) != 0) {
				pr_stderr("pthread_create [%d] error", i);
				exit(EXIT_FAILURE);
			}
		}

		for (int i = 0; i < PRODUCE_THREADS; i++) {
			pthread_join(produce_threads[i], NULL);
		}

		for (int i = 0; i < CONSUME_THREADS; i++) {
			pthread_join(consume_threads[i], NULL);
		}

		/** 终止时间 */
		rc = clock_gettime(0, &end_tp);
		if (-1 == rc) {
			pr_stderr("get end_tp error");
			return -1;
		}

		if (start_tp.tv_nsec > end_tp.tv_nsec) {
			diff_tp.tv_sec = end_tp.tv_sec - start_tp.tv_sec - 1;
			diff_tp.tv_nsec = 1000000000 +
					end_tp.tv_nsec -
					start_tp.tv_nsec;
		} else {
			diff_tp.tv_sec = end_tp.tv_sec - start_tp.tv_sec;
			diff_tp.tv_nsec = end_tp.tv_nsec - start_tp.tv_nsec;
		}

		pr_info("total time spend [ seconds: %ld, nanoseconds: %ld ]\n"
			"\tname          : %s\n"
			"\tproduce       : %d\n"
			"\tconsume       : %d\n"
			"\ttotal enqueue : %ld\n"
			"\tleft count    : %d",
			diff_tp.tv_sec,
			diff_tp.tv_nsec,
			queuefun_array[q_i].name,
			PRODUCE_THREADS,
			CONSUME_THREADS,
			queuefun_array[q_i].get_ctr()->total_enqueue,
			queuefun_array[q_i].get_ctr()->count);

		queuefun_array[q_i].destroy();
	}

	return 0;
}
