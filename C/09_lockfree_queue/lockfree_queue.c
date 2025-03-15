/*
 * @Author: Alan Yin
 * @Date: 2024-04-21 22:42:50
 * @LastEditTime: 2024-04-22 00:01:04
 * @LastEditors: Alan Yin
 * @FilePath: /self_project/04_lockfree_queue/lockfree_queue.c
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by HT706, All Rights Reserved.
 */

#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>

/** 队列节点 */
typedef struct node {
	void *data;
	struct node *next;
} node_t;

/** 无锁队列结构 */
typedef struct lockfree_queue {
	atomic_uintptr_t head;
	atomic_uintptr_t tail;
} lockfree_queue_t;

void init_lockfree_queue(lockfree_queue_t *queue)
{
	node_t *node = malloc(sizeof(node_t));
	node->next = NULL;
	queue->head = atomic_load(&node);
	queue->tail = atomic_load(&node);
}

// void enqueue(lockfree_queue_t *queue, void *data)
// {
// 	node_t *node = malloc(sizeof(node_t));
// 	node->data = data;
// 	node->next = NULL;

// 	/** 使用原子操作更新尾指针 */
// 	node_t *prev_tail;
// 	while (true) {
// 		prev_tail = atomic_load_explicit(&queue->tail,
// 						 memory_order_relaxed);
// 		if (atomic_compare_exchange_weak_explicit(
// 				&prev_tail->next, NULL, node,
// 				memory_order_release, memory_order_relaxed)) {
// 			/** 入队成功，退出循环 */
// 			break;
// 		} else {
// 			/** 更新尾指针失败，可能有其他线程正在更新尾指针，重试 */
// 			atomic_compare_exchange_weak_explicit(
// 				&queue->tail, &prev_tail, prev_tail->next,
// 				memory_order_relaxed, memory_order_relaxed
// 			);
// 		}
// 	}

// 	/** 更新尾指针 */
// 	atomic_compare_exchange_weak_explicit(
// 		&queue->tail, &prev_tail,
// 		node, memory_order_relaxed, memory_order_relaxed);
// }

// void *dequeue(lockfree_queue_t *queue)
// {
// 	node_t *prev_head;
// 	void *data = NULL;

// 	while (true) {
// 		prev_head = atomic_load_explicit(&queue->head,
// 				memory_order_acquire);
// 		node_t *next = atomic_load_explicit(&prev_head->next,
// 				memory_order_relaxed);

// 		if (next == NULL) {
// 			return NULL;
// 		}

// 		data = next->data;

// 		/** 出队成功，退出循环 */
// 		if (atomic_compare_exchange_weak_explicit(
// 				&queue->head, &prev_head, next,
// 				memory_order_release, memory_order_relaxed)) {
// 			break;
// 		}
// 	}

// 	free(prev_head);
// 	return data;
// }