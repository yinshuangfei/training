/**
 * @file queue.h
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-03-09
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
 */

#ifndef LOCK_QUEUE_H
#define LOCK_QUEUE_H

#include <stdlib.h>

typedef struct node {
	struct node *next;
	struct node *prev;
	int data;
} node_t;

typedef struct queue {
	struct node head;
	size_t count;
	size_t total_enqueue;
	int shutdown;		/** 消费者退出 */
} queue_t;

#endif /** LOCK_QUEUE_H */